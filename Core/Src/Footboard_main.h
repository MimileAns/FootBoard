#ifndef SRC_FOOTBOARD_MAIN_H_
#define SRC_FOOTBOARD_MAIN_H_

#include "main.h"

#define CALL_BACK_DT 0.001f // callback period in [s]

// select what filter to use for IMU
#define ORIENTATION_FILTER USE_FILTER_COMPLEMENTARY
//#define ORIENTATION_FILTER ORIENTATION_FILTER

// select which filter to use for LC ADC
#define ADC_FILTER USE_SAPMAF // if using a Self-Adapting Pseudo-Moving Average Filter
//#define ADC_FIlTER USE_NO_FILTER // no filter

#define USE_FILTER_COMPLEMENTARY 1
#define USE_FILTER_KALMAN 0

#define TX_BOARD 1 // always set to 1 if we want the board to transmit on the CAN bus
#define WITH_IMU 1 // set to 0 for board without IMU, set to 1 for board with IMU

#if WITH_IMU
#define MESSAGE_SIZE_TX 8
#define MESSAGE_SIZE_RX 8
#else
#define MESSAGE_SIZE_TX 4
#define MESSAGE_SIZE_RX 4
#endif


void footboard_main();
void timer_callback_routine();

extern TIM_HandleTypeDef htim2;

#endif /* SRC_FOOTBOARD_MAIN_H_ */
