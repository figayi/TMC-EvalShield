/*
 * Connection.h
 *
 *  Created on: 30.01.2020
 *      Author: LK
 */

#ifndef HAL_TMC_CONNECTION_H_
#define HAL_TMC_CONNECTION_H_

#include "Definitions.h"

typedef enum {
	TMC_CONNECTION_STATUS_READY,
	TMC_CONNECTION_STATUS_BUSY
} TMC_Connection_Status;

typedef struct {
	void (*init)(void);
	void (*deInit)(void);
	void (*txRequest)(uint8_t *data, uint16_t size, uint32_t timeout);
	void (*rxRequest)(uint8_t *data, uint16_t size, uint32_t timeout);
	void (*txrx)(uint8_t *data_tx, uint8_t *data_rx, uint16_t size, uint32_t timeout);
	void (*txN)(size_t size);
	void (*rxN)(uint8_t *data, size_t size);
	uint8_t (*rx)(void);
	size_t (*dataAvailable)(void);
	void (*resetBuffers)(void);
	TMC_Connection_Status status;
	size_t word;
} TMC_Connection;

TMC_Connection_Status TMC_Connection_getStatus(TMC_Connection *con);

// Dummy functions
void Connection_init(void);
void Connection_deInit(void);
void Connection_txRequest(uint8_t *data, uint16_t size, uint32_t timeout);
void Connection_rxRequest(uint8_t *data, uint16_t size, uint32_t timeout);
void Connection_txrx(uint8_t *data_tx, uint8_t *data_rx, uint16_t size, uint32_t timeout);
void Connection_txN(size_t size);
void Connection_rxN(uint8_t *data, size_t size);
uint8_t Connection_rx(void);
size_t Connection_dataAvailable(void);
void Connection_resetBuffers(void);

#endif /* HAL_TMC_CONNECTION_H_ */
