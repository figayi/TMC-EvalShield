/*
 * IO.h
 *
 *  Created on: 23.01.2020
 *      Author: LK
 */

#ifndef HAL_TMC_GPIO_H_
#define HAL_TMC_GPIO_H_

#include "Definitions.h"
#include "stm32f1xx_hal.h"

#define TMC_IO_COUNT 4

typedef struct {
	uint8_t pin;
	GPIO_TypeDef *port;
	GPIO_InitTypeDef init;

	uint8_t isGPIO;
	GPIO_PinState resetState;
} TMC_IO;

TMC_IO ios[TMC_IO_COUNT];

void GPIO_init(void);
void GPIO_initIO(TMC_IO *io);

TMC_IO *GPIO_getIO(uint8_t number);

void GPIO_setToInput(TMC_IO *io);
void GPIO_setToOutput(TMC_IO *io);
void GPIO_setHigh(TMC_IO *io);
void GPIO_setLow(TMC_IO *io);
void GPIO_setFloating(TMC_IO *io);

#endif /* HAL_TMC_GPIO_H_ */
