/*
 * main.c
 *
 *  Created on: 23.01.2020
 *      Author: LK
 */

#include "main.h"

#include "GPIO.h"
#include "SPI.h"
#include "UART.h"
#include "USB.h"
#include "TMCL.h"
#include "boards/Board.h"

#include "ic/TMC5160/TMC5160.h"
#include "helpers/Config.h"

void SystemClock_Config(void);

const char *VersionString = MODULE_ID"V100";

int main(void) {
	HAL_Init();

	SystemClock_Config();

	GPIO_init();
	UART0_Init();
	SPIX_init();
	USB_Init();

	Boards_init();

	tmcl_init();

	while (1) {
//		volatile GPIO_PinState state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6);
//	    HAL_Delay(10);
//	    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
//	    state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6);
//	    HAL_Delay(10);
//	    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
		board[0].periodicJob(HAL_GetTick());
		tmcl_process();
		HAL_Delay(1);
	}
}

void SystemClock_Config(void) {
	  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

	  /** Initializes the CPU, AHB and APB busses clocks
	  */
	  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
	  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
	  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  /** Initializes the CPU, AHB and APB busses clocks
	  */
	  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
	                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
	  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL;
	  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	  {
	    Error_Handler();
	  }
}

void Error_Handler(void) {
	asm volatile("bkpt 255");
	while (1);
}

void deInit(void) {
	//GPIO_deInit();
	UART0_deInit();
	//SPI1_deInit();
}

void reset(void) {
	deInit();
	main();
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
	 tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
