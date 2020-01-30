/*
 * UART.c
 *
 *  Created on: 23.01.2020
 *      Author: LK
 */

#include "UART.h"
#include "Pins.h"
#include "stm32f1xx_hal.h"

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
void UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  UART.huart.Instance = USART2;
  UART.huart.Init.BaudRate = 115200;
  UART.huart.Init.WordLength = UART_WORDLENGTH_8B;
  UART.huart.Init.StopBits = UART_STOPBITS_1;
  UART.huart.Init.Parity = UART_PARITY_NONE;
  UART.huart.Init.Mode = UART_MODE_TX_RX;
  UART.huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  UART.huart.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&UART.huart) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

void UART_deInit(void)
{
	if (HAL_UART_DeInit(&UART.huart) != HAL_OK)
	{
		Error_Handler();
	}
}
