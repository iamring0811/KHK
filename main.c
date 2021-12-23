#define _MAIN_C_

#include <stdio.h>
#include "stm32f4xx.h"
#include "common\32f429_port.h"
#include "util\usart.h"
#include "util\tm_adc.h"
#include "util\eep.h"
#include "util\LCD.h"
#include "util\flash.h"
#include "util\para.h"
#include "USB\usb_file.h"
#include "sysadj.h"
#include "resm.h"
#include "touch.h"
#include "dsp.h"
#include "meas.h"
#include "com.h"

//#define SkipBmp		//skip BMP move
//#define SkipUsb		//skip USB initialize
//#define SkipIndex

// ���ڵ� 
extern u8 new_barcode;
extern char barcode[30];

#define RomBaseAdr		0x08020000
void (*ReRun)(void) = (void (*)(void))RomBaseAdr;

u32	tt = 0;
u8	even500;
u8	second_strip_check;
u16	read_strip_data;
u16	flag_usb_fs, dsp_usb_fs=0;;

void chk_barcode(void);
void time_loop(void);
void motor_loop(void);
void read_strip(void);
void dsp_first(void);
void init_ststem(void);

void test_meas(void);
/******************************************************************************************/
u16	usb_delay=0;
u8 tick_usb_process;

int main(void){
	dsp_first();
	init_ststem();
#ifndef SkipIndex
	make_index();
#endif
#define USB_FS_ON
#ifdef USB_FS_ON
//	USB_Init();
	USB_FS_Init();
#endif
	send_chk_ref();
	update_bmp();

//	read_balance();
//	if(flag_balance!=OK) set_balance(100);

	MS10 = 0;
//	test_flash(1);				//Sflash
//	test_rtc_init();			//����ð�����
//	test_flash_write();			//Int.NOR Flash
 	printf0("\r Main Loop");

//                                 123456789012345678                      1234567890
//	SEQ_NO= 301; sprintf(USER_ID, "T123456789        "); sprintf(OPER_ID, "abcdefghij"); modex = MODE_TEST;
//	QC_NO=   1; sprintf(USER_ID, "Q123456789        "); sprintf(OPER_ID, "abcdefghij"); modex = MODE_QC;
//	printf0("\r NO:%d,Qc:%d", SEQ_NO, QC_NO);
//	for(tt=1;tt<=1189;tt++){ run_measure(); OPER_ID[MAX_OPERID-1] = (tt%10)+'0'; delay_ms(10); }
//	for(tt=1;tt<=10;tt++){ view_result_store(tt+1); key_buzzer(); delay_ms(3100); }
//	run_calibration();
//	delay_ms(500);
//	srt_seqno = SEQ_NO;	dsp_memory(); modex = MODE_MEMORY;
//	delete_result_store(101, 220);
//SEQ_NO = 4990;
	if(StripIN) read_strip_move = 0x3FFF;
	stripCnt = 0;
	chk_table();
	init_system_check();//201021
	dsp_home();					//
//	SEQ_NO = 4980;
//	QC_NO = 5080;
	inputCnt = 0;
	errorCnt = 0;
	for(int i=0;i<errorStackSize;i++)errorStack[i] = 0;
	while(1){	
		if(MS10){
			time_loop();			
			tch_read();
			read_strip();
			motor_loop();
			if(modex>=MODE_TEST && modex<MODE_QC){
				chk_barcode();
			}else{
				new_barcode = 0;
			}
			chk_com_dly();
			chk_table();
			if(errorSwitch)dsp_Error();
			MS10=0;
		}else{
			prog_decode();
#ifdef USB_FS_ON
			if(tick_usb_process){ USB_FS_Process();	tick_usb_process=0; }
#endif
		}
	}	
}
/************************************************************************/

void chk_barcode(void){
	u16 i;
	u8 j, cnt, inputFlag = 0;
	
	//if((modex&0xFF00)==MODE_TEST){										//no use at MODE_QC
		if(new_barcode){
			usb_delay=0;
			printf0(" barcode=%s",barcode);
			new_barcode=0;

			for(i=0;i<MAX_USERID;i++)MEASuserid[i]=0;
			if(StripIN){												//test mode
				sprintf(MEASuserid, "%s-%3d", "Barcode", (test_loscnt>>3));
			}else sprintf(MEASuserid, "%s", barcode);						//normal mode
			MEASuserid[MAX_USERID] = NULL;
			for(i=0;i<1000;i++){
				cnt = 0;
				for(j=0;j<18;j++){
					if(modex==MODE_TEST && tempSampleID[i][j] == MEASuserid[j])cnt++;
					if(modex>MODE_TEST && modex<MODE_QC && tempSampleID2[i][j] == MEASuserid[j])cnt++;
				}
				if(cnt>=18){
					inputFlag = 1;//기존과 동일한 ID가 존재할 때 inputFlag = 1
					break;
				}
			}
			if(modex >= MODE_TEST && modex<MODE_TEST_seq && !inputFlag && SEQ_NO + maxTempCnt - measCnt <= END_SEQ_NO){
				//LCDbmpxx(bm_samid, 246, 146);
				//LCDbmpxx(bm_360_w, 246+120, 146);
				//out_line_16(371, 150, MEASuserid, BLK_LCD);
				barcodeFlag = 1;
				for (i = 0; i < 18; i++) {
					tempSampleID[maxTempCnt][i] = tempSampleID2[maxTempCnt][i] = MEASuserid[i];
					MEASuserid[i]=NULL;
				}
				tempMeasCnt++;
				if(tempMeasCnt>maxTempCnt)maxTempCnt = tempMeasCnt;
				returnFlag++;
				if(modex != MODE_TEST_Error && modex != MODE_TEST_Error2)dsp_los_table();//dsp_test();//210427
			}else if(modex >= MODE_TEST_seq && modex < MODE_QC && !inputFlag && SEQ_NO + maxTempCnt - measCnt <= END_SEQ_NO){
				for (i = 0; i < 18; i++) {
					tempSampleID2[tempMeasCnt][i] = MEASuserid[i];
					MEASuserid[i]=NULL;
				}
				if(modex != MODE_TEST_Error && modex != MODE_TEST_Error2)dsp_test_set_sid();
				returnFlag++;
			}
		}
		if((++flag_usb_fs)>100){
			flag_usb_fs = dsp_usb_fs = 1;
			if(StripIN) new_barcode = 1;								//test mode
		}else if(dsp_usb_fs){
			if((++dsp_usb_fs)>50){
				dsp_usb_fs = 0;
			}
		}
	//}
}
/************************************************************************/
void test_meas(void){
u8 a = 0;
	while(1){
		printf0("\r run meas %d ", a++);
		run_measure();
	}
}
/************************************************************************/
#define noStripCnt 5
void motor_loop(void){
	if(!motor_step && PH_TBL && PH_BOX){//No move Motor and No Error then strip input check
		if(!trayFullFlag && !errorCnt && read_strip_flag  && ((modex == MODE_TEST && SEQ_NO+inputCnt<=END_SEQ_NO) || (modex == MODE_QC && QC_NO+inputCnt<=END_QC_NO))){//Total SEQ =< END SEQ
			read_strip_flag=0;
			push_strip();
			motor_step=1;
			motor_wait=0;
			read_strip_move |= 0x0001;
			dsp_strip();
			if((modex == MODE_QC && QC_NO>=END_QC_NO-10) ||(modex == MODE_TEST && SEQ_NO>=END_SEQ_NO-50)){//SEQ almost Full then SEQ displays RED
				fullFlag = 1;
			}
		}
	}else if(motor_step==1 && PH_TBL && PH_BOX){
		if(motor_wait>=MOTOR_DLY){
			if(read_strip_move&0x0002){
#ifdef	SELF_TEST
				second_strip_check = 1;
#endif
				if(!second_strip_check){
					if(inputCnt)inputCnt--;//210713
					if(++noStripFlag>=noStripCnt)AddError(Err_PlateOut);
					read_strip_move &= ~0x0002;
					printf0(" *** No Strip[%d] ***", strip_adc);
				}else{
					noStripFlag = 0;
					printf0(" *** Is Strip ***");
				}
			}
			printf0("\r Move Strip");
			move_strip();
			read_strip_move <<= 1;
			motor_step=2;
			motor_wait=0;
			dsp_strip();
		}
	}else if(motor_step==1 && !PH_TBL && errorSwitch){
		if(!plateFlag){
			AddError(Err_PlateOut);
			plateFlag = 1;
		}
	}else if(motor_step==1 && !PH_BOX && errorSwitch){
		if(!trayFlag){
			AddError(Err_WasteOut);
			trayFlag = 1;
		} 
	}else if(motor_step==2 && PH_TBL && PH_BOX){
		if(motor_wait>=MOTOR_DLY){
			if(!trayFullFlag && !errorCnt && read_strip_flag  && ((modex == MODE_TEST && SEQ_NO+inputCnt<=END_SEQ_NO) || (modex == MODE_QC && QC_NO+inputCnt<=END_QC_NO))){//210713
				read_strip_flag=0;
				push_strip();
				read_strip_move |= 0x0001;
			}
			if(read_strip_move&0xFFFF) motor_step=1;
			else motor_step=0;
			motor_wait=0;
			dsp_strip();
			if((modex == MODE_QC && QC_NO>=END_QC_NO-10) ||(modex == MODE_TEST && SEQ_NO>=END_SEQ_NO-50)){//SEQ almost Full then SEQ displays RED
				fullFlag = 1;
			}
			if(read_strip_move & 0x4000)run_measure();
		}
	}
}					
/************************************************************************/
void read_strip(void){
u8 b1, b2;
	if(StripIN)read_strip_flag=1;
	if(rxw2!=rxr2){
		b1 = getcmd2();
		if(!read_strip_step){
			if(b1==0xAA){ read_strip_step=1; read_strip_wait = 3; }
		}else if(read_strip_step==1){
			read_strip_data = b1;
			read_strip_step++;
		}else if(read_strip_step==2){
			read_strip_data = (read_strip_data<<8) + b1;
			read_strip_step++;
		}else{
			b2=((0xAA+(read_strip_data>>8)+(read_strip_data&0xFF)))&0xFF;		//get byte sum
			if(b1==b2){
				if(read_strip_data&0x1000) second_strip_check = 1;
				else {
					second_strip_check = 0;
				}
				read_strip_data &= 0xFFF;
				if(read_strip_data!=0x7FF){
					read_strip_none=0;
					if((++read_strip_have)>=3){
						read_strip_have=0;
						if(!read_strip_flag) printf0(" SetCsFalg ");
						read_strip_flag=1;
					}
				}else{
					read_strip_have=0;
					if((++read_strip_none)>=3){
						read_strip_none=0;
						if(read_strip_flag) printf0(" ClrCsFalg ");
						read_strip_flag=0;

					}
				}
			}
			read_strip_step=read_strip_wait=0;
		}
	}else if(read_strip_wait){
		if(!(--read_strip_wait)) read_strip_step=0;
	}
}
/************************************************************************/
/************************************************************************/
u8	bm_dly=0;
u8	bm_pnt=1;

void time_loop(void){							// 10[ms]
//u32 a, b;
//	if(rxw1!=rxr1) savcmd1(getcmd1());
//	if(rxw2!=rxr2) savcmd2(getcmd2());
//	if(rxw3!=rxr3) savcmd3(getcmd3());
//	if(rxw4!=rxr4) savcmd4(getcmd4());
//	if(rxw6!=rxr6) savcmd6(getcmd6());
//	if(rxw7!=rxr7) savcmd7(getcmd7());

	if((++MS500)>=50){
		MS500 = 0;
		if(!rcv_step) readTime();
//printf0(" %d,%d", yp_adc, xp_adc);		
		if(!even500){
			PORT_LEDTog(LEDG);
			even500=1;
#ifdef	SELF_TEST
//printf0(" main_self:%d", flag_self);	
			if(flag_self){
				if(!(read_strip_move&0x0001)){
					if((++flag_self)>16){
						read_strip_flag=1;
						flag_self=1;
					}
				}else flag_self=1;
			}					 
#endif				
//			if(PH_BOX) printf0("\r BOX IN");	else printf0("\r BOX NO");
//			if(PH_TBL) printf0("   Table IN");	else printf0("   Table NO");

			if(!rcv_step){
				bm_dly++;
//				if((bm_dly&7)==0){ key_buzzer(); push_strip(); }
//				if((bm_dly&7)==0){ key_buzzer(); move_strip(); }
//				if((bm_dly&7)==4){ key_buzzer(); move_strip(); }
			}
		}else{
			even500=0;
			if(!rcv_step){
//				printf0("  DSW:%2x", DSW_Read() );
				readAllRTC(); 
//				printf0("  time:%2d-%2d-%2d,%2d-%2d-%2d", year,month,day,hour,min,sec);	
			}
			if(!rcv_step){
//				b=0;
//				for(a=0;a<rcv_size;a++)	b += *(u8*)(BMP_BUFF+a);		
//				printf0(" a=%lx, b=%8lx", a, b);	
			}
//			printf0(" FS:%d", flag_usb_fs);
		}
	}

	if(rcv_wait){ 
		if(!(--rcv_wait)){ 
			rcv_step=0; 
			printf0("\r RCV Stop: %d-%d,", rcv_size, rcvwp);			
		}
	}

}

/************************************************************************/
/**************************************************/
#define LCD_WAIT	100
void dsp_first(void){
int i, j;
	kdelay(20000);
	for(i=0;i<LCD_WAIT;i++){
		for(j=LCD_WAIT;j>0;j--){
			if(i>j) LCD_ON();
			else LCD_OFF();
			kdelay(250);
		}
	}
	LCD_ON();
}
/**************************************************/
void init_ststem(void){
	PORT_Config();
	DSW_Init();
	init_RAM();
	TIM_Config();
	TIM_SetUsec(60);			// 75us
	USART_Config();
 	printf0("\r Start 2nd -----");
	init_ADC();
	initRTC();
	readAllRTC();
	DSW_Read();
	read_jedec(); 
	initEEP();
	FMC_NAND_Check();
#ifndef SkipBmp
	printf0("\r LANGUAGE_FLAG : %d ", LANGUAGE_FLAG);
	if (LANGUAGE_FLAG) {
		move_BMP_second();
		if (BMP_NUMBER < 3) move_BMP();
	}
	else move_BMP();
#else
	delay_ms(1000);
#endif	
	move_FONT();
	TM4_LEDS_port();			//
#ifndef SkipUsb
//	USB_Init();
#endif	
	init_BUZZ();
}
/**************************************************/
/**************************************************/
/**************************************************/
/** (C) COPYRIGHT STMicroelectronics *END OF FILE**/
/**************************************************/
/**************************************************/
/**************************************************/
