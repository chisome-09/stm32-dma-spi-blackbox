
#ifndef DEVICES_MPU_DRIVER_H_
#define DEVICES_MPU_DRIVER_H_
#include "log_packet.h"


//MPU Settings Register Map
typedef enum {
    MPU_REG_CONFIG        = 0x1A,
	MPU_REG_GYRO_CONFIG   = 0x1B,
	MPU_REG_ACCEL_CONFIG  = 0x1C,
	MPU_REG_ACCEL_CONFIG2 = 0x1D,
	MPU_REG_LP_ACCEL_ODR  = 0x1E,
	MPU_REG_WOM_THR       = 0x1F,
	MPU_REG_FIFO_EN       = 0x23,
	MPU_USER_CONTROL = 0x6A,
	WHO_AM_I = 0x75,
	MPU_PWR_MGMT_2 =0X6C,
	MPU_ACCEL_INTEL_CTRL = 0X69,
	MPU_SMPLRT_DIV = 0x19,
	MPU_PWR_MGMT_1 =0x6B,
	MPU_INT_PIN_CFG = 0X37,
	MPU_INT_ENABLE = 0x38,
	MPU_INT_STATUS = 0x3A,
	MPU_ACCEL_XOUT_H = 0x3B
} MPU_General_Reg;


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
		uint8_t lposc_clksel : 4;
		uint8_t reserved : 4;
	}config_settings;
}Low_Power_Accel;

//Wake On Motion Threshold
extern uint8_t wom_threshold;

//FIFO Enable
typedef union {
	uint8_t config;
	struct __attribute__((packed)){
		uint8_t slv_0 : 1;
		uint8_t slv_1: 1;
		uint8_t slv_2: 1;
		uint8_t accel: 1;
		uint8_t gyro_zout: 1;
		uint8_t gyro_yout: 1;
		uint8_t gyro_xout: 1;
		uint8_t temp_out: 1;
	}config_settings;
}FIFO_ENABLE;

//User Control
typedef union{
	uint8_t config;
	struct __attribute__((packed)){
		uint8_t sig_cond_rst : 1;
		uint8_t i2c_mst_rst : 1;
		uint8_t fifo_rst: 1;
		uint8_t dmp_rst: 1;
		uint8_t i2c_if_dis: 1;
		uint8_t i2c_mst_en:1;
		uint8_t fifo_en: 1;
		uint8_t dmp_en: 1;
	}config_settings;
}USER_CTRL;

//Accelerometer Interrupt Control
typedef union{
	uint8_t config;
	struct __attribute__((packed)){
		uint8_t reserved : 6;
		uint8_t accel_intel_mode:1 ;
		uint8_t accel_intel_en:1;
	}config_settings;
}ACCEL_INT_CTRL;

//Power Management 1 Settings
typedef union{
	uint8_t config;
	struct __attribute__((packed)){
		uint8_t clksel : 3;
		uint8_t tempdis: 1;
		uint8_t gyro_standby:1;
		uint8_t cycle: 1;
		uint8_t sleep:1;
		uint8_t device_reset: 1;
	}config_settings;
}PWR_MGMT_1;

//Power Management 2 Settings
typedef union{
	uint8_t config;
	struct __attribute__((packed)){
		uint8_t disable_zg:1;
		uint8_t disable_yg: 1;
		uint8_t disable_xg: 1;
		uint8_t disable_za: 1;
		uint8_t disable_ya:1;
		uint8_t disable_xa:1;
		uint8_t lp_wake_ctrl:2;
	}config_settings;
}PWR_MGMT_2;

//Interrupt Pin Configuration
typedef union{
	uint8_t config;
	struct __attribute__((packed)){
		uint8_t reserved:1;
		uint8_t bypass_en: 1;
		uint8_t fsync_int_mode_en: 1;
		uint8_t actl_fsync: 1;
		uint8_t int_anyrd_2clear:1;
		uint8_t latch_int_en:1;
		uint8_t open:1;
		uint8_t actl:1;
	}config_settings;
}INT_PIN_Typedef;

//Interrupt Enable
typedef union{
	uint8_t config;
	struct __attribute__((packed)){
		uint8_t raw_rdy_en:1;
		uint8_t reserved: 2;
		uint8_t fsync_int_en: 1;
		uint8_t fifo_overflow_en: 1;
		uint8_t reserved_2:1;
		uint8_t wom_en:1;
		uint8_t reserved_3:1;
	}config_settings;
}INT_EN_TypeDef;

//Interrupt Status
typedef union{
	uint8_t config;
	struct __attribute__((packed)){
		uint8_t raw_data_rdy_int:1;
		uint8_t dmp_int:1;
		uint8_t reserved:1;
		uint8_t fsync_int:1;
		uint8_t fifo_overflow_int:1;
		uint8_t reserved_2:1;
		uint8_t wom_int:1;
		uint8_t reserved_3:1;
	}config_settings;
}INT_STATUS_TypeDef;


//Driver Functions
uint16_t compute_DC_bias(void);//Gyroscope Offset Registers
uint8_t mpu_config(void);
uint8_t mpu_wom_mode(void);
uint8_t mpu_read(IMU_Sample_t* imu_sample);
uint8_t mpu_signal_reset(void);
uint8_t mpu_reset(void);
#endif /* DEVICES_MPU_DRIVER_H_ */
