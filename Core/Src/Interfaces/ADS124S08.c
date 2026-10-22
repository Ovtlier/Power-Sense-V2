/*
 * ADS124S08.c
 *
 *  Created on: Jul 31, 2024
 *      Author: ethan
 */

#include "Interfaces/ADS124S08.h"
#include "Device/common.h"
#include "spi.h"

//#define SendStat 1 // Undefine if ADS is configured to Send Status at start of transmitting sample reading
//#define SendCRC 1 // Undefine if ADS is configured to Send CRC at end of transmitting sample reading

#ifndef ADC_CS_TIED_LOW
/*
 * Writes the nCS pin low and waits a while for the device to finish working before
 * handing control back to the caller for a SPI transfer.
 */
void ADS_SelectDevice(void) {
	HAL_GPIO_WritePin(ADS_CS_PORT, ADS_CS_PIN, GPIO_PIN_RESET);
}
/*
 * Pulls the nCS pin high. Performs no waiting.
*/
void ADS_ReleaseDevice(void) {
	HAL_GPIO_WritePin(ADS_CS_PORT, ADS_CS_PIN, GPIO_PIN_SET);
}
#define ADS_SELECT() ADS_SelectDevice()
#define ADS_RELEASE() ADS_ReleaseDevice()
#else
#define ADS_SELECT() // No operation or alternative behavior
#define ADS_RELEASE() // No operation or alternative behavior
#endif



void ADS_Write(uint8_t data) {
    if(HAL_SPI_Transmit(&hspi1,&data,1,2000)!=HAL_OK) {
        Error_Handler();
    };
}

uint8_t ADS_WriteReceive(uint8_t data) {
    uint8_t result;
    if(HAL_SPI_TransmitReceive(&hspi1,&data,&result,1,2000)!=HAL_OK) {
        Error_Handler();
    };
    return result;
}



void ADS_StartSampling() {
	ADS_SELECT();

	ADS_Write(START_OPCODE_MASK);

	ADS_RELEASE();
}

void ADS_StopSampling() {
	ADS_SELECT();

	ADS_Write(STOP_OPCODE_MASK);

	ADS_RELEASE();
}

void ADS_SetGPIOHigh(uint8_t pin) {
	ADS_SELECT();

	uint8_t GPIOData;

	GPIOData = ADS_ReadReg(GPIODAT_ADDR_MASK); // Read the current GPIO data from the reg

	ADS_WriteReg(GPIODAT_ADDR_MASK, GPIOData | (0x01 << pin) ); // Set the specified GPIO pin high while leaving the rest as they were read

	ADS_RELEASE();
}

void ADS_SetGPIOLow(uint8_t pin) {

	ADS_SELECT();

	uint8_t GPIOData;

	GPIOData = ADS_ReadReg(GPIODAT_ADDR_MASK); // Read the current GPIO data from the reg

	ADS_WriteReg(GPIODAT_ADDR_MASK, GPIOData & (0x0f ^ (0x01 << pin) ) ); // Set the specified GPIO pin low while leaving the rest as they were read

	ADS_RELEASE();
}

/* Read a register
   param1: address of register to read
   return: value of register
 */
uint8_t ADS_ReadReg(uint8_t reg){

	uint8_t regValue;

	ADS_SELECT();

	ADS_Write(REGRD_OPCODE_MASK | reg); // Tell ADS to READ and the register address to read
	ADS_Write(0x00); // Reading one register
	regValue = ADS_WriteReceive(NOP_OPCODE_MASK);

	ADS_RELEASE();

	return regValue;
}

/* Write to a register
   param1: address of register to write
   param2: data to write into the register
 */
void ADS_WriteReg(uint8_t reg, uint8_t data) {

	ADS_SELECT();

	ADS_Write(REGWR_OPCODE_MASK | reg); // Tell ADS to WRITE and the register address to write
	ADS_Write(0x00); // Writing one register
	ADS_Write(data); // Data to write to the register

	ADS_RELEASE();
}


/*
 * Returns a single reading
 * Can be used in either continuous conversion mode or after requesting a sample
 */
uint32_t ADS_ReadSample() {
	uint32_t result;
	ADS_SELECT();


	// Read data from ADS
	#ifdef SendStat
	uint8_t status = ADS_WriteReceive(NOP_OPCODE_MASK); // STATUS - optional
	#endif

	uint8_t reading1 = ADS_WriteReceive(NOP_OPCODE_MASK);
	uint8_t reading2 = ADS_WriteReceive(NOP_OPCODE_MASK);
	uint8_t reading3 = ADS_WriteReceive(NOP_OPCODE_MASK);

	#ifdef SendCRC
	uint8_t readingCRC = ADS_WriteReceive(NOP_OPCODE_MASK);
	// Do CRC Stuff

	#endif


	result = (reading1 << 16) | (reading2 << 8) | (reading3);

	ADS_RELEASE();

	return result;
}

/*
 * Sign-extends the 24-bit two's complement result from ADS_ReadSample() to int32_t.
 */
int32_t ADS_ReadSampleSigned(void) {
	uint32_t raw = ADS_ReadSample();
	return (int32_t)(raw << 8) >> 8;  // sign-extend 24-bit to 32-bit
}

/*
 * Reads multiple samples
 * Param1: Buffer that the readings will be placed into
 * Param2: Number of readings to put into the buffer
 */
void ADS_ReadSamples(uint32_t* results, uint16_t numSamples) {
	//TODO: Throw-away first reading of each channel

	while(numSamples--) {
		*results++ = ADS_ReadSample(); // Maybe replace with 'ADS_WriteReceive(NOP_OPCODE_MASK);' if it doesn't work?
	}
}

/*
 * Function used to request a sample when not in continuous conversion mode
 */
void ADS_RequestSample(){
	ADS_SELECT();

	ADS_Write(RDATA_OPCODE_MASK);

	ADS_RELEASE();
}


void ADS_init(void){
	ADS_SELECT();
	HAL_GPIO_WritePin(SPI_ADC_RST_GPIO_Port, SPI_ADC_RST_Pin, 0);
	HAL_Delay(1);
	HAL_GPIO_WritePin(SPI_ADC_RST_GPIO_Port, SPI_ADC_RST_Pin, 1);
	HAL_Delay(1);
	//ADS_WriteReg(ID_ADDR_MASK, 0b00001000); // ADS124S08
	ADS_WriteReg(STATUS_ADDR_MASK, 0b00000000); // Read only except for 7th bit
	ADS_WriteReg(INPMUX_ADDR_MASK, 0b00011100); // MuxN: AINCOM
	ADS_WriteReg(PGA_ADDR_MASK, 0b00000000); // PGA powered down and bypassed
	ADS_WriteReg(DATARATE_ADDR_MASK, 0b10010100); // GCHOP: ON; CLK: Internal; MODE: Cont; Filter: Low-lat; Data Rate: 5sps;
	ADS_WriteReg(REF_ADDR_MASK, 0b00111010); // Int. Volt. Ref: Always on; RefSel: Internal 2.5v; RefN_Buf: Disabled; RefP_Buf: Disabled; Reference Monitor: Disabled
	ADS_WriteReg(IDACMAG_ADDR_MASK, 0b00000000); // Excitation current: Off; Low-side power switch: Open; PGA Out rail mon: Disabled.
	ADS_WriteReg(IDACMUX_ADDR_MASK, 0b11111111); // Excitation Current output channel selection: Everything disabled
	ADS_WriteReg(VBIAS_ADDR_MASK, 0b00000000); // VBIAS Level: (AVDD+AVSS) / 2; VBIAS Channels: None;
	ADS_WriteReg(SYS_ADDR_MASK, 0b00010000); // SendStat: Disabled; CRC: Disabled; SPI Timeout: Disabled; Calibration Sample: 8 Samples; Sys Mon: Disabled
	ADS_WriteReg(OFCAL0_ADDR_MASK, 0b00000000);
	ADS_WriteReg(OFCAL1_ADDR_MASK, 0b00000000);
	ADS_WriteReg(OFCAL2_ADDR_MASK, 0b00000000);
	// FSCAL reset default = 0x400000 (gain 1x); do NOT write zeros — that zeroes the gain
	// ADS_WriteReg(FSCAL0_ADDR_MASK, 0b00000000);
	// ADS_WriteReg(FSCAL1_ADDR_MASK, 0b00000000);
	// ADS_WriteReg(FSCAL2_ADDR_MASK, 0b00000000);
	/* GPIO pins (AIN8-11/GPIO0-3) are tied to ISO_GND on Array board — not used.
	 * Leave at reset defaults: GPIOCON=0x00 (all analog), GPIODAT=don't care. */

	ADS_RELEASE();

	// The ADS124S08 will now be in standby mode

}






