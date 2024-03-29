#include "drv_ADS130.h"
#include <math.h>
#include <stdbool.h>
#include <string.h>

// Filtering and conversion constants
const float FS = 3.3; // Upper boundary voltage Half-Bridge
const float FORCE_CST = 9.81; // Force conversion constant
const float alph = 0.98;
const float bet = 1 - alph;

// ADC buffer and addresses for SPI
uint8_t ADC_buf[19];

const uint8_t nRESET = 0x06;
const uint8_t SDATAC = 0x11;
const uint8_t START = 0x08;
const uint8_t STOP = 0x0A;
const uint8_t GET_DATA = 0x12;
const uint8_t CR1 = 0x41;
const uint8_t CR2 = 0x42;
const uint8_t CR3 = 0x43;
const uint8_t CHn = 0x45;
const uint8_t CR1_read = 0x21;
const uint8_t CR2_read = 0x22;
const uint8_t CR3_read = 0x23;
const uint8_t CHn_read = 0x25;


//Data structures
LC_data_RAW16 RAW_LoadCells;
LC_data LoadCells;
LC_data prev_LoadCells = {0};
LC_data Load;
LC_data_RAW32 SUM = {0};
static LC_data_RAW16 Init_LC;
// Status checks and help
static bool ADC_FAULT = false;
uint8_t counter;


//@brief : Initializes the ADS130E8 in read data by command mode and starts the conversions
//@param: None
//@return: None
//Note: This function needs to be called before initializing the IMU, as the default setting of the ADS causes the SPI
//bus to saturate

void ADC_init(void)
{
	HAL_Delay(5000);
	uint8_t write_reg[3] = {0};
	uint8_t txbuffer[10] = {0};
	// set CS high to avoid conflicts
	HAL_GPIO_WritePin(ADC_nCS_GPIO_Port, ADC_nCS_Pin, GPIO_PIN_SET);
	// Set up read data by command
	HAL_GPIO_WritePin(ADC_nCS_GPIO_Port, ADC_nCS_Pin, GPIO_PIN_RESET); //CS
	utils_DelayUs(2);
	HAL_SPI_Transmit(&hspi1,(uint8_t *)&nRESET, 1, ADC_TIMEOUT/100); //reset device
	HAL_Delay(1000); // Wait for device to stabalize

	HAL_SPI_Transmit(&hspi1,(uint8_t *)&SDATAC, 1, ADC_TIMEOUT/100); //stop read data continuously
	HAL_Delay(20);

 // Configure CR1
	write_reg[0] = CR1_read;
	write_reg[1] = 0x00;
	HAL_SPI_Transmit(&hspi1,write_reg, 2, ADC_TIMEOUT/100);
	HAL_SPI_Receive(&hspi1, ADC_buf, 1,ADC_TIMEOUT);
	HAL_Delay(50);

	write_reg[0] = CR1;
	write_reg[1] = 0x00;
	write_reg[2] = 0x01;
	HAL_SPI_Transmit(&hspi1,write_reg, 3, ADC_TIMEOUT/100);
	HAL_Delay(50);

	write_reg[0] = CR1_read;
	write_reg[1] = 0x00;
	HAL_SPI_Transmit(&hspi1,write_reg, 2, ADC_TIMEOUT/100);
	HAL_SPI_Receive(&hspi1, ADC_buf, 1,ADC_TIMEOUT);
	HAL_Delay(50);


// Configure CR2
	write_reg[0] = CR2;
	write_reg[1] = 0x00;
	write_reg[2] = 0x60;
	HAL_SPI_Transmit(&hspi1,write_reg, 3, ADC_TIMEOUT/100);
	HAL_Delay(50);

	write_reg[0] = CR2_read;
	write_reg[1] = 0x00;
	HAL_SPI_Transmit(&hspi1,write_reg,2, ADC_TIMEOUT/100);
	HAL_SPI_Receive(&hspi1,ADC_buf,1,ADC_TIMEOUT);
	HAL_Delay(50);

//Configure CR3
	write_reg[0] = CR3;
	write_reg[1] = 0x00;
	write_reg[2] = 0xCC;
	HAL_SPI_Transmit(&hspi1,write_reg, 3, ADC_TIMEOUT/100);
	HAL_Delay(50);

	write_reg[0] = CR3_read;
	write_reg[1] = 0x00;
	HAL_SPI_Transmit(&hspi1,write_reg,2, ADC_TIMEOUT/100);
	HAL_SPI_Receive(&hspi1,ADC_buf,1,ADC_TIMEOUT);
	HAL_Delay(50);

//Configure CHn

		txbuffer[0] = CHn;
		txbuffer[1] = 0x7;
		for (int i=2; i<10; i++)
		{
			txbuffer[i] = 0x20;
		}

		HAL_SPI_Transmit(&hspi1, txbuffer, 10, ADC_TIMEOUT);
		HAL_Delay(50);

		write_reg[0] = 0x25;
		write_reg[1] = 0x07;
		HAL_SPI_Transmit(&hspi1,write_reg,2, ADC_TIMEOUT/100);
		HAL_SPI_Receive(&hspi1,ADC_buf,7,ADC_TIMEOUT);
		HAL_Delay(50);

//Read a first conversion
		//HAL_SPI_TransmitReceive(&hspi1,(uint8_t *)&GET_DATA,ADC_buf,19,ADC_TIMEOUT);
		HAL_SPI_Transmit(&hspi1,(uint8_t *)&GET_DATA,1,ADC_TIMEOUT);
		HAL_SPI_Receive(&hspi1, ADC_buf,19,ADC_TIMEOUT);


//Start Conversions
	HAL_SPI_Transmit(&hspi1,(uint8_t *)&START, 1, ADC_TIMEOUT/100); // Start conversions
	HAL_Delay(10);
	HAL_GPIO_WritePin(ADC_nCS_GPIO_Port, ADC_nCS_Pin, GPIO_PIN_SET); // CS High
	HAL_Delay(100);

//Calulate first measure and substract to initial value 
	Update_Offset_data();


}

void Update_Offset_data(void)
{
HAL_GPIO_WritePin(ADC_nCS_GPIO_Port, ADC_nCS_Pin, GPIO_PIN_RESET); //CS

	//Send a Conversion Read request and store data
	//HAL_SPI_TransmitReceive(&hspi1,(uint8_t *)&GET_DATA,ADC_buf,19,ADC_TIMEOUT);
	HAL_SPI_Transmit(&hspi1,(uint8_t *)&GET_DATA,1,ADC_TIMEOUT);
	HAL_SPI_Receive(&hspi1, ADC_buf,19,ADC_TIMEOUT);

	HAL_GPIO_WritePin(ADC_nCS_GPIO_Port, ADC_nCS_Pin, GPIO_PIN_SET); //CS
	//Check ADC channel input faults
	if(ADC_buf[0] != 0xC0 || ADC_buf[1] != 0x00 || ADC_buf[2] != 0x00 )
	{
		ADC_FAULT = true;
	}
	//Update RAW variables
	//Update init value to offset
	Init_LC.Front1_RAW = (ADC_buf[3] << 8 | ADC_buf[4]);
	Init_LC.Front2_RAW = (ADC_buf[5] << 8 | ADC_buf[6]);
	Init_LC.Middle1_RAW = (ADC_buf[7] << 8 | ADC_buf[8]);
	Init_LC.Middle2_RAW = (ADC_buf[9] << 8 | ADC_buf[10]);
	Init_LC.Middle3_RAW = (ADC_buf[11] << 8 | ADC_buf[12]);
	Init_LC.Middle4_RAW = (ADC_buf[13] << 8 | ADC_buf[14]);
	Init_LC.Back1_RAW = (ADC_buf[15] << 8 | ADC_buf[16]);
	Init_LC.Back2_RAW = (ADC_buf[17] << 8 | ADC_buf[18]);
}


//@brief: Updates the RAW ADC data of the LoadCells (by sending a read data request)
//@param: None
//@return: None

void Update_ADC_data(void)
{
	HAL_GPIO_WritePin(ADC_nCS_GPIO_Port, ADC_nCS_Pin, GPIO_PIN_RESET); //CS

	//Send a Conversion Read request and store data
	//HAL_SPI_TransmitReceive(&hspi1,(uint8_t *)&GET_DATA,ADC_buf,19,ADC_TIMEOUT);
	HAL_SPI_Transmit(&hspi1,(uint8_t *)&GET_DATA,1,ADC_TIMEOUT);
	HAL_SPI_Receive(&hspi1, ADC_buf,19,ADC_TIMEOUT);

	HAL_GPIO_WritePin(ADC_nCS_GPIO_Port, ADC_nCS_Pin, GPIO_PIN_SET); //CS
	//Check ADC channel input faults
	if(ADC_buf[0] != 0xC0 || ADC_buf[1] != 0x00 || ADC_buf[2] != 0x00 )
	{
		ADC_FAULT = true;
	}
	//Update RAW variables
	RAW_LoadCells.Front1_RAW = (ADC_buf[3] << 8 | ADC_buf[4]) - Init_LC.Front1_RAW;
	RAW_LoadCells.Front2_RAW = (ADC_buf[5] << 8 | ADC_buf[6]) - Init_LC.Front2_RAW;
	RAW_LoadCells.Middle1_RAW = (ADC_buf[7] << 8 | ADC_buf[8]) - Init_LC.Middle1_RAW;
	RAW_LoadCells.Middle2_RAW = (ADC_buf[9] << 8 | ADC_buf[10]) - Init_LC.Middle2_RAW;
	RAW_LoadCells.Middle3_RAW = (ADC_buf[11] << 8 | ADC_buf[12]) - Init_LC.Middle3_RAW;
	RAW_LoadCells.Middle4_RAW = (ADC_buf[13] << 8 | ADC_buf[14]) - Init_LC.Middle4_RAW;
	RAW_LoadCells.Back1_RAW = (ADC_buf[15] << 8 | ADC_buf[16]) - Init_LC.Back1_RAW;
	RAW_LoadCells.Back2_RAW = (ADC_buf[17] << 8 | ADC_buf[18]) - Init_LC.Back2_RAW;


	//memset(ADC_buf,0,sizeof(ADC_buf));

}



//@brief: Updates and Filters the load measured on the load cells.
//@param: None
//@return: None

void Update_LC_data(void)
{
	Update_ADC_data();

#if ADC_FILTER == MOVING_AVERAGE

		counter++;
		Update_ADC_data();
		SUM.Front1_RAW = SUM.Front1_RAW + (uint32_t)RAW_LoadCells.Front1_RAW;
		SUM.Front2_RAW = SUM.Front2_RAW + (uint32_t)RAW_LoadCells.Front2_RAW;
		SUM.Middle1_RAW = SUM.Middle1_RAW + (uint32_t)RAW_LoadCells.Middle1_RAW;
		SUM.Middle2_RAW = SUM.Middle2_RAW + (uint32_t)(RAW_LoadCells.Middle2_RAW);
		SUM.Middle3_RAW = SUM.Middle3_RAW + (uint32_t)(RAW_LoadCells.Middle3_RAW);
		SUM.Middle4_RAW = SUM.Middle4_RAW + (uint32_t)(RAW_LoadCells.Middle4_RAW);
		SUM.Back1_RAW = SUM.Back1_RAW + (uint32_t)(RAW_LoadCells.Back1_RAW);
		SUM.Back2_RAW = SUM.Back2_RAW + (uint32_t)(RAW_LoadCells.Back2_RAW);


	if(counter%M == 0)
	{
	LoadCells.Front1 = (float)(SUM.Front1_RAW/M);
	LoadCells.Front2 = (float)(SUM.Front2_RAW/M);
	LoadCells.Middle1 = (float)(SUM.Middle1_RAW/M);
	LoadCells.Middle2 = (float)(SUM.Middle2_RAW/M);
	LoadCells.Middle3 = (float)(SUM.Middle3_RAW/M);
	LoadCells.Middle4 = (float)(SUM.Middle4_RAW/M);
	LoadCells.Back1 = (float)(SUM.Back1_RAW/M);
	LoadCells.Back2 = (float)(SUM.Back2_RAW/M);
	counter=0;
	memset(&SUM,0,sizeof(SUM));
	}

#elif ADC_FILTER == USE_LPF

	Update_ADC_data();

	//Calculate value from previous sample and conversion result
	LoadCells.Front1 = (float)(alph * RAW_LoadCells.Front1_RAW) + bet * prev_LoadCells.Front1;
	LoadCells.Front2 = (float)(alph * RAW_LoadCells.Front2_RAW) + bet * prev_LoadCells.Front2;
	LoadCells.Middle1 = (float)(alph * RAW_LoadCells.Middle1_RAW) + bet * prev_LoadCells.Middle1;
	LoadCells.Middle2 = (float)(alph * RAW_LoadCells.Middle2_RAW) + bet * prev_LoadCells.Middle2;
	LoadCells.Middle3 = (float)(alph * RAW_LoadCells.Middle3_RAW) + bet * prev_LoadCells.Middle3;
	LoadCells.Middle4 = (float)(alph * RAW_LoadCells.Middle4_RAW) + bet * prev_LoadCells.Middle4;
	LoadCells.Back1 = (float)(alph * RAW_LoadCells.Back1_RAW) + bet * prev_LoadCells.Back1;
	LoadCells.Back2 = (float)(alph * RAW_LoadCells.Back2_RAW) + bet * prev_LoadCells.Back2;
	// update previous readings
	prev_LoadCells.Front1 =	LoadCells.Front1;
	prev_LoadCells.Front2 = LoadCells.Front2;
	prev_LoadCells.Middle1 = LoadCells.Middle1;
	prev_LoadCells.Middle2 = LoadCells.Middle2;
	prev_LoadCells.Middle3 = LoadCells.Middle3;
	prev_LoadCells.Middle4 = LoadCells.Middle4;
	prev_LoadCells.Back1 = LoadCells.Back1;
	prev_LoadCells.Back2 = LoadCells.Back2;

#elif ADC_FILTER == ADC_FILTER

	Update_ADC_data();

	LoadCells.Front1=  RAW_LoadCells.Front1_RAW;
	LoadCells.Front2 =  RAW_LoadCells.Front2_RAW;
	LoadCells.Middle1 =  RAW_LoadCells.Middle1_RAW;
	LoadCells.Middle2 = RAW_LoadCells.Middle2_RAW;
	LoadCells.Middle3 = RAW_LoadCells.Middle3_RAW;
	LoadCells.Middle4 = RAW_LoadCells.Middle4_RAW;
	LoadCells.Back1 = RAW_LoadCells.Back1_RAW;
	LoadCells.Back2 = RAW_LoadCells.Back2_RAW;

#endif

}

//@brief: Updates and sends back the converted values of the Load Cells
//@param: None
//@return: None

LC_data Get_LC_data(void)
{

	//Convert Digital measurements to Load metrics in [kg]
	Load.Front1 = LoadCells.Front1 * (MAX_LOAD/pow(2,RES));
	Load.Front2 = LoadCells.Front2 * (MAX_LOAD/pow(2,RES));
	Load.Middle1 = LoadCells.Middle1 * (MAX_LOAD/pow(2,RES));
	Load.Middle2 = LoadCells.Middle2 * (MAX_LOAD/pow(2,RES));
	Load.Middle3 = LoadCells.Middle3 * (MAX_LOAD/pow(2,RES));
	Load.Middle4 = LoadCells.Middle4 * (MAX_LOAD/pow(2,RES));
	Load.Back1 = LoadCells.Back1 * (MAX_LOAD/pow(2,RES));
	Load.Back2 = LoadCells.Back2 * (MAX_LOAD/pow(2,RES));

#if CONV == NEWT
	Load.Front1 = Load.Front1 * FORCE_CST;
	Load.Front2 = Load.Front2 * FORCE_CST;
	Load.Middle1 = Load.Middle1 * FORCE_CST;
	Load.Middle2 = Load.Middle2 * FORCE_CST;
	Load.Middle3 = Load.Middle3 * FORCE_CST;
	Load.Middle4 = Load.Middle4 * FORCE_CST;
	Load.Back1 = Load.Back1 * FORCE_CST;
	Load.Back2 = Load.Back2 * FORCE_CST;
#endif

	return Load;
}

