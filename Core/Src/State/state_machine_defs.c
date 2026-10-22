/*
 * state_machine_funcs.c
 *
 * Defines functions and states for state machine
 *
 *  Created on: Jul 20, 2024
 *      Author: ethan
 */

/*
 * Notes on SM use:
 * Don't use g_state_machine_change_state for going into error state
 * TODO: Think about adding reset state
 */

#include "State/state_machine_defs.h"
#include "State/state_machine.h"
#include "Power/power_control.h"
#include "Device/common.h"
#include "main.h"
#include "global_defines.h"
#include "Device/array_power_sense.h"

/* CAN request flags set by RX callback in main.c */
extern volatile bool g_can_hv_on_requested;
extern volatile bool g_can_hv_off_requested;
extern volatile float g_mppt_max_temp_c;

/* CAN TX status helpers defined in main.c */
void can_send_status_ok(void);
void can_send_status_off(void);
void can_send_status_error(void);

static void update_fan_control(void);

bool initStateCheckEnter();
void initStateEnter();
void initStateHandler();
bool initStateExit();

bool postStateCheckEnter();
void postStateEnter();
void postStateHandler();
bool postStateExit();

bool startupStateCheckEnter();
void startupStateEnter();
void startupStateHandler();
bool startupStateExit();

bool idleStateCheckEnter();
void idleStateEnter();
void idleStateHandler();
bool idleStateExit();

bool prechargeStateCheckEnter();
void prechargeStateEnter();
void prechargeStateHandler();
bool prechargeStateExit();

bool outputEnabledStateCheckEnter();
void outputEnabledStateEnter();
void outputEnabledStateHandler();
bool outputEnabledStateExit();

bool errorStateCheckEnter();
void errorStateEnter();
void errorStateHandler();
bool errorStateExit();


state_t initState = {
	.stateID = STATE_INIT,
	.entryTime = 0,
	.check_enter_fp = &initStateCheckEnter,
	.entry_fp = &initStateEnter,
	.handle_fp = &initStateHandler,
	.exit_fp = &initStateExit,
	.prio = PRIORITY_LOW,
	.permittedStates = {STATE_POST, STATE_ERROR}
};

state_t postState = {
	.stateID = STATE_POST,
	.entryTime = 0,
	.check_enter_fp = &postStateCheckEnter,
	.entry_fp = &postStateEnter,
	.handle_fp = &postStateHandler,
	.exit_fp = &postStateExit,
	.prio = PRIORITY_LOW,
	.permittedStates = {STATE_STARTUP, STATE_ERROR}
};

state_t startupState = {
	.stateID = STATE_STARTUP,
	.entryTime = 0,
	.check_enter_fp = &startupStateCheckEnter,
	.entry_fp = &startupStateEnter,
	.handle_fp = &startupStateHandler,
	.exit_fp = &startupStateExit,
	.prio = PRIORITY_LOW,
	.permittedStates = {STATE_IDLE, STATE_ERROR}
};

state_t idleState = { // If we ever want to implement enable/disable via canbus - can wait in this state
	.stateID = STATE_IDLE,
	.entryTime = 0,
	.check_enter_fp = &idleStateCheckEnter,
	.entry_fp = &idleStateEnter,
	.handle_fp = &idleStateHandler,
	.exit_fp = &idleStateExit,
	.prio = PRIORITY_LOW,
	.permittedStates = {STATE_PRECHARGE, STATE_ERROR}
};

state_t prechargeState = {
	.stateID = STATE_PRECHARGE,
	.entryTime = 0,
	.check_enter_fp = &prechargeStateCheckEnter,
	.entry_fp = &prechargeStateEnter,
	.handle_fp = &prechargeStateHandler,
	.exit_fp = &prechargeStateExit,
	.prio = PRIORITY_LOW,
	.permittedStates = {STATE_OUTPUT_ENABLED, STATE_ERROR}
};

state_t outputEnabledState = {
	.stateID = STATE_OUTPUT_ENABLED,
	.entryTime = 0,
	.check_enter_fp = &outputEnabledStateCheckEnter,
	.entry_fp = &outputEnabledStateEnter,
	.handle_fp = &outputEnabledStateHandler,
	.exit_fp = &outputEnabledStateExit,
	.prio = PRIORITY_LOW,
	.permittedStates = {STATE_POST, STATE_PRECHARGE, STATE_IDLE, STATE_ERROR}
};

state_t errorState = {
	.stateID = STATE_ERROR,
	.entryTime = 0,
	.check_enter_fp = &errorStateCheckEnter,
	.entry_fp = &errorStateEnter,
	.handle_fp = &errorStateHandler,
	.exit_fp = &errorStateExit,
	.prio = PRIORITY_HIGH,
	.permittedStates = {STATE_POST, STATE_PRECHARGE}
};

/*
 * Control fans based on MPPT temperature with hysteresis.
 * CANOUT_EN drives the fans.
 */
static void update_fan_control(void) {
	if (g_mppt_max_temp_c >= FAN_ON_TEMP_C) {
		g_power_control_enable_canout();
	} else if (g_mppt_max_temp_c < FAN_OFF_TEMP_C) {
		g_power_control_disable_canout();
	}
	// between FAN_OFF_TEMP_C and FAN_ON_TEMP_C: hold current state (hysteresis)
}

/*
*********************************************************************************************
 * INIT STATE																				*
 ********************************************************************************************
*/
bool initStateCheckEnter() {
	bool returnVal = true;

	return returnVal;
}

void initStateEnter() {
	return;
}

void initStateHandler() {
	g_state_machine_change_state(postState);
	return;
}

bool initStateExit() {
	bool returnVal = true;

	return returnVal; // Can change out of state
}

/*
*********************************************************************************************
 * POST STATE																			    *
 ********************************************************************************************
*/
bool postStateCheckEnter() {
	bool returnVal = true;

	return returnVal;
}

void postStateEnter() {
	return;
}

void postStateHandler() {

	/* Wait until rail voltages are within acceptable margins defined within common.h */


	g_state_machine_change_state(startupState);

	return;
}

bool postStateExit() {
	bool returnVal = true;

	return returnVal;
}

/*
*********************************************************************************************
 * STARTUP STATE																		    *
 ********************************************************************************************
*/
bool startupStateCheckEnter() {
	bool returnVal = true;

	return returnVal;
}

void startupStateEnter() {
	return;
}

void startupStateHandler() {

	/* Bring mppt online  */

	g_power_control_check_canout_fault();
	g_power_control_check_hv_output_fault();

	g_state_machine_change_state(idleState);

	return;
}

bool startupStateExit() {
	bool returnVal = true;

	return returnVal;
}

/*
*********************************************************************************************
 * IDLE STATE																			    *
 ********************************************************************************************
*/
bool idleStateCheckEnter() {
	bool returnVal = true;

	return returnVal;
}

void idleStateEnter() {

	return;
}

void idleStateHandler() {

	update_fan_control();

	if (g_can_hv_on_requested) {
		g_can_hv_on_requested = false;
		g_state_machine_change_state(prechargeState);
	}

	return;
}

bool idleStateExit() {
	bool returnVal = true;

	return returnVal;
}

/*
*********************************************************************************************
 * PRECHARGE STATE																		    *
 ********************************************************************************************
*/
bool prechargeStateCheckEnter() {
	bool returnVal = true;

	return returnVal;
}

void prechargeStateEnter() {
	g_power_control_enable_precharge();

	return;
}

void prechargeStateHandler() {

	uint32_t elapsed = HAL_GetTick() - g_stateMachine.currentState.entryTime;
	if (elapsed > PRECHARGE_TIMEOUT_MS) {
		g_power_control_disable_precharge();
		g_state_machine_set_error(ERR_PRECHARGE);
		return;
	}

	/* Close relay when V_array has risen to >= 70% of V_mppt */
	if (V_MPPT_IN > 1.0f && V_ARRAY_OUT >= PRECHARGE_RATIO * V_MPPT_IN) {
		g_powerControlState.precharge_completed = true;
		g_state_machine_change_state(outputEnabledState);
	}

	update_fan_control();

	return;
}

bool prechargeStateExit() {
	bool returnVal = true;

	return returnVal;
}

/*
*********************************************************************************************
 * OUTPUT ENABLED STATE																		*
 ********************************************************************************************
*/
bool outputEnabledStateCheckEnter() {
	bool returnVal = true;

	g_power_control_check_hv_output_fault();

	if(true == g_powerControlState.hv_output_fault) {
		returnVal = false;
		g_state_machine_set_error(ERR_HV_OUTPUT_FAULT);
	}

	return returnVal;
}

void outputEnabledStateEnter() {

	g_power_control_enable_hv_output();
	g_power_control_disable_precharge();
	can_send_status_ok();

	return;
}

void outputEnabledStateHandler() {

	/* Non-blocking relay-off: once HV off is requested, open relay first,
	 * wait 3s for capacitors to bleed through precharge resistor,
	 * then open precharge and transition to idle. */
	static bool     shutting_down = false;
	static uint32_t shutdown_tick = 0;

	if (shutting_down) {
		if (HAL_GetTick() - shutdown_tick >= 3000) {
			g_power_control_disable_precharge();
			can_send_status_off();
			shutting_down = false;
			g_state_machine_change_state(idleState);
		}
		update_fan_control();
		return;
	}

	g_power_control_check_hv_output_fault();
	if (g_powerControlState.hv_output_fault) {
		g_state_machine_set_error(ERR_HV_OUTPUT_FAULT);
		return;
	}

	if (g_can_hv_off_requested) {
		g_can_hv_off_requested = false;
		g_power_control_disable_hv_output();
		shutdown_tick = HAL_GetTick();
		shutting_down = true;
		return;
	}

	update_fan_control();

	return;
}

bool outputEnabledStateExit() {
	bool returnVal = true;

	g_power_control_disable_hv_output();

	return returnVal;
}

/*
*********************************************************************************************
 * ERROR STATE																				*
 ********************************************************************************************
*/
bool errorStateCheckEnter() {
	bool returnVal = true;

	return returnVal;
}

void errorStateEnter() {
	g_power_control_disable_hv_output();
	g_power_control_disable_precharge();

	return;
}

void errorStateHandler() {
	switch(g_stateMachine.currentError) {

	case ERR_HV_OUTPUT_FAULT: // Wait for HV_OUTPUT_FAULT to clear
		g_power_control_check_hv_output_fault();

		if(g_powerControlState.hv_output_fault == false) {
			g_state_machine_clear_error(initState);
		}

		break;

	case ERR_CANOUT_FAULT: // Wait for ERR_CANOUT_FAULT to clear
		g_power_control_check_canout_fault();

		if(g_powerControlState.canout_fault == false) {
			g_state_machine_clear_error(initState);
		}

		break;

	case ERR_PRECHARGE:
		can_send_status_error();
		g_state_machine_clear_error(idleState);
		break;

	case ERR_UNKNOWN:
		// Log error over CANBUS
		break;

	case ERR_POST:
		// Log error over CANBUS
		g_state_machine_clear_error(initState);
		break;

	case ERR_NONE:
		g_state_machine_clear_error(initState);
		break;


	default:
		break;
	}


	return;
}

bool errorStateExit() {
	bool returnVal = true;

	return returnVal;
}
