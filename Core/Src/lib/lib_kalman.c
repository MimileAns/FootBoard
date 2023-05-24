#include <math.h>
#include "lib_kalman.h"


// Initialize the Kalman orientation filter by setting initial values
void KalmanOrientationFilter_Init(KalmanHandle_Typedef* filter, float* initial_state, float* initial_covariance, float* process_noise, float* measurement_noise)
{
    for (int i = 0; i < 3; i++) {
        filter->x[i] = initial_state[i];
    }
    for (int i = 0; i < 9; i++) {
        filter->P[i] = initial_covariance[i];
        filter->Q[i] = process_noise[i];
        filter->R[i] = measurement_noise[i];
    }

}

void KalmanOrientationFilter_Update(KalmanHandle_Typedef* filter, float dt) {

    // Predict the next state based on gyroscope measurements
    filter->roll += filter->gyro_x * dt;
    filter->pitch += filter->gyro_y * dt;
    filter->yaw += filter->gyro_z * dt;

    // Update the covariance based on process noise
    for (int i = 0; i < 9; i++) {
        filter->P[i] += filter->R[i];
    }

    // Calculate the acceleration angles
    float accel_roll = atan2(filter->accel_y, filter->accel_z);
    float accel_pitch = atan2(-filter->accel_x, sqrt(filter->accel_y * filter->accel_y + filter->accel_z * filter->accel_z));

    // Update the state estimate based on acceleration measurements
    for (int i = 0; i < 3; i++) {
        filter->K[i] = filter->P[i] / (filter->P[i] + filter->Q[i]);
        filter->x[i] += filter->K[i] * (accel_roll - filter->x[i]);
    }

    // Update the covariance estimate based on acceleration measurements
    for (int i = 0; i < 9; i++) {
        filter->P[i] *= (1.0 - filter->K[i]);
    }

    // Update the estimated orientation
    filter->roll = filter->x[0];
    filter->pitch = filter->x[1];
    filter->yaw = filter->x[2];
}



