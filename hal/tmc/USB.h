/*
 * USB.h
 *
 *  Created on: 23.01.2020
 *      Author: LK
 */

#ifndef HAL_TMC_USB_H_
#define HAL_TMC_USB_H_

#include "Connection.h"
#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
#include "usbd_def.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"

typedef struct {
	TMC_Connection con;
	USBD_HandleTypeDef *hUsbDeviceFS;
} TMC_USB;

TMC_USB USB0;
USBD_HandleTypeDef hUsbDeviceFS;

void USB_Init(void);
void USB_deInit(void);

#endif /* HAL_TMC_USB_H_ */
