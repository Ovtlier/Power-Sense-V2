/*
 * structs.h
 *
 *  Created on: Jul 28, 2025
 *      Author: Peter
 */

#include "Auto_lib/auto_ADC.h"
#include "adc.h"
#include "structs.h"
#include "global_defines.h"



volatile intADC_state_t intADC_state;
extern uint32_t adc1_dma_results[ADC1_CHANNELS_COUNT];
extern double adc1_processed[ADC1_CHANNELS_COUNT];
//  extern uint32_t adc2_dma_results[ADC2_CHANNELS_COUNT];
//  extern double adc2_processed[ADC2_CHANNELS_COUNT];
extern uint32_t adc3_dma_results[ADC3_CHANNELS_COUNT];
extern double adc3_processed[ADC3_CHANNELS_COUNT];


void autoADC_init() {

	HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
	HAL_ADCEx_Calibration_Start(&hadc3, ADC_SINGLE_ENDED);

	HAL_Delay(10);

	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc1_dma_results, ADC1_CHANNELS_COUNT);
	HAL_ADC_Start_DMA(&hadc3, (uint32_t*)adc3_dma_results, ADC3_CHANNELS_COUNT);

	return;
}


// Called from within ISR usually
void autoADC_flag_for_handle(ADC_HandleTypeDef *hadc) {
	if(hadc == &hadc1)
	{
		intADC_state.ADC1Pending = true;
	} //else if (hadc == &hadc2)
//	{
//		intADC_state.ADC2Pending = true;
//	}
	else if (hadc == &hadc3)
	{
		intADC_state.ADC3Pending = true;
	}
	return;
}

raw_calib adc1_cfg[ADC1_CHANNELS_COUNT] = {
    {.mode = calib_multiplier, .multiplier = 19.89763, .offset = 0}, //VCAN_DIV
    {.mode = calib_multiplier, .multiplier = 250, .offset = 1.654},   //IBOARD
    {.mode = calib_multiplier, .multiplier = 800, .offset = 1.10875}, //ICANOUT
    {.mode = calib_multiplier, .multiplier = 19.89763, .offset = 0}, //VBOARD_DIV
    {.mode = calib_multiplier, .multiplier = 250, .offset = 0}, //IBOARD_REF
    {.mode = calib_multiplier, .multiplier = 800, .offset = 0}, //ICANOUT_REF
    {.mode = calib_multiplier, .multiplier = 19.89763, .offset = 0}, //V5V_DIV
    {.mode = calib_multiplier, .multiplier = 19.89763, .offset = 0}, //V3V3_DIV
    {.mode = calib_multiplier, .multiplier = 1, .offset = 0}, //ADC1_IN11_SPARE
    {.mode = calib_multiplier, .multiplier = 1, .offset = 0}, //ADC1_IN12_SPARE
    {.mode = calib_multiplier, .multiplier = 1, .offset = 0}, //ADC1_IN13_SPARE
    {.mode = calib_multiplier, .multiplier = 1, .offset = 0}, //ADC1_IN14_SPARE
    {.mode = calib_multiplier, .multiplier = 1, .offset = 0}, //VREFINT
};

	//raw_calib adc2_cfg[ADC2_CHANNELS_COUNT] = {
	//    {.mode = calib_multiplier, .multiplier = 1, .offset = 0},
	//};

raw_calib adc3_cfg[ADC3_CHANNELS_COUNT] = {
    {.mode = calib_multiplier, .multiplier = 1, .offset = 0}, //ADC3_IN9_SPARE
    {.mode = calib_multiplier, .multiplier = 1, .offset = 0}, //ADC3_IN10_SPARE
    {.mode = calib_multiplier, .multiplier = 60.975, .offset = 1.65176}, //IARRAY1
    {.mode = calib_multiplier, .multiplier = 1, .offset = 0}, //ADC3_IN12_SPARE
    {.mode = calib_multiplier, .multiplier = 19.89763, .offset = 0}, //VCANOUT_DIV
};

double parse_raw_calib(double raw, raw_calib cfg) {
  double ret = 0;
	switch (cfg.mode) {
  case calib_none:
    return (double)raw;
    break;
  case calib_multiplier:
    ret = (raw - cfg.offset);
    ret = ret * cfg.multiplier;
	return ret;
    break;
  case calib_remap:
    double ret =
        cfg.remap_low_out + ((double)raw - cfg.remap_low_in) *
                                (cfg.remap_high_out - cfg.remap_low_out) /
                                (cfg.remap_high_in - cfg.remap_low_in);
    if (cfg.clamp) {
      if (ret > cfg.remap_high_out)
        ret = cfg.remap_high_out;
      if (ret < cfg.remap_low_out)
        ret = cfg.remap_low_out;
    }
    return ret;

  default:
	 return (double)raw;
    break;
  }
}


void parse_adc() {

	double intermediate_value=0;

	  if(intADC_state.ADC1Pending){

	  for (size_t i = 0; i < ADC1_CHANNELS_COUNT; i++) { //loop through all the channels
		  intermediate_value = UVPERCOUNT;
		  intermediate_value = intermediate_value/1000000;
		  intermediate_value = intermediate_value * adc1_dma_results[i];
		  adc1_processed[i] = parse_raw_calib(intermediate_value, adc1_cfg[i]);
	  }
	  intADC_state.ADC1Pending = false;
  	  }

//		if(intADC_state.ADC2Pending){
//
//		  for (size_t i = 0; i < ADC2_CHANNELS_COUNT; i++) { //loop through all the channels
//			  adc2_processed[i] = parse_raw_calib(adc2_dma_results[i] * VPERCOUNT, adc2_cfg[i]);
//		  }
//
//		  intermediate_value = parse_raw_calib(vrefint_cal_val * VPERCOUNT, adc1_cfg[ADC1_CHANNELS_COUNT-1]);
//		  vref_scale = intermediate_value / VREFINT;
//
//		  for (size_t i = 0; i < ADC2_CHANNELS_COUNT; i++) { //Now VREF corrections
//
//			  adc2_processed[i] = vref_scale * adc2_processed[i];
//		  }
//  	  	  intADC_state.ADC2Pending = false;
//		}
  	  if(intADC_state.ADC3Pending){

  	  for (size_t i = 0; i < ADC3_CHANNELS_COUNT; i++) { //loop through all the channels
		  intermediate_value = UVPERCOUNT;
		  intermediate_value = intermediate_value/1000000;
		  intermediate_value = intermediate_value * adc3_dma_results[i];
		  adc3_processed[i] = parse_raw_calib(intermediate_value, adc3_cfg[i]);
  	  }
  	  intADC_state.ADC3Pending = false;
  	  }
}


__weak void update_can_packets(uint8_t ADCid) {
	/* Override/implement this in user file for putting processed ADC data into CAN packets */


}
