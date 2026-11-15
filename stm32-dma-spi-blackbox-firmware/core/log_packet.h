#ifndef LOG_PACKET_H
#define LOG_PACKET_H
#include <stdint.h>


//IMU Sample
typedef struct __attribute__((packed)) {
    uint16_t dt_ms;
    int16_t ax,ay,az;
    int16_t gx,gy,gz;
} IMU_Sample_t;			/*14 bytes*/


//Flash page
typedef struct __attribute__((packed)){
	uint8_t written_indicator;	//1 byte
	uint8_t no_of_valid_readings;//1 byte
	uint16_t reserved;//2 bytes
	uint32_t no_of_pages;//4 bytes
	IMU_Sample_t IMU_Samples[17];//238 bytes
	uint8_t padding[6];//6 bytes
	uint32_t crc32;//4 bytes
} Log_Page_t;					/*256 bytes*/
#endif
