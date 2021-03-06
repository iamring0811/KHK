/**
  ******************************************************************************
  * @file    usbh_usr_lcd.h
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    09-November-2015
  * @brief   This file includes the declarations for user routines for LCD 
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBH_USR_LCD_H
#define __USBH_USR_LCD_H

/* Includes ------------------------------------------------------------------*/
#include "USB\USBLib\usbh_hid_core.h"

/** @addtogroup USBH_USER
  * @{
  */
  
/** @defgroup USBH_USR_LCD 
  * @brief This file is the header file for user LCD routines
  * @{
  */ 

/** @defgroup USBH_USR_LCD_Exported_Types
  * @{
  */ 


/**
  * @}
  */ 

/** @defgroup USBH_USR_LCD_Exported_Defines
  * @{
  */ 



/**
  * @}
  */ 

/** @defgroup USBH_USR_LCD_Exported_Constants
  * @{
  */ 
/*Left Button : Report data :0x01*/
#define HID_MOUSE_BUTTON1                0x01 
/*Right Button : Report data :0x02*/
#define HID_MOUSE_BUTTON2                0x02 
/*Middle Button : Report data : 0x04*/
#define HID_MOUSE_BUTTON3                0x04 

/* Mouse directions */
#define MOUSE_TOP_DIR                   0x80
#define MOUSE_BOTTOM_DIR                0x00
#define MOUSE_LEFT_DIR                  0x80
#define MOUSE_RIGHT_DIR                 0x00
#if defined(USE_STM324x9I_EVAL)
 #define MOUSE_WINDOW_X                  140
 #define MOUSE_WINDOW_Y                  100
 #define MOUSE_WINDOW_HEIGHT             60
 #define MOUSE_WINDOW_WIDTH              208

#define HID_MOUSE_BUTTON_HEIGHT         15
#define HID_MOUSE_BUTTON_WIDTH          25
#define HID_MOUSE_BUTTON1_XCHORD        145
#define HID_MOUSE_BUTTON2_XCHORD        225
#define HID_MOUSE_BUTTON3_XCHORD        315
#define HID_MOUSE_BUTTON_YCHORD         175
 
#else

#define MOUSE_WINDOW_X                  68
#define MOUSE_WINDOW_Y                  220
#define MOUSE_WINDOW_X_MAX              181
#define MOUSE_WINDOW_Y_MIN              101
#define MOUSE_WINDOW_HEIGHT             90
#define MOUSE_WINDOW_WIDTH              128

#define HID_MOUSE_BUTTON_HEIGHT         10
#define HID_MOUSE_BUTTON_WIDTH          24
#define HID_MOUSE_BUTTON_XCHORD         MOUSE_WINDOW_X + 101
#define HID_MOUSE_BUTTON1_YCHORD        220
#define HID_MOUSE_BUTTON3_YCHORD        166
#define HID_MOUSE_BUTTON2_YCHORD        116

#endif

#define MOUSE_LEFT_MOVE                  1
#define MOUSE_RIGHT_MOVE                 2
#define MOUSE_UP_MOVE                    3
#define MOUSE_DOWN_MOVE                  4

#define HID_MOUSE_HEIGHTLSB              2
#define HID_MOUSE_WIDTHLSB               2
#define HID_MOUSE_RES_X                  4  
#define HID_MOUSE_RES_Y                  4

#define SMALL_FONT_COLUMN_WIDTH          8
#define SMALL_FONT_LINE_WIDTH            12

/**
  * @}
  */ 

/** @defgroup USBH_USR_LCD_Exported_Variables
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBH_USR_LCD_Exported_Functions
  * @{
  */ 
void HID_MOUSE_UpdatePosition(int8_t x ,int8_t y);
void HID_MOUSE_ButtonReleased(uint8_t button_idx);
void HID_MOUSE_ButtonPressed (uint8_t button_idx);

/**
  * @}
  */ 

#endif /* __USBH_USR_LCD_H */

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
