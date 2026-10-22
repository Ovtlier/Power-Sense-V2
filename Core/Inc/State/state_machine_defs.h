/*
 * state_machine_funcs.h
 *
 *  Created on: Jul 20, 2024
 *      Author: ethan
 */

#ifndef INC_STATE_STATE_MACHINE_DEFS_H_
#define INC_STATE_STATE_MACHINE_DEFS_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum state_machine_states_e {
	STATE_INIT = 0,
	STATE_POST,
	STATE_STARTUP,
	STATE_IDLE,
	STATE_PRECHARGE,
	STATE_OUTPUT_ENABLED,
	STATE_ERROR,
	N_STATES
} state_machine_states_t;

typedef enum state_machine_error_states_e {
	ERR_NONE = 0,
	ERR_UNKNOWN,
	ERR_POST,
	ERR_PRECHARGE,
	ERR_HV_OUTPUT_FAULT,
	ERR_CANOUT_FAULT,
	N_ERRORSTATES
} state_machine_error_states_t;

typedef enum state_machine_state_priority_e {
	PRIORITY_LOW = 0,
	PRIORITY_MED,
	PRIORITY_HIGH,
	N_PRIORITY
} state_machine_state_priority_t;


typedef struct state_s {
	state_machine_states_t stateID;

	state_machine_state_priority_t prio;

	state_machine_states_t permittedStates[N_STATES];

	uint32_t entryTime;

	bool (*check_enter_fp)(void); // Called when attempting to transition to this, returns true if ready to change to this state, false if not (overridden by priority)
	void (*entry_fp)(void);  // Called once when entering state
	void (*handle_fp)(void); // Called every loop while in this state
	bool (*exit_fp)(void);   // Called when exiting the state, returns true if successfully ready to change to new state, false if not (overridden by priority)

} state_t;


typedef struct state_machine_s {
	state_t previousState;
	state_t currentState;
	state_t newState;
	state_machine_error_states_t currentError;
	state_machine_error_states_t previousError;
} state_machine_t;


/* Global vars */
extern state_t initState;
extern state_t postState;
extern state_t startupState;
extern state_t idleState;
extern state_t prechargeState;
extern state_t outputEnabledState;
extern state_t errorState;


#endif /* INC_STATE_STATE_MACHINE_DEFS_H_ */
