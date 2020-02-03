/*
 * Connection.h
 *
 *  Created on: 30.01.2020
 *      Author: LK
 */

#ifndef HAL_TMC_CONNECTION_H_
#define HAL_TMC_CONNECTION_H_

#include "Definitions.h"

typedef struct {
	void (*init)(void);
	void (*deInit)(void);
	void (*tx)(uint8_t *data, uint16_t size, uint32_t timeout);
	void (*rx)(uint8_t *data, uint16_t size, uint32_t timeout);
	uint16_t (*dataAvailable)(void);
} TMC_Connection;

#endif /* HAL_TMC_CONNECTION_H_ */
