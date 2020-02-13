/*
 * SPI.c
 *
 *  Created on: 23.01.2020
 *      Author: LK
 */

#include "SPI.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_spi.h"

extern void Error_Handler(void);

static SPI_HandleTypeDef hspi;
static uint8_t initialized = 0;

TMC_SPI TMC_SPI_Channel[TMC_SPI_CHANNELS] = {
	{
		.con = {
			.init = SPIX_init,
			.deInit = SPIX_deInit,
			.txRequest = SPI0_txRequest,
			.rxRequest = SPI0_rxRequest,
			.txrx = SPI0_txrx,
			.txN = Connection_txN,
			.rxN = Connection_rxN,
			.rx = Connection_rx,
			.dataAvailable = Connection_dataAvailable,
			.resetBuffers = Connection_resetBuffers,
			.status = TMC_CONNECTION_STATUS_READY
		},
		.hspi = &hspi,
		.cs = &ios[1]
	},
	{
		.con = {
			.init = SPIX_init,
			.deInit = SPIX_deInit,
			.txRequest = SPI1_txRequest,
			.rxRequest = SPI1_rxRequest,
			.txrx = SPI1_txrx,
			.txN = Connection_txN,
			.rxN = Connection_rxN,
			.rx = Connection_rx,
			.dataAvailable = Connection_dataAvailable,
			.resetBuffers = Connection_resetBuffers,
			.status = TMC_CONNECTION_STATUS_READY
		},
		.hspi = &hspi,
		.cs = &ios[2]
	},
	{
		.con = {
			.init = SPIX_init,
			.deInit = SPIX_deInit,
			.txRequest = SPI2_txRequest,
			.rxRequest = SPI2_rxRequest,
			.txrx = SPI2_txrx,
			.txN = Connection_txN,
			.rxN = Connection_rxN,
			.rx = Connection_rx,
			.dataAvailable = Connection_dataAvailable,
			.resetBuffers = Connection_resetBuffers,
			.status = TMC_CONNECTION_STATUS_READY
		},
		.hspi = &hspi,
		.cs = &ios[3]
	}
};

////////////////////////////////////////////////////////////////////////////////
// Specific functions
////////////////////////////////////////////////////////////////////////////////

void SPI0_txRequest(uint8_t *data, uint16_t size, uint32_t timeout) {
	GPIO_setLow(TMC_SPI_Channel[0].cs);
	SPIX_txRequest(data, size, timeout);
	GPIO_setHigh(TMC_SPI_Channel[0].cs);
}

void SPI0_rxRequest(uint8_t *data, uint16_t size, uint32_t timeout) {
	GPIO_setLow(TMC_SPI_Channel[0].cs);
	SPIX_rxRequest(data, size, timeout);
	GPIO_setHigh(TMC_SPI_Channel[0].cs);
}

void SPI0_txrx(uint8_t *data_tx, uint8_t *data_rx, uint16_t size, uint32_t timeout) {
	GPIO_setLow(TMC_SPI_Channel[0].cs);
	//GPIO_setLow(TMC_SPI_Channel[1].cs);
	//GPIO_setLow(TMC_SPI_Channel[2].cs);
	//HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
	SPIX_txrx(data_tx, data_rx, size, timeout);
	GPIO_setHigh(TMC_SPI_Channel[0].cs);
	//GPIO_setHigh(TMC_SPI_Channel[1].cs);
	//GPIO_setHigh(TMC_SPI_Channel[2].cs);
	//HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
}

void SPI1_txRequest(uint8_t *data, uint16_t size, uint32_t timeout) {
	GPIO_setLow(TMC_SPI_Channel[1].cs);
	SPIX_txRequest(data, size, timeout);
	GPIO_setHigh(TMC_SPI_Channel[1].cs);
}

void SPI1_rxRequest(uint8_t *data, uint16_t size, uint32_t timeout) {
	GPIO_setLow(TMC_SPI_Channel[1].cs);
	SPIX_rxRequest(data, size, timeout);
	GPIO_setHigh(TMC_SPI_Channel[1].cs);
}

void SPI1_txrx(uint8_t *data_tx, uint8_t *data_rx, uint16_t size, uint32_t timeout) {
	GPIO_setLow(TMC_SPI_Channel[1].cs);
	SPIX_txrx(data_tx, data_rx, size, timeout);
	GPIO_setHigh(TMC_SPI_Channel[1].cs);
}

void SPI2_txRequest(uint8_t *data, uint16_t size, uint32_t timeout) {
	GPIO_setLow(TMC_SPI_Channel[2].cs);
	SPIX_txRequest(data, size, timeout);
	GPIO_setHigh(TMC_SPI_Channel[2].cs);
}

void SPI2_rxRequest(uint8_t *data, uint16_t size, uint32_t timeout) {
	GPIO_setLow(TMC_SPI_Channel[2].cs);
	SPIX_rxRequest(data, size, timeout);
	GPIO_setHigh(TMC_SPI_Channel[2].cs);
}

void SPI2_txrx(uint8_t *data_tx, uint8_t *data_rx, uint16_t size, uint32_t timeout) {
	GPIO_setLow(TMC_SPI_Channel[2].cs);
	SPIX_txrx(data_tx, data_rx, size, timeout);
	GPIO_setHigh(TMC_SPI_Channel[2].cs);
}

////////////////////////////////////////////////////////////////////////////////
// Semi specific functions
////////////////////////////////////////////////////////////////////////////////

void SPIX_init(void) {
	if(!initialized) {
		/* SPI1 parameter configuration*/
		hspi.Instance = SPI1;
		hspi.Init.Mode = SPI_MODE_MASTER;
		hspi.Init.Direction = SPI_DIRECTION_2LINES;
		hspi.Init.DataSize = SPI_DATASIZE_8BIT;
		hspi.Init.CLKPolarity = SPI_POLARITY_HIGH;
		hspi.Init.CLKPhase = SPI_PHASE_2EDGE;
		hspi.Init.NSS = SPI_NSS_SOFT;
		hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
		hspi.Init.FirstBit = SPI_FIRSTBIT_MSB;
		hspi.Init.TIMode = SPI_TIMODE_DISABLE;
		hspi.Init.CRCCalculation = SPI_CRCCALCULATION_ENABLE;
		hspi.Init.CRCPolynomial = 0;
		if (HAL_SPI_Init(&hspi) != HAL_OK) {
			Error_Handler();
		}
		initialized = 1;
	}
}

void SPIX_deInit(void) {
	if(initialized) {
		if (HAL_SPI_DeInit(&hspi) != HAL_OK) {
			Error_Handler();
		}
		initialized = 0;
	}
}

void SPIX_txRequest(uint8_t *data, uint16_t size, uint32_t timeout) {
	if(initialized)
		HAL_SPI_Transmit(&hspi, data, size, timeout);
}

void SPIX_rxRequest(uint8_t *data, uint16_t size, uint32_t timeout) {
	if(initialized)
		HAL_SPI_Receive(&hspi, data, size, timeout);
}

void SPIX_txrx(uint8_t *data_tx, uint8_t *data_rx, uint16_t size, uint32_t timeout) {
	if(initialized)
		if(HAL_SPI_TransmitReceive(&hspi, data_tx, data_rx, size, timeout) != HAL_OK)
			Error_Handler();
}

////////////////////////////////////////////////////////////////////////////////
// Generic functions
////////////////////////////////////////////////////////////////////////////////

void SPI_init(TMC_SPI *SPI) {
	SPI->con.init();
}

void SPI_deInit(TMC_SPI *SPI) {
	SPI->con.deInit();
}

void SPI_txRequest(TMC_SPI *SPI, uint8_t *data, uint16_t size, uint32_t timeout) {
	GPIO_setHigh(SPI->cs);
	SPI->con.txRequest(data, size, timeout);
	GPIO_setLow(SPI->cs);
}

void SPI_rxRequest(TMC_SPI *SPI, uint8_t *data, uint16_t size, uint32_t timeout) {
	GPIO_setHigh(SPI->cs);
	SPI->con.rxRequest(data, size, timeout);
	GPIO_setLow(SPI->cs);
}

void SPI_txrx(TMC_SPI *SPI, uint8_t *data_tx, uint8_t *data_rx, uint16_t size, uint32_t timeout) {
	GPIO_setHigh(SPI->cs);
	SPI->con.txrx(data_tx, data_rx, size, timeout);
	GPIO_setLow(SPI->cs);
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
