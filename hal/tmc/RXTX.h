/*
 * RXTX.h
 *
 *  Created on: 29.01.2020
 *      Author: LK
 */

#ifndef HAL_TMC_RXTX_H_
#define HAL_TMC_RXTX_H_

#include "Definitions.h"

typedef struct
{
	void (*init)();
	void (*deInit)(void);
	void (*tx)(uint8_t ch);
	uint8_t (*rx)(uint8_t *ch);
	void (*txN)(uint8_t *ch, unsigned char number);
	uint8_t (*rxN)(uint8_t *ch, unsigned char number);
	void (*clearBuffers)(void);
	uint32_t (*bytesAvailable)(void);
	uint32_t baudRate;
} TMC_RXTX;

typedef struct
{
	TMC_Buffer tx;
	TMC_Buffer rx;
} TMC_RXTX_Buffer;

#endif /* HAL_TMC_RXTX_H_ */
