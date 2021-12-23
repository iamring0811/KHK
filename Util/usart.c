#define _UART_C_

#include <stdarg.h>
#include <string.h>
#include "common\32f429_port.h"
#include "util\usart.h"
#include "util\tm_adc.h"
#include "util\eep.h"
#include "resm.h"

extern void STM_EVAL_COMInit(COM_TypeDef COM, USART_InitTypeDef* USART_InitStruct);
extern u16	flag_motor_run;
extern u8	flag_flash;

void stepa_loop(void);
void stepb_loop(void);

u16 dly_stepa, dly_stepb;

/*******************************************************************************/
void wait_tx2(void){ while(txw2!=txr2); }
void wait_tx3(void){ while(txw3!=txr3); }
void wait_tx4(void){ while(txw4!=txr4); }
void wait_tx5(void){ while(txw5!=txr5); }

void Uart_Send2(char *str){
//	while(*str)	savcmd2(*str++);
	while(*str){
		*(__IO uint8_t*)(tx2buf+txw2) = *str++;	txw2++;
	}
}

void Uart_Send3(char *str){
	while(*str){
		*(__IO uint8_t*)(tx3buf+txw3) = *str++;	txw3++;
	}
}
void Uart_Send4(char *str){
	while(*str){
		*(__IO uint8_t*)(tx4buf+txw4) = *str++;	txw4++;
	}
}
void Uart_Send5(char *str){ 
//	while(*str) tx5buf[txw5++] = *str++; 
	while(*str){
		*(__IO uint8_t*)(tx5buf+txw5) = *str++;	txw5++;
	}
}

/*******************************************************************************/
//void savcmd1(char ch){	*(__IO uint8_t*)(tx1buf+txw1) = ch;	txw1++; }
void savcmd1(u8 ch){ tx1buf[txw1++] = ch; }
void savcmd2(u8 ch){
//	tx2buf[txw2++] = ch; 
	*(__IO uint8_t*)(tx2buf+txw2) = ch;	txw2++;
}
void savcmd3(u8 ch){ 
	*(u8*)(tx3buf+txw3) = ch; txw3++; 		//usb
	*(u8*)(tx4buf+txw4) = ch; txw4++; 		//rs232
}
void savcmd4(u8 ch){ *(u8*)(tx4buf+txw4) = ch; txw4++; }
void savcmd5(u8 ch){ *(u8*)(tx5buf+txw5) = ch; txw5++; }
void savcmd6(u8 ch){ tx6buf[txw6++] = ch; }
void savcmd7(u8 ch){ tx7buf[txw7++] = ch; }

u8 getcmd1(void){	return(rx1buf[rxr1++]); }
u8 getcmd2(void){	return(rx2buf[rxr2++]); }
u8 getcmd3(void){	return(rx3buf[rxr3++]); }
u8 getcmd4(void){	return(rx4buf[rxr4++]); }
u8 getcmd5(void){	
#ifdef INT_RXBUF5
	return(rx5buf[rxr5++]); 
#else
	u8 b1;
	b1 = *(u8*)(rx5buf+rxr5); rxr5++;
	return(b1); 
#endif
}
u8 getcmd6(void){	return(rx6buf[rxr6++]); }
u8 getcmd7(void){	return(rx7buf[rxr7++]); }

/*******************************************************************************/
void printf2(char *fmt,...)
{
	va_list ap;
    va_start(ap,fmt);
    vsprintf(string,fmt,ap);
	Uart_Send2(string);
	va_end(ap);
}
void printf3(char *fmt,...)
{
	va_list ap;
    va_start(ap,fmt);
    vsprintf(string,fmt,ap);
	Uart_Send3(string);
	va_end(ap);
}
void printf4(char *fmt,...)
{
	va_list ap;
    va_start(ap,fmt);
    vsprintf(string,fmt,ap);
	Uart_Send4(string);
//	Uart_Send5(string);
	va_end(ap);
}
void printf5(char *fmt,...)
{
	va_list ap;
    va_start(ap,fmt);
    vsprintf(string,fmt,ap);
	Uart_Send5(string);
//	if(flag_flash==3) Uart_Send3(string);
	va_end(ap);
}
void prt_msg(char *fmt,...)
{
	if(OUT_MSG) printf0(fmt);
	wait_tx0();
}
/*******************************************************************************/
/*******************************************************************************/
void USART_Config(void)
{
	USART_InitTypeDef USART_InitStructure;

	rxw1=rxr1=txw1=txr1=0;
	rxw3=rxr3=txw3=txr3=0;
	rxw5=rxr5=txw5=txr5=0;    

	rxw2=rxr2=txw2=txr2=0;
	rxw4=rxr4=txw4=txr4=0;
	rxw6=rxr6=txw6=txr6=0;
	rxw7=rxr7=txw7=txr7=0;

	/* USARTx configured as follows:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
	*/
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_InitStructure.USART_BaudRate = 115200;
	STM_EVAL_COMInit(COM5, &USART_InitStructure);		// Debug com5
if  (BAUD_FLAG == 2)	STM_EVAL_COMInit(COM4, &USART_InitStructure);		// RS232 com4
if  (SBAUD_FLAG == 2)	STM_EVAL_COMInit(COM3, &USART_InitStructure);		// PC Com.(USB) 
	USART_InitStructure.USART_BaudRate = 38400;

	if (BAUD_FLAG == 1)		STM_EVAL_COMInit(COM4, &USART_InitStructure);		// RS232 com4
	if (SBAUD_FLAG == 1)	STM_EVAL_COMInit(COM3, &USART_InitStructure);		// PC Com.(USB) 
	STM_EVAL_COMInit(COM6, &USART_InitStructure);		// Earthner     
	STM_EVAL_COMInit(COM7, &USART_InitStructure);		// Bluetooth    

	USART_InitStructure.USART_BaudRate = 57600;
//	USART_InitStructure.USART_BaudRate = 57692;
//	USART_InitStructure.USART_BaudRate = 19230;
	STM_EVAL_COMInit(COM1, &USART_InitStructure);		// Measure



	USART_InitStructure.USART_BaudRate = 9600;
	if (BAUD_FLAG == 0)		STM_EVAL_COMInit(COM4, &USART_InitStructure);		// RS232 com4
	if (SBAUD_FLAG == 0)	STM_EVAL_COMInit(COM3, &USART_InitStructure);		// PC Com.(USB) 
	STM_EVAL_COMInit(COM2, &USART_InitStructure);		// Strip Check + print_thermal
}
/*******************************************************************************/
void set_baud_rs232(void){
	USART_InitTypeDef USART_InitStructure;
	rxw4 = rxr4 = txw4 = txr4 = 0;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	if (BAUD_FLAG == 0) {
		USART_InitStructure.USART_BaudRate = 9600;
	}
	else if (BAUD_FLAG == 1) {
		USART_InitStructure.USART_BaudRate = 38400;
	}
	else {			// 2
		USART_InitStructure.USART_BaudRate = 115200;
	}
	STM_EVAL_COMInit(COM4, &USART_InitStructure);		// RS232

}
void set_baud_usb(void) {
	USART_InitTypeDef USART_InitStructure;
	rxw3 = rxr3 = txw3 = txr3 = 0;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	if (SBAUD_FLAG == 0) {
		USART_InitStructure.USART_BaudRate = 9600;
	}
	else if (SBAUD_FLAG == 1) {
		USART_InitStructure.USART_BaudRate = 38400;
	}
	else {			// 7
		USART_InitStructure.USART_BaudRate = 115200;
	}
	STM_EVAL_COMInit(COM3, &USART_InitStructure);		// USB
}
/*******************************************************************************/
void com_clear(void){
	wait_tx3();
	rxw3=rxr3=txw3=txr3=0x00;
	wait_tx4();
	rxw4=rxr4=txw4=txr4=0x00;
	wait_tx5();
	rxw5=rxr5=txw5=txr5=0x00;
}

/*******************************************************************************/
/*************************    TIMER2     ***************************************/
/*******************************************************************************/

void TIM_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	/* Configure the Priority Group to 2 bits */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	/* Enable the TIM2 gloabal Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);  
}

void TIM_SetUsec(uint32_t usec)
{  
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_Cmd(TIM2,DISABLE);
	TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE); 
    
	TIM_TimeBaseStructure.TIM_Period = usec * 15;
	TIM_TimeBaseStructure.TIM_Prescaler = 5;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	
  	TIM_ARRPreloadConfig(TIM2, ENABLE);    
	/* TIM IT enable */
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	/* TIM2 enable counter */ 
	TIM_Cmd(TIM2, ENABLE);  
}

/*****************************************************************************/
extern u8 tick_usb_process;
#define MOVE_SPEED				25										//motor speed
#define PUSH_SPEED				25

void TIM2_IRQHandler(void)												// 75[us]
{
u8 baud_delay_rs232, baud_delay_usb;
	if(BAUD_FLAG == 0)baud_delay_rs232=7;
	if(BAUD_FLAG == 1)baud_delay_rs232=1;
	if(BAUD_FLAG == 2)baud_delay_rs232=0;
	if(SBAUD_FLAG == 0)baud_delay_usb=7;
	if(SBAUD_FLAG == 1)baud_delay_usb=1;
	if(SBAUD_FLAG == 2)baud_delay_usb=0;
	
   	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	PORT_LEDOn(LEDR);
	
	if(flag_mtab==MT_A){
		if((++dly_stepa)>=MOVE_SPEED){										
			dly_stepa=0;
			stepa_loop();												// Shifter
		}
	}else{
		if((++dly_stepa)>=PUSH_SPEED){										
			stepb_loop();												// PushBar
			dly_stepa=0;
		}
	}
	
	if(USART_GetFlagStatus(EVAL_COM1, USART_FLAG_RXNE)){				// Measure
		rx1buf[rxw1++] = USART_ReceiveData(EVAL_COM1);
	} 
	if(USART_GetFlagStatus(EVAL_COM5, USART_FLAG_RXNE)){				// Debug
#ifdef INT_RXBUF5
		rx5buf[rxw5++] = USART_ReceiveData(EVAL_COM5);
#else
		*(u8*)(rx5buf+rxw5) = USART_ReceiveData(EVAL_COM5); rxw5++;
#endif
	}
	if(USART_GetFlagStatus(EVAL_COM4, USART_FLAG_RXNE)){				// RS232
		rx4buf[rxw4++] = USART_ReceiveData(EVAL_COM4);
	}

	if(!evenodd){														// 150[us]
		if(txw5!=txr5){
			UART5->DR  = *(__IO uint8_t*)(tx5buf+txr5); txr5++;
		}
		if((++baud_delay)>baud_delay_rs232){
			if(txw4!=txr4){												// RS232
				UART4->DR  = *(__IO uint8_t*)(tx4buf+txr4); txr4++;		
			}
			baud_delay=0;
		}
		if((++baud_delay2)>baud_delay_usb){
			if(txw3!=txr3){												// 이동
				USART3->DR  = *(__IO uint8_t*)(tx3buf+txr3); txr3++;	// 이동
			}		
			baud_delay2=0;												// 이동
		}
		if(!evenod2){													// 300[us] FOR 38,400BPS TX
			if(txw6!=txr6) USART6->DR = tx6buf[txr6++];
			evenod2=1;
		}else{
			if(txw1!=txr1) USART1->DR = tx1buf[txr1++];					// Measure
			if(txw7!=txr7) UART7->DR = tx7buf[txr7++];
			evenod2=0;
		}

		evenodd=1;
		delay_150++;
	}else{																// 150[us]			
		if(USART_GetFlagStatus(EVAL_COM3, USART_FLAG_RXNE)){			// PC Com.
			rx3buf[rxw3++] = USART_ReceiveData(EVAL_COM3);
		}
		if(USART_GetFlagStatus(EVAL_COM6, USART_FLAG_RXNE)){
			rx6buf[rxw6++] = USART_ReceiveData(EVAL_COM6);
		}
		if(USART_GetFlagStatus(EVAL_COM7, USART_FLAG_RXNE)){
			rx7buf[rxw7++] = USART_ReceiveData(EVAL_COM7);
		}						
		switch(dscan){
			case 0:														// 600[us]
				if(!evenod3){											//1200[us]
					if(txw2!=txr2){ USART2->DR = *(__IO uint8_t*)(tx2buf+txr2); txr2++; }
					evenod3=1;
				}else{
					if(buzz_delay){	
						if(!(--buzz_delay))	PORT_LEDlo(ENVE );
					}
					evenod3=0;
				}
			break;
			case 1:														//600[us]
				tick_usb_process=1;
				usec_delay += 600;
				if(usec_delay>=10000){
					usec_delay -= 10000;
					MS10 = 1;
					if(flag_motor_run) motor_wait++;
				}
			break;
			case 2:														//600[us]
				if(USART_GetFlagStatus(EVAL_COM2, USART_FLAG_RXNE)){
					rx2buf[rxw2++] = USART_ReceiveData(EVAL_COM2);
				}
			break;
			case 3:
				adc_loop();												//600[us]
			break;
		}
		dscan = (dscan+1) & 3;
		evenodd=0;
	}	
	PORT_LEDOff(LEDR);
}
/*****************************************************************************/
/*****************************************************************************/
#define MTA_MASK	(80 * 8) - 1
void stepa_loop(void){				//PushBar
	SENS_ON();
	if(run_stepa){
		switch(run_stepa&7){
			case 0:	step1hi(); step2lo(); step3lo(); step4lo(); break;
			case 1:	step1hi(); step2hi(); step3lo(); step4lo(); break;
			case 2:	step1lo(); step2hi(); step3lo(); step4lo(); break;
			case 3:	step1lo(); step2hi(); step3hi(); step4lo(); break;
			case 4:	step1lo(); step2lo(); step3hi(); step4lo(); break;
			case 5:	step1lo(); step2lo(); step3hi(); step4hi(); break;
			case 6:	step1lo(); step2lo(); step3lo(); step4hi(); break;
			case 7:	step1hi(); step2lo(); step3lo(); step4hi(); break;
		}
		run_stepa--;
	}else{ step1lo(); step2lo(); step3lo(); step4lo(); }

	if((run_stepa<(TICK_STEPA-200))&&(run_stepa>MTA_MASK)){
		if(PH_MTA){
#ifndef NO_MT_SENSOR
			if((++dly_phsw3)>10){
				run_stepa = MTA_MASK & (run_stepa|0xFFF8);
//				run_stepb = 0;
				dly_phsw3 = 0;
			}
#endif
		}else{
			if(dly_phsw3) dly_phsw3--;
		}
	}
//	SENS_OFF();
}

/*****************************************************************************/
void stepb_loop(void){				//Shifter
	SENS_ON();
	if(run_stepb){
		switch(run_stepb&7){
			case 0:	step1hi(); step2lo(); step3lo(); step4lo(); break;
			case 1:	step1hi(); step2hi(); step3lo(); step4lo(); break;
			case 2:	step1lo(); step2hi(); step3lo(); step4lo(); break;
			case 3:	step1lo(); step2hi(); step3hi(); step4lo(); break;
			case 4:	step1lo(); step2lo(); step3hi(); step4lo(); break;
			case 5:	step1lo(); step2lo(); step3hi(); step4hi(); break;
			case 6:	step1lo(); step2lo(); step3lo(); step4hi(); break;
			case 7:	step1hi(); step2lo(); step3lo(); step4hi(); break;
		}
		run_stepb--;
	}else{ step1lo(); step2lo(); step3lo(); step4lo(); }
	if(run_stepb<(TICK_STEPB-200)){
		if(PH_MTB){
#ifndef NO_MT_SENSOR
			if((++dly_phsw2)>10){
				run_stepb=0;
				step1lo(); step2lo(); step3lo(); step4lo();
			}
#endif
		}else{
			if(dly_phsw2) dly_phsw2--;
		}
	}
//	SENS_OFF();
}
/*****************************************************************************/
void key_buzzer(void){
	if (SOUND_FLAG) {
		PORT_LEDhi(ENVE);
		buzz_delay = 42;	// 50ms/1.2ms
	}
}
/*****************************************************************************/
void key_buzzerF(void){
	PORT_LEDhi(ENVE);
	buzz_delay = 42;	// 50ms/1.2ms
}
/*****************************************************************************/
void kdelay( int dl){
volatile int a, b=1;	
	for(a=0;a<dl;a++) b *= a;
}
/*****************************************************************************/
/*****************************************************************************/
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
/*****************************************************************************/
