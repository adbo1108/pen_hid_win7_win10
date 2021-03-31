/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usbd_custom_hid_if.c
  * @version        : v1.0_Cube
  * @brief          : USB Device Custom HID interface file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "usbd_custom_hid_if.h"

/* USER CODE BEGIN INCLUDE */

/* USER CODE END INCLUDE */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @brief Usb device.
  * @{
  */

/** @addtogroup USBD_CUSTOM_HID
  * @{
  */

/** @defgroup USBD_CUSTOM_HID_Private_TypesDefinitions USBD_CUSTOM_HID_Private_TypesDefinitions
  * @brief Private types.
  * @{
  */

/* USER CODE BEGIN PRIVATE_TYPES */

/* USER CODE END PRIVATE_TYPES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Defines USBD_CUSTOM_HID_Private_Defines
  * @brief Private defines.
  * @{
  */

/* USER CODE BEGIN PRIVATE_DEFINES */

/* USER CODE END PRIVATE_DEFINES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Macros USBD_CUSTOM_HID_Private_Macros
  * @brief Private macros.
  * @{
  */

/* USER CODE BEGIN PRIVATE_MACRO */

/* USER CODE END PRIVATE_MACRO */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Variables USBD_CUSTOM_HID_Private_Variables
  * @brief Private variables.
  * @{
  */

/** Usb HID report descriptor. */
__ALIGN_BEGIN static uint8_t CUSTOM_HID_ReportDesc_FS[USBD_CUSTOM_HID_REPORT_DESC_SIZE] __ALIGN_END =
{
  /* USER CODE BEGIN 0 */
  /* USER CODE BEGIN 0 */
#if 0	
    0x06, 0x00, 0xff,               //Usage Page(Undefined )
        0x09, 0x01,                    // USAGE (Undefined)
        0xa1, 0x01,                    // COLLECTION (Application)
        
        0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
        0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
        0x75, 0x08,                    //   REPORT_SIZE (8)
   //Report ID
        0x85,0x02,                     // REPORT_ID(2)
        0x95, 0x08,                    //   REPORT_COUNT (8))
        0x09, 0x01,                    //   USAGE (Undefined)
        0x81, 0x02,                    //   INPUT (Data,Var,Abs)

        0x95, 0x40,                    //   REPORT_COUNT (64)
        0x09, 0x01,                    //   USAGE (Undefined)
        0x91, 0x02,                    //   OUTPUT (Data,Var,Abs)

        0x95, 0x01,                    //   REPORT_COUNT (1)
        0x09, 0x01,                    //   USAGE (Undefined)
        0xb1, 0x02,                    //   FEATURE (Data,Var,Abs)
#endif
#if 0
 0x05, 0x0d,                    // USAGE_PAGE (Digitizers)
    0x09, 0x02,                    // USAGE (Pen)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x85, 0x02,                    //   REPORT_ID (2)
    0x09, 0x20,                    //   USAGE (Stylus)
    0xa1, 0x00,                    //   COLLECTION (Physical)
    0x09, 0x42,                    //     USAGE (Tip Switch)
    0x09, 0x44,                    //     USAGE (Barrel Switch)
    0x09, 0x45,                    //     USAGE (Eraser)
    0x09, 0x3c,                    //     USAGE (Invert)
    0x09, 0x5A,                    //     USAGE (Secondary Barrel Switch)
    0x09, 0x32,                    //     USAGE (In Range)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x95, 0x06,                    //     REPORT_COUNT (6)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x95, 0x02,                    //     REPORT_COUNT (2)
    0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    0x09, 0x30,                    //     USAGE (X)
    0x26, 0xC0, 0x72,              //     LOGICAL_MAXIMUM (29376)  
    0x46, 0xC0, 0x72,              //     PHYSICAL_MAXIMUM (29376)
   // 0x26, 0xFA, 0x72,              //     LOGICAL_MAXIMUM (29434)  
  //  0x46, 0xFA, 0x72,              //     PHYSICAL_MAXIMUM (29434)
    0x65, 0x11,                    //     UNIT (SI Lin:Distance)
    0x55, 0x0e,                    //     UNIT_EXPONENT (-2)
    0x75, 0x10,                    //     REPORT_SIZE (16)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x09, 0x31,                    //     USAGE (Y)
    0x26, 0x8C, 0x40,              //     LOGICAL_MAXIMUM (16524)  
    0x46, 0x8C, 0x40,              //     PHYSICAL_MAXIMUM (16524)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x45, 0x00,                    //     PHYSICAL_MAXIMUM (0)
    0x65, 0x00,                    //     UNIT (None)
    0x55, 0x00,                    //     UNIT_EXPONENT (0)
    0x05, 0x0d,                    //     USAGE_PAGE (Digitizers)
    0x09, 0x30,                    //     USAGE (Tip Pressure)
    0x26, 0xff, 0x0F,              //     LOGICAL_MAXIMUM (4095)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0xc0,                          //   END_COLLECTION
    0x09, 0x00,                    //   USAGE (Undefined)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0xb1, 0x02,                    //   FEATURE (Data,Var,Abs)
    0x85, 0x03,                    //   REPORT_ID (3)
    0x09, 0x00,                    //   USAGE (Undefined)
    0x95, 0x10,                    //   REPORT_COUNT (16)
    0xb1, 0x02,                    //   FEATURE (Data,Var,Abs)
    0x85, 0x04,                    //   REPORT_ID (4)
    0x09, 0x00,                    //   USAGE (Undefined)
    0xb1, 0x02,                    //   FEATURE (Data,Var,Abs)
    0x85, 0x05,                    //   REPORT_ID (5)
    0x09, 0x00,                    //   USAGE (Undefined)
    0x95, 0x04,                    //   REPORT_COUNT (4)
    0xb1, 0x02,                    //   FEATURE (Data,Var,Abs)
    0x85, 0x06,                    //   REPORT_ID (6)
    0x09, 0x00,                    //   USAGE (Undefined)
    0x95, 0x24,                    //   REPORT_COUNT (36)
    0xb1, 0x02,                    //   FEATURE (Data,Var,Abs)
    0x85, 0x14,                    //   REPORT_ID (20)
	 0x06, 0x00,0xFF,               // USAGE PAGE(Vendor Define Page)
    0x09, 0x00,                    //   USAGE (Undefined)
    0x96, 0x00, 0x01,              //   REPORT_COUNT (256)
    0xb1, 0x02,                    //   FEATURE (Data,Var,Abs)
    0x85, 0x15,                    //   REPORT_ID (21)
    0x09, 0x00,                    //   USAGE (Undefined)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0xb1, 0x02,                    //   FEATURE (Data,Var,Abs)
    0x85, 0x16,                    //   REPORT_ID (22)
    0x09, 0x00,                    //   USAGE (Undefined)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
    0x95, 0x06,                    //   REPORT_COUNT (6)
    0xb1, 0x02,                    //   FEATURE (Data,Var,Abs)
    0x85, 0x17,                    //   REPORT_ID (23)
    0x09, 0x00,                    //   USAGE (Undefined)
    0x95, 0x0c,                    //   REPORT_COUNT (12)
    0xb1, 0x02,                    //   FEATURE (Data,Var,Abs)
#endif
  
#if 1
	0x06, 0x00, 0xFF,  // Usage Page (Vendor Defined 0xFF00)
	0x09, 0x01,        // Usage (0x01)
	0xA1, 0x01,        // Collection (Application)
	0x85, 0x03,        //   Report ID (3)
	0x15, 0x00,        //   Logical Minimum (0)
	0x26, 0xFF, 0x00,  //   Logical Maximum (255)
	0x75, 0x08,        //   Report Size (8)
	0x95, 0x08,        //   Report Count (8)
	0x09, 0x01,        //   Usage (0x01)
	0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x95, 32,//0x40,        //   Report Count (64)
	0x09, 0x01,        //   Usage (0x01)
	//0x85, 0x03,        //   Report ID (3)
	0x91, 0x02,        //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
	0x95, 0x01,        //   Report Count (1)
	0x09, 0x01,        //   Usage (0x01)
	0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
	0xC0,              // End Collection
	0x05, 0x0D,        // Usage Page (Digitizer)
	0x09, 0x02,        // Usage (Pen)
	0xA1, 0x01,        // Collection (Application)
	0x85, 0x02,        //   Report ID (2)
	0x09, 0x20,        //   Usage (Stylus)
	0xA1, 0x00,        //   Collection (Physical)
	0x09, 0x42,        //     Usage (Tip Switch)
	0x09, 0x44,        //     Usage (Barrel Switch)
	0x09, 0x45,        //     Usage (Eraser)
	0x09, 0x3C,        //     Usage (Invert)
	0x09, 0x5A,        //     Usage (0x5A)
	0x09, 0x32,        //     Usage (In Range)
	0x15, 0x00,        //     Logical Minimum (0)
	0x25, 0x01,        //     Logical Maximum (1)
	0x75, 0x01,        //     Report Size (1)
	0x95, 0x06,        //     Report Count (6)
	0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x95, 0x02,        //     Report Count (2)
	0x81, 0x03,        //     Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
	0x09, 0x30,        //     Usage (X)
	0x26, 0xC0, 0x72,  //     Logical Maximum (29376)
	0x46, 0xC0, 0x72,  //     Physical Maximum (29376)
	0x65, 0x11,        //     Unit (System: SI Linear, Length: Centimeter)
	0x55, 0x0E,        //     Unit Exponent (-2)
	0x75, 0x10,        //     Report Size (16)
	0x95, 0x01,        //     Report Count (1)
	0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x09, 0x31,        //     Usage (Y)
	0x26, 0x8C, 0x40,  //     Logical Maximum (16524)
	0x46, 0x8C, 0x40,  //     Physical Maximum (16524)
	0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x45, 0x00,        //     Physical Maximum (0)
	0x65, 0x00,        //     Unit (None)
	0x55, 0x00,        //     Unit Exponent (0)
	0x05, 0x0D,        //     Usage Page (Digitizer)
	0x09, 0x30,        //     Usage (Tip Pressure)
	0x26, 0xFF, 0x0F,  //     Logical Maximum (4095)
	0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0xC0,              //   End Collection
#endif
  
  
  
  /* USER CODE END 0 */
  0xC0    /*     END_COLLECTION	             */
};

/* USER CODE BEGIN PRIVATE_VARIABLES */

/* USER CODE END PRIVATE_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Exported_Variables USBD_CUSTOM_HID_Exported_Variables
  * @brief Public variables.
  * @{
  */
extern USBD_HandleTypeDef hUsbDeviceFS;

/* USER CODE BEGIN EXPORTED_VARIABLES */

/* USER CODE END EXPORTED_VARIABLES */
/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_FunctionPrototypes USBD_CUSTOM_HID_Private_FunctionPrototypes
  * @brief Private functions declaration.
  * @{
  */

static int8_t CUSTOM_HID_Init_FS(void);
static int8_t CUSTOM_HID_DeInit_FS(void);
static int8_t CUSTOM_HID_OutEvent_FS(uint8_t* buf);

/**
  * @}
  */

USBD_CUSTOM_HID_ItfTypeDef USBD_CustomHID_fops_FS =
{
  CUSTOM_HID_ReportDesc_FS,
  CUSTOM_HID_Init_FS,
  CUSTOM_HID_DeInit_FS,
  CUSTOM_HID_OutEvent_FS
};

/** @defgroup USBD_CUSTOM_HID_Private_Functions USBD_CUSTOM_HID_Private_Functions
  * @brief Private functions.
  * @{
  */

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes the CUSTOM HID media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_Init_FS(void)
{
  /* USER CODE BEGIN 4 */
  return (USBD_OK);
  /* USER CODE END 4 */
}

/**
  * @brief  DeInitializes the CUSTOM HID media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_DeInit_FS(void)
{
  /* USER CODE BEGIN 5 */
  return (USBD_OK);
  /* USER CODE END 5 */
}

uint8_t buffer[65];
/**
  * @brief  Manage the CUSTOM HID class events
  * @param  event_idx: Event index
  * @param  state: Event state
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_OutEvent_FS(uint8_t* buff)
{
  /* USER CODE BEGIN 6 */
  uint8_t i = 0 ;
  memset(buffer , 0 ,sizeof(buffer));
  memcpy(aRxBuffer,buff+1,32);
  handle_buffer() ;
   
//    for(i=0; i<33; i++)
//    {
//        printf("Receive Report_buf[%d]=%d\n\r",i,buffer[i]);
//    }    

  /* Start next USB packet transfer once data processing is completed */
  USBD_CUSTOM_HID_ReceivePacket(&hUsbDeviceFS);
	
	/*Resend data for test */ 
 // USBD_CUSTOM_HID_SendReport_FS(buffer,9) ;
  return (USBD_OK);
  /* USER CODE END 6 */
}

/* USER CODE BEGIN 7 */
/**
  * @brief  Send the report to the Host
  * @param  report: The report to be sent
  * @param  len: The report length
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */

  int8_t USBD_CUSTOM_HID_SendReport_FS(uint8_t *report, uint16_t len)
{
  return USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, report, len);
}

/* USER CODE END 7 */

/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */

/* USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION */
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

