#include "dma.h"

static uint32_t dma_stream_index(DMA_Handle_t* DMA_Handler) {
    return (((uint32_t)DMA_Handler->DMA_Stream & 0x3FFU) - 0x10U) / 0x18U;
}



static void dma_clear_mask(DMA_Handle_t* DMA_Handler, uint8_t flags) {
    static const uint8_t shift[4] = {0, 6, 16, 22};
    uint32_t n    = dma_stream_index(DMA_Handler);
    uint32_t mask = ((uint32_t)flags & 0x3DU) << shift[n & 3U];

    if (n < 4) {
        DMA_Handler->DMA_Periph->LIFCR = mask;
    }
    else {
        DMA_Handler->DMA_Periph->HIFCR = mask;
    }
}



uint8_t dma_clear_flags(DMA_Handle_t* DMA_Handler) {
    dma_clear_mask(DMA_Handler, 0x3DU);
    return 1;
}




uint8_t dma_get_flags(DMA_Handle_t* DMA_Handler) {
    static const uint8_t shift[4] = {0, 6, 16, 22};
    uint32_t n   = dma_stream_index(DMA_Handler);
    uint32_t isr = (n < 4) ? DMA_Handler->DMA_Periph->LISR : DMA_Handler->DMA_Periph->HISR;

    return (uint8_t)((isr >> shift[n & 3U]) & 0x3DU);
}




uint8_t get_fifo_status(DMA_Handle_t* DMA_Handler) {
    return (uint8_t)((DMA_Handler->DMA_Stream->FCR >> 3) & 7U);
}




uint8_t dma_init(DMA_Handle_t* DMA_Handler) {
    if (DMA_Handler == NULL || DMA_Handler->DMA_Stream == NULL) {
        return 0;
    }

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

    dma_clear_flags(DMA_Handler);
    DMA_Handler->busy_flag  = DMA_IDLE;
    DMA_Handler->error_code = 0;
    DMA_Handler->event      = 0;
    DMA_Handler->overrun    = 0;

    stream->CR &= ~(7U << 25);
    stream->CR |= (cfg->channel & (7U << 25));

    stream->CR &= ~(3U << 23);
    stream->CR |= ((cfg->memory_burst & 3U) << 23);

    stream->CR &= ~(3U << 21);
    stream->CR |= ((cfg->peripheral_burst & 3U) << 21);

    stream->CR &= ~(1U << 19);
    stream->CR |= (cfg->current_target & (1U << 19));

    stream->CR &= ~(1U << 18);
    stream->CR |= ((cfg->double_buffer_mode & 1U) << 18);

    stream->CR &= ~(3U << 16);
    stream->CR |= ((cfg->priority & 3U) << 16);

    stream->CR &= ~(1U << 15);
    stream->CR |= ((cfg->pincos & 1U) << 15);

    stream->CR &= ~(3U << 13);
    stream->CR |= ((cfg->data_size & 3U) << 13);

    stream->CR &= ~(3U << 11);
    stream->CR |= ((cfg->peripheral_size & 3U) << 11);

    stream->CR &= ~(1U << 10);
    stream->CR |= ((cfg->memory_increment_mode & 1U) << 10);

    stream->CR &= ~(1U << 9);
    stream->CR |= ((cfg->peripheral_increment_mode & 1U) << 9);

    stream->CR &= ~(1U << 8);
    stream->CR |= ((cfg->circular_mode & 1U) << 8);

    stream->CR &= ~(3U << 6);
    stream->CR |= ((cfg->data_transfer_direction & 3U) << 6);

    stream->CR &= ~(1U << 5);
    stream->CR |= ((cfg->peripheral_flow_controller & 1U) << 5);

    stream->CR &= ~(1U << 4);
    stream->CR |= ((cfg->transfer_int & 1U) << 4);

    stream->CR &= ~(1U << 3);
    stream->CR |= ((cfg->half_transfer_int & 1U) << 3);

    stream->CR &= ~(1U << 2);
    stream->CR |= ((cfg->transfer_err & 1U) << 2);

    stream->CR &= ~(1U << 1);
    stream->CR |= ((cfg->direct_mode_err & 1U) << 1);

    stream->FCR &= ~(1U << 7);
    stream->FCR |= ((cfg->fifo_err_int & 1U) << 7);

    stream->FCR &= ~(1U << 2);
    stream->FCR |= ((cfg->direct_mode & 1U) << 2);

    stream->FCR &= ~(3U << 0);
    stream->FCR |= (cfg->fifo_thres & 3U);

    return 1;
}




uint8_t dma_transmit(DMA_Handle_t*DMA_Handler,uint16_t len,const void* periph_address,const void* mem0,const void* mem1){
	 if (DMA_Handler == NULL || DMA_Handler->DMA_Stream == NULL || len == 0) {
	        return DMA_ERR_PARAM;
	    }

	 DMA_Stream_TypeDef* stream = DMA_Handler->DMA_Stream;
	 DMA_Stream_Config* cfg     = &DMA_Handler->DMA_Config;

	 if (cfg->double_buffer_mode && (mem1 == NULL)) {
	        return DMA_ERR_PARAM;
	    }

	 uint32_t primask = __get_PRIMASK();
	 __disable_irq();
	 if (DMA_Handler->busy_flag == DMA_BUSY || (stream->CR & (1U << 0))) {
		 __set_PRIMASK(primask);
		 return DMA_ERR_BUSY;
	 }
	 DMA_Handler->busy_flag = DMA_BUSY;
	 __set_PRIMASK(primask);

	 DMA_Handler->error_code = 0;

	 if (!cfg->transfer_int) {
		 dma_clear_flags(DMA_Handler);
	 }

	stream->PAR = (uint32_t)periph_address;
	stream->M0AR = (uint32_t)mem0;

	if(cfg->double_buffer_mode){
		stream->M1AR = (uint32_t)mem1;
	}

	stream->NDTR = len;

	stream->CR |= (1U << 0);
	__DSB();
	return DMA_OK;

}




uint8_t dma_abort(DMA_Handle_t* DMA_Handler) {
    DMA_Handler->DMA_Stream->CR &= ~(1U << 0);
    while (DMA_Handler->DMA_Stream->CR & (1U << 0)) {}

    dma_clear_flags(DMA_Handler);
    DMA_Handler->busy_flag = DMA_IDLE;
    return 1;
}




uint8_t dma_is_busy(DMA_Handle_t* DMA_Handler) {
    return (DMA_Handler->busy_flag == DMA_BUSY);
}




uint8_t dma_poll_done(DMA_Handle_t* DMA_Handler) {
    uint8_t flags = dma_get_flags(DMA_Handler);

    if (flags & (DMA_FLAG_TE | DMA_FLAG_DME)) {
        DMA_Handler->error_code = flags;
        dma_abort(DMA_Handler);
        return 1;
    }

    if (DMA_Handler->DMA_Config.circular_mode || DMA_Handler->DMA_Config.double_buffer_mode) {
        uint8_t ev = flags & (DMA_FLAG_HT | DMA_FLAG_TC);
        if (ev) {
            dma_clear_mask(DMA_Handler, flags);
            DMA_Handler->event |= ev;
            return 1;
        }
        return 0;
    }

    if (flags & DMA_FLAG_TC) {
        dma_clear_flags(DMA_Handler);
        DMA_Handler->busy_flag = DMA_IDLE;
        return 1;
    }

    return 0;
}




uint8_t dma_irq_handler(DMA_Handle_t* DMA_Handler) {
    uint8_t flags = dma_get_flags(DMA_Handler);

    if (flags == 0) {
        return 0;
    }

    if (flags & (DMA_FLAG_TE | DMA_FLAG_DME)) {
        DMA_Handler->error_code = flags;
        dma_abort(DMA_Handler);
        return flags;
    }

    dma_clear_mask(DMA_Handler, flags);

    if (flags & DMA_FLAG_FE) {
        DMA_Handler->error_code |= DMA_FLAG_FE;
    }

    if (DMA_Handler->DMA_Config.circular_mode || DMA_Handler->DMA_Config.double_buffer_mode) {
        uint8_t ev = flags & (DMA_FLAG_HT | DMA_FLAG_TC);

        if (DMA_Handler->event & ev) {
            DMA_Handler->overrun++;
        }
        DMA_Handler->event |= ev;
        return flags;
    }

    if (flags & DMA_FLAG_HT) {
        DMA_Handler->event |= DMA_FLAG_HT;
    }

    if (flags & DMA_FLAG_TC) {
        DMA_Handler->event |= DMA_FLAG_TC;
        DMA_Handler->busy_flag = DMA_IDLE;
    }
    else if ((flags & DMA_FLAG_FE) && !(DMA_Handler->DMA_Stream->CR & (1U << 0))) {
        DMA_Handler->busy_flag = DMA_IDLE;
    }

    return flags;
}




uint8_t dma_take_error(DMA_Handle_t* DMA_Handler) {
    uint8_t e = DMA_Handler->error_code;
    DMA_Handler->error_code = 0;
    return e;
}




uint8_t dma_take_event(DMA_Handle_t* DMA_Handler, uint8_t mask) {
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    uint8_t e = DMA_Handler->event & mask;
    DMA_Handler->event &= ~mask;
    __set_PRIMASK(primask);
    return e;
}



uint8_t dma_peek_event(DMA_Handle_t* DMA_Handler, uint8_t mask) {
    return (DMA_Handler->event & mask);
}
