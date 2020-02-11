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
		buffer->target++;
		if(buffer->target == TMC_RXTX_BUFFER_SIZE) {
			buffer->overflow++;
			buffer->target = 0;
		}
	}
}

size_t TMC_RXTX_readBuffer(TMC_RXTX_Buffer *buffer, uint8_t *data, size_t length)
{
	for(size_t i = 0; i < length; i++) {
		if((buffer->pos == buffer->target) && (buffer->overflow == 0))
			return i;
		data[i] = buffer->buffer[buffer->pos];
		buffer->pos++;
		if(buffer->pos == TMC_RXTX_BUFFER_SIZE) {
			buffer->overflow--;
			buffer->pos = 0;
		}
	}
	return length;
}

size_t TMC_RXTX_dataAvailable(const TMC_RXTX_Buffer *buffer)
{
	size_t available = buffer->overflow * TMC_RXTX_BUFFER_SIZE;
	if(buffer->target >= buffer->pos)
		available += (buffer->target - buffer->pos);
	else
		available += (TMC_RXTX_BUFFER_SIZE + buffer->target - buffer->pos);
	return available;
}

void TMC_RXTX_resetBuffer(TMC_RXTX_Buffer *buffer)
{
	buffer->pos = buffer->target = buffer->overflow = 0;
}

void TMC_RXTX_incrementBuffer(TMC_RXTX_Buffer *buffer, size_t length)
{
	size_t inc = buffer->target + length;
//	if((inc % TMC_RXTX_BUFFER_SIZE) > 20)
//		asm volatile("bkpt 100");
	buffer->target = inc % TMC_RXTX_BUFFER_SIZE;
	buffer->overflow += (inc / TMC_RXTX_BUFFER_SIZE);
}
