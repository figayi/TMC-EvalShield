/*
 * SPI.h
 *
 *  Created on: 23.01.2020
 *      Author: LK
 */

#ifndef HAL_TMC_SPI_H_
#define HAL_TMC_SPI_H_

#include "Connection.h"
#include "stm32f1xx_hal.h"

typedef struct {
	TMC_Connection con;
	SPI_HandleTypeDef hspi;
} TMC_SPI;

TMC_SPI SPI0;

void SPI0_Init(void);
void SPI0_deInit(void);
void SPI0_txRequest(uint8_t *data, uint16_t size, uint32_t timeout);
void SPI0_rxRequest(uint8_t *data, uint16_t size, uint32_t timeout);
void SPI0_txrx(uint8_t *data_tx, uint8_t *data_rx, uint16_t size, uint32_t timeout);

void SPI_Init(TMC_SPI *SPI);
void SPI_deInit(TMC_SPI *SPI);
void SPI_txRequest(TMC_SPI *SPI, uint8_t *data, uint16_t size, uint32_t timeout);
void SPI_rxRequest(TMC_SPI *SPI, uint8_t *data, uint16_t size, uint32_t timeout);
void SPI_txrx(TMC_SPI *SPI, uint8_t *data_tx, uint8_t *data_rx, uint16_t size, uint32_t timeout);

#endif /* HAL_TMC_SPI_H_ */
