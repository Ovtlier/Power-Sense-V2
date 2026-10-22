/*
 * power_control.c
 *
 *  Created on: Jul 22, 2024
 *      Author: ethan
 */
#include "Power/power_control.h"
#include "main.h"
#include "Device/common.h"

power_control_state_t g_powerControlState = {0};

wsst_error_t g_power_control_enable_precharge() {
	wsst_error_t functionStatus = WSST_OK;

	#ifdef PRECHARGE_HV

	HAL_GPIO_WritePin(PRECHARGE_EN_GPIO_Port, PRECHARGE_EN_Pin, GPIO_PIN_SET);
	g_powerControlState.precharge_enabled = true;

	#endif

	return functionStatus;
}


wsst_error_t g_power_control_disable_precharge() {
	wsst_error_t functionStatus = WSST_OK;

	#ifdef PRECHARGE_HV

	HAL_GPIO_WritePin(PRECHARGE_EN_GPIO_Port, PRECHARGE_EN_Pin, GPIO_PIN_RESET);
	g_powerControlState.precharge_enabled = false;

	#endif

	return functionStatus;
}


wsst_error_t g_power_control_enable_hv_output() {
	wsst_error_t functionStatus = WSST_OK;

	HAL_GPIO_WritePin(RLY_EN_GPIO_Port, RLY_EN_Pin, GPIO_PIN_SET);
	g_powerControlState.hv_output_enabled = true;

	g_power_control_check_hv_output_fault();

	return functionStatus;
}


wsst_error_t g_power_control_disable_hv_output() {
	wsst_error_t functionStatus = WSST_OK;

	HAL_GPIO_WritePin(RLY_EN_GPIO_Port, RLY_EN_Pin, GPIO_PIN_RESET);
	g_powerControlState.hv_output_enabled = false;
	g_powerControlState.precharge_completed = false;

	return functionStatus;
}

wsst_error_t g_power_control_check_hv_output_fault() {
	wsst_error_t functionStatus = WSST_OK;

	g_powerControlState.hv_output_fault = HAL_GPIO_ReadPin(RLY_FAULT_GPIO_Port, RLY_FAULT_Pin);

	return functionStatus;
}


wsst_error_t g_power_control_enable_canout() {
	wsst_error_t functionStatus = WSST_OK;

	HAL_GPIO_WritePin(CANOUT_EN_GPIO_Port, CANOUT_EN_Pin, GPIO_PIN_SET);
	g_powerControlState.canout_enabled = true;

	g_power_control_check_canout_fault();

	return functionStatus;
}


wsst_error_t g_power_control_disable_canout() {
	wsst_error_t functionStatus = WSST_OK;

	HAL_GPIO_WritePin(CANOUT_EN_GPIO_Port, CANOUT_EN_Pin, GPIO_PIN_RESET);
	g_powerControlState.canout_enabled = false;

	return functionStatus;
}

wsst_error_t g_power_control_check_canout_fault() {
	wsst_error_t functionStatus = WSST_OK;

	g_powerControlState.canout_fault = HAL_GPIO_ReadPin(CANOUT_EN_SNS_GPIO_Port, CANOUT_EN_SNS_Pin);

	return functionStatus;
}

