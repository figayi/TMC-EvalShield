/*
 * IO.h
 *
 *  Created on: 23.01.2020
 *      Author: LK
 */

#ifndef HAL_TMC_GPIO_H_
#define HAL_TMC_GPIO_H_

#include "stm32f1xx_hal.h"
#include "Pins.h"

typedef struct {
	uint32_t pin;
	GPIO_TypeDef *port;
	GPIO_InitTypeDef init;
} TMC_Pin;

void GPIO_initPin(TMC_Pin *pin);
void GPIO_Init(void);

void GPIO_setToInput(TMC_Pin *pin);
void GPIO_setToOutput(TMC_Pin *pin);
void GPIO_setHigh(TMC_Pin *pin);
void GPIO_setLow(TMC_Pin *pin);
void GPIO_setFloating(TMC_Pin *pin);

#endif /* HAL_TMC_GPIO_H_ */
