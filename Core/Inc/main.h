/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define PRECHARGE_EN_Pin GPIO_PIN_0
#define PRECHARGE_EN_GPIO_Port GPIOF
#define DEBUG_LED_Pin GPIO_PIN_5
#define DEBUG_LED_GPIO_Port GPIOF
#define ADC3_IN9_SPARE_Pin GPIO_PIN_6
#define ADC3_IN9_SPARE_GPIO_Port GPIOF
#define ADC3_IN10_SPARE_Pin GPIO_PIN_7
#define ADC3_IN10_SPARE_GPIO_Port GPIOF
#define ADC3_IN11_SPARE_Pin GPIO_PIN_8
#define ADC3_IN11_SPARE_GPIO_Port GPIOF
#define ADC3_IN12_SPARE_Pin GPIO_PIN_9
#define ADC3_IN12_SPARE_GPIO_Port GPIOF
#define VCANOUT_DIV_Pin GPIO_PIN_10
#define VCANOUT_DIV_GPIO_Port GPIOF
#define VCAN_DIV_Pin GPIO_PIN_0
#define VCAN_DIV_GPIO_Port GPIOC
#define IBOARD_Pin GPIO_PIN_1
#define IBOARD_GPIO_Port GPIOC
#define ICANOUT_Pin GPIO_PIN_2
#define ICANOUT_GPIO_Port GPIOC
#define VBOARD_DIV_Pin GPIO_PIN_3
#define VBOARD_DIV_GPIO_Port GPIOC
#define IBOARD_REF_Pin GPIO_PIN_0
#define IBOARD_REF_GPIO_Port GPIOA
#define ICANOUT_REF_Pin GPIO_PIN_1
#define ICANOUT_REF_GPIO_Port GPIOA
#define V5V_DIV_Pin GPIO_PIN_2
#define V5V_DIV_GPIO_Port GPIOA
#define V3V3_DIV_Pin GPIO_PIN_3
#define V3V3_DIV_GPIO_Port GPIOA
#define ADC1_IN11_SPARE_Pin GPIO_PIN_6
#define ADC1_IN11_SPARE_GPIO_Port GPIOA
#define ADC1_IN12_SPARE_Pin GPIO_PIN_7
#define ADC1_IN12_SPARE_GPIO_Port GPIOA
#define ADC1_IN13_SPARE_Pin GPIO_PIN_4
#define ADC1_IN13_SPARE_GPIO_Port GPIOC
#define ADC1_IN14_SPARE_Pin GPIO_PIN_5
#define ADC1_IN14_SPARE_GPIO_Port GPIOC
#define RLY_FAULT_Pin GPIO_PIN_0
#define RLY_FAULT_GPIO_Port GPIOB
#define RLY_EN_Pin GPIO_PIN_1
#define RLY_EN_GPIO_Port GPIOB
#define SPI_ADC_RST_Pin GPIO_PIN_12
#define SPI_ADC_RST_GPIO_Port GPIOE
#define INPUT1_Pin GPIO_PIN_11
#define INPUT1_GPIO_Port GPIOB
#define SPI1_DRDY_Pin GPIO_PIN_8
#define SPI1_DRDY_GPIO_Port GPIOD
#define CANOUT_EN_Pin GPIO_PIN_0
#define CANOUT_EN_GPIO_Port GPIOD
#define CANOUT_EN_SNS_Pin GPIO_PIN_1
#define CANOUT_EN_SNS_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
