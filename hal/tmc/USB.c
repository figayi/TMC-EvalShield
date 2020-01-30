/*
 * USB.c
 *
 *  Created on: 23.01.2020
 *      Author: LK
 */

#include "USB.h"
#include "Pins.h"
#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
#include "usbd_def.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"

/**
  * Init USB device Library, add supported class and start the library
  * @retval None
  */
void USB_DEVICE_Init(void)
{
  /* USER CODE BEGIN USB_DEVICE_Init_PreTreatment */

  /* USER CODE END USB_DEVICE_Init_PreTreatment */

  /* Init Device Library, add supported class and start the library. */
  if (USBD_Init(&USB.hUsbDeviceFS, &FS_Desc, DEVICE_FS) != USBD_OK)
  {
    Error_Handler();
  }
  if (USBD_RegisterClass(&USB.hUsbDeviceFS, &USBD_CDC) != USBD_OK)
  {
    Error_Handler();
  }
  if (USBD_CDC_RegisterInterface(&USB.hUsbDeviceFS, &USBD_Interface_fops_FS) != USBD_OK)
  {
    Error_Handler();
  }
  if (USBD_Start(&USB.hUsbDeviceFS) != USBD_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN USB_DEVICE_Init_PostTreatment */

  /* USER CODE END USB_DEVICE_Init_PostTreatment */
}
