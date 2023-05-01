#include "drv_ADS130.h"
#include <math.h>
#include <stdbool.h>

// Filtering and conversion constants
const float FS = 3.3; // Upper boundary voltage Half-Bridge
const float FORCE_CST = 9.81; // Force conversion constant
static uint8_t counter = 0;

// ADC RX Buffer for ADC response
uint8_t ADC_buf[19];

static LC_data_RAW16 MVA_data[M];
static LC_data_RAW16 RAW_LoadCells;
static LC_data_RAW16 LoadCells;
static LC_data_RAW32 prev_LoadCells = {0};
static LC_data Load;

// Status checks
static bool ADC_FAULT = false;



//@brief : Initializes the ADS130E8 in read data by command mode and starts the conversions
//@param: None
//@return: None
//Note: This function needs to be called before initializing the IMU, as the default setting of the ADS causes the SPI
//bus to saturate

void ADC_init(void)
{
	// set CS high to avoid conflicts
	HAL_GPIO_WritePin(ADC_nCS_Pin, ADC_nCS_Pin, GPIO_PIN_SET);
	// Reset the device to default settings to be sure and then set to
	// Read data by command
	uint8_t write_reg[2];
	write_reg[0] = 0b00010001;
	HAL_GPIO_WritePin(ADC_nCS_GPIO_Port, ADC_nCS_Pin, GPIO_PIN_RESET); //CS
	HAL_SPI_Transmit(&hspi1, write_reg[0], 1, ADC_TIMEOUT/100);
	HAL_SPI_Transmit(&hspi1,0b00001000, 1, ADC_TIMEOUT/100);
	HAL_GPIO_WritePin(ADC_nCS_Pin, ADC_nCS_Pin, GPIO_PIN_SET);

}

//@brief: Updates the RAW ADC data of the LoadCells (by sending a read data request)
//@param: None
//@return: None

void Update_ADC_data(void)
{
	HAL_GPIO_WritePin(ADC_nCS_GPIO_Port, ADC_nCS_Pin, GPIO_PIN_RESET); //CS

	//Send a Conversion Read request and store data
	uint8_t write_reg[1];
	write_reg[0] = 0b00010010;
	HAL_SPI_TransmitReceive(&hspi1,write_reg,(uint8_t *)ADC_buf,sizeof(ADC_buf),ADC_TIMEOUT);
	HAL_GPIO_WritePin(ADC_nCS_GPIO_Port, ADC_nCS_Pin, GPIO_PIN_SET); //CS

	//Check ADC channel input faults
	if(ADC_buf[0] != 0xC0 || ADC_buf[1] != 0x00 || ADC_buf[2] != 0x00 )
	{
		ADC_FAULT = true;
	}

	//Update RAW variables
	RAW_LoadCells.Front1_RAW = ADC_buf[3] << 8 | ADC_buf[4];
	RAW_LoadCells.Front2_RAW = ADC_buf[5] << 8 | ADC_buf[6];
	RAW_LoadCells.Middle1_RAW = ADC_buf[7] << 8 | ADC_buf[8];
	RAW_LoadCells.Middle2_RAW = ADC_buf[9] << 8 | ADC_buf[10];
	RAW_LoadCells.Middle3_RAW = ADC_buf[11] << 8 | ADC_buf[12];
	RAW_LoadCells.Middle4_RAW = ADC_buf[13] << 8 | ADC_buf[14];
	RAW_LoadCells.Back1_RAW = ADC_buf[15] << 8 | ADC_buf[16];
	RAW_LoadCells.Back2_RAW = ADC_buf[17] << 8 | ADC_buf[18];
}

//@brief: Updates and Filters the load measured on the load cells.
//@param: None
//@return: None

void Update_LC_data(void)
{
	Update_ADC_data();

#if ADC_FILTER == MOVING_AVERAGE

	// Load buffer with RAW values
	MVA_data[counter] = RAW_LoadCells;

	// If counter up to MVA ratio, calculate Average LoadCell value
	if(counter%M == 0 && counter !=0)
	{
		LC_data_RAW32 SUM = {0};
		for(int i = 0; i<M; i++)
		{
			SUM.Front1_RAW = SUM.Front1_RAW + MVA_data[i].Front1_RAW;
			SUM.Front2_RAW = SUM.Front2_RAW + MVA_data[i].Front2_RAW;
			SUM.Middle1_RAW = SUM.Middle1_RAW + MVA_data[i].Middle1_RAW;
			SUM.Middle2_RAW = SUM.Middle2_RAW + MVA_data[i].Middle2_RAW;
			SUM.Middle3_RAW = SUM.Middle3_RAW + MVA_data[i].Middle3_RAW;
			SUM.Middle4_RAW = SUM.Middle4_RAW + MVA_data[i].Middle4_RAW;
			SUM.Back1_RAW = SUM.Back1_RAW + MVA_data[i].Back1_RAW;
			SUM.Back2_RAW = SUM.Back2_RAW + MVA_data[i].Back2_RAW;
		}

		LoadCells.Front1_RAW = SUM.Front1_RAW/M;
		LoadCells.Front2_RAW = SUM.Front2_RAW/M;
		LoadCells.Middle1_RAW = SUM.Middle1_RAW/M;
		LoadCells.Middle2_RAW = SUM.Middle2_RAW/M;
		LoadCells.Middle3_RAW = SUM.Middle3_RAW/M;
		LoadCells.Middle4_RAW = SUM.Middle4_RAW/M;
		LoadCells.Back1_RAW = SUM.Back1_RAW/M;
		LoadCells.Back2_RAW = SUM.Back2_RAW/M;
		counter = 0;
		return;
	}

#elif ADC_FILTER == USE_LPF

	//Calculate value from previous sample and conversion result
	LoadCells =	RAW_LoadCells - prev_LoadCells;

	// update previous readings
	prev_LoadCells.Front1 =	LoadCells;

#endif

counter++;

}

//@brief: Updates and sends back the converted values of the Load Cells
//@param: None
//@return: None

void Get_LC_data(void)
{
	Update_LC_data();
	//#TODO : Make conversion algorithm to get actual Load/Force on Load cells according to specific calibration

#if CONV == KG
	//Convert Digital measurements to Load metrics in [kg]
	Load.Front1 = LoadCells.Front1_RAW * (MAX_LOAD/2^RES);
	Load.Front2 = LoadCells.Front2_RAW * (MAX_LOAD/2^RES);
	Load.Middle1 = LoadCells.Middle1_RAW * (MAX_LOAD/2^RES);
	Load.Middle2 = LoadCells.Middle2_RAW * (MAX_LOAD/2^RES);
	Load.Middle3 = LoadCells.Middle3_RAW * (MAX_LOAD/2^RES);
	Load.Middle4 = LoadCells.Middle4_RAW * (MAX_LOAD/2^RES);
	Load.Back1 = LoadCells.Back1_RAW * (MAX_LOAD/2^RES);
	Load.Back2 = LoadCells.Back2_RAW * (MAX_LOAD/2^RES);
#elif CONV == NEWT
	//Convert Digital measurements to Load metrics in [kg]
	Load.Front1 = LoadCells.Front1_RAW * (MAX_LOAD/2^RES);
	Load.Front2 = LoadCells.Front2_RAW * (MAX_LOAD/2^RES);
	Load.Middle1 = LoadCells.Middle1_RAW * (MAX_LOAD/2^RES);
	Load.Middle2 = LoadCells.Middle2_RAW * (MAX_LAOD/2^RES);
	Load.Middle3 = LoadCells.Middle3_RAW * (MAX_LAOD/2^RES);
	Load.Middle4 = LoadCells.Middle4_RAW * (MAX_LAOD/2^RES);
	Load.Back1 = LoadCells.Back1_RAW * (MAX_LAOD/2^RES);
	Load.Back2 = LoadCells.Back2_RAW * (MAX_LAOD/2^RES);

	Load.Front1 = Load.Front1 * FORCE_CST;
	Load.Front2 = Load.Front2 * FORCE_CST;
	Load.Middle1 = Load.Middle1 * FORCE_CST;
	Load.Middle2 = Load.Middle2 * FORCE_CST;
	Load.Middle3 = Load.Middle3 * FORCE_CST;
	Load.Middle4 = Load.Middle4 * FORCE_CST;
	Load.Back1 = Load.Back1 * FORCE_CST;
	Load.Back2 = Load.Back2 * FORCE_CST;
#endif
}
