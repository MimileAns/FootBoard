#ifndef SRC_DRIVERS_DRV_CAN_BUS_H_
#define SRC_DRIVERS_DRV_CAN_BUS_H_

#include "../Footboard_main.h"

// Select board CAN ID
#define DEVICE_CAN_ID 0x202 // Left hip
//#define DEVICE_CAN_ID 0x203 // Left knee
//#define DEVICE_CAN_ID 0x302 // Right hip
//#define DEVICE_CAN_ID 0x303 // Right knee

void CAN_bus_Init(void);


extern CAN_HandleTypeDef hcan1;
extern CAN_TxHeaderTypeDef CAN_TxHeader; // CAN transmit header data
extern CAN_RxHeaderTypeDef CAN_RxHeader; // CAN received header data
extern uint32_t CAN_TxMailbox;
extern uint8_t CAN_TxData[];
extern uint8_t CAN_RxData[];
extern int JB202_data;
extern int JB203_data;

#endif /* SRC_DRIVERS_DRV_CAN_BUS_H_ */
