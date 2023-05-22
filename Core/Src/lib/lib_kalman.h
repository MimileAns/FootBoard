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

    float gyro_x_prev;   // Previous gyroscope x-axis measurement
    float gyro_y_prev;   // Previous gyroscope y-axis measurement
    float gyro_z_prev;   // Previous gyroscope z-axis measurement
    float accel_x_prev;  // Previous acceleration x-axis measurement
    float accel_y_prev;  // Previous acceleration y-axis measurement
    float accel_z_prev;  // Previous acceleration z-axis measurement

    float x[6];    // State vector
    float P[36];   // Error covariance matrix
    float Q[36];   // Process noise covariance
    float R[36];   // Measurement noise covariance
    float K[36];   // Kalman gain
} KalmanFilter;


#endif /* SRC_LIB_LIB_KALMAN_H_ */
