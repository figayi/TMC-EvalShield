/*
 * Definitions.h
 *
 *  Created on: 30.01.2020
 *      Author: LK
 */

#ifndef HAL_TMC_DEFINITIONS_H_
#define HAL_TMC_DEFINITIONS_H_

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

typedef struct {
	unsigned int read;
	unsigned int wrote;
	volatile uint8_t *buffer;
} TMC_Buffer;

#ifndef NULL
#define NULL 0
#endif

#endif /* HAL_TMC_DEFINITIONS_H_ */
