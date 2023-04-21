#include "drv_ICM-42670.h"
#include <math.h>

// IMU data
vectors accel;
vectors gyro;
// IMU orientation data
vectors orientation;
vectors prev_orientation;

vectors accel_orient;
vectors gyro_orient;
vectors prev_accel_orient;
vectors prev_gyro_orient;

//complementary filter coefficient
const float alpha = 0.98;
const float beta = 1 - alpha;

// read only register address (x & 0x80) is to add read bit to MSB
const uint8_t TEMP_DATA1 	= 0x09 | 0x80;		// TEMP_DATA[15:8]
const uint8_t TEMP_DATA0	= 0x0A | 0x80;		// TEMP_DATA[7:0]
const uint8_t ACCEL_DATA_X1 = 0x0B | 0x80;		// ACCEL_DATA_X[15:8]
const uint8_t ACCEL_DATA_X0 = 0x0C | 0x80;		// ACCEL_DATA_X[7:0]
const uint8_t ACCEL_DATA_Y1 = 0x0D | 0x80;		// ACCEL_DATA_Y[15:8]
const uint8_t ACCEL_DATA_Y0 = 0x0E | 0x80;		// ACCEL_DATA_Y[7:0]
const uint8_t ACCEL_DATA_Z1 = 0x0F | 0x80;		// ACCEL_DATA_Z[15:8]
const uint8_t ACCEL_DATA_Z0 = 0x10 | 0x80;		// ACCEL_DATA_Z[7:0]
const uint8_t GYRO_DATA_X1 	= 0x11 | 0x80;		// GYRO_DATA_X[15:8]
const uint8_t GYRO_DATA_X0 	= 0x12 | 0x80;		// GYRO_DATA_X[7:0]
const uint8_t GYRO_DATA_Y1 	= 0x13 | 0x80;		// GYRO_DATA_Y[15:8]
const uint8_t GYRO_DATA_Y0	= 0x14 | 0x80;		// GYRO_DATA_Y[7:0]
const uint8_t GYRO_DATA_Z1 	= 0x15 | 0x80;		// GYRO_DATA_Z[15:8]
const uint8_t GYRO_DATA_Z0 	= 0x16 | 0x80;		// GYRO_DATA_Z[7:0]

// define write register
const uint8_t PWR_MGMT0 	= 0x1F | 0x00;		// GYRO_DATA_Z[7:0]

uint8_t spi_buf[12];

void init_IMU(){

	// set CS to high
	HAL_GPIO_WritePin(IMU_nCS_GPIO_Port, IMU_nCS_Pin, GPIO_PIN_SET);
	// turn accelerometer and gyroscope ON in low-noise mode
	uint8_t write_reg[2];
	write_reg[0] = PWR_MGMT0; 	// address
	write_reg[1] = 0b01001111;	// set accel. and gyro. in Low noise mode
	HAL_GPIO_WritePin(IMU_nCS_GPIO_Port, IMU_nCS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, write_reg, 2, 10);
	HAL_GPIO_WritePin(IMU_nCS_GPIO_Port, IMU_nCS_Pin, GPIO_PIN_SET);

	// initialise vectors
	accel.x = 0.0;
	accel.y = 0.0;
	accel.z = 0.0;

	gyro.x = 0.0;
	gyro.y = 0.0;
	gyro.z = 0.0;

	orientation.x = 0.0;
	orientation.y = 0.0;
	orientation.z = 0.0;

	prev_orientation.x = 0.0;
	prev_orientation.y = 0.0;
	prev_orientation.z = 0.0;

	accel_orient.x = 0.0;
	accel_orient.y = 0.0;
	accel_orient.z = 0.0;

	gyro_orient.x = 0.0;
	gyro_orient.y = 0.0;
	gyro_orient.z = 0.0;

	prev_accel_orient.x = 0.0;
	prev_accel_orient.y = 0.0;
	prev_accel_orient.z = 0.0;

	prev_gyro_orient.x = 0.0;
	prev_gyro_orient.y = 0.0;
	prev_gyro_orient.z = 0.0;

}

void update_IMU_data(){
	// request data from accelerometer
	//HAL_GPIO_WritePin(ADC_nCS_GPIO_Port, ADC_nCS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(IMU_nCS_GPIO_Port, IMU_nCS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, (uint8_t *)&ACCEL_DATA_X1, 1, 1000);
	HAL_SPI_Receive(&hspi1, (uint8_t *)spi_buf, 12, 1000);
	HAL_GPIO_WritePin(IMU_nCS_GPIO_Port, IMU_nCS_Pin, GPIO_PIN_SET);
	// parse data to accelerometer vector
	accel.x =  (float)(int16_t)(spi_buf[0] << 8 | spi_buf[1])  / FS11B; // FS11B used for 16g scale
	accel.y =  (float)(int16_t)(spi_buf[2] << 8 | spi_buf[3])  / FS11B;
	accel.z = -(float)(int16_t)(spi_buf[4] << 8 | spi_buf[5])  / FS11B;
	// parse data to gyroscope vector
	gyro.x = -(float)(int16_t)(spi_buf[6]  << 8 | spi_buf[7])  / 16.4; // 16.4 used for 200dps scale
	gyro.y = -(float)(int16_t)(spi_buf[8]  << 8 | spi_buf[9])  / 16.4;
	gyro.z =  (float)(int16_t)(spi_buf[10] << 8 | spi_buf[11]) / 16.4;
}

void update_IMU_orient_data(){
	// update IMU_data
	update_IMU_data();
#if ORIENTATION_FILTER == USE_FILTER_COMPLEMENTARY
	// orientation from accelerometer
	accel_orient.x =  atan2(accel.y, accel.z) * 180 / M_PI;
	accel_orient.y = -atan2(accel.x, accel.z) * 180 / M_PI;
	accel_orient.z =  atan2(accel.x, accel.y) * 180 / M_PI;

	// update previous values
	prev_accel_orient.x = accel_orient.x;
	prev_accel_orient.y = accel_orient.y;
	prev_accel_orient.z = accel_orient.z;

	// orientation from gyroscope
	gyro_orient.x = CALL_BACK_DT * gyro.x + prev_gyro_orient.x;
	gyro_orient.y = CALL_BACK_DT * gyro.y + prev_gyro_orient.y;
	gyro_orient.z = CALL_BACK_DT * gyro.z + prev_gyro_orient.z;

	// complementary filter
	orientation.x = alpha * (prev_orientation.x + CALL_BACK_DT * gyro.x) + beta * accel_orient.x;
	orientation.y = alpha * (prev_orientation.y + CALL_BACK_DT * gyro.y) + beta * accel_orient.y;
	orientation.z = alpha * (prev_orientation.z + CALL_BACK_DT * gyro.z) + beta * accel_orient.z;

	// update previous values
	prev_accel_orient.x = accel_orient.x;
	prev_accel_orient.y = accel_orient.y;
	prev_accel_orient.z = accel_orient.z;

	prev_gyro_orient.x = gyro_orient.x;
	prev_gyro_orient.y = gyro_orient.y;
	prev_gyro_orient.z = gyro_orient.z;

	prev_orientation.x = orientation.x;
	prev_orientation.y = orientation.y;
	prev_orientation.z = orientation.z;


#elif ORIENTATION_FILTER == ORIENTATION_FILTER
	// parse data orientation vector
	orientation.x = 0.0;
	orientation.y = 0.0;
	orientation.z = 0.0;
#endif
}

float get_IMU_orient_x(){
	return orientation.x;
}
float get_IMU_orient_y(){
	return orientation.y;
}
float get_IMU_orient_z(){
	return orientation.z;
}
