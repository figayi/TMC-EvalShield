/*
 * SPI.h
 *
 *  Created on: 23.01.2020
 *      Author: LK
 */

#ifndef HAL_TMC_SPI_H_
#define HAL_TMC_SPI_H_

#include "Connection.h"
#include "GPIO.h"
#include "stm32f1xx_hal.h"

#define TMC_SPI_CHANNELS 3

typedef struct {
	TMC_Connection con;
	SPI_HandleTypeDef *hspi;
	TMC_Pin *cs;
} TMC_SPI;

TMC_SPI TMC_SPI_Channel[TMC_SPI_CHANNELS];
//TMC_SPI TMC_SPI0;
//TMC_SPI TMC_SPI1;
//TMC_SPI TMC_SPI2;

void SPIX_init(void);
void SPIX_deInit(void);
void SPIX_txRequest(uint8_t *data, uint16_t size, uint32_t timeout);
void SPIX_rxRequest(uint8_t *data, uint16_t size, uint32_t timeout);
void SPIX_txrx(uint8_t *data_tx, uint8_t *data_rx, uint16_t size, uint32_t timeout);

void SPI0_txRequest(uint8_t *data, uint16_t size, uint32_t timeout);
void SPI0_rxRequest(uint8_t *data, uint16_t size, uint32_t timeout);
void SPI0_txrx(uint8_t *data_tx, uint8_t *data_rx, uint16_t size, uint32_t timeout);

void SPI1_txRequest(uint8_t *data, uint16_t size, uint32_t timeout);
void SPI1_rxRequest(uint8_t *data, uint16_t size, uint32_t timeout);
void SPI1_txrx(uint8_t *data_tx, uint8_t *data_rx, uint16_t size, uint32_t timeout);

void SPI2_txRequest(uint8_t *data, uint16_t size, uint32_t timeout);
void SPI2_rxRequest(uint8_t *data, uint16_t size, uint32_t timeout);
void SPI2_txrx(uint8_t *data_tx, uint8_t *data_rx, uint16_t size, uint32_t timeout);

void SPI_init(TMC_SPI *SPI);
void SPI_deInit(TMC_SPI *SPI);
void SPI_txRequest(TMC_SPI *SPI, uint8_t *data, uint16_t size, uint32_t timeout);
void SPI_rxRequest(TMC_SPI *SPI, uint8_t *data, uint16_t size, uint32_t timeout);
void SPI_txrx(TMC_SPI *SPI, uint8_t *data_tx, uint8_t *data_rx, uint16_t size, uint32_t timeout);

#endif /* HAL_TMC_SPI_H_ */
