#ifndef RING_BUFFER_H_
#define RING_BUFFER_H_
#include <stdint.h>
#include "log_packet.h"
#define RING_SIZE 8

typedef struct{
	volatile Log_Page_t log_package[RING_SIZE];
	volatile uint8_t head;
	volatile uint8_t tail;
}flash_buffer;

typedef struct{
	volatile IMU_Sample_t imu_samples[RING_SIZE];
	volatile uint8_t head;
	volatile uint8_t tail;
}imu_buffer;

uint8_t read_flash_buffer(flash_buffer* buffer,Log_Page_t* log_page);
uint8_t read_imu_buffer(imu_buffer* buffer,IMU_Sample_t* imu_sample);

uint8_t write_flash_buffer(flash_buffer* buffer,const Log_Page_t* log_page);
uint8_t write_imu_buffer(imu_buffer* buffer,const IMU_Sample_t* imu_sample);
#endif /* RING_BUFFER_H_ */
