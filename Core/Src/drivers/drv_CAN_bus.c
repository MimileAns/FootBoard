#include "drv_CAN_bus.h"

CAN_TxHeaderTypeDef CAN_TxHeader; // CAN transmit header data
CAN_RxHeaderTypeDef CAN_RxHeader; // CAN received header data

uint32_t CAN_TxMailbox;

uint8_t CAN_TxData[MESSAGE_SIZE_TX];
uint8_t CAN_RxData[MESSAGE_SIZE_RX];

// known device ID
#define JB202 0x202
#define JB203 0x203
// flag for which device sent the data
int JB202_data = 0;
int JB203_data = 0;


void CAN_bus_Init(void)
{
	// CAN bus filter configuration
	CAN_FilterTypeDef canfilterconfig;
	canfilterconfig.FilterActivation = CAN_FILTER_ENABLE;
	canfilterconfig.FilterBank = 0;
	canfilterconfig.FilterFIFOAssignment = CAN_RX_FIFO0;
	canfilterconfig.FilterIdHigh = 0;
	canfilterconfig.FilterIdLow = 0x0000;
	canfilterconfig.FilterMaskIdHigh = 0;
	canfilterconfig.FilterMaskIdLow = 0x0000;
	canfilterconfig.FilterMode = CAN_FILTERMODE_IDMASK;
	canfilterconfig.FilterScale = CAN_FILTERSCALE_32BIT;
	canfilterconfig.SlaveStartFilterBank = 0;
	HAL_CAN_ConfigFilter(&hcan1, &canfilterconfig);

	// Start CAN bus
	HAL_CAN_Start(&hcan1);

	// Activate CAN message notification
	HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);

	// set CAN arbitration and control field
	CAN_TxHeader.DLC = MESSAGE_SIZE_TX; 						// how many data bytes to be sent
	CAN_TxHeader.ExtId = 0;
	CAN_TxHeader.IDE = CAN_ID_STD;
	CAN_TxHeader.RTR = CAN_RTR_DATA;
	CAN_TxHeader.StdId = DEVICE_CAN_ID;
	CAN_TxHeader.TransmitGlobalTime = DISABLE;
	// Data to be sent
	CAN_TxData[0] = 0x00;
	CAN_TxData[1] = 0x00;
	CAN_TxData[2] = 0x00;
	CAN_TxData[3] = 0x00;
	//  CAN_TxData[4] = 0x00;
	//  CAN_TxData[5] = 0x00;
	//  CAN_TxData[6] = 0x00;
	//  CAN_TxData[7] = 0x00;

	// send the messages
	HAL_CAN_AddTxMessage(&hcan1, &CAN_TxHeader, CAN_TxData, &CAN_TxMailbox);
}

// CAN bus received message, Message pending callback to execute
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &CAN_RxHeader, CAN_RxData);
	// check size of received data and which device sent it
	if (CAN_RxHeader.DLC == MESSAGE_SIZE_RX && CAN_RxHeader.StdId == JB202)   						// how many data bytes to receive
	{
		JB202_data = 1;
		JB203_data = 0;
	}
	else if (CAN_RxHeader.DLC == MESSAGE_SIZE_RX && CAN_RxHeader.StdId == JB203)   						// how many data bytes to receive
	{
		JB203_data = 1;
		JB202_data = 0;
	}
}

