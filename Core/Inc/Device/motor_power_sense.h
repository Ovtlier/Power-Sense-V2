/*
 * motor_power_sense.h
 *
 *  Created on: Jul 20, 2024
 *      Author: ethan
 */

#ifndef INC_DEVICE_MOTOR_POWER_SENSE_H_
#define INC_DEVICE_MOTOR_POWER_SENSE_H_

/* Functionality */
#define PRECHARGE_HV
#define ADC_CS_TIED_LOW // Undefine if CS is being used

/* Specifications */
#ifdef PRECHARGE_HV
#define PRECHARGE_WAIT 1000 // TODO: Change
#endif

#endif /* INC_DEVICE_MOTOR_POWER_SENSE_H_ */
