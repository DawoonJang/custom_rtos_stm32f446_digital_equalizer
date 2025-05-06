/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32f446xx.h"
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
#define TFT_RST_Pin GPIO_PIN_0
#define TFT_RST_GPIO_Port GPIOC
#define TFT_RS_Pin GPIO_PIN_1
#define TFT_RS_GPIO_Port GPIOC
#define TFT_LED_Pin GPIO_PIN_2
#define TFT_LED_GPIO_Port GPIOC
#define LED7_Pin GPIO_PIN_3
#define LED7_GPIO_Port GPIOC
#define SWITCH2_Pin GPIO_PIN_4
#define SWITCH2_GPIO_Port GPIOA
#define SWITCH2_EXTI_IRQn EXTI4_IRQn
#define SWITCH1_Pin GPIO_PIN_2
#define SWITCH1_GPIO_Port GPIOD
#define SWITCH1_EXTI_IRQn EXTI2_IRQn

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
