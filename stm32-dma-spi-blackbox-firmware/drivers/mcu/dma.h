#ifndef MCU_DMA_H_
#define MCU_DMA_H_
#define STM32F411xE
#define NULL ((void *)0)
#include "stm32f411xe.h"
#include "stm32f4xx.h"
#include <stdint.h>

//ENABLE/DISABLE MACROS
#define ENABLE 1U
#define DISABLE 0U

//CLOCK MACROS
#define DMA1EN (1U<<21)
#define DMA2EN (1U<<22)
//TRANSFER MODES
#define PERIPH2MEM	0U
#define MEM2PERIPH 1U
#define MEM2MEM 2U
#define RESERVED 3U

//CHANNELS
#define STREAM_CHANNEL_0 0U
#define STREAM_CHANNEL_1 (1U<<25)
#define STREAM_CHANNEL_2 (2U<<25)
#define STREAM_CHANNEL_3 (3U<<25)
#define STREAM_CHANNEL_4 (4U<<25)
#define STREAM_CHANNEL_5 (5U<<25)
#define STREAM_CHANNEL_6 (6U<<25)
#define STREAM_CHANNEL_7 (7U<<25)

//BURST TRANSFER CONFIG
#define DMA_SINGLE_TRANSFER 0U
#define DMA_INCR4 1U
#define DMA_INCR8 2U
#define DMA_INCR16 3U

//TARGET MEMORY
#define DMA_M0AR (0U<<19)
#define DMA_M1AR (1U<<19)

//PRIORITY LEVEL
#define DMA_LOW_PRIORITY 0U
#define DMA_MEDIUM_PRIORITY 1U
#define DMA_HIGH_PRIORITY 2U
#define DMA_VERY_HIGH_PRIORITY 3U

//DATA SIZE
#define BYTE_SIZE 0U
#define TWO_BYTE_SIZE 1U
#define FOUR_BYTE_SIZE 2U
#define RESERVED_SIZE 3U

typedef struct{
	uint32_t channel;
	uint8_t memory_burst;
	uint8_t peripheral_burst;
	uint32_t current_target;
	uint8_t double_buffer_mode;
	uint8_t priority;
	uint8_t pincos;
	uint8_t data_size;
	uint8_t peripheral_size;
	uint8_t memory_increment_mode;
	uint8_t peripheral_increment_mode;
	uint8_t circular_mode;
	uint8_t data_transfer_direction;
	uint8_t peripheral_flow_controller;
	uint8_t transfer_int;
	uint8_t half_transfer_int;
	uint8_t transfer_err;
	uint8_t direct_mode_err;
	uint8_t fifo_err_int;
	uint8_t direct_mode;
	uint8_t fifo_thres;
}DMA_Stream_Config;

typedef struct{
	DMA_TypeDef* DMA_Periph;
	DMA_Stream_TypeDef* DMA_Stream;
	DMA_Stream_Config DMA_Config;
}DMA_Handle_t;

uint8_t dma_init(DMA_Handle_t* DMA_Handler);
uint8_t dma_transmit(DMA_Handle_t*DMA_Handler,uint16_t len,const void* periph_address,const void* mem0,const void* mem1);
uint8_t get_fifo_status(DMA_Handle_t* DMA_Handler);



#endif /* MCU_DMA_H_ */
