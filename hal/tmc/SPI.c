/*
 * SPI.c
 *
 *  Created on: 23.01.2020
 *      Author: LK
 */

#include "SPI.h"
#include "Pins.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_spi.h"

extern void Error_Handler(void);

TMC_SPI SPI0 = {
	.con = {
		.init = SPI0_Init,
		.deInit = SPI0_deInit,
		.txRequest = SPI0_txRequest,
		.rxRequest = SPI0_rxRequest,
		.txrx = SPI0_txrx,
		.txN = Connection_txN,
		.rxN = Connection_rxN,
		.rx = Connection_rx,
		.dataAvailable = Connection_dataAvailable,
		.resetBuffers = Connection_resetBuffers,
		.status = TMC_CONNECTION_STATUS_READY
	}
};

////////////////////////////////////////////////////////////////////////////////
// Specific functions
////////////////////////////////////////////////////////////////////////////////

void SPI0_Init(void) {
	/* SPI1 parameter configuration*/
	SPI0.hspi.Instance = SPI1;
	SPI0.hspi.Init.Mode = SPI_MODE_MASTER;
	SPI0.hspi.Init.Direction = SPI_DIRECTION_2LINES;
	SPI0.hspi.Init.DataSize = SPI_DATASIZE_8BIT;
	SPI0.hspi.Init.CLKPolarity = SPI_POLARITY_LOW;
	SPI0.hspi.Init.CLKPhase = SPI_PHASE_1EDGE;
	SPI0.hspi.Init.NSS = SPI_NSS_HARD_OUTPUT;
	SPI0.hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
	SPI0.hspi.Init.FirstBit = SPI_FIRSTBIT_MSB;
	SPI0.hspi.Init.TIMode = SPI_TIMODE_DISABLE;
	SPI0.hspi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	SPI0.hspi.Init.CRCPolynomial = 10;
	if (HAL_SPI_Init(&SPI0.hspi) != HAL_OK) {
		Error_Handler();
	}
}

void SPI0_deInit(void) {
	if (HAL_SPI_DeInit(&SPI0.hspi) != HAL_OK) {
		Error_Handler();
	}
}

void SPI0_txRequest(uint8_t *data, uint16_t size, uint32_t timeout) {
	HAL_SPI_Transmit(&SPI0.hspi, data, size, timeout);
}

void SPI0_rxRequest(uint8_t *data, uint16_t size, uint32_t timeout) {
	HAL_SPI_Receive(&SPI0.hspi, data, size, timeout);
}

void SPI0_txrx(uint8_t *data_tx, uint8_t *data_rx, uint16_t size, uint32_t timeout) {
	HAL_SPI_TransmitReceive(&SPI0.hspi, data_tx, data_rx, size, timeout);
}

////////////////////////////////////////////////////////////////////////////////
// Generic functions
////////////////////////////////////////////////////////////////////////////////

void SPI_Init(TMC_SPI *SPI) {
	SPI->con.init();
}

void SPI_deInit(TMC_SPI *SPI) {
	SPI->con.deInit();
}

void SPI_txRequest(TMC_SPI *SPI, uint8_t *data, uint16_t size, uint32_t timeout) {
	SPI->con.txRequest(data, size, timeout);
}

void SPI_rxRequest(TMC_SPI *SPI, uint8_t *data, uint16_t size, uint32_t timeout) {
	SPI->con.rxRequest(data, size, timeout);
}

void SPI_txrx(TMC_SPI *SPI, uint8_t *data_tx, uint8_t *data_rx, uint16_t size, uint32_t timeout) {
	SPI->con.txrx(data_tx, data_rx, size, timeout);
}

////////////////////////////////////////////////////////////////////////////////
// MSP functions
////////////////////////////////////////////////////////////////////////////////

void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	if (hspi->Instance == SPI1) {
		/* Peripheral clock enable */
		__HAL_RCC_SPI1_CLK_ENABLE();

		__HAL_RCC_GPIOA_CLK_ENABLE();
		/**SPI1 GPIO Configuration
		 PA5     ------> SPI1_SCK
		 PA6     ------> SPI1_MISO
		 PA7     ------> SPI1_MOSI
		 */
		GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_7;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_6;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	}
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi) {
	if (hspi->Instance == SPI1) {
		/* Peripheral clock disable */
		__HAL_RCC_SPI1_CLK_DISABLE();

		/**SPI1 GPIO Configuration
		 PA5     ------> SPI1_SCK
		 PA6     ------> SPI1_MISO
		 PA7     ------> SPI1_MOSI
		 */
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
	}
}
