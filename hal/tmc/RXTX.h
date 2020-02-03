/*
 * RXTX.h
 *
 *  Created on: 29.01.2020
 *      Author: LK
 */

#ifndef HAL_TMC_RXTX_H_
#define HAL_TMC_RXTX_H_

#include "Definitions.h"

#define TMC_RXTX_BUFFER_SIZE 1024

typedef struct {
	uint8_t buffer[TMC_RXTX_BUFFER_SIZE];
	size_t pos;
	size_t target;
} TMC_RXTX_Buffer;

void TMC_RXTX_writeBuffer(TMC_RXTX_Buffer *buffer, const uint8_t *data, size_t length);
size_t TMC_RXTX_readBuffer(const TMC_RXTX_Buffer *buffer, uint8_t *data, size_t length);

#endif /* HAL_TMC_RXTX_H_ */
