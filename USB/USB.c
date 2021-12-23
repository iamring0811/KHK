///////////////////////
///  프로젝트 옵션에 USE_USB_OTG_HS,USE_USB_OTG_FS,USE_EMBEDDED_PHY 추가
///

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "USB\usb_conf.h"
#include "USB\USB.h"
#include "USB\USBLib\usbh_core.h"
#include "USB\usbh_msc_usr.h"
#include "USB\usbh_hid_usr.h"
#include "USB\USBLib\usbh_msc_core.h"
#include "USB\USBLib\usbh_hid_core.h"
#include "util\usart.h"

uint8_t USBH_USR_ApplicationState = USH_USR_FS_INIT;
DEMO_StateMachine             demo;

uint8_t USBFS_EnumDone = 0;
uint8_t USBHS_EnumDone = 0;
uint8_t HS_DemoDisconnHandled = 0;
uint8_t FS_DemoDisconnHandled = 0;



#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USB_OTG_CORE_HANDLE           USB_OTG_Core __ALIGN_END ;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USBH_HOST                     USB_Host __ALIGN_END ;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USB_OTG_CORE_HANDLE           USB_OTG_FS_Core __ALIGN_END ;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USBH_HOST                     USB_FS_Process_Host __ALIGN_END ;



void USB_Init (void)
{
	UsbPrint("\r usb init Start -------");	
	///////////////////////////////////////////////////////////
	/* Hardware Init */
   	USB_OTG_BSP_Init(0);  
	
   	/* configure GPIO pin used for switching VBUS power */
//	USB_OTG_BSP_ConfigVBUS(&USB_OTG_Core);  
//	USB_OTG_BSP_ConfigVBUS(&USB_OTG_FS_Core); 
	////////////////////////////////////////////////////////////
	
	
   	/* Init HS Core */
   	USBH_Init(&USB_OTG_Core, 
            USB_OTG_HS_CORE_ID,
            &USB_Host,
            &USBH_MSC_cb, 
            &USR_MSC_cb);
	
  	/* Init FS Core */
  	USBH_Init(&USB_OTG_FS_Core, 
            USB_OTG_FS_CORE_ID,
            &USB_FS_Process_Host,
            &HID_cb, 
            &USR_HID_cb);  
						
	// USB 전원 인가
//	USB_OTG_DriveVbus(&USB_OTG_FS_Core, 1);
//	USB_OTG_DriveVbus(&USB_OTG_Core, 1);
  
	/////////////////////////////////////////////
	/* Enable Interrupts */
  	USB_OTG_BSP_EnableInterrupt(&USB_OTG_FS_Core); 
	USB_OTG_BSP_EnableInterrupt(&USB_OTG_Core);
	///////////////////////////////////////////////

  	USB_OTG_BSP_mDelay(500);
  
  	DEMO_UNLOCK();  
	
	// usb buffer 
	usbpnt = 0;
	UsbPrint(" usb init finish");	
}

/********************************************************************************/
extern u16	first_USB_HS;
void USB_HS_Init (void)
{
	UsbPrint("\r usb hs init Start -------");	
	///////////////////////////////////////////////////////////
	/* Hardware Init */
// 	USB_OTG_BSP_Init(0);  
	
   	/* Init HS Core */
   	USBH_Init(&USB_OTG_Core, 
            USB_OTG_HS_CORE_ID,
            &USB_Host,
            &USBH_MSC_cb, 
            &USR_MSC_cb);
	
	/////////////////////////////////////////////
	/* Enable Interrupts */
	USB_OTG_BSP_EnableInterrupt(&USB_OTG_Core);
	///////////////////////////////////////////////

	UsbPrint(" usb hs init finish");	
	first_USB_HS = 1;
}

/********************************************************************************/
void USB_FS_Init (void)
{
	UsbPrint("\r usb FS init Start -------");	
	///////////////////////////////////////////////////////////
	/* Hardware Init */
  	USB_OTG_BSP_Init(0);  
	
  	/* configure GPIO pin used for switching VBUS power */
//	USB_OTG_BSP_ConfigVBUS(&USB_OTG_FS_Core); 
	////////////////////////////////////////////////////////////
	
  	/* Init FS Core */
  	USBH_Init(&USB_OTG_FS_Core, 
            USB_OTG_FS_CORE_ID,
            &USB_FS_Process_Host,
            &HID_cb, 
            &USR_HID_cb);  
						
	// USB 전원 인가
//	USB_OTG_DriveVbus(&USB_OTG_FS_Core, 1);
  
	/* Enable Interrupts */
  	USB_OTG_BSP_EnableInterrupt(&USB_OTG_FS_Core); 

  	USB_OTG_BSP_mDelay(500);
  
  	DEMO_UNLOCK();  
	
	// usb buffer 
	usbpnt = 0;
	UsbPrint(" usb fs init finish");	
}

/********************************************************************************/
extern u16	flag_usb_fs;

void USB_Process (void)
{
	/* Host Task handler */
	if(HCD_IsDeviceConnected(&USB_OTG_Core))
	{
    	USBH_Process(&USB_OTG_Core, &USB_Host);
	}
	
	if(HCD_IsDeviceConnected(&USB_OTG_FS_Core))
	{
    	USBH_Process(&USB_OTG_FS_Core, &USB_FS_Process_Host);
	}
}

void USB_HS_Process (void)												//memory
{
	if(HCD_IsDeviceConnected(&USB_OTG_Core))
	{
    	USBH_Process(&USB_OTG_Core, &USB_Host);
//    	flag_usb_hs = 0;
	}
}
void USB_FS_Process (void)												//barcode
{
	if(HCD_IsDeviceConnected(&USB_OTG_FS_Core))
	{
    	USBH_Process(&USB_OTG_FS_Core, &USB_FS_Process_Host);
    	flag_usb_fs = 0;
	}
}



