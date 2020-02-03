/*
 * UART.c
 *
 *  Created on: 23.01.2020
 *      Author: LK
 */

#include "UART.h"
#include "Pins.h"
#include "RXTX.h"

TMC_UART UART0 = {
	.con = {
		.init = UART0_Init,
		.deInit = UART0_deInit,
		.tx = UART0_tx,
		.rx = UART0_rx,
		.dataAvailable = UART0_dataAvailable
	}
};

static TMC_RXTX_Buffer buf_rx;
static uint8_t buffer[256];

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
void UART0_Init(void)
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

  HAL_UART_Receive_DMA(&UART0.huart, &buffer[0], 256);

  /* USER CODE END USART2_Init 2 */

}

void UART0_deInit(void)
{
	if (HAL_UART_DeInit(&UART0.huart) != HAL_OK)
	{
		Error_Handler();
	}
}

void UART0_tx(uint8_t *data, uint16_t size, uint32_t timeout)
{
	if (HAL_UART_Transmit(&UART0.huart, data, size, timeout) != HAL_OK)
	{
		Error_Handler();
	}
}

void UART0_rx(uint8_t *data, uint16_t size, uint32_t timeout)
{
	if (HAL_UART_Receive(&UART0.huart, data, size, timeout) != HAL_OK)
	{
		Error_Handler();
	}
}

uint16_t UART0_dataAvailable(void)
{

	return 0;
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
void UART_Init(TMC_UART *interface)
{
	interface->con.init();
}

void UART_deInit(TMC_UART *interface)
{
	interface->con.deInit();
}

void UART_tx(TMC_UART *interface, uint8_t *data, uint16_t size, uint32_t timeout)
{
	interface->con.tx(data, size, timeout);
}

void UART_rx(TMC_UART *interface, uint8_t *data, uint16_t size, uint32_t timeout)
{
	interface->con.rx(data, size, timeout);
}

uint16_t UART_dataAvailable(TMC_UART *interface)
{
	return interface->con.dataAvailable();
}


