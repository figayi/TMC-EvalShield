/*
 * UART.c
 *
 *  Created on: 23.01.2020
 *      Author: LK
 */

#include "UART.h"
#include "Pins.h"

TMC_UART UART0 = {
	.con = {
		.init = UART_Init,
		.deInit = UART_deInit,
		.tx = UART_tx,
		.rx = UART_rx
	}
};

void UART_tx(uint8_t *data, uint16_t size, uint32_t timeout)
{
	if (HAL_UART_Transmit(&UART0.huart, data, size, timeout) != HAL_OK)
	{
		Error_Handler();
	}
}

void UART_rx(uint8_t *data, uint16_t size, uint32_t timeout)
{
	if (HAL_UART_Receive(&UART0.huart, data, size, timeout) != HAL_OK)
	{
		Error_Handler();
	}
}

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
  UART0.huart.Instance = USART2;
  UART0.huart.Init.BaudRate = 115200;
  UART0.huart.Init.WordLength = UART_WORDLENGTH_8B;
  UART0.huart.Init.StopBits = UART_STOPBITS_1;
  UART0.huart.Init.Parity = UART_PARITY_NONE;
  UART0.huart.Init.Mode = UART_MODE_TX_RX;
  UART0.huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  UART0.huart.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&UART0.huart) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

void UART_deInit(void)
{
	if (HAL_UART_DeInit(&UART0.huart) != HAL_OK)
	{
		Error_Handler();
	}
}
