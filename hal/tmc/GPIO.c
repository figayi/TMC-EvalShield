/*
 * IO.c
 *
 *  Created on: 23.01.2020
 *      Author: LK
 */

#include "GPIO.h"

TMC_IO ios[TMC_IO_COUNT] = {
	// Blue button interrupt
	{
		.pin = 2,
		.port = GPIOC,
		.init = {
			.Pin = GPIO_PIN_13,
			.Mode = GPIO_MODE_IT_RISING,
			.Pull = GPIO_NOPULL
		},
		.isGPIO = 0,
		.isOpen = 0
	},
	// SPI0 Chip Select
	{
		.pin = 58,
		.port = GPIOB,
		.init = {
			.Pin = GPIO_PIN_6,
			.Mode = GPIO_MODE_OUTPUT_PP,
			.Pull = GPIO_PULLUP,
			.Speed = GPIO_SPEED_FREQ_HIGH
		},
		.isGPIO = 1,
		.resetState = GPIO_PIN_SET,
		.isOpen = 0
	},
	// SPI1 Chip Select
	{
		.pin = 38,
		.port = GPIOC,
		.init = {
			.Pin = GPIO_PIN_7,
			.Mode = GPIO_MODE_OUTPUT_PP,
			.Pull = GPIO_PULLUP,
			.Speed = GPIO_SPEED_FREQ_LOW
		},
		.isGPIO = 1,
		.resetState = GPIO_PIN_SET,
		.isOpen = 0
	},
	// SPI2 Chip Select
	{
		.pin = 42,
		.port = GPIOA,
		.init = {
			.Pin = GPIO_PIN_9,
			.Mode = GPIO_MODE_OUTPUT_PP,
			.Pull = GPIO_PULLUP,
			.Speed = GPIO_SPEED_FREQ_LOW
		},
		.isGPIO = 1,
		.resetState = GPIO_PIN_SET,
		.isOpen = 0
	}
};

static void setConfiguration(TMC_IO *io, GPIO_InitTypeDef *config);

static void setConfiguration(TMC_IO *io, GPIO_InitTypeDef *config)
{
	io->config.Mode = config->Mode;
	io->config.Pin = config->Pin;
	io->config.Pull = config->Pull;
	io->config.Speed = config->Speed;
	HAL_GPIO_Init(io->port, config);
}

void GPIO_initIO(TMC_IO *io)
{
	HAL_GPIO_WritePin(io->port, io->init.Pin, GPIO_PIN_RESET);
	setConfiguration(io, &io->init);
	if(io->isGPIO)
		HAL_GPIO_WritePin(io->port, io->init.Pin, io->resetState);
}

TMC_IO *GPIO_getIO(uint8_t number)
{
	for(size_t i = 0; i < TMC_IO_COUNT; i++)
		if(ios[i].pin == number)
			return &ios[i];
	return NULL;
}

void GPIO_setToInput(TMC_IO *io)
{
	GPIO_InitTypeDef config = io->config;
	config.Mode = GPIO_MODE_INPUT;
	setConfiguration(io, &config);
	io->isOpen = 0;
}

void GPIO_setToOutput(TMC_IO *io)
{
	GPIO_InitTypeDef config = io->config;
	config.Mode = GPIO_MODE_OUTPUT_PP;
	setConfiguration(io, &config);
}

void GPIO_setHigh(TMC_IO *io)
{
	HAL_GPIO_WritePin(io->port, io->init.Pin, GPIO_PIN_SET);
	io->isOpen = 0;
}

void GPIO_setLow(TMC_IO *io)
{
	HAL_GPIO_WritePin(io->port, io->init.Pin, GPIO_PIN_RESET);
	io->isOpen = 0;
}

void GPIO_setFloating(TMC_IO *io)
{
	GPIO_setToInput(io);
	io->isOpen = 1;
}

void GPIO_setToState(TMC_IO *io, TMC_IO_State state)
{
	switch(state) {
	case TMC_IO_LOW:
		GPIO_setLow(io);
		break;
	case TMC_IO_HIGH:
		GPIO_setHigh(io);
		break;
	case TMC_IO_OPEN:
		GPIO_setFloating(io);
		break;
	default:
		break;
	}
}

TMC_IO_State GPIO_getState(const TMC_IO *io)
{
	if(io->isOpen == 1)
		return TMC_IO_OPEN;

	switch(HAL_GPIO_ReadPin(io->port, io->init.Pin)) {
	case GPIO_PIN_RESET:
		return TMC_IO_LOW;
	case GPIO_PIN_SET:
		return TMC_IO_HIGH;
	}

	return TMC_IO_NOCHANGE;
}

void GPIO_init(void) {
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	for(size_t i = 0; i < TMC_IO_COUNT; i++)
		GPIO_initIO(&ios[i]);
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
