/*
 * state_machine.h
 *
 *  Created on: Jul 20, 2024
 *      Author: ethan
 */

#ifndef INC_STATE_STATE_MACHINE_H_
#define INC_STATE_STATE_MACHINE_H_

#include "State/state_machine_defs.h"

/* Global Functions */
void g_state_machine_init_states();
void g_state_machine_handle();
void g_state_machine_change_state(state_t newState);
void g_state_machine_set_error(state_machine_error_states_t newError);
void g_state_machine_clear_error(state_t restoredState);

/* Global Variables */
extern state_machine_t g_stateMachine;

#endif /* INC_STATE_STATE_MACHINE_H_ */
