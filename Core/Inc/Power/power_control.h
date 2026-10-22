/*
 * power_control.h
 *
 *  Created on: Jul 22, 2024
 *      Author: ethan
 */

#ifndef INC_POWER_POWER_CONTROL_H_
#define INC_POWER_POWER_CONTROL_H_

#include <stdbool.h>

#include "utils.h"

typedef struct power_control_state_s {
	bool precharge_enabled;
	bool precharge_fault;
	bool precharge_completed;
	bool hv_output_enabled;
	bool hv_output_fault;
	bool canout_enabled;
	bool canout_fault;
} power_control_state_t;

wsst_error_t g_power_control_enable_precharge();
wsst_error_t g_power_control_disable_precharge();
wsst_error_t g_power_control_enable_hv_output();
wsst_error_t g_power_control_disable_hv_output();
wsst_error_t g_power_control_check_hv_output_fault();
wsst_error_t g_power_control_enable_canout();
wsst_error_t g_power_control_disable_canout();
wsst_error_t g_power_control_check_canout_fault();

/* Global Variables */

extern power_control_state_t g_powerControlState;

#endif /* INC_POWER_POWER_CONTROL_H_ */
