/*
 * spi_driver.h
 *
 *  Created on: Nov 6, 2026
 *      Author: echis
 */
#define STM32F411xE
#include "stm32f411xe.h"
#include "stm32f4xx.h"
#ifndef SPI_DRIVER_H_
#define SPI_DRIVER_H_

#include<stdint.h>
#include "ring_buffer.h"


//frame)format settings
#define MSB_FIRST 0U
#define LSB_FIRST 1U

//DFF Settings
#define BIT_8_DFF 0U
#define BIT_16_DFF 1U

//mode settings
#define SLAVE_MODE 0U
#define MASTER_MODE 1U

//CPOL settings
#define IDLE_LOW 0U
#define IDLE_HIGH 1U

//CPHA settings
#define CPHA_0 0U
#define CPHA_1 1U

//NSS settings
#define SOFTWARE_SSM 3U
#define HARDWARE_SSM 0U

//baud_rate settings
#define BAUD_0 0U
#define BAUD_1 1U
#define BAUD_2 2U
#define BAUD_3 3U
#define BAUD_4 4U
#define BAUD_5 5U
#define BAUD_6 6U
#define BAUD_7 7U

//bus_config settings
#define two_line_bidi 0U
#define one_line_bidi 1U

//bidioe settings
#define RECEIVE_ONLY 0U
#define TRANSMIT_ONLY 1U

#define FULL_DUPLEX 0U
#define UNIDI_RX 1U
#define BIDI_TX 2U
#define BIDI_RX 3U

#define SPI_TXE (1U<<1)

typedef struct{
	uint8_t DFF;
	uint8_t NSS;
	uint8_t frame_format;
	uint8_t mode;
	uint8_t CPOL;
	uint8_t CPHA;
	uint8_t baud_rate;
	uint8_t bus_config;
}SPI_Config;


typedef struct{
	SPI_TypeDef* pSPIx;
	SPI_Config Config_Settings;
}SPI_Handle_t;

typedef struct{
	volatile uint8_t ring_buffer[64];
	volatile uint8_t head;
	volatile uint8_t tail;
}SPI_Circular_Buffer;

void SPI_Init(SPI_Handle_t 	*SPI_Handler);
void SPI_DeInit(SPI_TypeDef *pSPIx );
void  SPI_PeriClockControl(SPI_TypeDef  *pSPIx,uint8_t state);
void SPI_TX(SPI_TypeDef  *pSPIx,uint8_t  *pTxBuffer,uint32_t len);
void SPI_RX(SPI_TypeDef  *pSPIx,uint8_t *pRxBuffer,uint32_t len);
void SPI_IRQConfig(SPI_TypeDef *pSPIx,uint8_t IRQNumber,uint8_t PriorityNumber,uint8_t state);
void SPI_IRQHandler(uint8_t PinNumber);
uint8_t read_buffer(SPI_Circular_Buffer *pBuffer,uint8_t *data);
uint8_t write_buffer(SPI_Circular_Buffer *pBuffer,uint8_t *data);
#endif /* SPI_DRIVER_H_ */
