/*
 * UART.c
 *
 *  Created on: 23.01.2020
 *      Author: LK
 */

#include "UART.h"
#include "Pins.h"
#include "RXTX.h"

extern void Error_Handler(void);

TMC_UART UART0 = {
	.con = {
		.init = UART0_Init,
		.deInit = UART0_deInit,
		.txN = UART0_txN,
		.rxN = UART0_rxN,
		.rx = UART0_rx,
		.dataAvailable = UART0_dataAvailable,
		.resetBuffers = UART0_resetBuffers,
		.status = TMC_CONNECTION_STATUS_READY,
		.continuous = 0
	}
};

static void UART0_DMA_Init(void);

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
void UART0_Init(void)
{
	UART0_DMA_Init();

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

static void UART0_DMA_Init(void)
{
	/* DMA controller clock enable */
	__HAL_RCC_DMA1_CLK_ENABLE();

	/* DMA interrupt init */
	/* DMA1_Channel6_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);
}

void UART0_deInit(void)
{
	if (HAL_UART_DeInit(&UART0.huart) != HAL_OK)
	{
		Error_Handler();
	}
}

void UART0_txN(uint8_t *data, uint16_t size, uint32_t timeout)
{
	if (HAL_UART_Transmit(&UART0.huart, data, size, timeout) != HAL_OK)
	{
		Error_Handler();
	}
}

void UART0_rxN(uint8_t *data, uint16_t size, uint32_t timeout)
{
	UNUSED(timeout);
	//if(UART0.con.status == TMC_CONNECTION_STATUS_BUSY)
		//return;
	UART0.con.word = size;
	//TMC_RXTX_resetBuffer(&UART0.buffer_rx);
	if (HAL_UART_Receive_DMA(&UART0.huart, data ? data : (&UART0.buffer_rx.buffer[0]), data ? size : TMC_RXTX_BUFFER_SIZE) == HAL_OK)
		UART0.con.status = TMC_CONNECTION_STATUS_BUSY;
	else
		Error_Handler();
}

uint8_t UART0_rx(void)
{
	uint8_t d = 0xFF;
	if(UART0_dataAvailable()) {
		TMC_RXTX_readBuffer(&UART0.buffer_rx, &d, 1);
	}
	return d;
}

size_t UART0_dataAvailable(void)
{
	size_t delta = 0;
	uint32_t cndtr = UART0.hdma_rx.Instance->CNDTR;
	if(UART0.cndtr >= cndtr)
		delta = UART0.cndtr - cndtr;
	else
		delta = TMC_RXTX_BUFFER_SIZE - (cndtr - UART0.cndtr);
	UART0.buffer_rx.target = (UART0.buffer_rx.target + delta) % TMC_RXTX_BUFFER_SIZE;
	UART0.cndtr = cndtr;
	return TMC_RXTX_dataAvailable(&UART0.buffer_rx);
}

void UART0_resetBuffers(void)
{
	HAL_UART_DMAResume(&UART0.huart);
	//TMC_RXTX_resetBuffer(&UART0.buffer_rx);
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

void UART_txN(TMC_UART *interface, uint8_t *data, uint16_t size, uint32_t timeout)
{
	interface->con.txN(data, size, timeout);
}

void UART_rxN(TMC_UART *interface, uint8_t *data, uint16_t size, uint32_t timeout)
{
	interface->con.rxN(data, size, timeout);
}

uint8_t UART_rx(TMC_UART *interface)
{
	return interface->con.rx();
}

size_t UART_dataAvailable(TMC_UART *interface)
{
	return interface->con.dataAvailable();
}

void UART_resetBuffers(TMC_UART *interface)
{
	return interface->con.resetBuffers();
}

/**
* @brief UART MSP Initialization
* This function configures the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(huart->Instance==USART2)
	{
		/* USER CODE BEGIN USART2_MspInit 0 */

		/* USER CODE END USART2_MspInit 0 */
		/* Peripheral clock enable */
		__HAL_RCC_USART2_CLK_ENABLE();

		__HAL_RCC_GPIOA_CLK_ENABLE();
		/**USART2 GPIO Configuration
		PA2     ------> USART2_TX
		PA3     ------> USART2_RX
		*/
		GPIO_InitStruct.Pin = USART_TX_Pin|USART_RX_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* USART2 DMA Init */
		/* USART2_RX Init */
		UART0.hdma_rx.Instance = DMA1_Channel6;
		UART0.hdma_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
		UART0.hdma_rx.Init.PeriphInc = DMA_PINC_DISABLE;
		UART0.hdma_rx.Init.MemInc = DMA_MINC_ENABLE;
		UART0.hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		UART0.hdma_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		UART0.hdma_rx.Init.Mode = DMA_CIRCULAR;
		UART0.hdma_rx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
		if (HAL_DMA_Init(&UART0.hdma_rx) != HAL_OK)
		{
			Error_Handler();
		}

		__HAL_LINKDMA(huart,hdmarx,UART0.hdma_rx);

		/* USER CODE BEGIN USART2_MspInit 1 */

		/* USER CODE END USART2_MspInit 1 */
	}

}

/**
* @brief UART MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
	if(huart->Instance==USART2)
	{
		/* USER CODE BEGIN USART2_MspDeInit 0 */

		/* USER CODE END USART2_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_USART2_CLK_DISABLE();

		/**USART2 GPIO Configuration
		PA2     ------> USART2_TX
		PA3     ------> USART2_RX
		*/
		HAL_GPIO_DeInit(GPIOA, USART_TX_Pin|USART_RX_Pin);

		/* USART2 DMA DeInit */
		HAL_DMA_DeInit(huart->hdmarx);
		/* USER CODE BEGIN USART2_MspDeInit 1 */

		/* USER CODE END USART2_MspDeInit 1 */
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
//	TMC_RXTX_incrementBuffer(&UART0.buffer_rx, UART0.con.word);
//	if(UART0.con.continuous)
//		UART0_rxN(NULL, UART0.con.word, 0);
	//HAL_UART_DMAPause(huart);
//	if(huart == &UART0.huart) {
//		TMC_RXTX_incrementBuffer(&UART0.buffer_rx, UART0.con.word);
//		UART0.con.status = TMC_CONNECTION_STATUS_READY;
//		if(UART0.con.continuous)
//			UART0_rxN(NULL, UART0.con.word, 0);
//	}
}
