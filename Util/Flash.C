#define _FLASH_C_

#include <stdio.h>
#include "stm32f4xx.h"
#include "Common\stm324x9i_eval.h"
#include "common\32f429_port.h"
#include "common\32f429_lcd.h"
#include "common\fsmc_nand.h"
#include "util\usart.h"			// include port.h
#include "util\lcd.h"
#include "util\eep.h"
#include "util\Flash.h"
#include "com.h"
#include "resm.h"
#include "dsp.h"

/*****************************************************************************/
void prog_decode(void){
	if(flag_flash!=3){
		if(rxw3!=rxr3) system_decode( getcmd3() ); 
		if(rxw4!=rxr4) system_decode( getcmd4() ); 
		while(rxw0!=rxr0) flash_decode( getcmd0() );
	}else{
		while(rxw3!=rxr3) flash_decode( getcmd3() );
		while(rxw4!=rxr4) flash_decode( getcmd4() ); 

	}
}
//#define MDTROMW
//		00   : '$'
//		01-09: <�� ���ϸ�>    (ex :  MDT.001 )
//		10   : Data����(0:code,1:font,2:boot,3:BMP)
//		11   : start address (�� ������ ������ ���� ����)
//		12-14: Reserved
//		15-18: BIN data size LSB first
//		19   : RESERVED
//		20-23: checksum ( data�� byte������ ���� ��, LSB16bit�����, LSB first )
//		24-31: Date ( 07-04-20 )
//		32-END: Program data(BINary)
/*****************************************************************************/
#define RCV_DELAY	300			//3.0[sec]
void flash_decode(u8 dat){
	if(rcv_step==0){
		if(dat=='$'){
			rcv_buff[rcv_step++] = dat;
			rcv_wait = RCV_DELAY;
			printf0("\r Get$"); 
		}
	}else if(rcv_step<0x20){
//		printf0("%2x,", dat); 
		rcv_buff[rcv_step++] = dat;
		if(rcv_step>=0x20){
			if((rcv_buff[1]=='D')&&(rcv_buff[2]=='F')&&(rcv_buff[3]=='6')&&(rcv_buff[6]=='.')){
				rcv_size = rcv_buff[14];
				rcv_size = (rcv_size*0x100) + rcv_buff[13];
				rcv_size = (rcv_size*0x100) + rcv_buff[12];
				printf0(" [Size:%d]", rcv_size); 
				if(rcv_size>MAX_BLOCK){
					printf0(" **** Size Over : %lx ****", rcv_size); 
					rcv_size = MAX_BLOCK;
				}
				rcv_sum = rcv_buff[19];
				rcv_sum = (rcv_sum*0x100) + rcv_buff[18];
				rcv_sum = (rcv_sum*0x100) + rcv_buff[17];
				rcv_sum = (rcv_sum*0x100) + rcv_buff[16];
			}else{
				rcv_step = 0;	
				flag_flash = 0;
			}
			rcvwp = 0;
		}
	}else{
		*(u8*)(BMP_BUFF+rcvwp) = dat;	
		rcvwp++;
		if(!(rcvwp&0xFFF)){
			printf0(".");
			rcv_wait = RCV_DELAY;
		}
		if(rcvwp>=rcv_size){
			get_sum=0;
			for(rcvwp=0;rcvwp<rcv_size;rcvwp++)	get_sum += *(u8*)(BMP_BUFF+rcvwp);		
			if(rcv_sum!=get_sum){
				printf0("\r RS:%x-GS:%x  <Error>", rcv_sum, get_sum);
				get_sum=0;
				for(rcvwp=0;rcvwp<rcv_size;rcvwp++)	get_sum += *(u8*)(BMP_BUFF+rcvwp);		
			}
			printf0("\r RS:%x-GS:%x", rcv_sum, get_sum);
			rcv_wait = rcv_step = 0;
			if(rcv_sum==get_sum){
				if(rcv_buff[10]==2){
					store_bmp(1, 0);
				}else if(rcv_buff[10]==3){
					flag_second = 1;
					store_bmp(1, 1);						//second BMP
				}else if(rcv_buff[10]==4){
					printf0(" -%d-", rcv_buff[11]);
//					rcv_buff[11] -= 2;
					store_font(1);
				}else printf0("  *** Unknown File ***");
			}else printf0("  *** Error SUM ***");
			flag_flash = 0;
		}
	}
}
/**************************************************************/
void store_bmp_usb(void){
u16 ver;	
	if((rcv_buff[1]=='D')&&(rcv_buff[2]=='F')&&(rcv_buff[3]=='6')&&(rcv_buff[6]=='.')){
		rcv_size = rcv_buff[14];
		rcv_size = (rcv_size*0x100) + rcv_buff[13];
		rcv_size = (rcv_size*0x100) + rcv_buff[12];
		printf0(" [Size:%d]", rcv_size); 
		if(rcv_size>MAX_BLOCK){
			printf0(" **** Size Over : %lx ****", rcv_size); 
			rcv_size = MAX_BLOCK;
		}
		rcv_sum = rcv_buff[19];
		rcv_sum = (rcv_sum*0x100) + rcv_buff[18];
		rcv_sum = (rcv_sum*0x100) + rcv_buff[17];
		rcv_sum = (rcv_sum*0x100) + rcv_buff[16];

		ver = rcv_buff[7] - '0';
		ver = (ver*10) + (rcv_buff[8]-'0');
		ver = (ver*10) + (rcv_buff[9]-'0');
		
		get_sum=0;
		for(rcvwp=0;rcvwp<rcv_size;rcvwp++)	get_sum += *(u8*)(BMP_BUFF+rcvwp);		
		printf0("\r RS:%8x-GS:%8x", rcv_sum, get_sum);
		if(rcv_sum==get_sum){
			if(rcv_buff[10]==2){
				store_bmp(1, 0);
			}else if(rcv_buff[10]==3){
				flag_second = 1;
				store_bmp(1, 1);					//second BMP
			}else if(rcv_buff[10]==4){
				printf0(" -%d-", rcv_buff[11]);
//				rcv_buff[11] -= 2;
				store_font(1);
			}else printf0("  *** Unknown File ***");
		}else{
			printf0("  *** Error SUM ***");
		}
	}
	rcv_wait = rcv_step = 0;
	flag_flash = 0;
}
/**************************************************************/
void move_font16(u32 fadr, u16 jp);
void move_font24(u32 fadr, u16 jp);
void move_font32(u32 fadr, u16 jp);
void move_font40(u32 fadr, u16 jp);
void move_font48(u32 fadr, u16 jp);
void move_font56(u32 fadr, u16 jp);
void move_font64(u32 fadr, u16 jp);

/**************************************************************/
void FMC_NAND_Check(void){
uint16_t j, k;
	NAND_IDTypeDef NAND_ID;
	NAND_ADDRESS WriteReadAddr;

//	printf0("\r Check Format");
  	FMC_NANDDeInit(FMC_Bank3_NAND);
  	FMC_NAND_Init();

  	FMC_NAND_ReadID(&NAND_ID);
  	printf0("\r NandFlashID=%02X,%02X,%02X,%02X",NAND_ID.Maker_ID, NAND_ID.Device_ID,
		    	                                         NAND_ID.Third_ID, NAND_ID.Fourth_ID );
  	if ((NAND_ID.Maker_ID == 0x01) && (NAND_ID.Device_ID == 0xF1)
		&& (NAND_ID.Third_ID == 0x00) && (NAND_ID.Fourth_ID == 0x1D))
  	{
	 	printf0(" Type=S34ML01G100T");	//	 	printf0(" Type=S34ML01G100TFI00");
  	}else{
	 	printf0(" Type=Unknow");
  	}

  	/* NAND memory address to read to */
  	WriteReadAddr.Zone  = 0x00;
  	WriteReadAddr.Block = 0x00;
  	WriteReadAddr.Page  = 0x00;
	FMC_NAND_ReadSpareArea(nandRxBuffer, WriteReadAddr, 1);
	nandRxBuffer[33]=0x00;
//	FMV01-yy-mm-dd,hh:mm:ss
	if((nandRxBuffer[0]=='F')&&(nandRxBuffer[1]=='M')&&(nandRxBuffer[2]=='V')&&(nandRxBuffer[5]=='-'))
	{
	 	printf0(" Ver.%s", nandRxBuffer);							// version of Format type
	  	FMC_NAND_ReadSmallPage (nandRxBuffer, WriteReadAddr, 1);
		for(j=0;j<0x400;j++){
			k = nandRxBuffer[(j*2)+1];
			k = (k<<8) + nandRxBuffer[(j*2)+0];
			NAND_TB[j] = k;
//		 	printf0(" %3x,",k);
		}
	}else{
		if(run_format()>=40){
			if(run_format()>=40) printf0("\r Too much bad block!!!");
		}
	}
}
/**************************************************************/
/**************************************************************/
void NAND_Test(void){
u16 i;
	NAND_ADDRESS WriteReadAddr;

    printf0("\r Test NAND*");
  	WriteReadAddr.Zone  = 0x00;
  	WriteReadAddr.Block = NAND_TB[0x00];
  	WriteReadAddr.Page  = 0x00;

  	FMC_NAND_ReadSmallPage (nandRxBuffer, WriteReadAddr, 1);
    for(i = 0; i < 0x800; i++){
	    if(!(i&0x0f)) printf0("\r");
   		printf0("%2x,",  nandRxBuffer[i]);
    }
}
/********************************************************************************************************
1.NAND �÷����� ����� 20 �� ������ BAD BLOCK �� ���Ե� ��ǰ�� ���� �Ǹ��Ѵ�.
2.���� �˻�� BAD ���� ǥ�ô� BLOCK �� ù��° ������ �� �ι�° �������� 517 ��° ����Ʈ�� �о���
 �� 0xFF�� ������ ������ ��� �����̴�.
3.��� ������ ����� ������ ����� ���ų� ������ ���ƾ� �Ѵ�.
4.0 �� ������ ����� ��� ������ �ƴϴ�.
-------------------------------------------------------------------
1. Check FFh at 1st byte in the spare area of the 1st and 2nd page.
**********************************************************************************************************/
// S34NL01G100TFI00
// NAND FLASH MAX Address A22 = 128MB	0x8000000 ( 0x7FFFFF )
// Block Size = 2048*64   = 128KB (0x0020000)
// Zone  size = 128k*1024 = 128MB (0x8000000)
//  010000	64K
//  02
//  04
//  08
//  10 		1M
//  20
//  40
//  80
// 100 		16M
// 200 		32M
// 400 		64M
// 800 	   128M Byte = 1Gbit 
/**************************************************************/
/**************************************************************/
//	01234567890123456789012
//	FMV01-yy-mm-dd,hh:mm:ss
#define NAND_FORMAT_VERSION		10		//1.0
u8 run_format(void){
u16 i, j, cntBad=0;
u16 pntBlock=0;
	NAND_ADDRESS WriteReadAddr;
	wait_tx0();
	printf0("\r Start Format");
  	WriteReadAddr.Zone  = 0x00;
	for(i=1;i<NAND_ZONE_SIZE;i++){
  		WriteReadAddr.Block = i;
	  	WriteReadAddr.Page  = 0x00;
		FMC_NAND_ReadSpareArea(nandRxBuffer, WriteReadAddr, 1);
	  	if(nandRxBuffer[0]==0xFF){
		  	WriteReadAddr.Page  = 0x01;
			FMC_NAND_ReadSpareArea(nandRxBuffer, WriteReadAddr, 1);
		  	if(nandRxBuffer[0]==0xFF){
		  		NAND_TB[pntBlock++] = i;
		  		if(!(i&0x3F)) printf0(" %4x,", i);
		  	}else{
				wait_tx0();
		  		printf0("\r Bad Block at 2nd page:%4x-%4x", pntBlock, i);
		  		cntBad++;
		  	}
	  	}else{
			wait_tx0();
	  		printf0("\r Bad Block at 1st page:%4x-%4x", pntBlock, i);
	  		cntBad++;
	  	}
	}
	while(pntBlock<NAND_ZONE_SIZE) NAND_TB[pntBlock++] = 0x55AA;
	wait_tx0();
	printf0("\r Finish Format NAND Flash, Bad Block = %d", cntBad);

	for(j=0;j<0x400;j++){
		i = NAND_TB[j];
		nandTxBuffer[(j*2)+0] = i & 0xff;
		nandTxBuffer[(j*2)+1] = i >> 8;
	}
  	WriteReadAddr.Zone  = 0x00;
  	WriteReadAddr.Block = 0x00;
  	WriteReadAddr.Page  = 0x00;

   	printf0("\r Erase Block ");
  	FMC_NAND_EraseBlock(WriteReadAddr);

   	printf0("\r Write NAND Table Page ");
  	FMC_NAND_WriteSmallPage(nandTxBuffer, WriteReadAddr, 1);

  	FMC_NAND_ReadSmallPage (nandRxBuffer, WriteReadAddr, 1);
    printf0("\r Compare Table�� ");
    for(i = 0; i < 0x800; i++){
   		if(nandRxBuffer[i]!=nandTxBuffer[i]) printf0(" %x(%2x-%2x),", i, nandRxBuffer[i], nandTxBuffer[i]);
    }
    printf0(" *Finish Compare*");

	readAllRTC();
   	sprintf(charbuff, "FMV%2d-%2d-%2d-%2d,%2d:%2d:%2d", NAND_FORMAT_VERSION, year, month, day, hour, min, sec);
   	for(i=0;i<100;i++) nandTxBuffer[i] = charbuff[i];
   	printf0("\r Write NAND Table Spare");
    FMC_NAND_WriteSpareArea(nandTxBuffer, WriteReadAddr, 1);

//	FMC_NAND_ReadSpareArea(nandRxBuffer, WriteReadAddr, 1);
//	printf0("\r");
//	for(i=0;i<33;i++) printf0("%c", nandRxBuffer[i]);

	if(cntBad>0xFF) return(0xff);
	else return(cntBad);
}
/**************************************************************/
/**************************************************************/
/**************************************************************
	0xA00000 / 0x020000 = 0x50 ==> �ִ� 80Block 
 **************************************************************/
void write_bmp(u32 bp);
/**************************************************************/
void store_bmp(u8 bar, u8 sel){									//2020-07-19 12:44����
u32 i, bp;
	if(!sel) printf0("  *** Store BMP ***");		
	else printf0("  *** Store BMP Second ***");		wait_tx0();
	bp = rcv_buff[11];
	bp *= (MAX_FLASH*2);
	if(bar){
		for(i=0;i<(MAX_FLASH*2);i+=2){
			*(u8*)(BMP_BASE+bp+i+0) = *(u8*)(BMP_BUFF+i+1);
			*(u8*)(BMP_BASE+bp+i+1) = *(u8*)(BMP_BUFF+i+0);
		}
	}
	if(sel)	bp += (BMP_SECOND_BLOCK*MAX_FLASH);
	write_bmp(bp);

//	printf0(" (Size:%d)", rcv_size); 
	if(rcv_size>0){
		for(i=0;i<MAX_FLASH;i++) *(u8*)(BMP_BUFF+i) = *(u8*)(BMP_BUFF+i+MAX_FLASH);
		bp += MAX_FLASH;
//	printf0("\r *** bp : %7x", bp);	//	wait_tx0();
		write_bmp(bp);
	}
}
/**************************************************************/
/**************************************************************/
void write_bmp(u32 bp){									//2020-07-19 12:44����
u32	bm_pnt, bm_sav, rcv_sav;
u32	i, j, k;
	NAND_ADDRESS WriteReadAddr;
	bm_sav = bm_pnt = bp;

	for(i=0;i<5;i++){
	  	WriteReadAddr.Zone  = 0x00;
  		WriteReadAddr.Block = NAND_TB[bm_pnt/MAX_FLASH];
	  	WriteReadAddr.Page  = 0x00;
   		printf0("\r Erase Block : %d-0x%x,(%d)", i+1, bm_pnt, WriteReadAddr.Block);	//	wait_tx0();
	  	j = FMC_NAND_EraseBlock(WriteReadAddr);
   		printf0(" Ans:%x", j);
  		if(j!=NAND_TIMEOUT_ERROR){
		  	FMC_NAND_ReadSmallPage (nandRxBuffer, WriteReadAddr, 1);
			for(j=0;j<0x800;j++) if(nandRxBuffer[j]!=0xff) break;
			if(j==0x800) break;
  			else printf0(" <Erase Error>");
  		}else printf0(" *** NTO Error ***");
  	}
  	if(i>=5) printf0("\r ************************** NAND Erase Error ***********************");

   	printf0("\t Start A:%7x", bm_pnt );
   	rcv_sav = rcv_size;
   	for(k=0;k<5;k++){
		rcv_size = rcv_sav;
		bm_pnt = bm_sav;
	   	for(j=0;j<MAX_FLASH;j+=0x800){
			for(i=0;i<0x800;i++) nandTxBuffer[i] = *(u8*)(BMP_BUFF+j+i);
	  		WriteReadAddr.Zone  = 0x00;
		  	WriteReadAddr.Block = NAND_TB[bm_pnt/MAX_FLASH];
	  		WriteReadAddr.Page  = (bm_pnt%MAX_FLASH) / 0x800;
	  		FMC_NAND_WriteSmallPage(nandTxBuffer, WriteReadAddr, 1);

			bm_pnt += 0x800;
			if(rcv_size<0x800){
				rcv_size = 0x00;
				break;
			}else rcv_size -= 0x800;
		}
		/////////////////////////////////////////////////////////////
		rcv_size = rcv_sav;
		bm_pnt = bm_sav;
	   	for(j=0;j<MAX_FLASH;j+=0x800){
		  	WriteReadAddr.Zone  = 0x00;
  			WriteReadAddr.Block = NAND_TB[bm_pnt/MAX_FLASH];
	  		WriteReadAddr.Page  = (bm_pnt%MAX_FLASH) / 0x800;
		  	FMC_NAND_ReadSmallPage (nandRxBuffer, WriteReadAddr, 1);
			for(i=0;i<0x800;i++){ 
				if(nandRxBuffer[i]!=*(u8*)(BMP_BUFF+j+i)){
				   	printf0("\r Compare Error at : %x", bm_pnt+i );
				   	break;
				} 
			}
			if(i>=0x800){
				bm_pnt += 0x800;
				if(rcv_size<0x800){
					rcv_size = 0x00;
					break;
				}else rcv_size -= 0x800;
			}else break;
		}		
		if(j>=MAX_FLASH){
		   	printf0(" [CMP.OK] ", bm_pnt );
			break;
		}
	}
   	printf0(" - End A:%7x", bm_pnt );
}
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
void store_font(u8 bar){
u32 i, j, bp, sz;
	printf0("  *** Store FONT ***");		wait_tx0();
	j  = (rcv_buff[11] & 0x0F);
	j  = (j*MAX_FLASH*2);
	bp = (FONT_START_BLOCK*MAX_FLASH) + j;
	if(bar){
		for(i=0;i<rcv_size;i+=2){
			*(u8*)(FONT_BASE+j+i+0) = *(u8*)(BMP_BUFF+i+1);
			*(u8*)(FONT_BASE+j+i+1) = *(u8*)(BMP_BUFF+i+0);
		}
	}

	if(rcv_size<=MAX_FLASH){
//printf0("\r rcv_size:%lx", rcv_size);
		write_bmp(bp);
	}else{
		sz = rcv_size - MAX_FLASH;
		rcv_size = MAX_FLASH;
		write_bmp(bp);
		bp += MAX_FLASH;
		for(i=0;i<MAX_FLASH;i++) *(u8*)(BMP_BUFF+i) = *(u8*)(BMP_BUFF+i+MAX_FLASH);
		rcv_size = sz;
//printf0("\r rcv_size:%lx", rcv_size);
		write_bmp(bp);
	}
}

/**************************************************************/
void move_BMP(void){									//2020-07-19 12:44����
u32 size, bm_pnt;
u16 i;
	NAND_ADDRESS WriteReadAddr;

	printf0("\r Read BMP Head " );

  	WriteReadAddr.Zone  = 0x00;
  	WriteReadAddr.Block = NAND_TB[BMP_START_BLOCK];
  	WriteReadAddr.Page  = 0x01;									//was 0x00
  	FMC_NAND_ReadSmallPage (nandRxBuffer, WriteReadAddr, 1);

	if((nandRxBuffer[0x7f0]=='D')&&(nandRxBuffer[0x7F1]=='F')&&(nandRxBuffer[0x7F2]=='4')){
		size = nandRxBuffer[0x7FE];		
		size = (size<<8) + nandRxBuffer[0x7FD];		
		size = (size<<8) + nandRxBuffer[0x7FC];		
   		printf0("\r Move BMP ( size : %x ) ",  size );
//	size /= 2;
		bm_pnt = 0;
		while(bm_pnt<size){		
		  	WriteReadAddr.Zone  = 0x00;
  			WriteReadAddr.Block = NAND_TB[BMP_START_BLOCK+(bm_pnt/MAX_FLASH)];
	  		WriteReadAddr.Page  = (bm_pnt%MAX_FLASH) / 0x800;
		  	FMC_NAND_ReadSmallPage (nandRxBuffer, WriteReadAddr, 1);
//printf0(" <%7x", BMP_SECOND_BLOCK+(bm_pnt/MAX_FLASH) ); wait_tx0();
//printf0("-%4x>", (bm_pnt%MAX_FLASH) ); wait_tx0();
			for(i=0;i<0x800;i++) *(u8*)(BMP_BASE+bm_pnt+i) = nandRxBuffer[i];
			bm_pnt += 0x800;
	   		if(!(bm_pnt&0x1FFFF)) printf0(">");
	  	}
    }else{
		for(i=0;i<BMP_HEAD_SIZE;i++) *(u8*)(BMP_BASE+i) = 0x00;
   		printf0("\r No BMP File" );
	}
	for(i=0;i<BMP_HEAD_SIZE;i++) BMP_TB[i] = *(u8*)(BMP_BASE+i);	
	BMP_NUMBER = 0;
	for(i=0;i<BMP_HEAD_SIZE;i+=8) if(BMP_TB[i]=='K') BMP_NUMBER++;
	if((BMP_NUMBER>0)&&(BMP_TB[0xfF0]=='D')&&(BMP_TB[0xfF1]=='F')&&(BMP_TB[0xfF2]=='4')){
		BMP_VERSION = BMP_TB[0xfF8]-'0';
		BMP_VERSION = (BMP_VERSION*10) + (BMP_TB[0xfF9]-'0');
		BMP_VERSION = (BMP_VERSION*10) + (BMP_TB[0xfFA]-'0');
	}else BMP_VERSION = 0;
	printf0("\r BMP_NUMBER : %3d, Ver.%3d", BMP_NUMBER, BMP_VERSION);
}
/**************************************************************/
/**************************************************************/
void move_BMP_second(void){									//2020-07-19 12:44����
u32 size, bm_pnt;
u16 i;
	NAND_ADDRESS WriteReadAddr;

	printf0("\r Read BMP Head Second" );

  	WriteReadAddr.Zone  = 0x00;
  	WriteReadAddr.Block = NAND_TB[BMP_SECOND_BLOCK];
  	WriteReadAddr.Page  = 0x01;									//was 0x00
  	FMC_NAND_ReadSmallPage (nandRxBuffer, WriteReadAddr, 1);

	if((nandRxBuffer[0x7f0]=='D')&&(nandRxBuffer[0x7F1]=='F')&&(nandRxBuffer[0x7F2]=='4')){
		size = nandRxBuffer[0x7FE];		
		size = (size<<8) + nandRxBuffer[0x7FD];		
		size = (size<<8) + nandRxBuffer[0x7FC];		
   		printf0("\r Move BMP ( size : %x ) ",  size );
//	size /= 2;
		bm_pnt = 0;
		while(bm_pnt<size){		
		  	WriteReadAddr.Zone  = 0x00;
  			WriteReadAddr.Block = NAND_TB[BMP_SECOND_BLOCK+(bm_pnt/MAX_FLASH)];
	  		WriteReadAddr.Page  = (bm_pnt%MAX_FLASH) / 0x800;
		  	FMC_NAND_ReadSmallPage (nandRxBuffer, WriteReadAddr, 1);
			for(i=0;i<0x800;i++) *(u8*)(BMP_BASE+bm_pnt+i) = nandRxBuffer[i];
			bm_pnt += 0x800;
	   		if(!(bm_pnt&0x1FFFF)) printf0(">");
	  	}
    }else{
		for(i=0;i<BMP_HEAD_SIZE;i++) *(u8*)(BMP_BASE+i) = 0x00;
   		printf0("\r No BMP File" );
	}
	for(i=0;i<BMP_HEAD_SIZE;i++) BMP_TB[i] = *(u8*)(BMP_BASE+i);	
	BMP_NUMBER = 0;
	for(i=0;i<BMP_HEAD_SIZE;i+=8) if(BMP_TB[i]=='K') BMP_NUMBER++;
	if((BMP_NUMBER>0)&&(BMP_TB[0xfF0]=='D')&&(BMP_TB[0xfF1]=='F')&&(BMP_TB[0xfF2]=='4')){
		BMP_VERSION = BMP_TB[0xfF8]-'0';
		BMP_VERSION = (BMP_VERSION*10) + (BMP_TB[0xfF9]-'0');
		BMP_VERSION = (BMP_VERSION*10) + (BMP_TB[0xfFA]-'0');
	}else BMP_VERSION = 0;
	printf0("\r BMP_NUMBER : %3d, Ver.%3d", BMP_NUMBER, BMP_VERSION);
}
/**************************************************************/
/**************************************************************/
void move_FONT(void){
u32 size, s_pnt;
u32 i, j;
	NAND_ADDRESS WriteReadAddr;

	printf0("\r Read FONT Head : %3x ", NAND_TB[FONT_START_BLOCK] );
	for(i=0;i<2;i++){
	  	WriteReadAddr.Zone  = 0x00;
  		WriteReadAddr.Block = NAND_TB[FONT_START_BLOCK];
//		WriteReadAddr.Block = 0;
	  	WriteReadAddr.Page  = i;
  		FMC_NAND_ReadSmallPage (nandRxBuffer, WriteReadAddr, 1);
  		for(j=0;j<0x800;j++) FONT_HEAD[(i*0x800)+j] = nandRxBuffer[j];
  	}
  	size = 0;
  	FONT_VERSION = FONT_NUMBER = 0;
//  		for(j=0;j<0x1000;j++) printf0("%2x,",FONT_HEAD[j]);

	for(j=0;j<FONT_HEAD_SIZE;j+=0x100){
		if((FONT_HEAD[j]=='F')&&(FONT_HEAD[j+8]=='@')){
			i = FONT_HEAD[j+9];
			i = (i*0x100) + FONT_HEAD[j+10];
			i *= 0x100;
			     if(FONT_HEAD[j+1]==16){ move_font16(i, j); FONT_NUMBER++; }
			else if(FONT_HEAD[j+1]==24){ move_font24(i, j); FONT_NUMBER++; }
			else if(FONT_HEAD[j+1]==32){ move_font32(i, j); FONT_NUMBER++; }
			else if(FONT_HEAD[j+1]==40){ move_font40(i, j); FONT_NUMBER++; }
			else if(FONT_HEAD[j+1]==48){ move_font48(i, j); FONT_NUMBER++; }
			else if(FONT_HEAD[j+1]==56){ move_font56(i, j); FONT_NUMBER++; }
			else if(FONT_HEAD[j+1]==64){ move_font64(i, j);	FONT_NUMBER++; }
		}else if((FONT_HEAD[j]=='T')&&(FONT_HEAD[j+8]=='@')){
			size = FONT_HEAD[j+9];
			size = (size*0x100) + FONT_HEAD[j+10];
			size *= 0x100;
			FONT_VERSION = FONT_HEAD[j+1] - '0';
			FONT_VERSION = (FONT_VERSION*10) + (FONT_HEAD[j+2] - '0');
			FONT_VERSION = (FONT_VERSION*10) + (FONT_HEAD[j+3] - '0');
			break;
		}
	}	

	printf0("\t Move FONT (No.%3d, Ver.%3d, size:0x%x) ",  FONT_NUMBER, FONT_VERSION, size );
	printf0("\r Adr.%5x,%5x,%5x,%5x,%5x,%5x,%5x ", ENG16ADR, ENG24ADR, ENG32ADR, ENG40ADR, ENG48ADR, ENG56ADR, ENG64ADR);
	
	s_pnt = (FONT_START_BLOCK * MAX_FLASH);
	for(j=0;j<size;j+=0x800){
	  	WriteReadAddr.Zone  = 0x00;
		WriteReadAddr.Block = NAND_TB[(s_pnt+j)/MAX_FLASH];
  		WriteReadAddr.Page  = (j%MAX_FLASH) / 0x800;
	  	FMC_NAND_ReadSmallPage (nandRxBuffer, WriteReadAddr, 1);
		for(i=0;i<0x800;i++) *(u8*)(FONT_BASE+j+i) = nandRxBuffer[i];
   		if(!(j&0x1FFF)) printf0(">");
  	}
}
/**************************************************************/
u8 get_max_wide(u8 *str){
u8 a,b=0;	
	for(a='0';a<='9';a++) if(str[a]>b) b=str[a];
	return(b);
}
void move_font16(u32 fadr, u16 jp){
u16 a;
	ENG16ADR = fadr;
	for(a=0x00;a<0x20;a++) size16[a] = 9;
	for(a=0x20;a<0x80;a++) size16[a] = FONT_HEAD[jp+a] & 0x7F;
	for(a='0';a<='9';a++) size16[a] = get_max_wide(size16);
//printf0("\r size16\r");	for(a=0x20;a<0x80;a++) printf0("%2d,",size16[a]);
}
void move_font24(u32 fadr, u16 jp){
u16 a;
	ENG24ADR = fadr;
	for(a=0x20;a<0x80;a++) size24[a] = FONT_HEAD[jp+a] & 0x7F;
	for(a='0';a<'9';a++) size24[a+1] = get_max_wide(size24);
}
void move_font32(u32 fadr, u16 jp){
u16 a;
	ENG32ADR = fadr;
	for(a=0x20;a<0x80;a++) size32[a] = FONT_HEAD[jp+a] & 0x7F;
	for(a='0';a<'9';a++) size32[a+1] = get_max_wide(size32);
}
void move_font40(u32 fadr, u16 jp){
u16 a;
	ENG40ADR = fadr;
	for(a=0x20;a<0x80;a++) size40[a] = FONT_HEAD[jp+a] & 0x7F;
	for(a='0';a<'9';a++) size40[a+1] = get_max_wide(size40);
}
void move_font48(u32 fadr, u16 jp){
u16 a;
	ENG48ADR = fadr;
	for(a=0x20;a<0x80;a++) size48[a] = FONT_HEAD[jp+a] & 0x7F;
	for(a='0';a<'9';a++) size48[a+1] = get_max_wide(size48);
}
void move_font56(u32 fadr, u16 jp){
u16 a;
	ENG56ADR = fadr;
	for(a=0x20;a<0x80;a++) size56[a] = FONT_HEAD[jp+a] & 0x7F;
	for(a='0';a<'9';a++) size56[a+1] = get_max_wide(size56);
}
void move_font64(u32 fadr, u16 jp){
u16 a;
	ENG64ADR = fadr;
	for(a=0x20;a<0x80;a++) size64[a] = FONT_HEAD[jp+a] & 0x7F;
	for(a='0';a<'9';a++) size64[a+1] = get_max_wide(size64);
}
/**************************************************************/
extern uint16_t FLASH_If_Erase(uint32_t Add);

#define TEST_FADDR	0x08010000
#define TEST_FSIZE	0x10000
#define PRINT_SIZE	0x80
void test_flash_write(void){
u32	ww1, ww2, ww3;
	printf0("\r Read Flash1\r");
	for(ww1=0;ww1<PRINT_SIZE;ww1+=4){
		ww2 = *(uint32_t*)(TEST_FADDR + ww1);
		if(!ww1){
			if((ww2&0xFFFF0000)!=0x55550000) ww3 = 0x55550000;
			else ww3 = 0xAAAA0000;
		}
		wait_tx0();
		delay_ms(100);
		printf0(" %8lx", ww2);
	}

	FLASH_Unlock();
//	FLASH_ClearFlag( FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR | FLASH_FLAG_RDERR );
	FLASH_ClearFlag( FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR | FLASH_FLAG_RDERR );

	if(FLASH_If_Erase(TEST_FADDR)==OK){
		printf0("\r **** Erase Flash ****");
		for(ww1=0;ww1<PRINT_SIZE;ww1+=4){
			ww2 = *(uint32_t*)(TEST_FADDR + ww1);
			wait_tx0();
			delay_ms(100);
			printf0(" %8lx", ww2);
		}
		printf0("\r **** Write Flash ****");
		for(ww1=0;ww1<TEST_FSIZE;ww1+=4){
			FLASH_ProgramWord((TEST_FADDR + ww1), (ww3+(ww1/4)));
		}
	}else printf0("\r **** Erase Fail !!! ****");
	FLASH_Lock();

		printf0("\r Read Flash2\r");
	for(ww1=0;ww1<PRINT_SIZE;ww1+=4){
		ww2 = *(uint32_t*)(TEST_FADDR + ww1);
		wait_tx0();
		delay_ms(100);
		printf0(" %8lx", ww2);
	}
}
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
