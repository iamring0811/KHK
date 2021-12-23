/**
  ******************************************************************************
  * @file    usbh_usr.c
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    09-November-2015
  * @brief   This file includes the user application layer
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

/* Includes ------------------------------------------------------------------*/
#include "USB\USB.h"

#include "USB\usbh_hid_usr.h"
#include "USB\USBLib\usbh_hid_mouse.h"
#include "USB\USBLib\usbh_hid_keybd.h"

#include "util\usart.h"


/** @defgroup USBH_CORE_Exported_TypesDefinitions
* @{
*/ 
#if defined(USE_STM324x9I_EVAL)
  #define KYBRD_FIRST_COLUMN               (uint16_t)479
  #define KYBRD_LAST_COLUMN                (uint16_t)7
  #define KYBRD_FIRST_LINE                 (uint8_t)120
  #define KYBRD_LAST_LINE                  (uint8_t)200

#else
  #define KYBRD_FIRST_COLUMN               (uint16_t)319
  #define KYBRD_LAST_COLUMN                (uint16_t)7
  #define KYBRD_FIRST_LINE                 (uint8_t)120
  #define KYBRD_LAST_LINE                  (uint8_t)200

#endif


/**
* @}
*/ 
extern HID_Machine_TypeDef        HID_Machine;
extern uint8_t USBFS_EnumDone;
uint8_t  DEMO_StartHID = 0 ;



/////////////////////////////////////////////////////////
volatile u8 new_barcode = 0 ;
char barcode[30]  = {0};
volatile u8 barcode_counter =0;
const u8 BARCODE_MAX = 29;
////////////////////////////////////////////////////////

/** @defgroup USBH_HID_USR_Private_Variables
* @{
*/
extern USB_OTG_CORE_HANDLE           USB_OTG_FS_Core_dev;
/*  Points to the DEVICE_PROP structure of current device */
/*  The purpose of this register is to speed up the execution */

USBH_Usr_cb_TypeDef USR_HID_cb =
{
  USBH_HID_USR_Init,
  USBH_HID_USR_DeInit,
  USBH_HID_USR_DeviceAttached,
  USBH_HID_USR_ResetDevice,
  USBH_HID_USR_DeviceDisconnected,
  USBH_HID_USR_OverCurrentDetected,
  USBH_HID_USR_DeviceSpeedDetected,
  USBH_HID_USR_Device_DescAvailable,
  USBH_HID_USR_DeviceAddressAssigned,
  USBH_HID_USR_Configuration_DescAvailable,
  USBH_HID_USR_Manufacturer_String,
  USBH_HID_USR_Product_String,
  USBH_HID_USR_SerialNum_String,
  USBH_HID_USR_EnumerationDone,
  USBH_HID_USR_UserInput,
  NULL,
  USBH_HID_USR_DeviceNotSupported,
  USBH_HID_USR_UnrecoveredError
};

/**
* @}
*/

/** @defgroup USBH_HID_USR_Private_Constants
* @{
*/ 
/*--------------- LCD Messages ---------------*/
const uint8_t MSG_HOST_INIT[]          = "\r\n>USB FS:Host Library Initialized\n";
const uint8_t MSG_HID_DEV_ATTACHED[]       = "\r\n>USB FS:Device Attached\n";
const uint8_t MSG_HID_DEV_DISCONNECTED[]   = "\r\n>USB FS:Device Disconnected\n";
const uint8_t MSG_HID_DEV_ENUMERATED[]     = "\r\n>USB FS:Enumeration completed\n";
const uint8_t MSG_HID_DEV_HIGHSPEED[]      = "\r\n>USB FS:High speed device detected\n";
const uint8_t MSG_HID_DEV_FULLSPEED[]      = "\r\n>USB FS:Full speed device detected\n";
const uint8_t MSG_HID_DEV_LOWSPEED[]       = "\r\n>USB FS:Low speed device detected\n";
const uint8_t MSG_HID_DEV_ERROR[]          = "\r\n>USB FS:Device fault \r";
const uint8_t MSG_HID_CLASS[]              = "\r\n>USB FS:HID device connected\n";

const uint8_t USB_HID_MouseStatus[]    = "\r\n>USB FS:HID Demo Device : Mouse\n";
const uint8_t USB_HID_KeybrdStatus[]   = "\r\n>USB FS:HID Device : Barcode Ready!\r";
const uint8_t MSG_UNREC_ERROR[]        = "\r\n>USB FS:UNRECOVERED ERROR STATE\n";


/**
* @brief  USBH_HID_USR_Init 
*         Displays the message on LCD for host lib initialization
* @param  None
* @retval None
*/
void USBH_HID_USR_Init(void)
{
  
}

/**
* @brief  USBH_HID_USR_DeviceAttached 
*         Displays the message on LCD on device attached
* @param  None
* @retval None
*/
void USBH_HID_USR_DeviceAttached(void)
{  
		UsbPrint((void *)MSG_HID_DEV_ATTACHED);
}


/**
* @brief  USBH_HID_USR_UnrecoveredError
* @param  None
* @retval None
*/
void USBH_HID_USR_UnrecoveredError (void)
{
  LCD_ErrLog((void*)MSG_UNREC_ERROR);
}

/**
* @brief  USBH_HID_USR_DeInit
*         Deinit User state and associated variables
* @param  None
* @retval None
*/
void USBH_HID_USR_DeInit(void)
{
  
}


/**
* @brief  USBH_DisconnectEvent
*         Device disconnect event
* @param  None
* @retval None
*/
void USBH_HID_USR_DeviceDisconnected (void)
{
  LCD_ErrLog((void *)MSG_HID_DEV_DISCONNECTED);
  USBFS_EnumDone= 0;
}

/**
* @brief  USBH_HID_USR_ResetUSBDevice 
*         Reset USB Device
* @param  None
* @retval None
*/
void USBH_HID_USR_ResetDevice(void)
{
  /* Users can do their application actions here for the USB-Reset */
}


/**
* @brief  USBH_HID_USR_DeviceSpeedDetected 
*         Displays the message on LCD for device speed
* @param  Devicespeed : Device Speed
* @retval None
*/
void USBH_HID_USR_DeviceSpeedDetected(uint8_t DeviceSpeed)
{
  if(DeviceSpeed == HPRT0_PRTSPD_HIGH_SPEED)
  {
    UsbPrint2((void *)MSG_HID_DEV_HIGHSPEED);
  }  
  else if(DeviceSpeed == HPRT0_PRTSPD_FULL_SPEED)
  {
    UsbPrint2((void *)MSG_HID_DEV_FULLSPEED);
  }
  else if(DeviceSpeed == HPRT0_PRTSPD_LOW_SPEED)
  {
    UsbPrint2((void *)MSG_HID_DEV_LOWSPEED);
  }
  else
  {
    UsbPrint2((void *)MSG_HID_DEV_ERROR);
  }
}

/**
* @brief  USBH_HID_USR_Device_DescAvailable 
*         Displays the message on LCD for device descriptor
* @param  DeviceDesc : device descriptor
* @retval None
*/
void USBH_HID_USR_Device_DescAvailable(void *DeviceDesc)
{
	USBH_DevDesc_TypeDef *hs;
  hs = DeviceDesc;  
  
  
  UsbPrint("\r\n>USB FS:VID : %04Xh\n" , (uint32_t)(*hs).idVendor); 
  UsbPrint("\r\n>USB FS:PID : %04Xh\n" , (uint32_t)(*hs).idProduct); 
}

/**
* @brief  USBH_HID_USR_DeviceAddressAssigned 
*         USB device is successfully assigned the Address 
* @param  None
* @retval None
*/
void USBH_HID_USR_DeviceAddressAssigned(void)
{
  
}


/**
* @brief  USBH_HID_USR_Conf_Desc 
*         Displays the message on LCD for configuration descriptor
* @param  ConfDesc : Configuration descriptor
* @retval None
*/
void USBH_HID_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef * cfgDesc,
                                          USBH_InterfaceDesc_TypeDef *itfDesc,
                                          USBH_EpDesc_TypeDef *epDesc)
{
  USBH_InterfaceDesc_TypeDef *id;
  
  id = itfDesc;  
  
 if((*id).bInterfaceClass  == 0x03)
  {
    UsbPrint2((void *)MSG_HID_CLASS);
  }   
}

/**
* @brief  USBH_HID_USR_Manufacturer_String 
*         Displays the message on LCD for Manufacturer String 
* @param  ManufacturerString : Manufacturer String of Device
* @retval None
*/
void USBH_HID_USR_Manufacturer_String(void *ManufacturerString)
{
  char temp[100];
  sprintf(temp, "\r\n>USB FS:Manufacturer : %s\n", (char *)ManufacturerString);
  UsbPrint2((void *)temp);
  
}

/**
* @brief  USBH_HID_USR_Product_String 
*         Displays the message on LCD for Product String
* @param  ProductString : Product String of Device
* @retval None
*/
void USBH_HID_USR_Product_String(void *ProductString)
{
  char temp[100];
  sprintf((char *)temp, "\r\n>USB FS:Product : %s\n", (char *)ProductString);  
  UsbPrint2((void *)temp);
  
}

/**
* @brief  USBH_HID_USR_SerialNum_String 
*         Displays the message on LCD for SerialNum_String 
* @param  SerialNumString : SerialNum_String of device
* @retval None
*/
void USBH_HID_USR_SerialNum_String(void *SerialNumString)
{
  uint8_t temp[100];
  sprintf((char *)temp, "\r\n>USB FS:Serial Number : %s\n", (char *)SerialNumString);    
  UsbPrint2((void *)temp);
} 

/**
* @brief  EnumerationDone 
*         User response request is displayed to ask for
*         application jump to class
* @param  None
* @retval None
*/
void USBH_HID_USR_EnumerationDone(void)
{
  /* Enumeration complete */
  UsbPrint2((void *)MSG_HID_DEV_ENUMERATED);
  USBFS_EnumDone = 1;

} 

/**
* @brief  USBH_HID_USR_DeviceNotSupported
*         Device is not supported
* @param  None
* @retval None
*/
void USBH_HID_USR_DeviceNotSupported(void)
{
  UsbPrint2 ("\r\n>USB FS:Device not supported.\r");              
  
}  


/**
* @brief  USBH_HID_USR_UserInput
*         User Action for application state entry
* @param  None
* @retval USBH_USR_Status : User response for key button
*/
USBH_USR_Status USBH_HID_USR_UserInput(void)
{
 
  return USBH_USR_RESP_OK;
  
} 

/**
* @brief  USBH_HID_USR_OverCurrentDetected
*         Device Overcurrent detection event
* @param  None
* @retval None
*/
void USBH_HID_USR_OverCurrentDetected (void)
{
  LCD_ErrLog ("\r\n>USB FS:Overcurrent detected.\r");
  
}

/**
* @brief  USR_MOUSE_Init
*         Init Mouse window
* @param  None
* @retval None
*/
void USR_MOUSE_Init	(void)
{
  
}

/**
* @brief  USR_MOUSE_ProcessData
*         Process Mouse data
* @param  data : Mouse data to be displayed
* @retval None
*/
void USR_MOUSE_ProcessData(HID_MOUSE_Data_TypeDef *data)
{
  
 
  
}

/**
* @brief  USR_KEYBRD_Init
*         Init Keyboard window
* @param  None
* @retval None
*/
void  USR_KEYBRD_Init (void)
{
  

 // Xpos  = KYBRD_FIRST_LINE;
 // Ypos  = KYBRD_FIRST_COLUMN;

  //if(DEMO_HID_ShowData == 0)
  {
    UsbPrint((void*)USB_HID_KeybrdStatus); 
    //UsbPrint("\n______________ USER TEXT ______________\n"); 
  }  
}


/**
* @brief  USR_KEYBRD_ProcessData
*         Process Keyboard data
* @param  data : Keyboard data to be displayed
* @retval None
*/
void  USR_KEYBRD_ProcessData (uint8_t data)
{
	//UsbPrint("\r\n>USB FS: %c",data);
	
	if((data == '\r')||(data == '\n')||(data==0x0d)||(data==0x0a) )
  {
		if (barcode_counter > 0) // 이전 값이 있을때만 
		{
			barcode[barcode_counter]=0;
			new_barcode =1;					// 신규 바코드로 처리..
			barcode_counter=0;
			UsbPrint("\r\n>USB FS: new barcode %s",barcode);
		}
  }
  else
	{
		if(barcode_counter >= BARCODE_MAX)  // 버퍼 초과
		{
			barcode_counter=0;
			new_barcode =0;
			UsbPrint("\r\n>USB FS: barcode buffer overflow");
		}
		else
		{
			barcode[barcode_counter++]=data;
		}			
	}

}

/**
* @}
*/ 

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

