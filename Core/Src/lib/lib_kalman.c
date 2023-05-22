#include "lib_kalman.h"

KalmanFilter FiltIMU;


// Initialize the Kalman filter
void KalmanFilter_Init(KalmanFilter* filter, float* initial_state, float* initial_covariance, float* process_noise, float* measurement_noise) {
    for (int i = 0; i < 6; i++) {
        filter->x[i] = initial_state[i];
    }
    for (int i = 0; i < 36; i++) {
        filter->P[i] = initial_covariance[i];
        filter->Q[i] = process_noise[i];
        filter->R[i] = measurement_noise[i];
    }
}

// Update the state and covariance predictions
void KalmanFilter_Predict(KalmanFilter* filter) {
    // Calculate the gyroscope delta
    float gyro_delta_x = filter->gyro_x - filter->gyro_x_prev;
    float gyro_delta_y = filter->gyro_y - filter->gyro_y_prev;
    float gyro_delta_z = filter->gyro_z - filter->gyro_z_prev;

    // Predict the next state based on gyroscope measurements
    filter->x[0] += gyro_delta_x;   // Modify this line for your prediction model
    filter->x[1] += gyro_delta_y;   // Modify this line for your prediction model
    filter->x[2] += gyro_delta_z;   // Modify this line for your prediction model

    // Update the covariance based on process noise
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
            if (i == j) {
                filter->P[i * 6 + j] += filter->Q[i * 6 + j];
            }
        }
    }

    // Save the current gyroscope measurements as previous measurements
    filter->gyro_x_prev = filter->gyro_x;
    filter->gyro_y_prev = filter->gyro_y;
    filter->gyro_z_prev = filter->gyro_z;
}

// Update the state and covariance estimates with a new measurement
void KalmanFilter_Update(KalmanFilter* filter) {
    // Calculate the acceleration delta
    float accel_delta_x = filter->accel_x - filter->accel_x_prev;
    float accel_delta_y = filter->accel_y - filter->accel_y_prev;
    float accel_delta_z = filter->accel_z - filter->accel_z_prev;

    // Update the state estimate based on acceleration measurements
    filter->x[3] = filter->accel_x + accel_delta_x;   // Modify this line for your update model
    filter->x[4] = filter->accel_y + accel_delta_y;   // Modify this line for your update model
    filter->x[5] = filter->accel_z + accel_delta_z;   // Modify this line for your update model

    // Calculate the Kalman gain for each measurement
    for (int i = 0; i < 6; i++) {
        filter->K[i] = filter->P[i * 6 + i] / (filter->P[i * 6 + i] + filter->R[i * 6 + i]);
    }

    // Update the state estimate based on acceleration measurements
    for (int i = 0; i < 6; i++) {
        filter->x[i] += filter->K[i] * (filter->x[i] - filter->x[i]);
    }

    // Update the covariance estimate based on acceleration measurements
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
            filter->P[i * 6 + j] *= (1.0 - filter->K[i] * filter->P[i * 6 + j]);
        }
    }

    // Save the current acceleration measurements as previous measurements
    filter->accel_x_prev = filter->accel_x;
    filter->accel_y_prev = filter->accel_y;
    filter->accel_z_prev = filter->accel_z;
}
