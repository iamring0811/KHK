
#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_usart.h"
#include "Common\stm324x9i_eval.h"

#define INT_RXBUF5

/*************************
64K		  10000
128K	  20000
256K	  40000
512K	  80000
1M		 100000
2M		 200000
4M		 400000
8M		 800000
16M		1000000
32M		2000000
64M		4000000
128M	8000000
page size		0x0800
page number		0x8000
block size		0x20000 = page * 64 = 128k
block number	0x0400
*************************/
#define MAX_FLASH		0x20000
#define MAX_BLOCK		MAX_FLASH*2

/*****************  SDRAM AREA  *********************************/
// DRAM MAX Address	A18 = 16MB	0x01000000 (0x00FFFFFF)
// SDRAM   0xD0000000 ~ 0xD0FFFFFF	16Mbyte = 128Mbit

#define LcdBaseAdr0		LCD_FRAME_BUFFER						// 0xD0000000 ~ 0xD01193ff
#define LcdBaseAdr1		LCD_FRAME_BUFFER + BUFFER_OFFSET		// 0xD0119400 ~ 0xD02327ff
																// NoUse 0xD0232800 ~ 0xD03FFFFF
//#define USB_BUFF		0xD0400000								// End:0xD047FFFF
//#define BMP_BUFF		0xD0490000								// End:0xD04FFFFF 
//#define RSbuffBase	0xD0520000								// End:0xD057FFFF
//#define downdata		0xD0580000								// End:0xD05BFFFF	[0x40000];
//#define EXT_BUFF		0xD05C0000								// End:0xD05CFFFF	Internal Flash
//#define INT_BUFF		0xD05D0000								// End:0xD05DFFFF	External Serial
//#define TEST_INDEX	0xD0600000								// End:0xD064E1FF	result index size=64*5000=0x4E200
//#define QC_INDEX		0xD0650000								// End:0xD06518FF	result index size=64* 100=0x01900
//#define LOS_BUFF		0xD0660000								// End:0xD066F9FF	32*2000 = 0xFA00
//
//#define FONT_BASE		0xD0700000								// End:0xD07FFFFFF
//#define BMP_BASE		0xD0800000								// End:0xD0FFFFFFF

#define USB_BUFF		0xD0240000								// End:0xD029FFFF
#define BMP_BUFF		0xD02A0000								// End:0xD03FFFFF 
#define RSbuffBase		0xD0300000								// End:0xD057FFFF
#define downdata		0xD0360000								// End:0xD05BFFFF	[0x40000];
#define EXT_BUFF		0xD03A0000								// End:0xD05CFFFF	Internal Flash
#define INT_BUFF		0xD03B0000								// End:0xD05DFFFF	External Serial
#define TEST_INDEX		0xD03C0000								// End:0xD064E1FF	result index size=64*5000=0x4E200
#define QC_INDEX		0xD040E600								// End:0xD06518FF	result index size=64* 100=0x01900
#define LOS_BUFF		0xD0410000								// End:0xD066F9FF	32*2000 = 0xFA00

#define FONT_BASE		0xD0420000								// End:0xD07FFFFFF
/****** MAX BMP SIZE =  0x00B00000 ******/
#define BMP_BASE		0xD0500000								// End:0xD0FFFFFFF

/*****************   NAND AREA  *********************************/
// NAND MAX Address	A22 = 128MB	0x0800-0000 ( 0x07FFFFFF )
#define BMP_START_BLOCK			0x000			//0x00000000 ~ 0x03FFFFFF	 32[MB]
#define BMP_SECOND_BLOCK		0x100			//0x02000000 ~ 0x03FFFFFF	 32[MB]									//2020-07-19 12:44����
#define FONT_START_BLOCK		0x200			//0x04000000 ~ 0x047FFFFF	  8[MB]
#define SAVE_START_BLOCK		0x240			//0x04800000 ~ 0x04FFFFFF	  8[MB]  System Para Save
#define DATA_START_BLOCK		0x280			//0x05000000 ~ 0x07FFFFFF	 32[MB]  �������

/****************************************************************/

#define OK	1
#define NG	0

#define USE_COM		5//����� ��� ��Ʈ

#if( USE_COM == 3 )									//usb
	#define wait_tx0	wait_tx3
	#define printf0 	printf3
	#define getcmd0 	getcmd3
	#define rxw0		rxw3
	#define rxr0		rxr3
	#define savcmd0		savcmd3
#elif( USE_COM == 4 )								//rs232
	#define wait_tx0	wait_tx4
	#define printf0 	printf4
	#define getcmd0 	getcmd4
	#define rxw0		rxw4
	#define rxr0		rxr4
	#define savcmd0		savcmd4
#else	//USE_COM == 5								//debug
	#define wait_tx0	wait_tx5
	#define printf0 	printf5
	#define getcmd0 	getcmd5
	#define rxw0		rxw5
	#define rxr0		rxr5
	#define savcmd0		savcmd5
#endif

#define print_thermal	printf2
#define savcmd_print	savcmd2
//#define print_thermal	printf3
//#define savcmd_print	savcmd3

#define TIM_MSEC_DELAY                     	0x01
#define TIM_USEC_DELAY                     	0x02

#define RX1BUF_SIZE		0x100						// measure			51200
#define RX2BUF_SIZE		RX1BUF_SIZE					// print/st-chk		 9600
#define RX3BUF_SIZE		RX1BUF_SIZE					// USB				38400
#define RX4BUF_SIZE		RX1BUF_SIZE					// RS232		   115200
#define RX5BUF_SIZE		RX1BUF_SIZE					// debug		   115200
#define RX6BUF_SIZE		RX1BUF_SIZE					// earthnet			38400
#define RX7BUF_SIZE		RX1BUF_SIZE					// bluetooth		38400

#define tx2buf			RSbuffBase + 0x00000
#define tx3buf			RSbuffBase + 0x10000
#define tx4buf			RSbuffBase + 0x20000
#define tx5buf			RSbuffBase + 0x30000
#ifndef INT_RXBUF5
	#define rx5buf		RSbuffBase + 0x40000
#endif

void wait_tx2(void);
void wait_tx3(void);
void wait_tx4(void);
void wait_tx5(void);

void printf2(char *fmt,...);
void printf3(char *fmt,...);
void printf4(char *fmt,...);
void printf5(char *fmt,...);
void prt_msg(char *fmt,...);

void savcmd0(u8 ch);
void savcmd1(u8 ch);
void savcmd2(u8 ch);
void savcmd3(u8 ch);
void savcmd4(u8 ch);
void savcmd5(u8 ch);
void savcmd6(u8 ch);
void savcmd7(u8 ch);
u8 getcmd1(void);
u8 getcmd2(void);
u8 getcmd3(void);
u8 getcmd4(void);
u8 getcmd5(void);
u8 getcmd6(void);
u8 getcmd7(void);
void com_clear(void);

void USART_Config(void);
void set_baud_rs232(void);
void set_baud_usb(void);
void TIM_Config(void);
void TIM_SetUsec(uint32_t usec);

void key_buzzer(void);
void key_buzzerF(void);
void kdelay( int dl);

#ifdef _UART_C_
u8  evenodd, evenod2, evenod3, dscan=0;
u16	txw2,txr2;
u16 txw3,txr3;
u16 txw4,txr4;
u16 txw5,txr5;
u16	dly_phsw2, dly_phsw3;

u8  rx1buf[RX1BUF_SIZE], tx1buf[RX1BUF_SIZE], rxw1,rxr1,txw1,txr1;
u8  rx2buf[RX2BUF_SIZE], rxw2,rxr2;
u8  rx3buf[RX3BUF_SIZE], rxw3,rxr3;
u8  rx4buf[RX4BUF_SIZE], rxw4,rxr4;
#ifdef INT_RXBUF5
	u8  rx5buf[RX5BUF_SIZE], rxw5,rxr5;
#else
	u16	rxw5,rxr5;
#endif
u8  rx6buf[RX6BUF_SIZE], tx6buf[RX6BUF_SIZE], rxw6,rxr6,txw6,txr6;
u8  rx7buf[RX7BUF_SIZE], tx7buf[RX7BUF_SIZE], rxw7,rxr7,txw7,txr7;
char string[256];
char charbuff[256];
char rcv_buff[0x20];
u16	usec_delay;
u16	MS10, MS500;
u16	rcv_wait, rcv_step;
u32	rcv_size, rcv_sum, rcvwp, get_sum;
u32	delay_150;
u16 meas_sum;

u8	buzz_delay;
u8	baud_delay, baud_delay2;//baud_delay:rs232, baud_delay2:usb

//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
u32	usbpnt;

#else
extern u16	txw2,txr2;
extern u16 txw3,txr3;
extern u16 txw4,txr4;
extern u16 txw5,txr5;
extern u16	dly_phsw2, dly_phsw3;

extern u8  rx1buf[RX1BUF_SIZE], tx1buf[RX1BUF_SIZE], rxw1,rxr1,txw1,txr1;
extern u8  rx2buf[RX2BUF_SIZE], rxw2,rxr2;
extern u8  rx3buf[RX3BUF_SIZE], rxw3,rxr3;
extern u8  rx4buf[RX4BUF_SIZE], rxw4,rxr4;
#ifdef INT_RXBUF5
extern 	u8  rx5buf[RX5BUF_SIZE], rxw5,rxr5;
#else
extern 	u16	rxw5,rxr5;
#endif
extern u8  rx6buf[RX6BUF_SIZE], tx6buf[RX6BUF_SIZE], rxw6,rxr6,txw6,txr6;
extern u8  rx7buf[RX7BUF_SIZE], tx7buf[RX7BUF_SIZE], rxw7,rxr7,txw7,txr7;
extern char string[256];
extern char charbuff[256];
extern char rcv_buff[0x20];
extern u16	usec_delay;
extern u16	MS10, MS500;
extern u16	rcv_wait, rcv_step;
extern u32	rcv_size, rcv_sum, rcvwp, get_sum;
extern u32	delay_150;
extern u16 meas_sum;

//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
extern u32	usbpnt;

#endif /* __UART_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
