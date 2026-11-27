#include <stdint.h>
#include "ring_buffer.h"

uint8_t read_flash_buffer(flash_buffer* buffer,Log_Page_t* log_page){
	if(buffer->head == buffer->tail){
		return 0;
	}
	*log_page = buffer->log_package[buffer->tail];
	buffer->tail = (buffer->tail + 1) & 7;
	return 1;
}

uint8_t read_imu_buffer(imu_buffer* buffer,IMU_Sample_t* imu_sample){
	if(buffer->head == buffer->tail){
		return 0;
	}
	*imu_sample = buffer->imu_samples[buffer->tail];
	buffer->tail = (buffer->tail + 1) & 7;
	return 1;
}

uint8_t write_flash_buffer(flash_buffer* buffer,const Log_Page_t* log_page){
	uint8_t potential_new_head = (buffer->head + 1) & 7;
	if(potential_new_head == buffer->tail){
		return 0;
	}
	buffer->log_package[buffer->head] = *log_page;
	buffer->head = potential_new_head;
	return 1;

}

uint8_t write_imu_buffer(imu_buffer* buffer,const IMU_Sample_t* imu_sample){
	uint8_t potential_new_head = (buffer->head + 1) & 7;
		if(potential_new_head == buffer->tail){
			return 0;
		}
		buffer->imu_samples[buffer->head] = *imu_sample;
		buffer->head = potential_new_head;
		return 1;
}

uint8_t write_buffer(ring_buffer* buffer, uint8_t* value) {
    uint16_t next_head = (buffer->head + 1) & 63;

    if (next_head == buffer->tail) {
        return 0; // Buffer is full
    }

    buffer->data[buffer->head] = *value;
    buffer->head = next_head;
    return 1;
}

uint8_t read_buffer(ring_buffer* buffer, uint8_t* value) {
    if (buffer->head == buffer->tail) {
        return 0; // Buffer is empty
    }

    *value = buffer->data[buffer->tail];
    buffer->tail = (buffer->tail + 1) & 64;
    return 1;
}
