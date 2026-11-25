#ifndef DEVICES_MPU_DRIVER_H_
#define DEVICES_MPU_DRIVER_H_
#include "log_packet.h"


//MPU Settings Register Map
typedef enum {
    MPU_REG_CONFIG        = 0x1A,
	MPU_REG_GYRO_CONFIG   = 0x1B,
	MPU_REG_ACCEL_CONFIG  = 0x1C,
	MPU_REG_ACCEL_CONFIG2 = 0x1D,
	MPU_REG_LP_ACCEL_ODR  = 0x1E,
	MPU_REG_WOM_THR       = 0x1F,
	MPU_REG_FIFO_EN       = 0x23,
	MPU_USER_CONTROL = 0x6A,
	WHO_AM_I = 0x75,
	MPU_PWR_MGMT_2 =0X6C,
	MPU_ACCEL_INTEL_CTRL = 0X69,
	MPU_SMPLRT_DIV = 0x19,
	MPU_PWR_MGMT_1 =0x6B,
	MPU_INT_PIN_CFG = 0X37,
	MPU_INT_ENABLE = 0x38,
	MPU_INT_STATUS = 0x3A,
	MPU_ACCEL_XOUT_H = 0x3B
} MPU_General_Reg;


//CONFIG (0x1A)
#define MPU_CONFIG_DLPF_CFG_Pos          0U
#define MPU_CONFIG_DLPF_CFG_Msk          (7U << MPU_CONFIG_DLPF_CFG_Pos)
#define MPU_CONFIG_EXT_SYNC_SET_Pos      3U
#define MPU_CONFIG_EXT_SYNC_SET_Msk      (7U << MPU_CONFIG_EXT_SYNC_SET_Pos)
#define MPU_CONFIG_FIFO_MODE_Msk         (1U << 6)

//GYRO_CONFIG (0x1B)
#define MPU_GYRO_FCHOICE_B_Pos           0U
#define MPU_GYRO_FCHOICE_B_Msk           (3U << MPU_GYRO_FCHOICE_B_Pos)
#define MPU_GYRO_FS_SEL_Pos              3U
#define MPU_GYRO_FS_SEL_Msk              (3U << MPU_GYRO_FS_SEL_Pos)
#define MPU_GYRO_ZG_ST_Msk               (1U << 5)
#define MPU_GYRO_YG_ST_Msk               (1U << 6)
#define MPU_GYRO_XG_ST_Msk               (1U << 7)

//ACCEL_CONFIG (0x1C)
#define MPU_ACCEL_FS_SEL_Pos             3U
#define MPU_ACCEL_FS_SEL_Msk             (3U << MPU_ACCEL_FS_SEL_Pos)
#define MPU_ACCEL_ZA_ST_Msk              (1U << 5)
#define MPU_ACCEL_YA_ST_Msk              (1U << 6)
#define MPU_ACCEL_XA_ST_Msk              (1U << 7)

//ACCEL_CONFIG2 (0x1D)
#define MPU_ACCEL2_A_DLPF_CFG_Pos        0U
#define MPU_ACCEL2_A_DLPF_CFG_Msk        (7U << MPU_ACCEL2_A_DLPF_CFG_Pos)
#define MPU_ACCEL2_FCHOICE_B_Msk         (1U << 3)

//LP_ACCEL_ODR (0x1E)
#define MPU_LP_ODR_LPOSC_CLKSEL_Pos      0U
#define MPU_LP_ODR_LPOSC_CLKSEL_Msk      (15U << MPU_LP_ODR_LPOSC_CLKSEL_Pos)

//Wake On Motion Threshold
extern uint8_t wom_threshold;

//FIFO_EN (0x23)
#define MPU_FIFO_SLV0_Msk                (1U << 0)
#define MPU_FIFO_SLV1_Msk                (1U << 1)
#define MPU_FIFO_SLV2_Msk                (1U << 2)
#define MPU_FIFO_ACCEL_Msk               (1U << 3)
#define MPU_FIFO_GYRO_ZOUT_Msk           (1U << 4)
#define MPU_FIFO_GYRO_YOUT_Msk           (1U << 5)
#define MPU_FIFO_GYRO_XOUT_Msk           (1U << 6)
#define MPU_FIFO_TEMP_OUT_Msk            (1U << 7)

//USER_CTRL (0x6A)
#define MPU_UCTRL_SIG_COND_RST_Msk       (1U << 0)
#define MPU_UCTRL_I2C_MST_RST_Msk        (1U << 1)
#define MPU_UCTRL_FIFO_RST_Msk           (1U << 2)
#define MPU_UCTRL_DMP_RST_Msk            (1U << 3)
#define MPU_UCTRL_I2C_IF_DIS_Msk         (1U << 4)
#define MPU_UCTRL_I2C_MST_EN_Msk         (1U << 5)
#define MPU_UCTRL_FIFO_EN_Msk            (1U << 6)
#define MPU_UCTRL_DMP_EN_Msk             (1U << 7)

//ACCEL_INTEL_CTRL (0x69)
#define MPU_AINT_INTEL_MODE_Msk          (1U << 6)
#define MPU_AINT_INTEL_EN_Msk            (1U << 7)

//PWR_MGMT_1 (0x6B)
#define MPU_PWR1_CLKSEL_Pos              0U
#define MPU_PWR1_CLKSEL_Msk              (7U << MPU_PWR1_CLKSEL_Pos)
#define MPU_PWR1_TEMP_DIS_Msk            (1U << 3)
#define MPU_PWR1_GYRO_STANDBY_Msk        (1U << 4)
#define MPU_PWR1_CYCLE_Msk               (1U << 5)
#define MPU_PWR1_SLEEP_Msk               (1U << 6)
#define MPU_PWR1_DEVICE_RESET_Msk        (1U << 7)

//PWR_MGMT_2 (0x6C)
#define MPU_PWR2_DISABLE_ZG_Msk          (1U << 0)
#define MPU_PWR2_DISABLE_YG_Msk          (1U << 1)
#define MPU_PWR2_DISABLE_XG_Msk          (1U << 2)
#define MPU_PWR2_DISABLE_ZA_Msk          (1U << 3)
#define MPU_PWR2_DISABLE_YA_Msk          (1U << 4)
#define MPU_PWR2_DISABLE_XA_Msk          (1U << 5)
#define MPU_PWR2_LP_WAKE_CTRL_Pos        6U
#define MPU_PWR2_LP_WAKE_CTRL_Msk        (3U << MPU_PWR2_LP_WAKE_CTRL_Pos)

//INT_PIN_CFG (0x37)
#define MPU_INTPIN_BYPASS_EN_Msk         (1U << 1)
#define MPU_INTPIN_FSYNC_INT_MODE_Msk    (1U << 2)
#define MPU_INTPIN_ACTL_FSYNC_Msk        (1U << 3)
#define MPU_INTPIN_INT_ANYRD_2CLEAR_Msk  (1U << 4)
#define MPU_INTPIN_LATCH_INT_EN_Msk      (1U << 5)
#define MPU_INTPIN_OPEN_Msk              (1U << 6)
#define MPU_INTPIN_ACTL_Msk              (1U << 7)

//INT_ENABLE (0x38)
#define MPU_INTEN_RAW_RDY_Msk            (1U << 0)
#define MPU_INTEN_FSYNC_INT_Msk          (1U << 3)
#define MPU_INTEN_FIFO_OVERFLOW_Msk      (1U << 4)
#define MPU_INTEN_WOM_Msk                (1U << 6)

//INT_STATUS (0x3A)
#define MPU_INTST_RAW_DATA_RDY_Msk       (1U << 0)
#define MPU_INTST_DMP_INT_Msk            (1U << 1)
#define MPU_INTST_FSYNC_INT_Msk          (1U << 3)
#define MPU_INTST_FIFO_OVERFLOW_Msk      (1U << 4)
#define MPU_INTST_WOM_INT_Msk            (1U << 6)

//Helpers for multi-bit fields
#define MPU_SET_FIELD(val, name)   (((uint8_t)(val) << name##_Pos) & name##_Msk)
#define MPU_GET_FIELD(reg, name)   (((reg) & name##_Msk) >> name##_Pos)


//Driver Functions
uint16_t compute_DC_bias(void);//Gyroscope Offset Registers
uint8_t mpu_config(void);
uint8_t mpu_wom_mode(void);
uint8_t mpu_read(IMU_Sample_t* imu_sample);
uint8_t mpu_signal_reset(void);
uint8_t mpu_reset(void);
#endif /* DEVICES_MPU_DRIVER_H_ */
