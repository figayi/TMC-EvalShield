/*
 * UART.c
 *
 *  Created on: 23.01.2020
 *      Author: LK
 */

#include "UART.h"
#include "RXTX.h"

extern void Error_Handler(void);

TMC_UART UART0 = {
	.con = {
		.init = UART0_Init,
		.deInit = UART0_deInit,
		.txRequest = UART0_txRequest,
		.rxRequest = UART0_rxRequest,
		.txrx = Connection_txrx,
		.txN = Connection_txN,
		.rxN = UART0_rxN,
		.rx = UART0_rx,
		.dataAvailable = UART0_dataAvailable,
		.resetBuffers = UART0_resetBuffers,
		.status = TMC_CONNECTION_STATUS_READY
	}
};

static void UART0_DMA_Init(void);

////////////////////////////////////////////////////////////////////////////////
// Specific functions
////////////////////////////////////////////////////////////////////////////////

void UART0_Init(void) {
	UART0_DMA_Init();

	UART0.huart.Instance = USART3;
	UART0.huart.Init.BaudRate = 9600;
	UART0.huart.Init.WordLength = UART_WORDLENGTH_8B;
	UART0.huart.Init.StopBits = UART_STOPBITS_1;
	UART0.huart.Init.Parity = UART_PARITY_NONE;
	UART0.huart.Init.Mode = UART_MODE_TX_RX;
	UART0.huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	UART0.huart.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&UART0.huart) != HAL_OK) {
		Error_Handler();
	}

}

static void UART0_DMA_Init(void) {
	/* DMA controller clock enable */
	__HAL_RCC_DMA1_CLK_ENABLE();

	/* DMA interrupt init */
	/* DMA1_Channel6_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);
}

void UART0_deInit(void) {
	if (HAL_UART_DeInit(&UART0.huart) != HAL_OK) {
		Error_Handler();
	}
}

void UART0_txRequest(uint8_t *data, uint16_t size, uint32_t timeout) {
	if (HAL_UART_Transmit(&UART0.huart, data, size, timeout) != HAL_OK) {
		Error_Handler();
	}
}

void UART0_rxRequest(uint8_t *data, uint16_t size, uint32_t timeout) {
	UNUSED(timeout);
	UART0.con.word = size;
	UART0.cndtr = size;
	//UART0.extra = (uint8_t*)malloc(size);
	if (HAL_UART_Receive_DMA(&UART0.huart, data ? data : (&UART0.buffer_rx.buffer[0]), data ? size : TMC_RXTX_BUFFER_SIZE) == HAL_OK)
		UART0.con.status = TMC_CONNECTION_STATUS_BUSY;
	else
		Error_Handler();
}

void UART0_rxN(uint8_t *data, size_t size) {
	if(UART0_dataAvailable() >= size)
		TMC_RXTX_readBuffer(&UART0.buffer_rx, data, size);
}

uint8_t UART0_rx(void) {
	uint8_t d = 0xFF;
	if (UART0_dataAvailable()) {
		TMC_RXTX_readBuffer(&UART0.buffer_rx, &d, 1);
	}
	return d;
}

size_t UART0_dataAvailable(void) {
	uint32_t cndtr = UART0.hdma_rx.Instance->CNDTR;
	if(cndtr <= UART0.cndtr)
		TMC_RXTX_incrementBuffer(&UART0.buffer_rx, UART0.cndtr - cndtr);
	else
		TMC_RXTX_incrementBuffer(&UART0.buffer_rx, TMC_RXTX_BUFFER_SIZE + UART0.cndtr - cndtr);
	UART0.cndtr = cndtr;
	return TMC_RXTX_dataAvailable(&UART0.buffer_rx);
}

void UART0_resetBuffers(void) {
	HAL_UART_AbortReceive(&UART0.huart);
	TMC_RXTX_resetBuffer(&UART0.buffer_rx);
}

////////////////////////////////////////////////////////////////////////////////
// Generic functions
////////////////////////////////////////////////////////////////////////////////

void UART_Init(TMC_UART *interface) {
	interface->con.init();
}

void UART_deInit(TMC_UART *interface) {
	interface->con.deInit();
}

void UART_txRequest(TMC_UART *interface, uint8_t *data, uint16_t size, uint32_t timeout) {
	interface->con.txRequest(data, size, timeout);
}

void UART_rxRequest(TMC_UART *interface, uint8_t *data, uint16_t size, uint32_t timeout) {
	interface->con.rxRequest(data, size, timeout);
}

void UART_txN(TMC_UART *interface, size_t size) {
	interface->con.txN(size);
}

void UART_rxN(TMC_UART *interface, uint8_t *data, size_t size) {
	interface->con.rxN(data, size);
}

uint8_t UART_rx(TMC_UART *interface) {
	return interface->con.rx();
}

size_t UART_dataAvailable(TMC_UART *interface) {
	return interface->con.dataAvailable();
}

void UART_resetBuffers(TMC_UART *interface) {
	return interface->con.resetBuffers();
}

void HAL_UART_MspInit(UART_HandleTypeDef* huart) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	if(huart->Instance == USART2) {
		/* USER CODE BEGIN USART2_MspInit 0 */

		/* USER CODE END USART2_MspInit 0 */
		/* Peripheral clock enable */
		__HAL_RCC_USART2_CLK_ENABLE();

		__HAL_RCC_GPIOA_CLK_ENABLE();
		/**USART2 GPIO Configuration
		 PA2     ------> USART2_TX
		 PA3     ------> USART2_RX
		 */
		GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
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
		if (HAL_DMA_Init(&UART0.hdma_rx) != HAL_OK) {
			Error_Handler();
		}

		__HAL_LINKDMA(huart, hdmarx, UART0.hdma_rx);
	} else if(huart->Instance == USART3) {
		/* USER CODE BEGIN USART2_MspInit 0 */

		/* USER CODE END USART2_MspInit 0 */
		/* Peripheral clock enable */
		__HAL_RCC_USART3_CLK_ENABLE();

		__HAL_RCC_GPIOC_CLK_ENABLE();
		/**USART2 GPIO Configuration
		 PA2     ------> USART2_TX
		 PA3     ------> USART2_RX
		 */
		GPIO_InitStruct.Pin = GPIO_PIN_10;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_11;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

		__HAL_AFIO_REMAP_USART3_PARTIAL();

		/* USART2 DMA Init */
		/* USART2_RX Init */
		UART0.hdma_rx.Instance = DMA1_Channel3;
		UART0.hdma_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
		UART0.hdma_rx.Init.PeriphInc = DMA_PINC_DISABLE;
		UART0.hdma_rx.Init.MemInc = DMA_MINC_ENABLE;
		UART0.hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		UART0.hdma_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		UART0.hdma_rx.Init.Mode = DMA_CIRCULAR;
		UART0.hdma_rx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
		if (HAL_DMA_Init(&UART0.hdma_rx) != HAL_OK) {
			Error_Handler();
		}

		__HAL_LINKDMA(huart, hdmarx, UART0.hdma_rx);
	}
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* huart) {
	if(huart->Instance == USART2) {
		/* USER CODE BEGIN USART2_MspDeInit 0 */

		/* USER CODE END USART2_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_USART2_CLK_DISABLE();

		/**USART2 GPIO Configuration
		 PA2     ------> USART2_TX
		 PA3     ------> USART2_RX
		 */
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2 | GPIO_PIN_3);

		/* USART2 DMA DeInit */
		HAL_DMA_DeInit(huart->hdmarx);
		/* USER CODE BEGIN USART2_MspDeInit 1 */

		/* USER CODE END USART2_MspDeInit 1 */
	} else if(huart->Instance == USART3) {
		/* USER CODE BEGIN USART2_MspDeInit 0 */

		/* USER CODE END USART2_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_USART3_CLK_DISABLE();

		/**USART2 GPIO Configuration
		 PA2     ------> USART2_TX
		 PA3     ------> USART2_RX
		 */
		HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10 | GPIO_PIN_11);

		/* USART2 DMA DeInit */
		HAL_DMA_DeInit(huart->hdmarx);
		/* USER CODE BEGIN USART2_MspDeInit 1 */

		/* USER CODE END USART2_MspDeInit 1 */
	}
}

//void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart) {
//	if(huart == &UART0.huart) {
//		HAL_UART_DMAPause(huart);
//	}
//}
