#ifndef SRC_DRIVERS_DRV_ADS130_H_
#define SRC_DRIVERS_DRV_ADS130_H_


#include "../Footboard_main.h"

#define ADC_TIMEOUT 1000


#define FRONT_CELL1		1
#define FRONT_CELL2		2
#define MIDDLE_CELL1	3
#define MIDDLE_CELL2	4
#define MIDDLE_CELL3	5
#define MIDDLE_CELL4	6
#define BACK_CELL1 		7
#define BACK_CELL2 		8

// Raw load cells values

typedef struct{

	uint16_t	Front1_RAW,
				Front2_RAW,
				Middle1_RAW,
				Middle2_RAW,
				Middle3_RAW,
				Middle4_RAW,
				Back1_RAW,
				Back2_RAW;

}LC_data_RAW;


typedef struct{

	float	Front1,
			Front2,
			Middle1,
			Middle2,
			Middle3,
			Middle4,
			Back1,
			Back2;

}LC_data;

//if 8 LOAD CELLS : set to 1
#define CELLCOUNT 1


void ADC_init(void);
void Update_ADC_data(void);
void Update_LC_data(void);
LC_data Get_LC_data(void);

extern SPI_HandleTypeDef hspi1;

#endif /* SRC_DRIVERS_DRV_ADS130_H_ */
