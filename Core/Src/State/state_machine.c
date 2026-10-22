/*
 * state_machine.c
 *
 *  Created on: Jul 20, 2024
 *      Author: ethan
 */

#include "State/state_machine.h"
#include "main.h"

state_machine_t g_stateMachine = {0};

static bool _state_permitted(state_t currState, state_t newState);
static bool _evaluate_state_change(state_t newState);
static bool state_machine_change_state(state_t newState); // Higher level function that uses above two functions

void g_state_machine_init_states() {
	g_stateMachine.previousState = initState;
	g_stateMachine.currentState = postState;
	g_stateMachine.newState = postState;
	g_stateMachine.currentError = ERR_NONE;
	g_stateMachine.previousError = ERR_NONE;

	return;
}

/*
 * Called in main loop to handle state changes and
 * run necessary functions
 */
void g_state_machine_handle() {
	/* Check for pending state change */
	if(g_stateMachine.currentState.stateID != g_stateMachine.newState.stateID) {
		state_machine_change_state(g_stateMachine.newState);
	}

	/* Handle current state */
	g_stateMachine.currentState.handle_fp();
}

/*
 * Global state change function
 * Don't use this for going into error state
 */
void g_state_machine_change_state(state_t newState) {
	g_stateMachine.newState = newState;

	return;
}


/*
 * Call to enter error state
 */
void g_state_machine_set_error(state_machine_error_states_t newError) {
	if(_state_permitted(g_stateMachine.currentState, errorState) == true) {
		g_stateMachine.currentState.exit_fp(); // We don't care about return val here, more of a courtesy
		g_stateMachine.previousState = g_stateMachine.currentState;
		g_stateMachine.currentState = errorState;
		g_stateMachine.newState = errorState;

		g_stateMachine.previousError = g_stateMachine.currentError;
		g_stateMachine.currentError = newError;

		g_stateMachine.currentState.entryTime = HAL_GetTick();
		g_stateMachine.currentState.entry_fp();

	}

	return;
}

/* Clears the current error and sets the current state to the restoredState value*/
void g_state_machine_clear_error(state_t restoredState) {
	g_stateMachine.previousError = g_stateMachine.currentError;
	g_stateMachine.currentError = ERR_NONE;

	g_stateMachine.previousState = g_stateMachine.currentState;
	g_stateMachine.currentState = restoredState;
	g_stateMachine.newState = restoredState;

	g_stateMachine.currentState.entryTime = HAL_GetTick();
	g_stateMachine.currentState.entry_fp();

	return;
}


/*
 *
 */
static bool state_machine_change_state(state_t newState) {
	bool stateChanged = false;
	if(_state_permitted(g_stateMachine.currentState, newState) == true) {
		stateChanged = _evaluate_state_change(newState);
		if(true == stateChanged) {
			g_stateMachine.previousState = g_stateMachine.currentState;
			g_stateMachine.currentState = newState;
			g_stateMachine.currentState.entryTime = HAL_GetTick();
			g_stateMachine.currentState.entry_fp();
		}
	}

	return stateChanged;
}

/*
 * Internal func to check if newState is a permitted to go to from the current state
 */
static bool _state_permitted(state_t currState, state_t newState) {
	bool returnVal = false;

	for(uint8_t i = 0; i < N_STATES; i++) {
		if(newState.stateID == currState.permittedStates[i]) {
			returnVal = true;
			break;
		}
	}

	return returnVal;
}

/*
 * Internal func that evaluates first the current state exit_fp() and new state entry_fp() to check if we can change state
 * If that returns false, then we compare the priority, if new state higher prio, change state
 * if new state doesn't have priority then we return false
 * Don't use this for going into error state
 */
static bool _evaluate_state_change(state_t newState) {
	bool returnVal = false;

	if( true == (*g_stateMachine.currentState.exit_fp)() && true == (*newState.check_enter_fp)() ) {
		returnVal = true;
	} else if(newState.prio > g_stateMachine.currentState.prio) {
		returnVal = true;
	} else {
		returnVal = false;
	}


	return returnVal;
}
