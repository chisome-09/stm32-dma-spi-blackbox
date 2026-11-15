#include "crc.h"

/************************************************************
 * @fn 														-crc_init
 *
 *
 * @brief													-Initializes CRC Peripheral
 * @param[in]											-none
 *
 * @return												-none
 */
void crc_init(){
	RCC->AHB1ENR |= (1U<<12);
}

/************************************************************
 * @fn 														-compute_crc
 *
 *
 * @brief													-Computes 32 bit CRC
 * @param[in]											-Pointer to data
 *@param[in]											-Length of data
 *
 * @return												-32 bit CRC
 */
uint32_t compute_crc(const uint32_t *data,uint8_t len){
	CRC->CR = (1U<<0);//RESET CRC
	while(len != 0){
		CRC->DR = *data;
		data++;
		len--;
	}
	return CRC->DR;
}
