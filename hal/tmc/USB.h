/*
 * USB.h
 *
 *  Created on: 23.01.2020
 *      Author: LK
 */

#ifndef HAL_TMC_USB_H_
#define HAL_TMC_USB_H_

#include "RXTX.h"

typedef struct {
	TMC_RXTX RXTX;
	USBD_HandleTypeDef hUsbDeviceFS;
} TMC_USB;

TMC_USB USB;

void USB_DEVICE_Init(void);

#endif /* HAL_TMC_USB_H_ */
