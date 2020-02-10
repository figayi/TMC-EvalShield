/*
 * RXTX.h
 *
 *  Created on: 29.01.2020
 *      Author: LK
 */

#ifndef HAL_TMC_RXTX_H_
#define HAL_TMC_RXTX_H_

#include "Definitions.h"

#ifndef TMC_RXTX_BUFFER_SIZE
#define TMC_RXTX_BUFFER_SIZE 50
#endif

typedef struct {
	uint8_t buffer[TMC_RXTX_BUFFER_SIZE];
	size_t pos;
	size_t target;
} TMC_RXTX_Buffer;

void TMC_RXTX_incrementBuffer(TMC_RXTX_Buffer *buffer, size_t length);
void TMC_RXTX_writeBuffer(TMC_RXTX_Buffer *buffer, const uint8_t *data, size_t length);
size_t TMC_RXTX_readBuffer(TMC_RXTX_Buffer *buffer, uint8_t *data, size_t length);
size_t TMC_RXTX_dataAvailable(const TMC_RXTX_Buffer *buffer);
void TMC_RXTX_resetBuffer(TMC_RXTX_Buffer *buffer);

#endif /* HAL_TMC_RXTX_H_ */
