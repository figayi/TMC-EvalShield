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

#define TMC_PIN_COUNT 4

typedef struct {
	uint32_t pin;
	GPIO_TypeDef *port;
	GPIO_InitTypeDef init;

	uint8_t isGPIO;
	GPIO_PinState resetState;
} TMC_Pin;

TMC_Pin pins[TMC_PIN_COUNT];

void GPIO_init(void);
void GPIO_initPin(TMC_Pin *pin);

TMC_Pin *GPIO_getPin(GPIO_TypeDef *port, uint32_t number);

void GPIO_setToInput(TMC_Pin *pin);
void GPIO_setToOutput(TMC_Pin *pin);
void GPIO_setHigh(TMC_Pin *pin);
void GPIO_setLow(TMC_Pin *pin);
void GPIO_setFloating(TMC_Pin *pin);

#endif /* HAL_TMC_GPIO_H_ */
