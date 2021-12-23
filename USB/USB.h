/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_H
#define __USB_H

/* Includes ------------------------------------------------------------------*/
#include "USB\usb_conf.h"
#include <stdio.h>
#include "USB\FATLib\ff.h"
#include "USB\usbh_msc_usr.h"

#define  LCD_ErrLog(...)    printf0(__VA_ARGS__);
#define  UsbPrint(...)    printf0(__VA_ARGS__);
#define  LCD_DbgLog(...)    printf0(__VA_ARGS__);
#define  UsbPrint2(...)   printf0(__VA_ARGS__);                         

#define DEMO_LOCK()                       demo.lock = 1;
#define DEMO_UNLOCK()                     demo.lock = 0;
#define DEMO_IS_LOCKED()                  (demo.lock == 1)


typedef enum {
  DEMO_IDLE   = 0,
  DEMO_WAIT,  
  DEMO_HID,
  DEMO_MSC,
}Demo_State;

typedef enum {
  DEMO_MSC_IDLE   = 0,
  DEMO_MSC_WAIT,  
}Demo_MSC_State;

typedef enum {
  DEMO_HID_IDLE   = 0,
  DEMO_HID_WAIT,    
}Demo_HID_State;

typedef struct _DemoStateMachine
{
  
  __IO Demo_State           state;
  __IO Demo_MSC_State       msc_state;
  __IO Demo_HID_State       hid_state;
  __IO uint8_t              select;
  __IO uint8_t              lock;
  
}DEMO_StateMachine;


extern uint8_t USBFS_EnumDone;
extern uint8_t USBHS_EnumDone;

void USB_Init (void);
void USB_FS_Init (void);

void USB_Process (void);
void USB_HS_Process (void);
void USB_FS_Process (void);


#endif
/* __USB_H */
