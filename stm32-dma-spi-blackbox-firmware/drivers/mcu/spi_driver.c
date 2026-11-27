#include "spi_driver.h"
#include "dma.h"

/**
 * @brief  Enables the peripheral clock for the given SPI instance.
 * @param  pSPIx: Base address of the SPI peripheral (SPI1..SPI5).
 * @param  state: ENABLE or DISABLE. Currently unused; the clock is always enabled.
 * @retval None. Returns without action if pSPIx is not a valid SPI instance.
 */
void SPI_PeriClockControl(SPI_TypeDef *pSPIx, uint8_t state){
	if(pSPIx == SPI1){
		RCC->APB2ENR |= (1U<<1);
	}
	else if(pSPIx == SPI4){
		RCC->APB2ENR |= (1U<<13);
	}
	else if(pSPIx == SPI5){
		RCC->APB2ENR |= (1U<<20);
	}
	else if(pSPIx == SPI2){
		RCC->APB1ENR |= (1U<<14);
	}
	else if(pSPIx == SPI3){
		RCC->APB1ENR |= (1U<<15);
	}
	else{
		return;
	}
}

/**
 * @brief  Initializes the SPI peripheral registers according to the specified configuration.
 * @param  SPI_Handler: Pointer to an SPI_Handle_t structure containing the SPI base address
 *                      and configuration settings.
 * @retval None
 */
void SPI_Init(SPI_Handle_t *SPI_Handler){
	// 1. Disable SPI peripheral before modifying CR1 settings
	SPI_Handler->pSPIx->CR1 &= ~(1U << 6);

	uint32_t tempreg = SPI_Handler->pSPIx->CR1;
	tempreg &= ~((1U<<15)|(1U<<14)|(1U<<11)|(1U<<10)|(1U<<9)|(1U<<8)|(1U<<7)|(1U<<6)|(63U));

	// 2. Bus Configuration
	if(SPI_Handler->Config_Settings.bus_config == FULL_DUPLEX){
		tempreg &= ~(1U<<15);
		tempreg &= ~(1U<<10);
	}
	else if(SPI_Handler->Config_Settings.bus_config == UNIDI_RX){
		tempreg &= ~(1U<<15);
		tempreg |= (1U<<10);
	}
	else if(SPI_Handler->Config_Settings.bus_config == BIDI_RX){
		tempreg |= (1U<<15);
		tempreg &= ~(1U<<14);
	}
	else if(SPI_Handler->Config_Settings.bus_config == BIDI_TX){
		tempreg |= (1U<<15);
		tempreg |= (1U<<14);
	}

	// 3. NSS (Slave Select) Management
	if(SPI_Handler->Config_Settings.NSS == SOFTWARE_SSM){
		if(SPI_Handler->Config_Settings.mode == MASTER_MODE){
			tempreg |= (1U<<9) | (1U<<8); // Set SSM and SSI to prevent MODF fault
		}
		else{
			tempreg |= (1U<<9);           // Set SSM
			tempreg &= ~(1U<<8);          // Clear SSI (slave selected)
		}
	}
	else if(SPI_Handler->Config_Settings.NSS == HARDWARE_SSM){
		tempreg &= ~(1U<<9);              // Clear SSM for hardware NSS control
		if(SPI_Handler->Config_Settings.mode == MASTER_MODE){
			SPI_Handler->pSPIx->CR2 |= (1U<<2); // Enable SSOE in CR2 for master output
		}
	}

	// 4. Data Frame, Bit Order, Baud Rate, Mode, and Clock Phase/Polarity
	tempreg |= ((uint32_t)SPI_Handler->Config_Settings.DFF << 11);
	tempreg |= ((uint32_t)SPI_Handler->Config_Settings.frame_format << 7);
	tempreg |= ((uint32_t)SPI_Handler->Config_Settings.baud_rate << 3);
	tempreg |= ((uint32_t)SPI_Handler->Config_Settings.mode << 2);
	tempreg |= ((uint32_t)SPI_Handler->Config_Settings.CPOL << 1);
	tempreg |= ((uint32_t)SPI_Handler->Config_Settings.CPHA);

	// 5. Write configuration to CR1 and re-enable SPI
	SPI_Handler->pSPIx->CR1 = tempreg;
	SPI_Handler->pSPIx->CR1 |= (1U << 6);
}

/**
 * @brief  Blocking transmit. Waits for TXE before each write.
 * @param  pSPIx:     Base address of the SPI peripheral.
 * @param  pTxBuffer: Pointer to the data to send.
 * @param  len:       Number of bytes to send (must be even in 16-bit frame mode).
 * @retval None
 */
void SPI_TX(SPI_TypeDef *pSPIx, uint8_t *pTxBuffer, uint32_t len){
	while(len>0){
		while(!(pSPIx->SR & SPI_TXE)){};
		if(pSPIx->CR1 & (1U<<11)){   // FIX: was (1U<<7), which is LSBFIRST; DFF is bit 11
			pSPIx->DR = *(uint16_t*)pTxBuffer;
			len-=2;
			pTxBuffer+=2;
		}
		else{
			pSPIx->DR = *pTxBuffer;
			len-=1;
			pTxBuffer+=1;
		}
	}
}

/**
 * @brief  Blocking receive. Waits for RXNE before each read.
 * @param  pSPIx:     Base address of the SPI peripheral.
 * @param  pRxBuffer: Pointer to the buffer that receives the data.
 * @param  len:       Number of bytes to read (must be even in 16-bit frame mode).
 * @retval None
 */
void SPI_RX(SPI_TypeDef* pSPIx,uint8_t *pRxBuffer,uint32_t len){
	while(len>0){
		while(!((pSPIx->SR)&(1U<<0))){};
		if(pSPIx->CR1 & (1U<<11)){
			*((uint16_t*)pRxBuffer)=pSPIx->DR ;
			len-=2;
			pRxBuffer+=2;
		}
		else{
			*pRxBuffer=(uint8_t)pSPIx->DR ;
			len-=1;
			pRxBuffer+=1;
		}
	}
}

/**
 * @brief  Enables or disables an SPI interrupt line in the NVIC.
 * @param  pSPIx:          Unused.
 * @param  IRQNumber:      IRQ number of the interrupt (cast to IRQn_Type).
 * @param  PriorityNumber: NVIC priority. Applied only when state is ENABLE.
 * @param  state:          ENABLE or DISABLE.
 * @retval None
 */
void SPI_IRQConfig(SPI_TypeDef *pSPIx,uint8_t IRQNumber,uint8_t PriorityNumber,uint8_t state){
	if(state == ENABLE){
		NVIC_EnableIRQ((IRQn_Type) IRQNumber);
		NVIC_SetPriority((IRQn_Type) IRQNumber,PriorityNumber);
	}
	else{
		NVIC_DisableIRQ((IRQn_Type) IRQNumber);
	}
}

/**
 * @brief  Interrupt-driven receive step. Call from the SPI ISR.
 *         Reads one frame if RXNE is set, and disables the RXNE interrupt
 *         once RxLen reaches 0.
 * @param  pSPIHandle: Pointer to the SPI handle holding pSPIx, pRxBuffer and RxLen.
 * @retval None
 */
void SPI_RX_IT(SPI_Handle_t *pSPIHandle) {
    if (!(pSPIHandle->pSPIx->SR & (1U << 0))) {
        return;
    }

    if (pSPIHandle->pSPIx->CR1 & (1U << 11)) {
        *((uint16_t*)pSPIHandle->pRxBuffer) = (uint16_t)pSPIHandle->pSPIx->DR;
        pSPIHandle->RxLen -= 2;
        pSPIHandle->pRxBuffer += 2;
    } else {
        *(pSPIHandle->pRxBuffer) = (uint8_t)pSPIHandle->pSPIx->DR;
        pSPIHandle->RxLen -= 1;
        pSPIHandle->pRxBuffer += 1;
    }

    if (pSPIHandle->RxLen == 0) {
        pSPIHandle->pSPIx->CR2 &= ~(1U << 6);
    }
}

/**
 * @brief  Starts an interrupt-driven transmit by loading the handle and enabling TXEIE.
 * @param  pSPIHandle: Pointer to the SPI handle.
 * @param  tx_buffer:  Pointer to the data to send. Must stay valid until the transfer ends.
 * @param  len:        Number of bytes to send (0..255).
 * @retval SPI_OK   Transfer started.
 * @retval SPI_BUSY A previous transmit is still in progress; nothing was changed.
 */
uint8_t SPI_TX_IT(SPI_Handle_t* pSPIHandle,uint8_t* tx_buffer,uint8_t len){
	if(pSPIHandle->TxState == SPI_BUSY){
		return SPI_BUSY;
	}
	pSPIHandle->pTxBuffer = tx_buffer;
	pSPIHandle->TxState = SPI_BUSY;
	pSPIHandle->TxLen = len;
	pSPIHandle->pSPIx->CR2 |= (1U << 7);
	return SPI_OK;
}

/**
 * @brief  Interrupt-driven transmit step. Call from the SPI ISR.
 *         Writes one frame if TXE is set. When TxLen reaches 0 it disables the
 *         TXE interrupt and sets TxState to SPI_READY.
 * @param  pSPIHandle: Pointer to the SPI handle holding pSPIx, pTxBuffer, TxLen and TxState.
 * @retval None
 */
void SPI_TX_IT_ISR(SPI_Handle_t *pSPIHandle) {
    if (!(pSPIHandle->pSPIx->SR & (1U << 1))) {
        return;
    }
    if (pSPIHandle->pSPIx->CR1 & (1U << 11)) {
        pSPIHandle->pSPIx->DR = *((uint16_t*)pSPIHandle->pTxBuffer);
        pSPIHandle->TxLen -= 2;
        pSPIHandle->pTxBuffer += 2;
    } else {
        pSPIHandle->pSPIx->DR = *(pSPIHandle->pTxBuffer);
        pSPIHandle->TxLen -= 1;
        pSPIHandle->pTxBuffer += 1;
    }

    if (pSPIHandle->TxLen == 0) {
        pSPIHandle->pSPIx->CR2 &= ~(1U << 7);
        pSPIHandle->TxState = SPI_READY;
    }
}

uint8_t SPI_TX_DMA(DMA_Handle_t* DMA_Handler,SPI_Handle_t* pSPIHandle,uint8_t* tx_buffer,uint8_t len){
	if(pSPIHandle->TxState == SPI_BUSY){
		return SPI_BUSY;
	}
	pSPIHandle->pTxBuffer = tx_buffer;
	pSPIHandle->TxState = SPI_BUSY;
	pSPIHandle->TxLen = len;
	uint16_t items = (pSPIHandle->pSPIx->CR1 & (1U << 11)) ? (len / 2) : len;
	if(dma_transmit(DMA_Handler,items,(const void*)&pSPIHandle->pSPIx->DR,pSPIHandle->pTxBuffer,NULL) != DMA_OK){
		pSPIHandle->TxState = SPI_READY;
		return SPI_BUSY;
	}
	pSPIHandle->pSPIx->CR2 |= (1U << 1);
	return SPI_OK;
}

void SPI_TX_DMA_ISR(DMA_Handle_t* DMA_Handler, SPI_Handle_t* pSPIHandle){
	uint8_t flags = dma_irq_handler(DMA_Handler);
	if(!flags){
		return;
	}

	if(flags & (DMA_FLAG_TE | DMA_FLAG_DME)){
		pSPIHandle->pSPIx->CR2 &= ~(1U << 1);
		pSPIHandle->TxLen = 0;
		pSPIHandle->TxState = SPI_READY;
		return;
	}

	if(flags & DMA_FLAG_TC && !(DMA_Handler->DMA_Config.circular_mode)){
		pSPIHandle->pSPIx->CR2 &= ~(1U << 1);
		pSPIHandle->TxLen = 0;
	}
}

uint8_t SPI_RX_DMA(DMA_Handle_t* DMA_Handler,SPI_Handle_t* pSPIHandle,uint8_t* rx_buffer,uint8_t* rx_buffer_1,uint8_t len){
	if(pSPIHandle->RxState == SPI_BUSY){
			return SPI_BUSY;
	}
	pSPIHandle->pRxBuffer = rx_buffer;
	pSPIHandle->RxState = SPI_BUSY;
	pSPIHandle->RxLen = len;
	pSPIHandle->pRxBuffer_1 = rx_buffer_1;
	uint16_t items = (pSPIHandle->pSPIx->CR1 & (1U << 11)) ? (len / 2) : len;
	if(dma_transmit(DMA_Handler,items,(const void*)&pSPIHandle->pSPIx->DR,pSPIHandle->pRxBuffer,pSPIHandle->pRxBuffer_1) != DMA_OK){
		pSPIHandle->RxState = SPI_READY;
		return SPI_ERR;
	}
	pSPIHandle->pSPIx->CR2 |= (1U << 0);
	return SPI_OK;
}


