/*
 * RXTX.c
 *
 *  Created on: 03.02.2020
 *      Author: LK
 */

#include "RXTX.h"

void TMC_RXTX_writeBuffer(TMC_RXTX_Buffer *buffer, const uint8_t *data, size_t length)
{
	for(size_t i = 0; i < length; i++) {
		buffer->buffer[buffer->target] = data[i];
		buffer->target = (buffer->target + 1) % TMC_RXTX_BUFFER_SIZE;
	}
}

size_t TMC_RXTX_readBuffer(TMC_RXTX_Buffer *buffer, uint8_t *data, size_t length)
{
	for(size_t i = 0; i < length; i++) {
		if(buffer->pos == buffer->target)
			return i;
		data[i] = buffer->buffer[buffer->pos];
		buffer->pos = (buffer->pos + 1) % TMC_RXTX_BUFFER_SIZE;
	}
	return length;
}

size_t TMC_RXTX_dataAvailable(const TMC_RXTX_Buffer *buffer)
{
	size_t size = 0;
	if((buffer->target) >= (buffer->pos)) {
		size = (buffer->target - buffer->pos);
	} else {
		size = (TMC_RXTX_BUFFER_SIZE - (buffer->pos - buffer->target));
	}
	return size;
}

void TMC_RXTX_resetBuffer(TMC_RXTX_Buffer *buffer)
{
	buffer->pos = buffer->target = 0;
}
