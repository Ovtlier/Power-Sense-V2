/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "can.h"
#include "dac.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <math.h>
#include "global_defines.h"
#include "Device/common.h"
#include "Device/array_power_sense.h"
#include "State/state_machine.h"
#include "Power/power_control.h"
#include "Auto_Lib/auto_ADC.h"
#include "Interfaces/ADS124S08.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
volatile uint32_t adc1_dma_results[ADC1_CHANNELS_COUNT];
//volatile uint32_t adc2_dma_results[ADC2_CHANNELS_COUNT];
volatile uint32_t adc3_dma_results[ADC3_CHANNELS_COUNT];
double adc1_processed[ADC1_CHANNELS_COUNT];
//double adc2_processed[ADC2_CHANNELS_COUNT];
double adc3_processed[ADC3_CHANNELS_COUNT];

volatile int32_t ext_adc_results[EXT_CH_COUNT];
volatile float   ext_adc_volts[EXT_CH_COUNT];
volatile bool    g_can_hv_on_requested  = false;
volatile bool    g_can_hv_off_requested = false;
volatile float   g_mppt_max_temp_c      = 0.0f;
static float     mppt_temps[N_MPPTS]   = {0};  // per-MPPT hottest temp (controller or mosfet)
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
/* USER CODE BEGIN PFP */
static void _can_tx(uint32_t id, uint8_t *data, uint8_t len);
void can_tx_output_bus(void);
void can_tx_input_bus(void);
void can_tx_supply(void);
void can_tx_status(void);
void can_tx_temperature(void);
void can_send_status_ok(void);
void can_send_status_off(void);
void can_send_status_error(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */



/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */


  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

/* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */
  HAL_Delay(100);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_CAN1_Init();
  MX_DAC1_Init();
  MX_I2C1_Init();
  MX_SPI2_Init();
  MX_ADC3_Init();
  MX_SPI1_Init();
  MX_I2C3_Init();
  /* USER CODE BEGIN 2 */

  autoADC_init();
  ADS_init();
  ADS_StartSampling();
  g_state_machine_init_states();

  /* Configure CAN RX filter — pass all standard IDs (software filtering in callback) */
  CAN_FilterTypeDef sFilterConfig = {0};
  sFilterConfig.FilterBank = 0;
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  sFilterConfig.FilterIdHigh = 0x0000;
  sFilterConfig.FilterIdLow = 0x0000;
  sFilterConfig.FilterMaskIdHigh = 0x0000;
  sFilterConfig.FilterMaskIdLow = 0x0000;
  sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
  sFilterConfig.FilterActivation = ENABLE;
  HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig);
  HAL_CAN_Start(&hcan1);
  HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */


  while (1)
  {
	/* External ADC channel scan (DRDY polling, discard first sample after MUX change) */
	static ext_adc_channel_t ext_ch = EXT_CH_V3V3;
	static bool ext_discard = true;
	static const uint8_t ext_mux[EXT_CH_COUNT] = {
		EXT_ADC_MUX_V3V3, EXT_ADC_MUX_V5V,
		EXT_ADC_MUX_V_ARRAY, EXT_ADC_MUX_V_MPPT, EXT_ADC_MUX_I_ARRAY
	};
	static const float ext_scale[EXT_CH_COUNT] = {
		VSCALE_V3V3, VSCALE_V5V, VSCALE_V_ARRAY, VSCALE_V_MPPT, 1.0f
	};

	if (!HAL_GPIO_ReadPin(SPI1_DRDY_GPIO_Port, SPI1_DRDY_Pin)) {
		int32_t s = ADS_ReadSampleSigned();
		if (!ext_discard) {
			ext_adc_results[ext_ch] = s;
			if (ext_ch == EXT_CH_I_ARRAY) {
				/* Differential (AIN5-AIN2): both INA190A2 share same REF so it cancels.
				 * I(A) = V_diff / 0.025  (gain=50, shunt=0.5mΩ → 50*0.0005 = 0.025) */
				float v = (float)s * EXT_ADC_COUNTS_TO_V;
				ext_adc_volts[ext_ch] = v / 0.025f;
			} else {
				/* Single-ended vs AINCOM (2.5V): add offset back, then scale by divider */
				ext_adc_volts[ext_ch] = ((float)s * EXT_ADC_COUNTS_TO_V + EXT_ADC_AINCOM_OFFSET)
				                        * ext_scale[ext_ch];
			}
		}
		ext_ch = (ext_adc_channel_t)((ext_ch + 1) % EXT_CH_COUNT);
		ADS_WriteReg(INPMUX_ADDR_MASK, ext_mux[ext_ch]);
		ext_discard = true;
	} else {
		ext_discard = false;
	}

	/* Internal ADC — throttled processing */
	every_n_ms(30) {
		intADC_state.ADC1Pending = true;
		intADC_state.ADC3Pending = true;
		parse_adc();
	}

	/* State machine */
	g_state_machine_handle();

	/* CAN TX telemetry */
	every_n_ms(250) {
		can_tx_output_bus();   // 0x5F1: ARRAY_OUT, I_ARRAY
		can_tx_input_bus();    // 0x5F3: MPPT_IN
	}
	every_n_ms(500) {
		can_tx_supply();       // 0x5F4: VCAN, ICANOUT, VCANOUT, IBOARD
		can_tx_status();       // 0x5F5: status bits, 5V, 3V3
	}
	every_n_ms(1000) {
		can_tx_temperature();  // 0x5F7: board temp, terminal temps
	}

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_PLLSAI1;
  PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_MSI;
  PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
  PeriphClkInit.PLLSAI1.PLLSAI1N = 16;
  PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_ADC1CLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* --------------------------------------------------------------------------
 * CAN RX callback
 * -------------------------------------------------------------------------- */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
	CAN_RxHeaderTypeDef rx_hdr;
	uint8_t rx_data[8];
	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_hdr, rx_data);

	if (rx_hdr.StdId == CAN_ID_ARRAY_COMMAND) {
		if (rx_data[0] == CAN_CMD_HV_ON)  g_can_hv_on_requested  = true;
		if (rx_data[0] == CAN_CMD_HV_OFF) g_can_hv_off_requested = true;
		/* Byte 1: CANBUS output cmd — reserved for future use */
	}

	/* MPPT temperature messages: 0x602, 0x612, 0x622, 0x632, 0x642 */
	uint32_t id = rx_hdr.StdId;
	if ((id & 0xFF0) >= 0x600 && (id & 0xFF0) <= 0x640 && (id & 0x00F) == 0x002) {
		uint8_t mppt_idx = (uint8_t)(((id & 0xFF0) - 0x600) >> 4);  // 0-4
		if (mppt_idx < N_MPPTS) {
			float controller_temp, mosfet_temp;
			memcpy(&controller_temp, &rx_data[0], 4);
			memcpy(&mosfet_temp,     &rx_data[4], 4);
			float max_local = (controller_temp > mosfet_temp) ? controller_temp : mosfet_temp;
			mppt_temps[mppt_idx] = max_local;  // overwrite — allows cooling

			/* Recompute global max from all MPPTs */
			float new_max = 0.0f;
			for (uint8_t i = 0; i < N_MPPTS; i++) {
				if (mppt_temps[i] > new_max) new_max = mppt_temps[i];
			}
			g_mppt_max_temp_c = new_max;
		}
	}
}

/* --------------------------------------------------------------------------
 * CAN TX helpers
 * -------------------------------------------------------------------------- */
static void _can_tx(uint32_t id, uint8_t *data, uint8_t len) {
	CAN_TxHeaderTypeDef hdr = {
		.StdId = id, .IDE = CAN_ID_STD, .RTR = CAN_RTR_DATA,
		.DLC = len, .TransmitGlobalTime = DISABLE
	};
	uint32_t mailbox;
	if (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) > 0)
		HAL_CAN_AddTxMessage(&hcan1, &hdr, data, &mailbox);
}

void can_tx_output_bus(void) {
	uint8_t buf[8];
	float v = V_ARRAY_OUT, i = I_ARRAY_EXT;
	memcpy(&buf[0], &v, 4);
	memcpy(&buf[4], &i, 4);
	_can_tx(CAN_ID_TX_OUT_BUS, buf, 8);
}

void can_tx_input_bus(void) {
	uint8_t buf[4];
	float v = V_MPPT_IN;
	memcpy(buf, &v, 4);
	_can_tx(CAN_ID_TX_IN_BUS, buf, 4);
}

void can_tx_supply(void) {
	uint8_t buf[8];
	uint16_t vcan   = (uint16_t)(VCAN_DIV   * 1000.0);
	uint16_t ican   = (uint16_t)(ICANOUT    * 1000.0);
	uint16_t vout   = (uint16_t)(VCANOUT_DIV * 1000.0);
	uint16_t iboard = (uint16_t)(IBOARD     * 1000.0);
	memcpy(&buf[0], &vcan,   2);
	memcpy(&buf[2], &ican,   2);
	memcpy(&buf[4], &vout,   2);
	memcpy(&buf[6], &iboard, 2);
	_can_tx(CAN_ID_TX_SUPPLY, buf, 8);
}

void can_tx_status(void) {
	uint8_t buf[8] = {0};
	buf[0] = (g_powerControlState.precharge_enabled ? 0x01 : 0x00)
	       | (g_powerControlState.hv_output_enabled  ? 0x10 : 0x00);
	uint16_t v5  = (uint16_t)(V5V_DIV  * 1000.0);
	uint16_t v33 = (uint16_t)(V3V3_DIV * 1000.0);
	memcpy(&buf[4], &v5,  2);
	memcpy(&buf[6], &v33, 2);
	_can_tx(CAN_ID_TX_STATUS, buf, 8);
}

/* NTC conversion: 0603R-NTC-10K, B25/50 = 3380K, R25 = 10KΩ
 * Circuit: 3V3 → 10K pullup → ADC → NTC → GND
 * Returns temperature in °C, or -273.15 on invalid input. */
static float ntc_voltage_to_celsius(double v_adc) {
	const float T0_K = 298.15f;   // 25°C in Kelvin

	if (v_adc <= 0.0 || v_adc >= (double)NTC_V_SUPPLY) return -273.15f;  // invalid

	float r_ntc = NTC_PULLUP * (float)v_adc / (NTC_V_SUPPLY - (float)v_adc);
	float t_k   = 1.0f / (logf(r_ntc / NTC_R25) / NTC_BETA + 1.0f / T0_K);
	return t_k - 273.15f;
}

void can_tx_temperature(void) {
	/* BOARD_TEMP0 (NTC under relay)  = ADC1_IN14 → adc1_processed[11]
	 * BOARD_TEMP1 (NTC near ADC)     = ADC3_IN12 → adc3_processed[3]
	 * NTC: 0603R-NTC-10K, B3380, 10K pullup to 3V3
	 * Format: uint16 in 0.01°C units (e.g. 2500 = 25.00°C) */
	uint8_t buf[6];
	float temp0 = ntc_voltage_to_celsius(ADC1_IN14_SPARE);
	float temp1 = ntc_voltage_to_celsius(ADC3_IN12_SPARE);
	uint16_t t0 = (temp0 > 0.0f) ? (uint16_t)(temp0 * 100.0f) : 0;
	uint16_t t1 = (temp1 > 0.0f) ? (uint16_t)(temp1 * 100.0f) : 0;
	uint16_t reserved = 0;
	memcpy(&buf[0], &t0,       2);
	memcpy(&buf[2], &t1,       2);
	memcpy(&buf[4], &reserved, 2);
	_can_tx(CAN_ID_TX_TEMP, buf, 6);
}

void can_send_status_ok(void)    { can_tx_status(); /* relay = on — status bits set by g_powerControlState */ }
void can_send_status_off(void)   { can_tx_status(); /* relay = off */ }
void can_send_status_error(void) {
	uint8_t buf[8] = {0};
	buf[0] = 0xFF;  // error flag
	_can_tx(CAN_ID_TX_STATUS, buf, 8);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
