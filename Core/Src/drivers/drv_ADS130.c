#include "../drivers/drv_ADS130.h"
#include <math.h>

//ADC RX Buffer for ADC response
uint8_t ADC_buf[19];


//RAW values of cells
static uint16_t FRONT_LOAD1_RAW = 0; 	//Channel 1
static uint16_t FRONT_LOAD2_RAW = 0; 	//Channel 2
static uint16_t MIDDLE_LOAD1_RAW = 0;	//Channel 3
static uint16_t MIDDLE_LOAD2_RAW = 0;	//Channel 4
static uint16_t MIDDLE_LOAD3_RAW = 0;	//Channel 5
static uint16_t MIDDLE_LOAD4_RAW = 0;	//Channel 6
static uint16_t BACK_LOAD1_RAW = 0; 		//Channel 7
static uint16_t BACK_LOAD2_RAW = 0;		//Channel 8


//Status checks
static bool ADC_IN_FAULT = 0;


void ADC_init(void)
{
	//set CS high to avoid conflicts
	HAL_GPIO_WritePin(ADC_nCS_Pin, ADC_nCS_Pin, GPIO_PIN_SET);
	//Reset the device to default settings to be sure and then set to
	//Read data by command
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

	//Check Faults
	if((ADC_buf[])
	//Update RAW variables
	FRONT_LOAD1_RAW = ADC_buf[4] << 8 | ADC_buf[5];
	FRONT_LOAD2_RAW = ADC_buf[6] << 8 | ADC_buf[7];
	MIDDLE_LOAD1_RAW = ADC_buf[8] << 8 | ADC_buf[9];
	MIDDLE_LOAD2_RAW = ADC_buf[10] << 8 | ADC_buf[11];
	MIDDLE_LOAD3_RAW = ADC_buf[12] << 8 | ADC_buf[13];
	MIDDLE_LOAD4_RAW = ADC_buf[14] << 8 | ADC_buf[15];
	BACK_LOAD1_RAW = ADC_buf[16] << 8 | ADC_buf[17];
	BACK_LOAD2_RAW = ADC_buf[18] << 8 | ADC_buf[19];



}
