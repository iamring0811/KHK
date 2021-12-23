/**
  ******************************************************************************
  * @file    usb_bsp.c
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    09-November-2015
  * @brief   This file implements the board support package for the USB host library
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
#include "USB\USBLib\usb_bsp.h"
#include "USB\USB.h"

///////////////////////////////////////////////////////////////////////////////
#define RCC_APB2Periph_GPIOA             ((uint32_t)0x00000004)
///////////////////////////////////////////////////////////////////////////////

#define USE_ACCURATE_TIME
#define TIM_MSEC_DELAY                     0x01
#define TIM_USEC_DELAY                     0x02


/// TODO : 아래는 과전류 관련 포트 인데.. 
#define HOST_OVRCURR_PORT                  GPIOE
#define HOST_OVRCURR_LINE                  GPIO_Pin_1
#define HOST_OVRCURR_PORT_SOURCE           GPIO_PortSourceGPIOE
#define HOST_OVRCURR_PIN_SOURCE            GPIO_PinSource1
#define HOST_OVRCURR_PORT_RCC              RCC_APB2Periph_GPIOE
#define HOST_OVRCURR_EXTI_LINE             EXTI_Line1
#define HOST_OVRCURR_IRQn                  EXTI1_IRQn 



#define HOST_POWERSW_PORT_RCC_FS            RCC_AHB1Periph_GPIOA
#define HOST_POWERSW_PORT_FS                GPIOA
#define HOST_POWERSW_VBUS_FS             		GPIO_Pin_10

#define HOST_POWERSW_PORT_RCC_HS            RCC_AHB1Periph_GPIOB
#define HOST_POWERSW_PORT_HS                GPIOB
#define HOST_POWERSW_VBUS_HS            		GPIO_Pin_12

ErrorStatus HSEStartUpStatus;
__IO uint32_t BSP_delay = 0;


static void BSP_SetTime(uint8_t Unit);
static void BSP_Delay(uint32_t nTime,uint8_t Unit);
static void USB_OTG_BSP_TimeInit ( void );


/**
  * @brief  USB_OTG_BSP_Init
  *         Initializes BSP configurations
  * @param  None
  * @retval None
  */

void USB_OTG_BSP_Init(USB_OTG_CORE_HANDLE *pdev)
{
  GPIO_InitTypeDef GPIO_InitStructure;

	#ifdef USE_USB_OTG_FS 
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA , ENABLE);  
	
  /* Configure  VBUS DM DP Pins */
	  GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_9  | 
      GPIO_Pin_11 | 
        GPIO_Pin_12;
	
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);  
  
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_OTG1_FS) ; 
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource11,GPIO_AF_OTG1_FS) ; 
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource12,GPIO_AF_OTG1_FS) ;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_OTG_FS, ENABLE) ; 
#endif 

#ifdef USE_USB_OTG_HS 
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB , ENABLE);

  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_13 |
      GPIO_Pin_14 | 
        GPIO_Pin_15;

  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_Init(GPIOB, &GPIO_InitStructure);  
  
  //GPIO_PinAFConfig(GPIOB,GPIO_PinSource12,GPIO_AF_OTG2_FS) ; 
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_OTG2_FS) ; 
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource14,GPIO_AF_OTG2_FS) ; 
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_OTG2_FS) ;
	
  /* USB OTG HS ULPI clock Disabled*/
  RCC_AHB1PeriphClockLPModeCmd(RCC_AHB1Periph_OTG_HS_ULPI, DISABLE);    
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_OTG_HS, ENABLE);

#endif


  /* Initialize Timer for delay function */
  USB_OTG_BSP_TimeInit();   
}
/**
  * @brief  USB_OTG_BSP_EnableInterrupt
  *         Configures USB Global interrupt
  * @param  None
  * @retval None
  */
void USB_OTG_BSP_EnableInterrupt(USB_OTG_CORE_HANDLE *pdev)
{
  NVIC_InitTypeDef NVIC_InitStructure; 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	if(pdev->cfg.coreID == USB_OTG_FS_CORE_ID)
  {
		NVIC_InitStructure.NVIC_IRQChannel = OTG_FS_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);  
  }
	else if(pdev->cfg.coreID == USB_OTG_HS_CORE_ID)
	{
		NVIC_InitStructure.NVIC_IRQChannel = OTG_HS_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);    
	}
}

/**
  * @brief  BSP_Drive_VBUS
  *         Drives the Vbus signal through IO
  * @param  state : VBUS states
  * @retval None
  */

void USB_OTG_BSP_DriveVBUS(USB_OTG_CORE_HANDLE *pdev, uint8_t state)
{
  /*
  On-chip 5 V VBUS generation is not supported. For this reason, a charge pump 
  or, if 5 V are available on the application board, a basic power switch, must 
  be added externally to drive the 5 V VBUS line. The external charge pump can 
  be driven by any GPIO output. When the application decides to power on VBUS 
  using the chosen GPIO, it must also set the port power bit in the host port 
  control and status register (PPWR bit in OTG_FS_HPRT).
  
  Bit 12 PPWR: Port power
  The application uses this field to control power to this port, and the core 
  clears this bit on an overcurrent condition.
  */ 
	if(pdev->cfg.coreID == USB_OTG_FS_CORE_ID)
  {
		if (0 == state)
		{ 
			/* DISABLE is needed on output of the Power Switch */
			GPIO_SetBits(HOST_POWERSW_PORT_FS,HOST_POWERSW_VBUS_FS  );
		}
		else
		{
			/*ENABLE the Power Switch by driving the Enable LOW */
			GPIO_ResetBits(HOST_POWERSW_PORT_FS,HOST_POWERSW_VBUS_FS  );
		}
	}

  if(pdev->cfg.coreID == USB_OTG_HS_CORE_ID)
	{
		if (0 == state)
		{ 
			/* DISABLE is needed on output of the Power Switch */
			GPIO_SetBits(HOST_POWERSW_PORT_HS  , HOST_POWERSW_VBUS_HS       );
		}
		else
		{
			/*ENABLE the Power Switch by driving the Enable LOW */
			GPIO_ResetBits(HOST_POWERSW_PORT_HS   , HOST_POWERSW_VBUS_HS      );
		}
	}
}

/**
  * @brief  USB_OTG_BSP_ConfigVBUS
  *         Configures the IO for the Vbus and OverCurrent
  * @param  None
  * @retval None
  */

void  USB_OTG_BSP_ConfigVBUS(USB_OTG_CORE_HANDLE *pdev)
{
   if(pdev->cfg.coreID == USB_OTG_FS_CORE_ID)
	 {
			GPIO_InitTypeDef GPIO_InitStructure; 
			RCC_AHB1PeriphClockCmd( HOST_POWERSW_PORT_RCC_FS   , ENABLE);  
			//UsbPrint("\r\n--- USB_OTG_BSP_ConfigVBUS  - RCC_AHB1PeriphClockCmd pass\n");
			//kdelay(500000);	
		
			GPIO_InitStructure.GPIO_Pin = HOST_POWERSW_VBUS_FS  ;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
			GPIO_Init(HOST_POWERSW_PORT_FS,&GPIO_InitStructure);
			
			/* By Default, DISABLE is needed on output of the Power Switch */
			GPIO_SetBits(HOST_POWERSW_PORT_FS, HOST_POWERSW_VBUS_FS );
			
			USB_OTG_BSP_mDelay(200);   /* Delay is need for stabilising the Vbus Low */
			//in Reset Condition, when Vbus=1 and Reset-button is pressed by user *
	}

	if (pdev->cfg.coreID == USB_OTG_HS_CORE_ID)		
	{
		GPIO_InitTypeDef GPIO_InitStructure2; 
		RCC_AHB1PeriphClockCmd( HOST_POWERSW_PORT_RCC_HS   , ENABLE);  
		//UsbPrint("\r\n--- USB_OTG_BSP_ConfigVBUS  - RCC_APB2PeriphClockCmd pass\n");
		//kdelay(500000);	
	
		GPIO_InitStructure2.GPIO_Pin = HOST_POWERSW_VBUS_HS  ;
		GPIO_InitStructure2.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_InitStructure2.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure2.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure2.GPIO_PuPd = GPIO_PuPd_NOPULL ;
		GPIO_Init(HOST_POWERSW_PORT_HS ,&GPIO_InitStructure2);
		
		/* By Default, DISABLE is needed on output of the Power Switch */
		GPIO_SetBits(HOST_POWERSW_PORT_HS , HOST_POWERSW_VBUS_HS  );
		
		USB_OTG_BSP_mDelay(200);   /* Delay is need for stabilising the Vbus Low */
		//in Reset Condition, when Vbus=1 and Reset-button is pressed by user *
	}
}

/**
  * @brief  USB_OTG_BSP_TimeInit
  *         Initializes delay unit using Timer2
  * @param  None
  * @retval None
  */
static void USB_OTG_BSP_TimeInit ( void )
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Set the Vector Table base address at 0x08000000 */
//  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x00);
  
  /* Configure the Priority Group to 2 bits */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  /* Enable the TIM5 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  
  NVIC_Init(&NVIC_InitStructure);
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);  
}

/**
  * @brief  USB_OTG_BSP_uDelay
  *         This function provides delay time in micro sec
  * @param  usec : Value of delay required in micro sec
  * @retval None
  */
void USB_OTG_BSP_uDelay (const uint32_t usec)
{
	BSP_Delay(usec,TIM_USEC_DELAY); 
}


/**
  * @brief  USB_OTG_BSP_mDelay
  *          This function provides delay time in milli sec
  * @param  msec : Value of delay required in milli sec
  * @retval None
  */
void USB_OTG_BSP_mDelay (const uint32_t msec)
{ 
  BSP_Delay(msec,TIM_MSEC_DELAY);   
}


/**
  * @brief  USB_OTG_BSP_TimerIRQ
  *         Time base IRQ
  * @param  None
  * @retval None
  */

void USB_OTG_BSP_TimerIRQ (void)
{
  if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
    if (BSP_delay > 0x00)
    { 
      BSP_delay--;
    }
    else
    {
      TIM_Cmd(TIM5,DISABLE);
    }
  }
} 

/**
  * @brief  BSP_Delay
  *         Delay routine based on TIM5
  * @param  nTime : Delay Time 
  * @param  unit : Delay Time unit : mili sec / micro sec
  * @retval None
  */
static void BSP_Delay(uint32_t nTime, uint8_t unit)
{
  
  BSP_delay = nTime;
  BSP_SetTime(unit);  
  while(BSP_delay != 0);
  TIM_Cmd(TIM5,DISABLE);
}

/**
  * @brief  BSP_SetTime
  *         Configures TIM5 for delay routine based on TIM5
  * @param  unit : msec /usec
  * @retval None
  */
static void BSP_SetTime(uint8_t unit)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  
  TIM_Cmd(TIM5,DISABLE);
  TIM_ITConfig(TIM5, TIM_IT_Update, DISABLE); 
  
  
  if(unit == TIM_USEC_DELAY)
  {  
    TIM_TimeBaseStructure.TIM_Period = 11;
  }
  else if(unit == TIM_MSEC_DELAY)
  {
    TIM_TimeBaseStructure.TIM_Period = 11999;
  }
  TIM_TimeBaseStructure.TIM_Prescaler = 5;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  
  TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
  TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
  
  TIM_ARRPreloadConfig(TIM5, ENABLE);
  
  /* TIM IT enable */
  TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
  
  /* TIM5 enable counter */ 
  TIM_Cmd(TIM5, ENABLE);  
} 
