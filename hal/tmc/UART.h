/*
 * UART.h
 *
 *  Created on: 23.01.2020
 *      Author: LK
 */

#ifndef HAL_TMC_UART_H_
#define HAL_TMC_UART_H_

typedef struct {
	TMC_RXTX RXTX;
	UART_HandleTypeDef huart;
} TMC_UART;

TMC_UART UART;

void UART_Init(void);
void UART_deInit(void);

#endif /* HAL_TMC_UART_H_ */
