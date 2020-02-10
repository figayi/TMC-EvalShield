/*
 * stm32f1xx_hal_msp.c
 *
 *  Created on: 23.01.2020
 *      Author: LK
 */

#include "main.h"

/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void) {
	__HAL_RCC_AFIO_CLK_ENABLE();
	__HAL_RCC_PWR_CLK_ENABLE();

	/* System interrupt init*/

	/** NOJTAG: JTAG-DP Disabled and SW-DP Enabled
	 */
	__HAL_AFIO_REMAP_SWJ_NOJTAG();
}
