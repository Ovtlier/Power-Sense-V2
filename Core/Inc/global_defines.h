/*
 * global_defines.h
 *
 *	This file includes all of the global defines for this project, including but not limited to
 *	ADC channel names in the DMA buffer
 *
 *  Created on: Jul 28, 2025
 *      Author: Solarcar
 */

#ifndef INC_GLOBAL_DEFINES_H_
#define INC_GLOBAL_DEFINES_H_

#define PPCAT_NX(A, B) A##B
#define PPCAT(A, B) PPCAT_NX(A, B)
#define every_n_ms0(n, name1, name2) \
  static uint32_t name1 = 0; \
  int name2 = 0;\
  if (name1 + n < HAL_GetTick()){ \
    name1 = HAL_GetTick(); \
    name2=1; \
  } if (name2 == 1)


#define every_n_ms(n) every_n_ms0(n, PPCAT(ts_, __COUNTER__), PPCAT(a_, __COUNTER__))


//ADC DEFINES
#define UVPERCOUNT 50.36630

//ADC1 Defines
#define ADC1_CHANNELS_COUNT	13

#define VCAN_DIV_RAW		adc1_dma_results[0]
#define IBOARD_RAW			adc1_dma_results[1]
#define ICANOUT_RAW			adc1_dma_results[2]
#define VBOARD_DIV_RAW		adc1_dma_results[3]
#define IBOARD_REF_RAW		adc1_dma_results[4]
#define ICANOUT_REF_RAW		adc1_dma_results[5]
#define V5V_DIV_RAW			adc1_dma_results[6]
#define V3V3_DIV_RAW		adc1_dma_results[7]
#define ADC1_IN11_SPARE_RAW	adc1_dma_results[8]
#define ADC1_IN12_SPARE_RAW	adc1_dma_results[9]
#define ADC1_IN13_SPARE_RAW	adc1_dma_results[10]
#define ADC1_IN14_SPARE_RAW	adc1_dma_results[11]
#define VREFINT_RAW			adc1_dma_results[12]

#define VCAN_DIV		adc1_processed[0]
#define IBOARD			adc1_processed[1]
#define ICANOUT			adc1_processed[2]
#define VBOARD_DIV		adc1_processed[3]
#define IBOARD_REF		adc1_processed[4]
#define ICANOUT_REF		adc1_processed[5]
#define V5V_DIV			adc1_processed[6]
#define V3V3_DIV		adc1_processed[7]
#define ADC1_IN11_SPARE	adc1_processed[8]
#define ADC1_IN12_SPARE	adc1_processed[9]
#define ADC1_IN13_SPARE	adc1_processed[10]
#define ADC1_IN14_SPARE	adc1_processed[11]  // BOARD_TEMP0: NTC under relay (Array board)
#define VREFINT			adc1_processed[12]

	//ADC2 Defines
	//#define ADC2_CHANNELS_COUNT	0

	//#define ADC2_IN1_SPARE_RAW		adc2_dma_results[0]

	//#define ADC2_IN1_SPARE		adc2_processed[0]


//ADC3 Defines
#define ADC3_CHANNELS_COUNT 5

#define ADC3_IN9_SPARE_RAW	adc3_dma_results[0]
#define ADC3_IN10_SPARE_RAW	adc3_dma_results[1]
#define IARRAY1_RAW			adc3_dma_results[2]
#define ADC3_IN12_SPARE_RAW	adc3_dma_results[3]
#define VCANOUT_DIV_RAW		adc3_dma_results[4]

#define ADC3_IN9_SPARE	adc3_processed[0]
#define ADC3_IN10_SPARE	adc3_processed[1]
#define IARRAY1			adc3_processed[2]
#define ADC3_IN12_SPARE	adc3_processed[3]  // BOARD_TEMP1: NTC near ADC (Array board)
#define VCANOUT_DIV		adc3_processed[4]

/* External ADC (ADS124S08) results */
#include "Interfaces/ADS124S08.h"   // for ext_adc_channel_t / EXT_CH_COUNT

extern volatile int32_t ext_adc_results[EXT_CH_COUNT];
extern volatile float   ext_adc_volts[EXT_CH_COUNT];

#define V_ARRAY_OUT   ext_adc_volts[EXT_CH_V_ARRAY]
#define V_MPPT_IN     ext_adc_volts[EXT_CH_V_MPPT]
#define I_ARRAY_EXT   ext_adc_volts[EXT_CH_I_ARRAY]  // in Amps
#define V_3V3_ISO     ext_adc_volts[EXT_CH_V3V3]
#define V_5V_ISO      ext_adc_volts[EXT_CH_V5V]


#endif /* INC_GLOBAL_DEFINES_H_ */
