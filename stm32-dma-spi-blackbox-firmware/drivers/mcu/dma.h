#ifndef MCU_DMA_H_
#define MCU_DMA_H_
#define STM32F411xE
#include "stm32f411xe.h"
#include "stm32f4xx.h"
#include <stdint.h>


//TRANSFER MODES
#define PERIPH2MEM	0U
#define MEM2PERIPH 1U
#define MEM2MEM 2U
#define RESERVED 3U

//CHANNELS
#define CHANNEL_0 0U
#define CHANNEL_1 1U
#define CHANNEL_2 2U
#define CHANNEL_3 3U
#define CHANNEL_4 4U
#define CHANNEL_5 5U
#define CHANNEL_6 6U
#define CHANNEL_7 7U

//BURST TRANSFER CONFIG
#define SINGLE_TRANSFER 0U
#define INCR4 1U
#define INCR8 2U
#define INCR16 3U

//TARGET MEMORY
#define M0AR 0U
#define M1AR 1U

//PRIORITY LEVEL
#define LOW 0U
#define MEDIUM 1U
#define HIGH 2U
#define VERY_HIGH 3U


typedef struct{
	uint8_t channel;
	uint8_t memory_burst;
	uint8_t peripheral_burst;
	uint8_t current_target;
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
uint8_t dma_transmit(uint16_t len,uint32_t* mem0,uint32_t* mem1,uint32_t* periph_address);
uint8_t get_fifo_status(void);



#endif /* MCU_DMA_H_ */
