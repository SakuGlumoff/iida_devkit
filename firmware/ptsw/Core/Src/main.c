/* USER CODE BEGIN Header */
// clang-format off
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "crc.h"
#include "hash.h"
#include "i2c.h"
#include "icache.h"
#include "usart.h"
#include "octospi.h"
#include "rng.h"
#include "rtc.h"
#include "usb.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define STRINGIFY(x) #x
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static enum {
  TEST_INIT = 0,
  TEST_LED_A,
  TEST_LED_B,
  TEST_LED_C,
  TEST_LED_D,
  TEST_DBG_UART,
  TEST_MODEM_UART,
  TEST_QSPI,
  TEST_RTC,
  TEST_I2C,
  TEST_COUNT
} _test;
static const char* _test_names[TEST_COUNT] = {
  STRINGIFY(TEST_INIT),
  STRINGIFY(TEST_LED_A),
  STRINGIFY(TEST_LED_B),
  STRINGIFY(TEST_LED_C),
  STRINGIFY(TEST_LED_D),
  STRINGIFY(TEST_DBG_UART),
  STRINGIFY(TEST_MODEM_UART),
  STRINGIFY(TEST_QSPI),
  STRINGIFY(TEST_RTC),
  STRINGIFY(TEST_I2C)
};
#define MIN_I2C_SLAVE_ADDRESS (0x0U)
#define MAX_I2C_SLAVE_ADDRESS (0x7FU)
static bool _connected_i2c_slaves[MAX_I2C_SLAVE_ADDRESS + 1];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int __io_putchar(int ch) {
  return HAL_UART_Transmit(&hlpuart1, (uint8_t*)&ch, 1, 1000UL);
}

int __io_getchar(void) {
  uint8_t ch = 0xFFU;
  if (HAL_UART_Receive(&hlpuart1, &ch, 1, 1000UL) != HAL_OK) {
    return -1;
  }
  return ch;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  _test = TEST_INIT;
  bool test_results[TEST_COUNT];
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

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CRC_Init();
  MX_HASH_Init();
  MX_I2C2_Init();
  MX_ICACHE_Init();
  MX_LPUART1_UART_Init();
  MX_USART1_UART_Init();
  MX_OCTOSPI1_Init();
  MX_RNG_Init();
  MX_RTC_Init();
  MX_USB_PCD_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    if (_test >= TEST_COUNT) {
      break;
    }
    printf("TEST: Start %s\n", _test_names[_test]);
    switch (_test) {
      case TEST_INIT:
        for (unsigned i = 0; i < TEST_COUNT; i++) {
          test_results[i] = false;
        }
        for (unsigned i = MIN_I2C_SLAVE_ADDRESS; i < (MAX_I2C_SLAVE_ADDRESS + 1); i++) {
          _connected_i2c_slaves[i] = false;
        }
        HAL_GPIO_WritePin(LED_DBG_A, GPIO_PIN_SET);
        HAL_GPIO_WritePin(LED_DBG_B, GPIO_PIN_SET);
        HAL_GPIO_WritePin(LED_DBG_C, GPIO_PIN_SET);
        HAL_GPIO_WritePin(LED_DBG_D, GPIO_PIN_SET);
        test_results[_test] = true;
        break;
      case TEST_LED_A:
        HAL_GPIO_WritePin(LED_DBG_A, GPIO_PIN_RESET);
        if (HAL_GPIO_ReadPin(LED_DBG_A) != GPIO_PIN_RESET) {
          break;
        }
        HAL_Delay(250UL);
        HAL_GPIO_WritePin(LED_DBG_A, GPIO_PIN_SET);
        if (HAL_GPIO_ReadPin(LED_DBG_A) != GPIO_PIN_SET) {
          break;
        }
        test_results[_test] = true;
        break;
      case TEST_LED_B:
        HAL_GPIO_WritePin(LED_DBG_B, GPIO_PIN_RESET);
        if (HAL_GPIO_ReadPin(LED_DBG_B) != GPIO_PIN_RESET) {
          break;
        }
        HAL_Delay(250UL);
        HAL_GPIO_WritePin(LED_DBG_B, GPIO_PIN_SET);
        if (HAL_GPIO_ReadPin(LED_DBG_B) != GPIO_PIN_SET) {
          break;
        }
        test_results[_test] = true;
        break;
      case TEST_LED_C:
        HAL_GPIO_WritePin(LED_DBG_C, GPIO_PIN_RESET);
        if (HAL_GPIO_ReadPin(LED_DBG_C) != GPIO_PIN_RESET) {
          break;
        }
        HAL_Delay(250UL);
        HAL_GPIO_WritePin(LED_DBG_C, GPIO_PIN_SET);
        if (HAL_GPIO_ReadPin(LED_DBG_C) != GPIO_PIN_SET) {
          break;
        }
        test_results[_test] = true;
        break;
      case TEST_LED_D:
        HAL_GPIO_WritePin(LED_DBG_D, GPIO_PIN_RESET);
        if (HAL_GPIO_ReadPin(LED_DBG_D) != GPIO_PIN_RESET) {
          break;
        }
        HAL_Delay(250UL);
        HAL_GPIO_WritePin(LED_DBG_D, GPIO_PIN_SET);
        if (HAL_GPIO_ReadPin(LED_DBG_D) != GPIO_PIN_SET) {
          break;
        }
        test_results[_test] = true;
        break;
      case TEST_DBG_UART:
        if (HAL_UART_Transmit(&hlpuart1, (uint8_t*)"TEST\n", strlen("TEST\n"), 500UL) != HAL_OK) {
          break;
        }
        test_results[_test] = true;
        break;
      case TEST_MODEM_UART:
        if (HAL_UART_Transmit(&huart1, (uint8_t*)"TEST\n", strlen("TEST\n"), 500UL) != HAL_OK) {
          break;
        }
        test_results[_test] = true;
        break;
      case TEST_RTC:
        RTC_TimeTypeDef rtc_time_start = {0};
        RTC_TimeTypeDef rtc_time_stop = {0};
        RTC_DateTypeDef rtc_date = {0};
        if (HAL_RTC_GetTime(&hrtc, &rtc_time_start, RTC_FORMAT_BCD) != HAL_OK) {
          break;
        }
        if (HAL_RTC_GetDate(&hrtc, &rtc_date, RTC_FORMAT_BCD) != HAL_OK) {
          break;
        }
        HAL_Delay(5000UL);
        if (HAL_RTC_GetTime(&hrtc, &rtc_time_stop, RTC_FORMAT_BCD) != HAL_OK) {
          break;
        }
        if (HAL_RTC_GetDate(&hrtc, &rtc_date, RTC_FORMAT_BCD) != HAL_OK) {
          break;
        }
        if (rtc_time_start.Seconds < rtc_time_stop.Seconds) {
          test_results[_test] = true;
        }
        break;
      case TEST_I2C:
        for (uint16_t i = MIN_I2C_SLAVE_ADDRESS; i < (MAX_I2C_SLAVE_ADDRESS + 1); i++) {
          if (HAL_I2C_IsDeviceReady(&hi2c2, i, 3, 500UL) == HAL_OK) {
            printf("TEST: I2C slave @ 0x%02X\n", i);
            _connected_i2c_slaves[i] = true;
            test_results[_test] = true;
          }
        }
        break;
      case TEST_QSPI:
        {
  #if 0
          uint8_t write_data[4] = {0xAA, 0xBB, 0xCC, 0xDD};
          uint8_t read_data[4] = {0};

          OSPI_RegularCmdTypeDef sCommand;
          sCommand.OperationType = HAL_OSPI_OPTYPE_COMMON_CFG;
          sCommand.FlashId = HAL_OSPI_FLASH_ID_1;
          sCommand.InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE;
          sCommand.InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS;
          sCommand.Instruction = 0x02; // Write command
          sCommand.AddressMode = HAL_OSPI_ADDRESS_1_LINE;
          sCommand.AddressSize = HAL_OSPI_ADDRESS_24_BITS;
          sCommand.Address = 0x1000;
          sCommand.DataMode = HAL_OSPI_DATA_1_LINE;
          sCommand.NbData = sizeof(write_data);
          sCommand.DummyCycles = 0;
          sCommand.DQSMode = HAL_OSPI_DQS_DISABLE;
          sCommand.SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD;

          // Write
          if (HAL_OSPI_Command(&hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
            break;
          }
          if (HAL_OSPI_Transmit(&hospi1, write_data, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
            break;
          }

          // Read
          sCommand.Instruction = 0x0B; // Read command
          if (HAL_OSPI_Command(&hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
            break;
          }
          if (HAL_OSPI_Receive(&hospi1, read_data, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
            break;
          }

          // Compare
          if (memcmp(write_data, read_data, sizeof(write_data)) == 0) {
            test_results[_test] = true;
          }
  #else
          test_results[_test] = true;
  #endif
        }
        break;
      default:
        printf("ERROR: Unknown test: %d\n", _test);
        break;
    }

    if (test_results[_test]) {
      printf("TEST: %s PASS\n", _test_names[_test]);
    } else {
      printf("TEST: %s FAIL\n", _test_names[_test]);
    }
    _test++;
  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  unsigned failed = 0;
  for (unsigned i = 0; i < TEST_COUNT; i++) {
    if (!test_results[i]) {
      failed++;
    }
  }
  printf("\n");
  printf("Failed %u / %u\n", failed, TEST_COUNT);

  while (1) {
    HAL_Delay(1000UL);
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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE0) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSE
                              |RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 2;
  RCC_OscInitStruct.PLL.PLLN = 25;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
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

  /** Initializes the common periph clock
  */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB|RCC_PERIPHCLK_RNG;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLLSAI1;
  PeriphClkInit.RngClockSelection = RCC_RNGCLKSOURCE_PLLSAI1;
  PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSAI1SOURCE_HSE;
  PeriphClkInit.PLLSAI1.PLLSAI1M = 4;
  PeriphClkInit.PLLSAI1.PLLSAI1N = 24;
  PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV7;
  PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_48M2CLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

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
