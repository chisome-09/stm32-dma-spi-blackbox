#include "mpu_driver.h"
/**
 * @brief  Initiates and completes a non-blocking DMA burst read of MPU6500 sensor registers.
 * @note   Designed to be polled repeatedly inside the main loop. Starts a 15-byte SPI DMA
 *         transfer (1 address byte + 14 sensor bytes) when both DMA streams are idle, and
 *         pushes the 14 received data bytes into the ring buffer upon RX transfer completion.
 * @param  pSPIHandle Pointer to the SPI peripheral handle structure.
 * @param  pDMA_TX    Pointer to the TX DMA stream handle structure.
 * @param  pDMA_RX    Pointer to the RX DMA stream handle structure.
 * @param  buffer     Pointer to the destination ring buffer for the 14 sensor bytes.
 * @retval uint8_t    1 if a 14-byte burst read completed and was written to @p buffer;
 *                    0 if the transfer is still in progress or failed to start.
 */
uint8_t mpu_read_data(SPI_Handle_t* pSPIHandle, DMA_Handle_t* pDMA_TX, DMA_Handle_t* pDMA_RX, ring_buffer* buffer) {
    static uint8_t tx_buf[15] = {
        MPU_ACCEL_XOUT_H | 0x80,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
    };
    static uint8_t rx_buf[15] = {0};

    if (pDMA_RX->busy_flag != DMA_BUSY && pDMA_TX->busy_flag != DMA_BUSY) {
        MPU_CS_LOW();

        if (SPI_RX_DMA(pDMA_RX, pSPIHandle, rx_buf, NULL, 15) != SPI_OK) {
            MPU_CS_HIGH();
            return 0;
        }
        if (SPI_TX_DMA(pDMA_TX, pSPIHandle, tx_buf, 15) != SPI_OK) {
            MPU_CS_HIGH();
            return 0;
        }
    }

    if (dma_take_event(pDMA_RX, DMA_FLAG_TC) == DMA_FLAG_TC) {
        dma_take_event(pDMA_TX, DMA_FLAG_TC);

        while (pSPIHandle->pSPIx->SR & (1U << 7));
        MPU_CS_HIGH();

        for (uint8_t i = 1; i < 15; i++) {
            write_buffer(buffer, &rx_buf[i]);
        }
        return 1;
    }

    return 0;
}

/**
 * @brief  Reads the WHO_AM_I identification register from the MPU6500 via SPI DMA.
 * @note   Performs a blocking 2-byte full-duplex DMA transfer (1 read command byte +
 *         1 dummy byte) and waits for both DMA streams and the SPI busy flag to clear
 *         before deasserting Chip Select.
 * @param  pSPIHandle Pointer to the SPI peripheral handle structure.
 * @param  pDMA_TX    Pointer to the TX DMA stream handle structure.
 * @param  pDMA_RX    Pointer to the RX DMA stream handle structure.
 * @retval uint8_t    Device ID read from the WHO_AM_I register (expected 0x70 for MPU6500),
 *                    or 0 if the DMA transfer failed to start.
 */
uint8_t mpu_who_am_i(SPI_Handle_t* pSPIHandle, DMA_Handle_t* pDMA_TX, DMA_Handle_t* pDMA_RX) {
    static uint8_t tx_buf[2] = { WHO_AM_I | 0x80, 0xFF };
    static uint8_t rx_buf[2] = { 0 };

    MPU_CS_LOW();

    if (SPI_RX_DMA(pDMA_RX, pSPIHandle, rx_buf, NULL, 2) != SPI_OK) {
        MPU_CS_HIGH();
        return 0;
    }

    if (SPI_TX_DMA(pDMA_TX, pSPIHandle, tx_buf, 2) != SPI_OK) {
        MPU_CS_HIGH();
        return 0;
    }

    while (dma_take_event(pDMA_TX, DMA_FLAG_TC) != DMA_FLAG_TC);
    while (dma_take_event(pDMA_RX, DMA_FLAG_TC) != DMA_FLAG_TC);
    while (pSPIHandle->pSPIx->SR & (1U << 7));

    MPU_CS_HIGH();

    return rx_buf[1];
}

/**
 * @brief  Writes a single byte to an MPU6500 register using blocking SPI DMA.
 * @param  pSPIHandle Pointer to the SPI peripheral handle structure.
 * @param  pDMA_TX    Pointer to the TX DMA stream handle structure.
 * @param  pDMA_RX    Pointer to the RX DMA stream handle structure.
 * @param  reg        Pointer to the target 7-bit register address.
 * @param  value      Pointer to the data byte to write into the register.
 * @retval uint8_t    1 on success, 0 if parameters are invalid or DMA failed to start.
 */
uint8_t mpu_write_reg(SPI_Handle_t* pSPIHandle, DMA_Handle_t* pDMA_TX, DMA_Handle_t* pDMA_RX, const uint8_t* reg, const uint8_t* value) {
    if (pSPIHandle == NULL || pDMA_TX == NULL || pDMA_RX == NULL || reg == NULL || value == NULL) {
        return 0;
    }

    static uint8_t tx_buf[2];
    static uint8_t rx_buf[2];

    tx_buf[0] = (*reg) & ~(1U << 7);
    tx_buf[1] = *value;

    MPU_CS_LOW();

    if (SPI_RX_DMA(pDMA_RX, pSPIHandle, rx_buf, NULL, 2) != SPI_OK) {
        MPU_CS_HIGH();
        return 0;
    }

    if (SPI_TX_DMA(pDMA_TX, pSPIHandle, tx_buf, 2) != SPI_OK) {
        MPU_CS_HIGH();
        return 0;
    }

    while (dma_take_event(pDMA_TX, DMA_FLAG_TC) != DMA_FLAG_TC);
    while (dma_take_event(pDMA_RX, DMA_FLAG_TC) != DMA_FLAG_TC);
    while (pSPIHandle->pSPIx->SR & (1U << 7));

    MPU_CS_HIGH();
    return 1;
}

uint8_t mpu_config(SPI_Handle_t* pSPIHandle, DMA_Handle_t* pDMA_TX, const MPU_Config_t* config)
{
    if (config == NULL) {
        return 1;
    }

    uint8_t reg_val = 0;

    // 1. PWR_MGMT_1 (0x6B): Wake up & select clock source first
    reg_val = MPU_SET_FIELD(config->clksel, MPU_PWR1_CLKSEL) |
              (config->pwr1_flags & ~MPU_PWR1_CLKSEL_Msk);
    mpu_write_reg(pSPIHandle, pDMA_TX, MPU_PWR_MGMT_1, reg_val);

    // 2. PWR_MGMT_2 (0x6C): Axis enable/disable & LP wake frequency
    reg_val = MPU_SET_FIELD(config->lp_wake_ctrl, MPU_PWR2_LP_WAKE_CTRL) |
              (config->pwr2_flags & ~MPU_PWR2_LP_WAKE_CTRL_Msk);
    mpu_write_reg(pSPIHandle, pDMA_TX, MPU_PWR_MGMT_2, reg_val);

    // 3. USER_CTRL (0x6A): FIFO, I2C master, SPI-only mode flags
    mpu_write_reg(pSPIHandle, pDMA_TX, MPU_USER_CONTROL, config->user_ctrl);

    // 4. SMPLRT_DIV (0x19): Sample rate divider
    mpu_write_reg(pSPIHandle, pDMA_TX, MPU_SMPLRT_DIV, config->smplrt_div);

    // 5. CONFIG (0x1A): DLPF, FSYNC, FIFO mode
    reg_val = MPU_SET_FIELD(config->dlpf_cfg, MPU_CONFIG_DLPF_CFG) |
              MPU_SET_FIELD(config->ext_sync_set, MPU_CONFIG_EXT_SYNC_SET) |
              MPU_SET_FIELD(config->fifo_mode, MPU_CONFIG_FIFO_MODE);
    mpu_write_reg(pSPIHandle, pDMA_TX, MPU_REG_CONFIG, reg_val);

    // 6. GYRO_CONFIG (0x1B): Full-scale select & FCHOICE_B
    reg_val = MPU_SET_FIELD(config->gyro_fchoice_b, MPU_GYRO_FCHOICE_B) |
              MPU_SET_FIELD(config->gyro_fs_sel, MPU_GYRO_FS_SEL);
    mpu_write_reg(pSPIHandle, pDMA_TX, MPU_REG_GYRO_CONFIG, reg_val);

    // 7. ACCEL_CONFIG (0x1C): Full-scale select
    reg_val = MPU_SET_FIELD(config->accel_fs_sel, MPU_ACCEL_FS_SEL);
    mpu_write_reg(pSPIHandle, pDMA_TX, MPU_REG_ACCEL_CONFIG, reg_val);

    // 8. ACCEL_CONFIG2 (0x1D): Accel DLPF & FCHOICE_B
    reg_val = MPU_SET_FIELD(config->accel_dlpf_cfg, MPU_ACCEL2_A_DLPF_CFG) |
              MPU_SET_FIELD(config->accel_fchoice_b, MPU_ACCEL2_FCHOICE_B);
    mpu_write_reg(pSPIHandle, pDMA_TX, MPU_REG_ACCEL_CONFIG2, reg_val);

    // 9. LP_ACCEL_ODR (0x1E): Low-power accel output data rate
    reg_val = MPU_SET_FIELD(config->lposc_clksel, MPU_LP_ODR_LPOSC_CLKSEL);
    mpu_write_reg(pSPIHandle, pDMA_TX, MPU_REG_LP_ACCEL_ODR, reg_val);

    // 10. WOM_THR (0x1F): Wake-on-Motion threshold
    mpu_write_reg(pSPIHandle, pDMA_TX, MPU_REG_WOM_THR, config->wom_threshold);

    // 11. FIFO_EN (0x23): Sensor FIFO routing
    mpu_write_reg(pSPIHandle, pDMA_TX, MPU_REG_FIFO_EN, config->fifo_en);

    // 12. ACCEL_INTEL_CTRL (0x69): Wake-on-Motion hardware intelligence
    mpu_write_reg(pSPIHandle, pDMA_TX, MPU_ACCEL_INTEL_CTRL, config->accel_int_ctrl);

    // 13. INT_PIN_CFG (0x37): Interrupt pin electrical/latching behavior
    mpu_write_reg(pSPIHandle, pDMA_TX, MPU_INT_PIN_CFG, config->int_pin_cfg);

    // 14. INT_ENABLE (0x38): Enable interrupts last after config is ready
    mpu_write_reg(pSPIHandle, pDMA_TX, MPU_INT_ENABLE, config->int_enable);

    return 0;
}

