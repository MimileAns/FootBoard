#ifndef SRC_LIB_LIB_KALMAN_H_
#define SRC_LIB_LIB_KALMAN_H_

#include <math.h>

typedef struct {

    float gyro_x;    // Gyroscope x-axis measurement
    float gyro_y;    // Gyroscope y-axis measurement
    float gyro_z;    // Gyroscope z-axis measurement
    float accel_x;   // Acceleration x-axis measurement
    float accel_y;   // Acceleration y-axis measurement
    float accel_z;   // Acceleration z-axis measurement

    float roll;      // Estimated roll angle
    float pitch;     // Estimated pitch angle
    float yaw;       // Estimated yaw angle

    float x[3];      // State vector (roll, pitch, yaw)
    float P[9];      // Error covariance matrix
    float R[9];      // Process noise covariance
    float Q[9];      // Measurement noise covariance
    float K[9];      // Kalman gain

} KalmanHandle_Typedef;

void KalmanOrientationFilter_Init(KalmanHandle_Typedef* filter,
								  float* initial_state,
								  float* initial_covariance,
								  float* process_noise,
								  float* measurement_noise);

void KalmanOrientationFilter_Update(KalmanHandle_Typedef* filter, float dt);
#endif /* SRC_LIB_LIB_KALMAN_H_ */
