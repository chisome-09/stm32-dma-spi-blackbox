
#ifndef DEVICES_MPU_DRIVER_H_
#define DEVICES_MPU_DRIVER_H_

uint16_t compute_DC_bias();//Gyro Offset Registers

//MPU General Settings
typedef union {
	uint8_t config;
	struct __attribute__((packed)){
		uint8_t dlpf_cfg :3;
		uint8_t ext_sync_set : 3;
		uint8_t fifo_mode : 1;
		uint8_t reserved : 1;
	}config_settings;
}MPU_TypeDef;


//Gyroscope Settings
typedef union {
	uint8_t config;
	struct __attribute__((packed)){
		uint8_t fchoice_b :2;
		uint8_t reserved : 1;
		uint8_t gyro_full_scale_sel : 2;
		uint8_t z_gyro_test : 1;
		uint8_t y_gyro_test : 1;
		uint8_t x_gyro_test: 1;
	}config_settings;
}Gyro_TypeDef;

//Accelerometer Settings
typedef union {
	uint8_t config;
	struct __attribute__((packed)){
		uint8_t reserved :3;
		uint8_t accel_full_scale_sel : 2;
		uint8_t z_accel_test : 1;
		uint8_t y_accel_test : 1;
		uint8_t x_accel_test: 1;
	}config_settings;
}Accel_1_TypeDef;

//Accelerometer Additional Settings
typedef union {
	uint8_t config;
	struct __attribute__((packed)){
		uint8_t a_dlpf_cfg : 3;
		uint8_t accel_fchoice_b : 1;
		uint8_t reserved : 4;
	}config_settings;
}Accel_2_TypeDef;

//Low Power Accelerometer ODR Control
typedef union {
	uint8_t config;
	struct __attribute__((packed)){
		uint8_t lposk_clksel : 4;
		uint8_t reserved : 4;
	}config_settings;
}Low_Power_Accel;

//Wake On Motion Threshold
uint8_t wom_threshold;

//FIFO Enable
/*typedef union {
	uint8_t config;
	struct __attribute__((packed)){
		uint8_t slv_0 : 1;
		uint8_t slv_1: 1;
		uint8_t slv_2: 1;
		uint8_t
	}config_settings;
}FIFO_ENABLE;*/

#endif /* DEVICES_MPU_DRIVER_H_ */
