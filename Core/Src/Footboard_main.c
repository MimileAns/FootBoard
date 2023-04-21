#include "drivers/drv_CAN_bus.h"
#include "drivers/drv_ICM-42670.h"
#include "drivers/drv_ADS130.h"

#if !TX_BOARD
// global variable
float received_angle_202 = 0;
float received_angle_203 = 0;
#endif


// callback @ 1kHz
void timer_callback_routine(){
	HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_SET);
	Update_ADC_data();
#if WITH_IMU
	// get IMU
	//update_IMU_data();
	update_IMU_orient_data();
#endif

#if TX_BOARD
	#if !WITH_IMU
		// Packing data into 4 bytes
		union{
			float   angle;
			struct{
				uint8_t   byte0, byte1, byte2, byte3;
			};
		} data;
		// Get current angle from A5600
		data.angle = 0.0f;
		// Send data through CAN bus
		CAN_TxData[0] = data.byte0;
		CAN_TxData[1] = data.byte1;
		CAN_TxData[2] = data.byte2;
		CAN_TxData[3] = data.byte3;
		HAL_CAN_AddTxMessage(&hcan1, &CAN_TxHeader, CAN_TxData, &CAN_TxMailbox);
	#else
		// lossy compression of all float data to 16bit
		int16_t comp_enc_angle = (int16_t)(0.0f * 100);
		int16_t comp_orient_x  = (int16_t)(get_IMU_orient_x() * 100);
		int16_t comp_orient_y  = (int16_t)(get_IMU_orient_y() * 100);
		int16_t comp_orient_z  = (int16_t)(get_IMU_orient_z() * 100);
		// Send compressed data through CAN bus
		CAN_TxData[0]  = (comp_enc_angle >> 8) & 0xFF;	//byte 0-1 (encoder angle)
		CAN_TxData[1]  = (comp_enc_angle >> 0) & 0xFF;
		CAN_TxData[2]  = (comp_orient_x  >> 8) & 0xFF; 	//byte 2-3 (x axis angle)
		CAN_TxData[3]  = (comp_orient_x  >> 0) & 0xFF;
		CAN_TxData[4]  = (comp_orient_y  >> 8) & 0xFF; 	//byte 4-5 (y axis angle)
		CAN_TxData[5]  = (comp_orient_y  >> 0) & 0xFF;
		CAN_TxData[6]  = (comp_orient_z  >> 8) & 0xFF; 	//byte 6-7 (z axis angle)
		CAN_TxData[7]  = (comp_orient_z  >> 0) & 0xFF;

		HAL_CAN_AddTxMessage(&hcan1, &CAN_TxHeader, CAN_TxData, &CAN_TxMailbox);
	#endif
#else
	// check if CAN bus RX message available
	if (JB202_data){
		// unpacking data into float
		union{
			float   angle;
			struct{
				uint8_t   byte0, byte1, byte2, byte3;
			};
		} data_out;
		// copy data from RX_FIFO
		data_out.byte0 = CAN_RxData[0];
		data_out.byte1 = CAN_RxData[1];
		data_out.byte2 = CAN_RxData[2];
		data_out.byte3 = CAN_RxData[3];
		//Store received angle
		received_angle_202 = data_out.angle;
		// Reset CAN bus RX message available flag
		JB202_data = 0;
	}
	else if (JB203_data){
		// unpacking data into float
		union{
			float   angle;
			struct{
				uint8_t   byte0, byte1, byte2, byte3;
			};
		} data_out;
		// copy data from RX_FIFO
		data_out.byte0 = CAN_RxData[0];
		data_out.byte1 = CAN_RxData[1];
		data_out.byte2 = CAN_RxData[2];
		data_out.byte3 = CAN_RxData[3];
		//Store received angle
		received_angle_203 = data_out.angle;
		// Reset CAN bus RX message available flag
		JB203_data = 0;
	}

#endif
	HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_RESET);
}

void footboard_main(){
	// Start CAN bus
	CAN_bus_Init();
	// Initialise IMU
	init_IMU();
	// Initalise ADC
	ADC_init();
	// Start timer callback (frequency = 1kHz)
	HAL_TIM_Base_Start_IT(&htim2);

	while (1);
}

// Timer interrupt callback
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim){
	timer_callback_routine();
}
