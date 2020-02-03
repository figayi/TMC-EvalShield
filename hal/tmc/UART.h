/*
 * UART.h
 *
 *  Created on: 23.01.2020
 *      Author: LK
 */

#ifndef HAL_TMC_UART_H_
#define HAL_TMC_UART_H_

#include "Connection.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_uart.h"

typedef struct {
	TMC_Connection con;
	UART_HandleTypeDef huart;
} TMC_UART;

TMC_UART UART0;

void UART_Init(void);
void UART_deInit(void);
void UART_tx(uint8_t *data, uint16_t size, uint32_t timeout);
void UART_rx(uint8_t *data, uint16_t size, uint32_t timeout);

#endif /* HAL_TMC_UART_H_ */
