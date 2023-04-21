#include "../drivers/drv_ADS130.h"
#include <math.h>
#include <stdbool.h>

// ADC RX Buffer for ADC response
uint8_t ADC_buf[19];

LC_data_RAW RAW_LoadCells;
LC_data LoadCells;
LC_data prev_LoadCells;


// Status checks
static bool ADC_FAULT = false;

// Filtering and conversion constants
const int N = 2;

void ADC_init(void)
{
	// set CS high to avoid conflicts
	HAL_GPIO_WritePin(ADC_nCS_Pin, ADC_nCS_Pin, GPIO_PIN_SET);
	// Reset the device to default settings to be sure and then set to
	// Read data by command
	uint8_t write_reg[1];
	write_reg[0] = 0x11;
	HAL_GPIO_WritePin(ADC_nCS_GPIO_Port, ADC_nCS_Pin, GPIO_PIN_RESET); //CS
	HAL_SPI_Transmit(&hspi1, write_reg, 1, ADC_TIMEOUT/100);
	write_reg[0] = 0x06;
	HAL_SPI_Transmit(&hspi1, write_reg, 1, ADC_TIMEOUT/100);
	write_reg[0] = 0x11,
	HAL_SPI_Transmit(&hspi1, write_reg, 1, ADC_TIMEOUT/100);
	HAL_GPIO_WritePin(ADC_nCS_Pin, ADC_nCS_Pin, GPIO_PIN_SET);

}

void Update_ADC_data(void)
{
	HAL_GPIO_WritePin(ADC_nCS_GPIO_Port, ADC_nCS_Pin, GPIO_PIN_RESET); //CS

	//Send a Conversion Read request and store data
	uint8_t write_reg[1];
	write_reg[0] = 0x12;
	HAL_SPI_Transmit(&hspi1, write_reg, 1, ADC_TIMEOUT);
	HAL_SPI_Receive(&hspi1, (uint8_t *)ADC_buf, sizeof(ADC_buf), ADC_TIMEOUT);
	HAL_GPIO_WritePin(ADC_nCS_GPIO_Port, ADC_nCS_Pin, GPIO_PIN_SET); //CS

	//Check ADC channel input faults
	if(ADC_buf[0] != 0xC0 || ADC_buf[1] != 0x00 || ADC_buf[2] != 0x00 )
	{
		ADC_FAULT = true;
	}

	//Update RAW variables
	RAW_LoadCells.FRONT_LOAD1_RAW = ADC_buf[3] << 8 | ADC_buf[4];
	RAW_LoadCells.FRONT_LOAD2_RAW = ADC_buf[5] << 8 | ADC_buf[6];
	RAW_LoadCells.MIDDLE_LOAD1_RAW = ADC_buf[7] << 8 | ADC_buf[8];
	RAW_LoadCells.MIDDLE_LOAD2_RAW = ADC_buf[9] << 8 | ADC_buf[10];
	RAW_LoadCells.MIDDLE_LOAD3_RAW = ADC_buf[11] << 8 | ADC_buf[12];
	RAW_LoadCells.MIDDLE_LOAD4_RAW= ADC_buf[13] << 8 | ADC_buf[14];
	RAW_LoadCells.BACK_LOAD1_RAW = ADC_buf[15] << 8 | ADC_buf[16];
	RAW_LoadCells.BACK_LOAD2_RAW = ADC_buf[17] << 8 | ADC_buf[18];

}

void Update_LC_data(void)
{
	Update_ADC_data();

#if ADC_FILTER == USE_SAPMAF

	//Calculate value from previous sample and conversion result
	LoadCells.Front1 =	prev_LoadCells.Front1 + (RAW_LoadCells.FRONT_LOAD1_RAW - prev_LoadCells.Front1)/N;
	LoadCells.Front2 =	prev_LoadCells.Front2 + (RAW_LoadCells.FRONT_LOAD2_RAW - prev_LoadCells.Front2)/N;
	LoadCells.Middle1 =	prev_LoadCells.Middle1 + (RAW_LoadCells.MIDDLE_LOAD1_RAW - prev_LoadCells.Middle1)/N;
	LoadCells.Middle2 =	prev_LoadCells.Middle2 + (RAW_LoadCells.MIDDLE_LOAD2_RAW - prev_LoadCells.Middle2)/N;
	LoadCells.Middle3 =	prev_LoadCells.Middle3 + (RAW_LoadCells.MIDDLE_LOAD3_RAW - prev_LoadCells.Middle3)/N;
	LoadCells.Middle4 =	prev_LoadCells.Middle4 + (RAW_LoadCells.MIDDLE_LOAD4_RAW - prev_LoadCells.Middle4)/N;
	LoadCells.Back1 =	prev_LoadCells.Back1 + (RAW_LoadCells.BACK_LOAD1_RAW - prev_LoadCells.Back1)/N;
	LoadCells.Back2 =	prev_LoadCells.Back2 + (RAW_LoadCells.BACK_LOAD2_RAW - prev_LoadCells.Back2)/N;

	// update previous readings
	prev_LoadCells.Front1 =	LoadCells.Front1;
	prev_LoadCells.Front2 =	LoadCells.Front2;
	prev_LoadCells.Middle1 = LoadCells.Middle1;
	prev_LoadCells.Middle2 = LoadCells.Middle2;
	prev_LoadCells.Middle3 = LoadCells.Middle3;
	prev_LoadCells.Middle4 = LoadCells.Middle4;
	prev_LoadCells.Back1 = LoadCells.Back1;
	prev_LoadCells.Back2 = LoadCells.Back2;
#endif
	//#TODO : Make conversion algorithm to get actual Load/Force on Load cells according to specific calibration
}


LC_data Get_LC_data(void)
{
	Update_LC_data();
	return LoadCells;
}
