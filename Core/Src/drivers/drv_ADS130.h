#ifndef SRC_DRIVERS_DRV_ADS130_H_
#define SRC_DRIVERS_DRV_ADS130_H_


#include "../Footboard_main.h"

#define ADC_TIMEOUT 1000
#define NEWT 0

// Raw load cells values
typedef struct{

	int16_t		Front1_RAW,
				Front2_RAW,
				Middle1_RAW,
				Middle2_RAW,
				Middle3_RAW,
				Middle4_RAW,
				Back1_RAW,
				Back2_RAW;

}LC_data_RAW16;

typedef struct{

	int32_t		Front1_RAW,
				Front2_RAW,
				Middle1_RAW,
				Middle2_RAW,
				Middle3_RAW,
				Middle4_RAW,
				Back1_RAW,
				Back2_RAW;

}LC_data_RAW32;




// Converted LoadCell values
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

#define CELLCOUNT 1		 // If 8 LOAD CELLS : set to 1
#define M 50				 // MVA const
#define MAX_LOAD 50 	 // Maximal load on cells [kg]
#define RES 16 			 // Number of bits resolution ADC

void ADC_init(void);
void Update_ADC_data(void);
void Update_LC_data(void);
LC_data Get_LC_data(void);

extern SPI_HandleTypeDef hspi1;

#endif /* SRC_DRIVERS_DRV_ADS130_H_ */
