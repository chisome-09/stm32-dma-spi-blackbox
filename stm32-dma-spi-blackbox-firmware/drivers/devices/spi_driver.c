/*
 * spi_driver.c
 *
 *  Created on: Nov 6, 2026
 *      Author: echis
 */
/*#include "spi_driver.h"

uint8_t write_buffer(SPI_Circular_Buffer *pBuffer,uint8_t *data){
	uint8_t head = (pBuffer->head +1)&63;
	if(head == pBuffer->tail ){
		return 0;
	}
	pBuffer->ring_buffer[pBuffer->head] = *data;
	pBuffer->head = head;
	return 1;
}

uint8_t read_buffer(SPI_Circular_Buffer *pBuffer,uint8_t *data){
	if(pBuffer->head == pBuffer->tail ){
			return 0;
	}

	*data=pBuffer->ring_buffer[pBuffer->tail];
	return 1;
}

void SPI_Init(SPI_Handle_t *SPI_Handler){
	uint32_t tempreg = SPI_Handler->pSPIx->CR1;
	tempreg &= ~((1U<<15)|(1U<<14)|(1U<<11)|(1U<<10)|(1U<<9)|(1U<<8)|(1U<<7)|(1U<<6)|(63U));
	if(SPI_Handler->Config_Settings.bus_config == FULL_DUPLEX){
		tempreg &=~(1U<<15);
		tempreg &=~(1U<<10);

	}
	else if(SPI_Handler->Config_Settings.bus_config == UNIDI_RX){
		tempreg &=~(1U<<15);
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
	if(SPI_Handler->Config_Settings.NSS == SOFTWARE_SSM){
		if(SPI_Handler->Config_Settings.mode == MASTER_MODE){
			tempreg |= ((uint32_t)SOFTWARE_SSM<<8);
		}
		else{
			tempreg |= (1U<<9);
			tempreg &= ~(1U<<8);
		}
	}
	else if(SPI_Handler->Config_Settings.NSS == HARDWARE_SSM){
		tempreg &= ~(1U<<9);
	}
	tempreg |= ((uint32_t)SPI_Handler->Config_Settings.DFF<<11);
	tempreg |= ((uint32_t)SPI_Handler->Config_Settings.frame_format<<7);
	tempreg |= ((uint32_t)SPI_Handler->Config_Settings.baud_rate<<3);
	tempreg |=((uint32_t)SPI_Handler->Config_Settings.mode<<2);
	tempreg |=((uint32_t)SPI_Handler->Config_Settings.CPOL<<1);
	tempreg |=((uint32_t)SPI_Handler->Config_Settings.CPHA);

	SPI_Handler->pSPIx->CR1 = tempreg;
	SPI_Handler->pSPIx->CR1 |= (1U << 6);

}

void SPI_TX(SPI_TypeDef  *pSPIx,uint8_t *pTxBuffer,uint32_t len){
	while(len>0){
		while(!(pSPIx->SR & SPI_TXE)){};
		if(pSPIx->CR1 & (1U<<7)){
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

void SPI_IT_TX(SPI_TypeDef  *pSPIx,SPI_Circular_Buffer *pTxBuffer){
	uint8_t lower_byte = 0x00;
	uint8_t higher_byte = 0x00;
	if(pSPIx->CR1 & (1U<<11)){
		if(read_buffer(pTxBuffer,&higher_byte)){
			read_buffer(pTxBuffer,&lower_byte);
			pSPIx->DR = (higher_byte<<8) |(lower_byte);
		}
		else{
			pSPIx->CR2 &=~(1U<<7);
		}

	}
	if(!(pSPIx->CR1 & (1U<<11))){
		if(read_buffer(pTxBuffer,&lower_byte)){
			higher_byte = 0x00;
			pSPIx->DR = (higher_byte<<8) | (lower_byte);
		}
		else{
					pSPIx->CR2 &=~(1U<<7);
		}
	}


}

void SPI_IT_RX(SPI_TypeDef *pSPIx,SPI_Circular_Buffer *pRxBuffer){
	uint16_t data = pSPIx->DR;
	if(pSPIx->CR1 & (1U<<11)){
		if(write_buffer(pRxBuffer,&((data>>8)&0xFF))){
			if(!(write_buffer(pRxBuffer,&(data&0xFF)))){

			}
		}
	}

}
void SPI_IRQConfig(SPI_TypeDef *pSPIx,uint8_t IRQNumber,uint8_t PriorityNumber,uint8_t state){
	if(state){
		NVIC_EnableIRQ((IRQn_Type)IRQNumber);
		NVIC_SetPriority((IRQn_Type)IRQNumber,PriorityNumber);
		pSPIx->CR2 &=~(7U<<4);
		pSPIx->CR2 |= (7U<<4);
	}
	else{
		NVIC_DisableIRQ((IRQn_Type)IRQNumber);
	}
}

void SPI_IRQHandler(uint8_t PinNumber){

}



*/
