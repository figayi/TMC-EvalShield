/*
 * main.h
 *
 *  Created on: 23.01.2020
 *      Author: LK
 */

#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

#define MODULE_ID "0019"

void Error_Handler(void);
void deInit(void);
void reset(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
