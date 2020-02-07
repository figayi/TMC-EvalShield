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
	TMC_Connection_Status status;
} TMC_Connection;

#endif /* HAL_TMC_CONNECTION_H_ */
