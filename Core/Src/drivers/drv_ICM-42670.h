#ifndef SRC_DRIVERS_DRV_ICM_42670_H_
#define SRC_DRIVERS_DRV_ICM_42670_H_

#include "../Footboard_main.h"

#define FS11B 2048

#define ACC_CAL_OFFSET_X 0.000910f
#define ACC_CAL_OFFSET_Y 0.004500f
#define ACC_CAL_OFFSET_Z 0.005500f


#define GYRO_CAL_OFFSET_X -0.00537f
#define GYRO_CAL_OFFSET_Y -0.00341f
#define GYRO_CAL_OFFSET_Z -0.00230f

typedef struct
{
	float 	x,
			y,
			z;

} vectors;

typedef enum
{
	_250dps,
	_500dps,
	_1000dps,
	_2000dps
} gyro_full_scale;

typedef enum
{
	_2g,
	_4g,
	_8g,
	_16g
} accel_full_scale;

extern SPI_HandleTypeDef hspi1;

void init_IMU();
void update_IMU_data();
void update_IMU_orient_data();
float get_IMU_orient_x();
float get_IMU_orient_y();
float get_IMU_orient_z();

#endif /* SRC_DRIVERS_DRV_ICM_42670_H_ */
