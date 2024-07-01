/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32l5xx_hal.h"

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
#define LSE_IN_Pin GPIO_PIN_14
#define LSE_IN_GPIO_Port GPIOC
#define LSE_OUT_Pin GPIO_PIN_15
#define LSE_OUT_GPIO_Port GPIOC
#define HSE_IN_Pin GPIO_PIN_0
#define HSE_IN_GPIO_Port GPIOH
#define HSE_OUT_Pin GPIO_PIN_1
#define HSE_OUT_GPIO_Port GPIOH
#define WKUP_Pin GPIO_PIN_0
#define WKUP_GPIO_Port GPIOA
#define DBG_UART_TX_Pin GPIO_PIN_2
#define DBG_UART_TX_GPIO_Port GPIOA
#define DBG_UART_RX_Pin GPIO_PIN_3
#define DBG_UART_RX_GPIO_Port GPIOA
#define HWID1_Pin GPIO_PIN_4
#define HWID1_GPIO_Port GPIOA
#define HWID2_Pin GPIO_PIN_5
#define HWID2_GPIO_Port GPIOA
#define EXT_FLASH_IO3_Pin GPIO_PIN_6
#define EXT_FLASH_IO3_GPIO_Port GPIOA
#define EXT_FLASH_IO2_Pin GPIO_PIN_7
#define EXT_FLASH_IO2_GPIO_Port GPIOA
#define EXT_FLASH_IO1_Pin GPIO_PIN_0
#define EXT_FLASH_IO1_GPIO_Port GPIOB
#define EXT_FLASH_IO0_Pin GPIO_PIN_1
#define EXT_FLASH_IO0_GPIO_Port GPIOB
#define EXT_FLASH_CLK_Pin GPIO_PIN_10
#define EXT_FLASH_CLK_GPIO_Port GPIOB
#define EXT_FLASH_NCS_Pin GPIO_PIN_11
#define EXT_FLASH_NCS_GPIO_Port GPIOB
#define TEMP_ALERT_Pin GPIO_PIN_12
#define TEMP_ALERT_GPIO_Port GPIOB
#define TEMP_ALERT_EXTI_IRQn EXTI12_IRQn
#define I2C_SCL_Pin GPIO_PIN_13
#define I2C_SCL_GPIO_Port GPIOB
#define I2C_SDA_Pin GPIO_PIN_14
#define I2C_SDA_GPIO_Port GPIOB
#define LED_DBG_D_Pin GPIO_PIN_15
#define LED_DBG_D_GPIO_Port GPIOB
#define LED_DBG_C_Pin GPIO_PIN_8
#define LED_DBG_C_GPIO_Port GPIOA
#define LED_DBG_B_Pin GPIO_PIN_9
#define LED_DBG_B_GPIO_Port GPIOA
#define LED_DBG_A_Pin GPIO_PIN_10
#define LED_DBG_A_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define MODEM_PWRKEY_Pin GPIO_PIN_4
#define MODEM_PWRKEY_GPIO_Port GPIOB
#define MODEM_RST_N_Pin GPIO_PIN_5
#define MODEM_RST_N_GPIO_Port GPIOB
#define MODEM_TX_Pin GPIO_PIN_6
#define MODEM_TX_GPIO_Port GPIOB
#define MODEM_RX_Pin GPIO_PIN_7
#define MODEM_RX_GPIO_Port GPIOB
#define MODEM_IRQ_N_Pin GPIO_PIN_3
#define MODEM_IRQ_N_GPIO_Port GPIOH
#define MODEM_IRQ_N_EXTI_IRQn EXTI3_IRQn

/* USER CODE BEGIN Private defines */
#define LED_DBG_A LED_DBG_A_GPIO_Port, LED_DBG_A_Pin
#define LED_DBG_B LED_DBG_B_GPIO_Port, LED_DBG_B_Pin
#define LED_DBG_C LED_DBG_C_GPIO_Port, LED_DBG_C_Pin
#define LED_DBG_D LED_DBG_D_GPIO_Port, LED_DBG_D_Pin
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
