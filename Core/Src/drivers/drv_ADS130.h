#ifndef SRC_DRIVERS_DRV_ADS130_H_
#define SRC_DRIVERS_DRV_ADS130_H_


#include "../Footboard_main.h"

#define ADC_TIMEOUT 1000


#define FRONT_CELL1		1
#define FRONT_CELL1		2
#define MIDDLE_CELL1	3
#define MIDDLE_CELL2	4
#define MIDDLE_CELL3	5
#define MIDDLE_CELL4	6
#define BACK_CELL1 		7
#define BACK_CELL2 		8


//if 8 LOAD CELLS : set to 1
#define CELLCOUNT 1


void ADC_init(void);
void Update_ADC_data(void);

extern SPI_HandleTypeDef hspi1;

#endif /* SRC_DRIVERS_DRV_ADS130_H_ */
