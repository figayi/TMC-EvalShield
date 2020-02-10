/*
 * IO.c
 *
 *  Created on: 23.01.2020
 *      Author: LK
 */

#include "GPIO.h"

TMC_Pin pins[TMC_PIN_COUNT] = {
	// Blue button interrupt
	{
		.pin = GPIO_PIN_13,
		.port = GPIOC,
		.init = {
			.Pin = GPIO_PIN_13,
			.Mode = GPIO_MODE_IT_RISING,
			.Pull = GPIO_NOPULL
		},
		.isGPIO = 0
	},
	// SPI0 Chip Select
	{
		.pin = GPIO_PIN_6,
		.port = GPIOB,
		.init = {
			.Pin = GPIO_PIN_6,
			.Mode = GPIO_MODE_OUTPUT_PP,
			.Pull = GPIO_PULLUP,
			.Speed = GPIO_SPEED_FREQ_LOW
		},
		.isGPIO = 1,
		.resetState = GPIO_PIN_SET
	},
	// SPI1 Chip Select
	{
		.pin = GPIO_PIN_7,
		.port = GPIOC,
		.init = {
			.Pin = GPIO_PIN_7,
			.Mode = GPIO_MODE_OUTPUT_PP,
			.Pull = GPIO_PULLUP,
			.Speed = GPIO_SPEED_FREQ_LOW
		},
		.isGPIO = 1,
		.resetState = GPIO_PIN_SET
	},
	// SPI2 Chip Select
	{
		.pin = GPIO_PIN_9,
		.port = GPIOA,
		.init = {
			.Pin = GPIO_PIN_9,
			.Mode = GPIO_MODE_OUTPUT_PP,
			.Pull = GPIO_PULLUP,
			.Speed = GPIO_SPEED_FREQ_LOW
		},
		.isGPIO = 1,
		.resetState = GPIO_PIN_SET
	}
};

void GPIO_initPin(TMC_Pin *pin)
{
	HAL_GPIO_Init(pin->port, &pin->init);
	HAL_GPIO_WritePin(pin->port, pin->pin, pin->resetState);
}

TMC_Pin *GPIO_getPin(GPIO_TypeDef *port, uint32_t number)
{
	for(size_t i = 0; i < TMC_PIN_COUNT; i++)
		if((pins[i].port == port) && (pins[i].pin == number))
			return &pins[i];
	return NULL;
}

void GPIO_setToInput(TMC_Pin *pin)
{
	GPIO_InitTypeDef init = pin->init;
	init.Mode = GPIO_MODE_INPUT;
	HAL_GPIO_Init(pin->port, &init);
}

void GPIO_setToOutput(TMC_Pin *pin)
{
	GPIO_InitTypeDef init = pin->init;
	init.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(pin->port, &init);
}

void GPIO_setHigh(TMC_Pin *pin)
{
	HAL_GPIO_WritePin(pin->port, pin->pin, GPIO_PIN_SET);
}

void GPIO_setLow(TMC_Pin *pin)
{
	HAL_GPIO_WritePin(pin->port, pin->pin, GPIO_PIN_RESET);
}

void GPIO_setFloating(TMC_Pin *pin)
{
	// TODO
}

void GPIO_init(void) {

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
void GPIO_Init(void) {
	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}
