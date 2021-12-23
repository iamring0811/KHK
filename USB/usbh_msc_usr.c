/**
  ******************************************************************************
  * @file    usbh_usr.c
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    09-November-2015
  * @brief   This file includes the usb host library user callbacks
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
#include <string.h>
#include "USB\usbh_msc_usr.h"
#include "USB\FATLib\ff.h"       /* FATFS */
#include "USB\USBLib\usbh_msc_core.h"
#include "USB\USBLib\usbh_msc_scsi.h"
#include "USB\USBLib\usbh_msc_bot.h"

#include "util\usart.h"

extern FATFS fatfs;
extern FIL file;

extern DEMO_StateMachine             demo;
uint8_t line_idx = 0; 
uint8_t writeTextBuff[] = "STM32 Connectivity line Host Demo application using FAT_FS   ";  

extern uint8_t USBHS_EnumDone;
extern USB_OTG_CORE_HANDLE          USB_OTG_Core;

/*  Points to the DEVICE_PROP structure of current device */
/*  The purpose of this register is to speed up the execution */

USBH_Usr_cb_TypeDef USR_MSC_cb =
{
  USBH_USR_Init,
  USBH_USR_DeInit,
  USBH_USR_DeviceAttached,
  USBH_USR_ResetDevice,
  USBH_USR_DeviceDisconnected,
  USBH_USR_OverCurrentDetected,
  USBH_USR_DeviceSpeedDetected,
  USBH_USR_Device_DescAvailable,
  USBH_USR_DeviceAddressAssigned,
  USBH_USR_Configuration_DescAvailable,
  USBH_USR_Manufacturer_String,
  USBH_USR_Product_String,
  USBH_USR_SerialNum_String,
  USBH_USR_EnumerationDone,
  USBH_USR_UserInput,
  USBH_USR_MSC_Application,
  USBH_USR_DeviceNotSupported,
  USBH_USR_UnrecoveredError
    
};

/**
* @}
*/

/** @defgroup USBH_USR_Private_Constants
* @{
*/ 
/*--------------- LCD Messages ---------------*/
const uint8_t MSG_DEV_ATTACHED[]     = "\r\n>USB HS:Device Attached \r";
const uint8_t MSG_DEV_DISCONNECTED[] = "\r\n>USB HS:Device Disconnected\n";
const uint8_t MSG_DEV_ENUMERATED[]   = "\r\n>USB HS:Enumeration completed \r";
const uint8_t MSG_DEV_HIGHSPEED[]    = "\r\n>USB HS:High speed device detected\n";
const uint8_t MSG_DEV_FULLSPEED[]    = "\r\n>USB HS:Full speed device detected\n";
const uint8_t MSG_DEV_LOWSPEED[]     = "\r\n>USB HS:Low speed device detected\n";
const uint8_t MSG_DEV_ERROR[]        = "\r\n>USB HS:Device fault \r";

const uint8_t MSG_MSC_CLASS[]        = "\r\n>USB HS:Mass storage device connected\n";
const uint8_t MSG_DISK_SIZE[]        = "\r\n>USB HS:Size of the disk in MBytes: \r";
const uint8_t MSG_LUN[]              = "\r\n>USB HS:LUN Available in the device:\r";
const uint8_t MSG_ROOT_CONT[]        = "\r\n>USB HS:Exploring disk flash ...\r";
const uint8_t MSG_WR_PROTECT[]       = "\r\n>USB HS:The disk is write protected\n";
const uint8_t MSG_MSC_UNREC_ERROR[]  = "\r\n>USB HS:UNRECOVERED ERROR STATE\n";



/**
* @brief  USBH_USR_Init 
*         Displays the message on LCD for host lib initialization
* @param  None
* @retval None
*/
void USBH_USR_Init(void)
{
		USBH_USR_ApplicationState = USH_USR_FS_INIT;
}

/**
* @brief  USBH_USR_DeviceAttached 
*         Displays the message on LCD on device attached
* @param  None
* @retval None
*/
void USBH_USR_DeviceAttached(void)
{
	UsbPrint((void *)MSG_DEV_ATTACHED);
}


/**
* @brief  USBH_USR_UnrecoveredError
* @param  None
* @retval None
*/
void USBH_USR_UnrecoveredError (void)
{
  
  /* Set default screen color*/ 
  LCD_ErrLog((void *)MSG_MSC_UNREC_ERROR); 
}


/**
* @brief  USBH_DisconnectEvent
*         Device disconnect event
* @param  None
* @retval Status
*/
void USBH_USR_DeviceDisconnected (void)
{
	USBH_USR_ApplicationState = USH_USR_FS_INIT;
  LCD_ErrLog((void *)MSG_DEV_DISCONNECTED);
  USBHS_EnumDone= 0;
}
/**
* @brief  USBH_USR_ResetUSBDevice 
* @param  None
* @retval None
*/
void USBH_USR_ResetDevice(void)
{
  /* callback for USB-Reset */
	USBH_USR_ApplicationState = USH_USR_FS_INIT;
}


/**
* @brief  USBH_USR_DeviceSpeedDetected 
*         Displays the message on LCD for device speed
* @param  Device speed
* @retval None
*/
void USBH_USR_DeviceSpeedDetected(uint8_t DeviceSpeed)
{
  if(DeviceSpeed == HPRT0_PRTSPD_HIGH_SPEED)
  {
    UsbPrint((void *)MSG_DEV_HIGHSPEED);
  }  
  else if(DeviceSpeed == HPRT0_PRTSPD_FULL_SPEED)
  {
    UsbPrint((void *)MSG_DEV_FULLSPEED);
  }
  else if(DeviceSpeed == HPRT0_PRTSPD_LOW_SPEED)
  {
    UsbPrint((void *)MSG_DEV_LOWSPEED);
  }
  else
  {
    UsbPrint((void *)MSG_DEV_ERROR);
  }
}

/**
* @brief  USBH_USR_Device_DescAvailable 
*         Displays the message on LCD for device descriptor
* @param  device descriptor
* @retval None
*/
void USBH_USR_Device_DescAvailable(void *DeviceDesc)
{ 
	USBH_DevDesc_TypeDef *hs;
  hs = DeviceDesc;  
  
  
  UsbPrint("\r\nVID : %04Xh\n" , (uint32_t)(*hs).idVendor); 
  UsbPrint("\r\nPID : %04Xh\n" , (uint32_t)(*hs).idProduct); 
	
}

/**
* @brief  USBH_USR_DeviceAddressAssigned 
*         USB device is successfully assigned the Address 
* @param  None
* @retval None
*/
void USBH_USR_DeviceAddressAssigned(void)
{
  
}


/**
* @brief  USBH_USR_Conf_Desc 
*         Displays the message on LCD for configuration descriptor
* @param  Configuration descriptor
* @retval None
*/
void USBH_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef * cfgDesc,
                                          USBH_InterfaceDesc_TypeDef *itfDesc,
                                          USBH_EpDesc_TypeDef *epDesc)
{
  USBH_InterfaceDesc_TypeDef *id;
  
  id = itfDesc;  
  
  if((*id).bInterfaceClass  == 0x08)
  {
    UsbPrint((void *)MSG_MSC_CLASS);
  }
	
}

/**
* @brief  USBH_USR_Manufacturer_String 
*         Displays the message on LCD for Manufacturer String 
* @param  Manufacturer String 
* @retval None
*/
void USBH_USR_Manufacturer_String(void *ManufacturerString)
{
  UsbPrint("\r\n>USB HS:Manufacturer : %s\n", (char *)ManufacturerString);
}

/**
* @brief  USBH_USR_Product_String 
*         Displays the message on LCD for Product String
* @param  Product String
* @retval None
*/
void USBH_USR_Product_String(void *ProductString)
{
  UsbPrint("\r\n>USB HS:Product : %s\n", (char *)ProductString);  
}

/**
* @brief  USBH_USR_SerialNum_String 
*         Displays the message on LCD for SerialNum_String 
* @param  SerialNum_String 
* @retval None
*/
void USBH_USR_SerialNum_String(void *SerialNumString)
{
  UsbPrint( "\r\n>USB HS:Serial Number : %s\n", (char *)SerialNumString);    
} 



/**
* @brief  EnumerationDone 
*         User response request is displayed to ask application jump to class
* @param  None
* @retval None
*/
void USBH_USR_EnumerationDone(void)
{
  
  /* Enumeration complete */
  UsbPrint((void *)MSG_DEV_ENUMERATED);
  USBHS_EnumDone = 1;
 
} 


/**
* @brief  USBH_USR_DeviceNotSupported
*         Device is not supported
* @param  None
* @retval None
*/
void USBH_USR_DeviceNotSupported(void)
{
  UsbPrint ("\r\n>USB HS: Device not supported : LUN >Port2:0 "); 
}  


/**
* @brief  USBH_USR_UserInput
*         User Action for application state entry
* @param  None
* @retval USBH_USR_Status : User response for key button
*/
USBH_USR_Status USBH_USR_UserInput(void)
{
  return USBH_USR_RESP_OK;
}  

/**
* @brief  USBH_USR_OverCurrentDetected
*         Over Current Detected on VBUS
* @param  None
* @retval Status
*/
void USBH_USR_OverCurrentDetected (void)
{
  LCD_ErrLog ("\r\n>USB HS:Overcurrent detected.");
}


/**
* @brief  USBH_USR_MSC_Application 
*         Demo application for mass storage
* @param  None
* @retval Status
*/
int USBH_USR_MSC_Application(void)
{


	switch(USBH_USR_ApplicationState)
  {
		case USH_USR_FS_INIT: 

			/* Initialises the File System*/
			if ( f_mount(&fatfs, "", 0) != FR_OK ) 
			{
				/* efs initialisation fails*/
				LCD_ErrLog("\r\n>USB HS:Cannot initialize File System.\r");
				return(-1);
			}
			UsbPrint("\r\n>USB HS:File System initialized.\r");
			
			USBH_USR_ApplicationState = USH_USR_FS_FAT_INIT;
			
			//UsbPrint("\r\n>USB HS:Disk capacity : %d Bytes\n", USBH_MSC_Param.MSCapacity * \
			//	USBH_MSC_Param.MSPageLength); 
			

			break;
		case USH_USR_FS_FAT_INIT:  
			break;
		default: break;
  }
  return(0);
}

/**
* @brief  USBH_USR_DeInit
*         Deinit User state and associated variables
* @param  None
* @retval None
*/
void USBH_USR_DeInit(void)
{
  USBH_USR_ApplicationState = USH_USR_FS_INIT;
}




/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

