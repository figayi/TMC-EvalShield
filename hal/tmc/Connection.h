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
	void (*txN)(uint8_t *data, uint16_t size, uint32_t timeout);
	void (*rxN)(uint8_t *data, uint16_t size, uint32_t timeout);
	uint8_t (*rx)(void);
	size_t (*dataAvailable)(void);
	void (*resetBuffers)(void);
	TMC_Connection_Status status;
	size_t word;
	uint8_t continuous;
} TMC_Connection;

TMC_Connection_Status TMC_Connection_getStatus(TMC_Connection *con);

void TMC_Connection_setContinuous(TMC_Connection *con, uint8_t continuous);
uint8_t TMC_Connection_isContinuous(TMC_Connection *con);

#endif /* HAL_TMC_CONNECTION_H_ */
