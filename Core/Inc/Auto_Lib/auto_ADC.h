/*
 * auto_ADC.h
 *
 *  Created on: Jun 9, 2024
 *      Author: ethan
 *
 *	Heavily Modified by Peter
 *		 Jun 28, 2025
 */

#ifndef INC_SYSTEM_ADC_H_
#define INC_SYSTEM_ADC_H_

#include <stdbool.h>
#include "structs.h"
#include "main.h"
#include "global_defines.h"

typedef struct {
	volatile bool ADC1Pending : 1;
	volatile bool ADC2Pending : 1;
	volatile bool ADC3Pending : 1;
} intADC_state_t;



double parse_raw_calib(double raw, raw_calib cfg);

void autoADC_init();
void autoADC_flag_for_handle(ADC_HandleTypeDef *hadc);
void parse_adc();

/* Global Variables */
extern volatile intADC_state_t intADC_state;
extern raw_calib adc1_cfg[ADC1_CHANNELS_COUNT];
extern raw_calib adc3_cfg[ADC3_CHANNELS_COUNT];

#endif /* INC_SYSTEM_ADC_H_ */
