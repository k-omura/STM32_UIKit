/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <FSMC_ILI9341.h>
#include <bitmap.h>
#include <touch_2046.h>
#include <stm32uikit.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TERMINAL_LINE_MAX 30
#define COLOR_BACKGROUND 0x00

#define XPT2046_MIN_RAW_X 230
#define XPT2046_MAX_RAW_X 3900
#define XPT2046_MIN_RAW_Y 290
#define XPT2046_MAX_RAW_Y 3870
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi2;

SRAM_HandleTypeDef hsram1;

/* USER CODE BEGIN PV */
uint8_t frameBuffer[ILI9341_PIXEL_COUNT] = {0};

uint8_t touch_it = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_FSMC_Init(void);
static void MX_SPI2_Init(void);
static void MX_DMA_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	switch (GPIO_Pin) {
	case GPIO_PIN_5:
		touch_it++;
		break;
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
  MX_FSMC_Init();
  MX_SPI2_Init();
  MX_DMA_Init();
  /* USER CODE BEGIN 2 */

  // Touch init
  Coordinate_t touch_s3uikit;
	Touch_calib_t touch_cal;
	touch_cal.minX = XPT2046_MIN_RAW_X;
	touch_cal.minY = XPT2046_MIN_RAW_Y;
	touch_cal.maxX = XPT2046_MAX_RAW_X;
	touch_cal.maxY = XPT2046_MAX_RAW_Y;

  // LCD init
	ILI9341_init();
	ILI9341_setRotation(1);

	//----------bitmap init
	char string[30];
	bitmap_setparam(ILI9341_WIDTH, ILI9341_HEIGHT, COLOR_BACKGROUND, frameBuffer);
	bitmap_clear();

	//color test
	for (uint16_t i = 0; i < 256; i++) {
		bitmap_fillrect(15 * (i % 16), ((uint16_t) (i / 16) * 15), 15 * (i % 16) + 14, ((uint16_t) (i / 16) * 15) + 14, i);
	}
	bitmap_stringBitmap(20, 260, "STM32 UIKit", 2, 1, 0xff);

	ILI9341_printBitmap(frameBuffer);
	bitmap_clear();
	HAL_Delay(2000);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	uint16_t slide_val = 100;

	uint8_t switch_val1 = 0;
	uint8_t button_val1 = 0;
	uint8_t button_val2 = 0;
	int16_t button_conter1 = 0;
	int16_t button_conter2 = 0;

	while (1) {
		//get touched point
		touch_s3uikit = xpt2046_read(&hspi2, touch_cal);
		if(touch_s3uikit.z < 100){
			touch_s3uikit.x = 0;
			touch_s3uikit.y = 0;
			touch_s3uikit.z = 0;
		}

		//bar
		bitmap_stringBitmap(10, 10, "x", 1, 1, 0xff);
		bitmap_stringBitmap(10, 20, "y", 1, 1, 0xff);
		stm32uikit_roundProgress(20, 10, 210, 1000 * touch_s3uikit.x / 240);
		stm32uikit_rectProgress(20, 20, 210, 1000 * touch_s3uikit.y / 320);

		//slide bar
		stm32uikit_sllideBar(touch_s3uikit, 10, 100, 190, &slide_val);
		sprintf(string, "%4d", slide_val);
		bitmap_stringBitmap(205, 100, string, 1, 1, 0xff);

		//analog meter
		bitmap_stringBitmap(110, 80, "Slide", 0, 0, 0xff);
		stm32uikit_analogMeter(80, 50, slide_val);

		//switch
		stm32uikit_switch(touch_s3uikit, 180, 250, &switch_val1);

		//button
		stm32uikit_roundButton(touch_s3uikit, 10, 280, 105, &button_val1);
		if(button_val1 == 2){
			if(switch_val1){
				button_conter1++;
			}else{
				button_conter1--;
			}
		}
		sprintf(string, "%5d", button_conter1);
		bitmap_stringBitmap(15, 288, string, 2, 1, 0xff);
		stm32uikit_roundButton(touch_s3uikit, 120, 280, 105, &button_val2);
		if(button_val2 == 2){
			if(switch_val1){
				button_conter2++;
			}else{
				button_conter2--;
			}
		}
		sprintf(string, "%5d", button_conter2);
		bitmap_stringBitmap(135, 288, string, 2, 1, 0xff);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		ILI9341_printBitmap(frameBuffer);
		bitmap_clear();
		//HAL_Delay(10);
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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_HARD_OUTPUT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
  /* DMA2_Stream6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream6_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pins : btn_k1_Pin btn_k0_Pin */
  GPIO_InitStruct.Pin = btn_k1_Pin|btn_k0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : touch_it_Pin */
  GPIO_InitStruct.Pin = touch_it_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(touch_it_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}

/* FSMC initialization function */
static void MX_FSMC_Init(void)
{

  /* USER CODE BEGIN FSMC_Init 0 */

  /* USER CODE END FSMC_Init 0 */

  FSMC_NORSRAM_TimingTypeDef Timing = {0};

  /* USER CODE BEGIN FSMC_Init 1 */

  /* USER CODE END FSMC_Init 1 */

  /** Perform the SRAM1 memory initialization sequence
  */
  hsram1.Instance = FSMC_NORSRAM_DEVICE;
  hsram1.Extended = FSMC_NORSRAM_EXTENDED_DEVICE;
  /* hsram1.Init */
  hsram1.Init.NSBank = FSMC_NORSRAM_BANK1;
  hsram1.Init.DataAddressMux = FSMC_DATA_ADDRESS_MUX_DISABLE;
  hsram1.Init.MemoryType = FSMC_MEMORY_TYPE_SRAM;
  hsram1.Init.MemoryDataWidth = FSMC_NORSRAM_MEM_BUS_WIDTH_16;
  hsram1.Init.BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE;
  hsram1.Init.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW;
  hsram1.Init.WrapMode = FSMC_WRAP_MODE_DISABLE;
  hsram1.Init.WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS;
  hsram1.Init.WriteOperation = FSMC_WRITE_OPERATION_ENABLE;
  hsram1.Init.WaitSignal = FSMC_WAIT_SIGNAL_DISABLE;
  hsram1.Init.ExtendedMode = FSMC_EXTENDED_MODE_DISABLE;
  hsram1.Init.AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE;
  hsram1.Init.WriteBurst = FSMC_WRITE_BURST_DISABLE;
  hsram1.Init.PageSize = FSMC_PAGE_SIZE_NONE;
  /* Timing */
  Timing.AddressSetupTime = 6;
  Timing.AddressHoldTime = 15;
  Timing.DataSetupTime = 6;
  Timing.BusTurnAroundDuration = 0;
  Timing.CLKDivision = 16;
  Timing.DataLatency = 17;
  Timing.AccessMode = FSMC_ACCESS_MODE_A;
  /* ExtTiming */

  if (HAL_SRAM_Init(&hsram1, &Timing, NULL) != HAL_OK)
  {
    Error_Handler( );
  }

  /* USER CODE BEGIN FSMC_Init 2 */

  /* USER CODE END FSMC_Init 2 */
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


