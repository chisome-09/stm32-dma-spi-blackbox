#include "dma.h"

uint8_t dma_init(DMA_Handle_t* DMA_Handler) {
    if (DMA_Handler == NULL || DMA_Handler->DMA_Stream == NULL) {
        return 0;
    }

    // 1. Enable DMA Peripheral Clock
    if (DMA_Handler->DMA_Periph == DMA1) {
        RCC->AHB1ENR |= DMA1EN;
        __DSB();
    }
    else if (DMA_Handler->DMA_Periph == DMA2) {
        RCC->AHB1ENR |= DMA2EN;
        __DSB();
    }
    else {
        return 0;
    }

    DMA_Stream_TypeDef* stream = DMA_Handler->DMA_Stream;
    DMA_Stream_Config* cfg     = &DMA_Handler->DMA_Config;

    stream->CR &= ~(1U << 0);
    while (stream->CR & (1U << 0)) {}

    // 3. Configurations
    // Channel [27:25]
    stream->CR &= ~(7U << 25);
    stream->CR |= (cfg->channel & (7U << 25));

    // Memory Burst Configurations [24:23]
    stream->CR &= ~(3U << 23);
    stream->CR |= ((cfg->memory_burst & 3U) << 23);

    // Peripheral Burst Configurations [22:21]
    stream->CR &= ~(3U << 21);
    stream->CR |= ((cfg->peripheral_burst & 3U) << 21);

    // Current Target [19] (already shifted in M0AR / M1AR macro)
    stream->CR &= ~(1U << 19);
    stream->CR |= (cfg->current_target & (1U << 19));

    // Double Buffer Mode [18]
    stream->CR &= ~(1U << 18);
    stream->CR |= ((cfg->double_buffer_mode & 1U) << 18);

    // Priority Level [17:16]
    stream->CR &= ~(3U << 16);
    stream->CR |= ((cfg->priority & 3U) << 16);

    // Peripheral Increment Offset Size (PINCOS) [15]
    stream->CR &= ~(1U << 15);
    stream->CR |= ((cfg->pincos & 1U) << 15);

    // Memory Data Size (MSIZE) [14:13]
    stream->CR &= ~(3U << 13);
    stream->CR |= ((cfg->data_size & 3U) << 13);

    // Peripheral Data Size (PSIZE) [12:11]
    stream->CR &= ~(3U << 11);
    stream->CR |= ((cfg->peripheral_size & 3U) << 11);

    // Memory Increment Mode (MINC) [10]
    stream->CR &= ~(1U << 10);
    stream->CR |= ((cfg->memory_increment_mode & 1U) << 10);

    // Peripheral Increment Mode (PINC) [9]
    stream->CR &= ~(1U << 9);
    stream->CR |= ((cfg->peripheral_increment_mode & 1U) << 9);

    // Circular Mode (CIRC) [8]
    stream->CR &= ~(1U << 8);
    stream->CR |= ((cfg->circular_mode & 1U) << 8);

    // Data Transfer Direction (DIR) [7:6]
    stream->CR &= ~(3U << 6);
    stream->CR |= ((cfg->data_transfer_direction & 3U) << 6);

    // Peripheral Flow Controller (PFCTRL) [5]
    stream->CR &= ~(1U << 5);
    stream->CR |= ((cfg->peripheral_flow_controller & 1U) << 5);

    // Transfer Complete Interrupt Enable (TCIE) [4]
    stream->CR &= ~(1U << 4);
    stream->CR |= ((cfg->transfer_int & 1U) << 4);

    // Half Transfer Interrupt Enable (HTIE) [3]
    stream->CR &= ~(1U << 3);
    stream->CR |= ((cfg->half_transfer_int & 1U) << 3);

    // Transfer Error Interrupt Enable (TEIE) [2]
    stream->CR &= ~(1U << 2);
    stream->CR |= ((cfg->transfer_err & 1U) << 2);

    // Direct Mode Error Interrupt Enable (DMEIE) [1]
    stream->CR &= ~(1U << 1);
    stream->CR |= ((cfg->direct_mode_err & 1U) << 1);

    // FIFO Error Interrupt Enable (FEIE) [7]
    stream->FCR &= ~(1U << 7);
    stream->FCR |= ((cfg->fifo_err_int & 1U) << 7);

    // Direct Mode Disable
    stream->FCR &= ~(1U << 2);
    stream->FCR |= ((cfg->direct_mode & 1U) << 2);

    // FIFO Threshold Selection (FTH) [1:0]
    stream->FCR &= ~(3U << 0);
    stream->FCR |= (cfg->fifo_thres & 3U);

    return 1;
}

uint8_t dma_transmit(DMA_Handle_t*DMA_Handler,uint16_t len,const void* periph_address,const void* mem0,const void* mem1){
	 if (DMA_Handler == NULL || DMA_Handler->DMA_Stream == NULL) {
	        return 0;
	    }
	 DMA_Stream_TypeDef* stream = DMA_Handler->DMA_Stream;
	 DMA_Stream_Config* cfg     = &DMA_Handler->DMA_Config;

	 stream->CR &= ~(1U << 0);
	 while (stream->CR & (1U << 0)) {}


	stream->PAR = (uint32_t)periph_address;
	stream->M0AR = (uint32_t)mem0;

	if(cfg->double_buffer_mode && (mem1 != NULL)){
		stream->M1AR = (uint32_t)mem1;
	}

	stream->NDTR = len;

	stream->CR |= (1U << 0);
	__DSB();
	return 1;

}
