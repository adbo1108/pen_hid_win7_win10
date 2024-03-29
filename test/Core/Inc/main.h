/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "sw_iic.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SW_IIC_SDA_Pin GPIO_PIN_9
#define SW_IIC_SDA_GPIO_Port GPIOC
#define SW_IIC_SCL_Pin GPIO_PIN_8
#define SW_IIC_SCL_GPIO_Port GPIOA

#define EMR_INT_Pin GPIO_PIN_4
#define EMR_INT_GPIO_Port GPIOB
#define EMR_INT_EXTI_IRQn EXTI4_IRQn
#define EMR_RST_Pin GPIO_PIN_5
#define EMR_RST_GPIO_Port GPIOB

#define OSD_MENU_DETECT_Pin  GPIO_PIN_12
/* USER CODE BEGIN Private defines */


 extern   uint8_t aRxBuffer[64];
 extern   uint8_t page_buf_full_idx ,flag_start_fill_page_buf ;
 extern uint8_t page_idx;
 extern   volatile uint8_t page_buf[512];
  
  
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
