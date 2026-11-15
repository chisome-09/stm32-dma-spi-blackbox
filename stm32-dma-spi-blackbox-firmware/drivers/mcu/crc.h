#ifndef CRC_H_
#define CRC_H_
#define STM32F411xE
#include "stm32f411xe.h"
#include "stm32f4xx.h"
#include <stdint.h>


void crc_init();
uint32_t compute_crc(const uint32_t *data,uint8_t len);
#endif
