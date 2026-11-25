#include "dma.h"

/**
 * @brief  Returns the stream number (0..7) of this handle.
 * @param  DMA_Handler: Pointer to the DMA handle.
 * @retval Stream number taken from DMA_Config.stream_number.
 */
static uint32_t dma_stream_index(DMA_Handle_t* DMA_Handler) {
    return DMA_Handler->DMA_Config.stream_number;
}



/**
 * @brief  Clears the selected status flags of this handle's stream by writing
 *         LIFCR (streams 0-3) or HIFCR (streams 4-7).
 * @param  DMA_Handler: Pointer to the DMA handle.
 * @param  flags:       Bit mask of flags to clear, in stream-relative position
 *                      (FE, DME, TE, HT, TC). Bits outside 0x3D are ignored.
 * @retval None
 */
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



/**
 * @brief  Clears all status flags (FE, DME, TE, HT, TC) of this handle's stream.
 * @param  DMA_Handler: Pointer to the DMA handle.
 * @retval 1 (always).
 */
uint8_t dma_clear_flags(DMA_Handle_t* DMA_Handler) {
    dma_clear_mask(DMA_Handler, 0x3DU);
    return 1;
}




/**
 * @brief  Reads the hardware status flags of this handle's stream from
 *         LISR (streams 0-3) or HISR (streams 4-7).
 * @param  DMA_Handler: Pointer to the DMA handle.
 * @retval Stream-relative flag mask (DMA_FLAG_FE, DME, TE, HT, TC). 0 if none are set.
 */
uint8_t dma_get_flags(DMA_Handle_t* DMA_Handler) {
    static const uint8_t shift[4] = {0, 6, 16, 22};
    uint32_t n   = dma_stream_index(DMA_Handler);
    uint32_t isr = (n < 4) ? DMA_Handler->DMA_Periph->LISR : DMA_Handler->DMA_Periph->HISR;

    return (uint8_t)((isr >> shift[n & 3U]) & 0x3DU);
}




/**
 * @brief  Reads the FIFO status field (FS[2:0]) of the stream.
 * @param  DMA_Handler: Pointer to the DMA handle.
 * @retval FIFO level code, 0..7 (see RM0383, DMA_SxFCR FS bits).
 */
uint8_t get_fifo_status(DMA_Handle_t* DMA_Handler) {
    return (uint8_t)((DMA_Handler->DMA_Stream->FCR >> 3) & 7U);
}




/**
 * @brief  Enables the DMA controller clock, disables the stream, clears flags and
 *         handle state, and writes the CR/FCR configuration from DMA_Config.
 *         Does not start a transfer.
 * @param  DMA_Handler: Pointer to the DMA handle. DMA_Periph must be DMA1 or DMA2,
 *                      DMA_Stream must be non-NULL, and DMA_Config must be filled in.
 *                      Note: channel and current_target must be pre-shifted
 *                      (channel << 25, current_target << 19); all other fields are unshifted.
 * @retval 1  Success.
 * @retval 0  DMA_Handler or DMA_Stream is NULL, or DMA_Periph is neither DMA1 nor DMA2.
 */
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




/**
 * @brief  Starts a DMA transfer: marks the handle busy, loads PAR/M0AR(/M1AR)/NDTR
 *         and sets EN. In polled mode (transfer_int == 0) stale flags are cleared first.
 * @param  DMA_Handler:    Pointer to an initialized DMA handle.
 * @param  len:            Number of data items to transfer (NDTR). Must be non-zero.
 * @param  periph_address: Peripheral data register address.
 * @param  mem0:           Memory buffer 0 address.
 * @param  mem1:           Memory buffer 1 address. Required (non-NULL) in double-buffer
 *                         mode, ignored otherwise.
 * @retval DMA_OK         Transfer started.
 * @retval DMA_ERR_PARAM  NULL handle or stream, len == 0, or double-buffer mode with mem1 == NULL.
 * @retval DMA_ERR_BUSY   Handle is busy or the stream's EN bit is still set; no register was changed.
 */
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




/**
 * @brief  Stops the stream (waits until EN reads 0), clears all flags and
 *         sets the handle to idle.
 * @param  DMA_Handler: Pointer to the DMA handle (must be non-NULL).
 * @retval 1 (always).
 */
uint8_t dma_abort(DMA_Handle_t* DMA_Handler) {
    DMA_Handler->DMA_Stream->CR &= ~(1U << 0);
    while (DMA_Handler->DMA_Stream->CR & (1U << 0)) {}

    dma_clear_flags(DMA_Handler);
    DMA_Handler->busy_flag = DMA_IDLE;
    return 1;
}




/**
 * @brief  Reports whether a transfer is in progress.
 * @param  DMA_Handler: Pointer to the DMA handle.
 * @retval 1  busy_flag is DMA_BUSY.
 * @retval 0  Idle.
 */
uint8_t dma_is_busy(DMA_Handle_t* DMA_Handler) {
    return (DMA_Handler->busy_flag == DMA_BUSY);
}




/**
 * @brief  Polls the hardware flags (for use without interrupts) and updates the handle.
 *         Error: records error_code and aborts the stream.
 *         Circular/double-buffer: latches HT/TC into event and clears the flags.
 *         One-shot: on TC clears flags and sets the handle idle.
 * @param  DMA_Handler: Pointer to the DMA handle.
 * @retval 1  Something happened (error, HT/TC event, or one-shot transfer complete).
 * @retval 0  Nothing to report yet.
 */
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




/**
 * @brief  Interrupt handler body. Call from the stream's IRQ handler.
 *         TE/DME: records error_code and aborts the stream.
 *         Otherwise: clears the flags, latches FE into error_code, latches HT/TC into
 *         event (incrementing overrun if the previous event was not consumed in
 *         circular/double-buffer mode), and sets the handle idle when a one-shot
 *         transfer completes.
 * @param  DMA_Handler: Pointer to the DMA handle.
 * @retval Flags that were active (DMA_FLAG_* mask), or 0 if the interrupt was not for this stream.
 */
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




/**
 * @brief  Returns and clears the latched error flags.
 * @param  DMA_Handler: Pointer to the DMA handle.
 * @retval Error mask (DMA_FLAG_TE, DME, FE), or 0 if no error has occurred.
 */
uint8_t dma_take_error(DMA_Handle_t* DMA_Handler) {
    uint8_t e = DMA_Handler->error_code;
    DMA_Handler->error_code = 0;
    return e;
}




/**
 * @brief  Atomically returns and clears the selected latched events.
 *         Interrupts are disabled during the read-modify-write so the ISR cannot lose an update.
 * @param  DMA_Handler: Pointer to the DMA handle.
 * @param  mask:        Events to consume (DMA_FLAG_HT and/or DMA_FLAG_TC).
 * @retval The subset of mask that was pending (0 if none).
 */
uint8_t dma_take_event(DMA_Handle_t* DMA_Handler, uint8_t mask) {
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    uint8_t e = DMA_Handler->event & mask;
    DMA_Handler->event &= ~mask;
    __set_PRIMASK(primask);
    return e;
}



/**
 * @brief  Checks pending events without clearing them.
 * @param  DMA_Handler: Pointer to the DMA handle.
 * @param  mask:        Events to look at (DMA_FLAG_HT and/or DMA_FLAG_TC).
 * @retval The subset of mask that is pending (0 if none).
 */
uint8_t dma_peek_event(DMA_Handle_t* DMA_Handler, uint8_t mask) {
    return (DMA_Handler->event & mask);
}
