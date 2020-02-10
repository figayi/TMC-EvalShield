/*
 * Connection.c
 *
 *  Created on: 07.02.2020
 *      Author: LK
 */

#include "Connection.h"

extern void Error_Handler(void);

TMC_Connection_Status TMC_Connection_getStatus(TMC_Connection *con)
{
	return con->status;
}

////////////////////////////////////////////////////////////////////////////////
// Dummy functions
////////////////////////////////////////////////////////////////////////////////

void Connection_init(void) {
	Error_Handler();
}

void Connection_deInit(void) {
	Error_Handler();
}

void Connection_txRequest(uint8_t *data, uint16_t size, uint32_t timeout) {
	(void)(data);
	(void)(size);
	(void)(timeout);
	Error_Handler();
}

void Connection_rxRequest(uint8_t *data, uint16_t size, uint32_t timeout) {
	(void)(data);
	(void)(size);
	(void)(timeout);
	Error_Handler();
}

void Connection_txrx(uint8_t *data_tx, uint8_t *data_rx, uint16_t size, uint32_t timeout) {
	(void)(data_tx);
	(void)(data_rx);
	(void)(size);
	(void)(timeout);
	Error_Handler();
}

void Connection_txN(size_t size) {
	(void)(size);
	Error_Handler();
}

void Connection_rxN(uint8_t *data, size_t size) {
	(void)(data);
	(void)(size);
	Error_Handler();
}

uint8_t Connection_rx(void) {
	Error_Handler();
	return 0;
}

size_t Connection_dataAvailable(void) {
	Error_Handler();
	return 0;
}

void Connection_resetBuffers(void) {
	Error_Handler();
}
