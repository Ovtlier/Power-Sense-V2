/*
 * common.h
 *
 *  Created on: Jul 20, 2024
 *      Author: ethan
 */

#ifndef INC_DEVICE_COMMON_H_
#define INC_DEVICE_COMMON_H_

#define PS_ARRAY // Change target here


#define RAIL_12V_LOW 9000
#define RAIL_12V_HIGH 30000

#define RAIL_5v_LOW 4000
#define RAIL_5v_HIGH 6000

#define RAIL_3V3_LOW 2500
#define RAIL_3V3_HIGH 4000

#ifdef PS_MOTOR
#include "Device/motor_power_sense.h"
#endif

#ifdef PS_ARRAY
#include "Device/array_power_sense.h"
#endif


#endif /* INC_DEVICE_COMMON_H_ */
