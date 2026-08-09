/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ADXL345_CS_GPIO_Port GPIOB
#define ADXL345_CS_Pin       GPIO_PIN_0
#define ADXL345_DEVICE_ID    0xE5
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

static uint8_t uart_rx_buffer[32];
static uint8_t uart_rx_byte;
static uint8_t uart_rx_index = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */

static void UART_SendString(const char *str);
static void Process_Command(char *command);
static void Test_UART(void);
static void Test_GPIO(void);
static void Test_ADC(void);
static void Test_I2C(void);
static void Test_SPI(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

static void UART_SendString(const char *str)
{
    HAL_UART_Transmit(&huart1,
                      (uint8_t *)str,
                      strlen(str),
                      HAL_MAX_DELAY);
}

static void Test_UART(void)
{
    UART_SendString("[TEST:UART][RESULT:PASS]\r\n");
}

static void Test_GPIO(void)
{
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

    UART_SendString("[TEST:GPIO][RESULT:PASS]\r\n");
}

static void Process_Command(char *command)
{
    if (strcmp(command, "HELP") == 0)
    {
        UART_SendString("\r\n");
        UART_SendString("STM32 VALIDATION FRAMEWORK\r\n");
        UART_SendString("Commands:\r\n");
        UART_SendString("HELP\r\n");
        UART_SendString("TEST UART\r\n");
        UART_SendString("TEST GPIO\r\n");
        UART_SendString("TEST ADC\r\n");
        UART_SendString("TEST I2C\r\n");
        UART_SendString("TEST SPI\r\n");
        UART_SendString("TEST ALL\r\n");
        UART_SendString("\r\n");
    }
    else if (strcmp(command, "TEST UART") == 0)
    {
        Test_UART();
    }
    else if (strcmp(command, "TEST GPIO") == 0)
    {
        Test_GPIO();
    }
    else if (strcmp(command, "TEST ADC") == 0)
    {
        Test_ADC();
    }
    else if (strcmp(command, "TEST I2C") == 0)
    {
        Test_I2C();
    }
    else if (strcmp(command, "TEST SPI") == 0)
    {
        Test_SPI();
    }
    else if (strcmp(command, "TEST ALL") == 0)
    {
        Test_UART();
        Test_GPIO();
        Test_ADC();
        Test_I2C();
        Test_SPI();

        UART_SendString("[TEST:ALL][RESULT:PASS]\r\n");
    }
    else
    {
        UART_SendString("[ERROR][UNKNOWN_COMMAND]\r\n");
    }
}

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

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */

  UART_SendString("\r\n====================================\r\n");
  UART_SendString("STM32 VALIDATION FRAMEWORK\r\n");
  UART_SendString("Firmware Core v0.3\r\n");
  UART_SendString("UART: 115200 8-N-1\r\n");
  UART_SendString("ADC: 12-bit / PA0\r\n");
  UART_SendString("Type HELP for commands\r\n");
  UART_SendString("====================================\r\n\r\n");

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

      if (HAL_UART_Receive(&huart1,
                           &uart_rx_byte,
                           1,
                           HAL_MAX_DELAY) == HAL_OK)
      {
          if (uart_rx_byte == '\r' || uart_rx_byte == '\n')
          {
              if (uart_rx_index > 0)
              {
                  uart_rx_buffer[uart_rx_index] = '\0';

                  Process_Command((char *)uart_rx_buffer);

                  uart_rx_index = 0;
                  memset(uart_rx_buffer, 0, sizeof(uart_rx_buffer));
              }
          }
          else
          {
              if (uart_rx_index < sizeof(uart_rx_buffer) - 1)
              {
                  uart_rx_buffer[uart_rx_index++] = uart_rx_byte;
              }
              else
              {
                  uart_rx_index = 0;
                  memset(uart_rx_buffer, 0, sizeof(uart_rx_buffer));

                  UART_SendString("[ERROR][COMMAND_TOO_LONG]\r\n");
              }
          }
      }
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PB0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
static void Test_SPI(void)
{
    uint8_t tx_data[2];
    uint8_t rx_data[2];
    uint8_t device_id;

    tx_data[0] = 0x80 | 0x00;   // Read command, register 0x00
    tx_data[1] = 0x00;

    HAL_GPIO_WritePin(ADXL345_CS_GPIO_Port,
                      ADXL345_CS_Pin,
                      GPIO_PIN_RESET);

    if (HAL_SPI_TransmitReceive(&hspi1,
                                tx_data,
                                rx_data,
                                2,
                                100) != HAL_OK)
    {
        HAL_GPIO_WritePin(ADXL345_CS_GPIO_Port,
                          ADXL345_CS_Pin,
                          GPIO_PIN_SET);

        UART_SendString("[TEST:SPI][RESULT:FAIL][HAL]\r\n");
        return;
    }

    HAL_GPIO_WritePin(ADXL345_CS_GPIO_Port,
                      ADXL345_CS_Pin,
                      GPIO_PIN_SET);

    device_id = rx_data[1];

    if (device_id == ADXL345_DEVICE_ID)
    {
        UART_SendString("[TEST:SPI][RESULT:PASS][DEVID:0xE5]\r\n");
    }
    else
    {
        char message[64];

        snprintf(message,
                 sizeof(message),
                 "[TEST:SPI][RESULT:FAIL][DEVID:0x%02X]\r\n",
                 device_id);

        UART_SendString(message);
    }
}

static void Test_ADC(void)
{
    uint32_t sum = 0;
    uint16_t min = 4095;
    uint16_t max = 0;
    uint16_t sample;
    uint32_t average;
    uint32_t voltage_mv;
    char message[96];

    for (uint8_t i = 0; i < 20; i++)
    {
        if (HAL_ADC_Start(&hadc1) != HAL_OK)
        {
            UART_SendString("[TEST:ADC][RESULT:FAIL]\r\n");
            return;
        }

        if (HAL_ADC_PollForConversion(&hadc1, 100) != HAL_OK)
        {
            HAL_ADC_Stop(&hadc1);
            UART_SendString("[TEST:ADC][RESULT:FAIL]\r\n");
            return;
        }

        sample = HAL_ADC_GetValue(&hadc1);

        HAL_ADC_Stop(&hadc1);

        sum += sample;

        if (sample < min)
        {
            min = sample;
        }

        if (sample > max)
        {
            max = sample;
        }

        HAL_Delay(5);
    }

    average = sum / 20;

    voltage_mv = (average * 3300UL) / 4095UL;

    UART_SendString("[TEST:ADC][RESULT:PASS]\r\n");

    snprintf(message,
             sizeof(message),
             "[ADC:AVG:%lu][MIN:%u][MAX:%u][MV:%lu]\r\n",
             average,
             min,
             max,
             voltage_mv);

    UART_SendString(message);
}

static void Test_I2C(void)
{
    HAL_StatusTypeDef status;

    status = HAL_I2C_IsDeviceReady(&hi2c1,
                                   (0x48 << 1),
                                   3,
                                   100);

    if (status == HAL_OK)
    {
        UART_SendString("[TEST:I2C][RESULT:PASS][ADDR:0x48]\r\n");
    }
    else
    {
        UART_SendString("[TEST:I2C][RESULT:FAIL][ADDR:0x48]\r\n");
    }
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
#ifdef USE_FULL_ASSERT
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
