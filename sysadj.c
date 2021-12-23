#define	_SYSADJ_C_

#include <stdio.h>
#include <string.h>
#include "stm32f4xx.h"
#include "Common\stm324x9i_eval.h"
#include "common\32f429_port.h"

#include "common\32f429_lcd.h"
#include "util\usart.h"			// include port.h
#include "util\lcd.h"
#include "util\eep.h"
#include "util\Flash.h"
#include "USB\usb_file.h"
#include "com.h"
#include "sysadj.h"
#include "meas.h"
#include "resm.h"
#include "touch.h"
#include "dsp.h"

char PrintSystem1[] = "= = = = System Check  = = = = = ";	
char PrintSystem3[] = "= = = = = = = = = = = = = = = = ";	
char PrintSystem4[] = "      System Check Fail         ";
char PrintSystem5[] = "      System Check Success      ";
void set_print_time(void);

extern void backup_intflash(u16 adr, u16 size);
void run_meas_adj(void);


/********************************************************************************************/
void out_dac(u8 dat){
//	if(dat&0x01){ FIO2DIR |= DAC0; DAC0_LO(); }else{ FIO2DIR &= ~DAC0; }
//	if(dat&0x02){ FIO1DIR |= DAC1; DAC1_LO(); }else{ FIO1DIR &= ~DAC1; }
//	if(dat&0x04){ FIO1DIR |= DAC2; DAC2_LO(); }else{ FIO1DIR &= ~DAC2; }
//	if(dat&0x08){ FIO3DIR |= DAC3; DAC3_LO(); }else{ FIO3DIR &= ~DAC3; }
}

#define DAC_RGB	2800
u16 dac_abs(u16 dat){
	if(dat>DAC_RGB) return(dat-DAC_RGB);
	else return(DAC_RGB-dat);	
}

short set_balance(u16 yoff){
u16 w1, w2;
u8  sav_r,sav_g,sav_b;
u8	msb, b1;
	
	sav_r=dac_r; 
	sav_g=dac_g; 
	sav_b=dac_b;
	dac_r = dac_g = dac_b = 0xFF;
	
	savcmd1(BIAS_CMD); 	rxw1 = rxr1 = 0;
	delay_us(200);	savcmd1(BIAS_CMD); 	
	delay_us(200);	savcmd1(BIAS_CMD); 	
	
	for(w2=0;w2<5000;w2++){
		delay_ms(1);
		if(rxw1!=rxr1){
			b1 = getcmd1();
			if(!msb){
				if((b1==BIAS_RED)||(b1==BIAS_GRN)||(b1==BIAS_BLU)) msb = b1;
			}else{
				if((b1&0xF0)==BIAS_LOW){
					     if(msb==BIAS_RED) dac_r = b1&0x0F; 
					else if(msb==BIAS_GRN) dac_g = b1&0x0F;
					else if(msb==BIAS_BLU) dac_b = b1&0x0F;
				}
				msb = 0;
				if((dac_r<0x10)&&(dac_g<0x10)&&(dac_b<0x10)) break;
			}
		}		
	}
	
	if((dac_r>=0x10)||(dac_g>=0x10)&&(dac_b>=0x10)){
		printf0("\r Error RGB balance!!!");
		sprintf(charbuff, "Error.RGB balance:%d-%d-%d",  dac_r, dac_g, dac_b);
		msg24(62,294+yoff,RED_LCD,charbuff);
		return(NG);
	}

	printf0("\r R:%d", dac_r);	printf0("   G:%d", dac_g);	printf0("   B:%d", dac_b);	

	sprintf(charbuff, "R:%2d  G:%2d  B:%2d", dac_r, dac_g, dac_b );
	msg16(62+10,294+yoff,BLU_LCD,charbuff);

	if((sav_r!=dac_r)||(sav_g!=dac_g)||(sav_b!=dac_b)){
		w1 = ((dac_r<<12)&0xF000) | ((dac_g<<8)&0x0F00) | ((dac_b<<4)&0x00F0) | ((dac_r+dac_g+dac_b)&0x000F);
		write24w(DAC_ADR, w1);
		write_id_chksum();
	}
	flag_balance=OK;
	return(OK);
}
/********************************************************************************************/
#define INI_RGB		0x0777		//R-G-B
void set_default_RGB(void){
	dac_r = (INI_RGB>>8)&0x0F;
	dac_g = (INI_RGB>>4)&0x0F;
	dac_b = (INI_RGB>>0)&0x0F;
}
/********************************************************************************************/
void read_balance(void){
u16 w1;
	w1=read24w(DAC_ADR);
	dac_r = (w1>>12)&15;
	dac_g = (w1>> 8)&15;
	dac_b = (w1>> 4)&15;
	if(((dac_r+dac_g+dac_b)&15)==(w1&15)){
		flag_balance=OK;
		printf0("\r DAC Balance: R-%d, G-%d, B-%d", dac_r, dac_g, dac_b );
	}else{
		flag_balance=NG;
		set_default_RGB();
		printf0("\r Error. DAC Balance!!! - %4x", w1);
	}
}

/********************************************************************************************/
/********************************************************************************************/
/********************************************************************************************/
extern void read_strip(void);
extern u8	second_strip_check;
#define OFF_SYSCHK		-20
extern void correction_curve(void);

void run_calibration(void){
u16 i, j;	
	printf0("\r Start Calibration");
	dsp_base_bar();
	out_line_24(50, OFF_SYSCHK+100, "Check Strip", 	BLK_LCD);	
	send_meas_step('1');
	while(rxw2!=rxr2) read_strip();
	read_strip_flag = read_strip_have = 0;
	delay_ms(100);
	for(i=0;i<1000;i++){				//wait 10[sec]
		delay_ms(10);
		while(rxw2!=rxr2) read_strip();
		if(read_strip_flag) break;
	}
	if(i>=1000){
		printf0("\r No strip at 1st");
		out_line_24(400, OFF_SYSCHK+100, "No Strip!!!", RED_LCD);	
		for(i=0;i<3;i++){ key_buzzer(); delay_ms(300); }
		return;
	}
	send_meas_step('2');
	printf0("\r Push strip");
	out_line_24(50, OFF_SYSCHK+130, "Push Strip", 	BLK_LCD);	
	push_strip();
	read_strip_move = 0x0001;
	while(run_stepb) delay_ms(10);		delay_ms(200);
	
	printf0("\r Move strip 1");
	out_line_24(50, OFF_SYSCHK+160, "Move Strip  1", 	BLK_LCD);	
	move_strip();
	read_strip_move <<= 1;
	while(run_stepa) delay_ms(10);	delay_ms(200);	

	while(rxw2!=rxr2) read_strip();
	second_strip_check = 0;	
	for(i=0;i<100;i++){					//wait 1[sec]
		delay_ms(10);
		while(rxw2!=rxr2) read_strip();
second_strip_check = 1;	
		if(second_strip_check) break;
	}
	if(i>=100){
		printf0("\r No strip at 2nd");
		out_line_24(400, OFF_SYSCHK+160, "No Strip!!!", 	RED_LCD);	
		for(i=0;i<3;i++){ key_buzzer(); delay_ms(300); }
		return;
	}
	for(i=0;i<13;i++){
//	for(i=0;i<2;i++){
		printf0(" %d", i+2);
		sprintf(charbuff, " %2d", i+2 );
		if(i>=8){	j = 221 + ((i-8)*39); }else j = i*26;
		out_line_24(227+j, OFF_SYSCHK+160, charbuff, 	BLK_LCD);	
		move_strip();
		read_strip_move <<= 1;
		while(run_stepa) delay_ms(10);		delay_ms(200);
	}
/*	
*/
	out_line_24(50, OFF_SYSCHK+190, "Read RGB data", 	BLK_LCD);	
	send_meas_step('3');
	run_meas_adj();
	span_curve();
	store_reference_rgb();
	out_line_24(50, OFF_SYSCHK+220, "Store RGB data", 	BLK_LCD);	
	read_ref_data();
//	for(i=0;i<MAX_PAD_NO;i++){			
//		dataRED1[i] = whtRED;
//		dataGRN1[i] = whtGRN;
//		dataBLU1[i] = whtBLU;
//	}

	send_meas_step('4');
	move_strip_info();
	printf0("\r Move strip info.");
	delay_ms(3000);
	out_line_24(50, OFF_SYSCHK+250, "Read RGB data", 	BLK_LCD);	
	run_meas_adj();
	printf0("\r Run measure adj.");
	span_curve();
	correction_curve();
//	for(i=0;i<MAX_PAD_NO;i++){			
//		dataRED1[i] = whtRED;
//		dataGRN1[i] = whtGRN;
//		dataBLU1[i] = whtBLU;
//	}
	delay_ms(500);
	send_meas_step('5');
	out_check();

	delay_ms(1500);
	for(i=0;i<2;i++){
		printf0(" %d", i+2);
		sprintf(charbuff, " %2d", i+2 );
		if(i>=8){	j = 221 + ((i-8)*39); }else j = i*26;
		out_line_24(227+j, OFF_SYSCHK+160, charbuff, 	BLK_LCD);	
		move_strip();
		read_strip_move <<= 1;
		while(run_stepa) delay_ms(10);		delay_ms(200);
	}
	key_clear();
}
/***************************************************************/
/***************************************************************/
/***************************************************************/

void run_meas_adj(void){
u16 a, msb;
u16	r_pnt, g_pnt, b_pnt, i;

	printf0("\r Start measure adj,");
	MEASyear = year;
	MEASmonth = month;
	MEASday = day;
	MEAShour = hour;
	MEASmin = min;
	MEASsec = sec;

	savcmd1(RUN_CMD); 	rxw1 = rxr1 = 0;
	delay_us(200);	savcmd1(RUN_CMD); 	
	delay_us(200);	savcmd1(RUN_CMD); 	
	r_pnt = g_pnt = b_pnt = 0;
	motor_wait = 0;
	msb = 0xFF;
	a = 0;												//for NO_MOTOR
	flag_motor_run = 1;
	while(motor_wait<MOTOR_DLY){
#ifndef NO_MOTOR		
		if(rxw1!=rxr1){
			a = getcmd1();
			if(a&0x80){
				if(a>=0xA0) msb = a;
				else if(a==ADC_END1) msb = a;
				else msb = 0xFF;
			}else{
				     if((msb&0xE0)==ADC_RED) buffRED1[r_pnt++] = ((msb&0x1F)<<7)|(a&0x7F);
				else if((msb&0xE0)==ADC_GRN) buffGRN1[g_pnt++] = ((msb&0x1F)<<7)|(a&0x7F);
				else if((msb&0xE0)==ADC_BLU) buffBLU1[b_pnt++] = ((msb&0x1F)<<7)|(a&0x7F);
				else if((msb==ADC_END1)&&(a==ADC_END2)) break;
				msb = 0xFF;
			}
		}
#else
		delay_ms(2);
		buffRED1[r_pnt++] = meas_dat1[a++];
		buffGRN1[g_pnt++] = meas_dat1[a++];
		buffBLU1[b_pnt++] = meas_dat1[a++];
#endif
		if((r_pnt>=MaxRawData)||(g_pnt>=MaxRawData)||(r_pnt>=MaxRawData)) break;
	}
	flag_motor_run = 0;
	printf0("  End measure : %d-%d-%d", r_pnt, g_pnt, b_pnt);
	if(motor_wait>=MOTOR_DLY) printf0("\r Time Over!!!");
	if(r_pnt<MaxRawData) for(i=r_pnt;i<MaxRawData;i++) buffRED1[i] = buffRED1[i-1]; 
	if(g_pnt<MaxRawData) for(i=g_pnt;i<MaxRawData;i++) buffGRN1[i] = buffGRN1[i-1]; 
	if(b_pnt<MaxRawData) for(i=b_pnt;i<MaxRawData;i++) buffBLU1[i] = buffBLU1[i-1]; 
}
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
#define wait_printing	wait_tx2
#define PadPoint		1		//was 0

#define LED_Range_MAX 1020
#define LED_Range_MIN 980
void out_check(void){
short  chk_result;

u16 R_MIN, R_MAX;
u16 G_MIN, G_MAX;
u16 B_MIN, B_MAX;
u16 i;
	R_MIN=G_MIN=B_MIN=1000;
	R_MAX=G_MAX=B_MAX=0;
	
	for(i=StripPosition[2];i<MaxRawData;i++){
		if(spanRED[i]>R_MAX) R_MAX = spanRED[i];
		if(spanRED[i]<R_MIN) R_MIN = spanRED[i];
		if(spanGRN[i]>G_MAX) G_MAX = spanGRN[i];
		if(spanGRN[i]<G_MIN) G_MIN = spanGRN[i];
		if(spanBLU[i]>B_MAX) B_MAX = spanBLU[i];
		if(spanBLU[i]<B_MIN) B_MIN = spanBLU[i];
	}
	
	//printf3("\r Rmin:%4d,Rmax:%4d", R_MIN,R_MAX);
	//printf3("\t Gmin:%4d,Gmax:%4d", G_MIN,G_MAX);
	//printf3("\t Bmin:%4d,Bmax:%4d", B_MIN,B_MAX);

	sprintf(charbuff, "V1:%4d (%4d-%4d)", (R_MIN+R_MAX)/2, LED_Range_MIN, LED_Range_MAX);
	out_line_24(50, OFF_SYSCHK+300, charbuff, 	BLU_LCD);	
	sprintf(charbuff, "V2:%4d (%4d-%4d)", (G_MIN+G_MAX)/2, LED_Range_MIN, LED_Range_MAX);
	out_line_24(50, OFF_SYSCHK+330, charbuff, 	BLU_LCD);	
	sprintf(charbuff, "V3:%4d (%4d-%4d)", (B_MIN+B_MAX)/2, LED_Range_MIN, LED_Range_MAX);
	out_line_24(50, OFF_SYSCHK+360, charbuff, 	BLU_LCD);

	chk_result=OK;
	if((R_MIN+R_MAX)/2>LED_Range_MAX) chk_result=NG;
	if((R_MIN+R_MAX)/2<LED_Range_MIN) chk_result=NG;
	if((G_MIN+G_MAX)/2>LED_Range_MAX) chk_result=NG;
	if((G_MIN+G_MAX)/2<LED_Range_MIN) chk_result=NG;
	if((B_MIN+B_MAX)/2>LED_Range_MAX) chk_result=NG;
	if((B_MIN+B_MAX)/2<LED_Range_MIN) chk_result=NG;
	if(chk_result!=OK){
		out_line_24(50, OFF_SYSCHK+390, "FAIL!!!", 	RED_LCD);
	}else{
		out_line_24(50, OFF_SYSCHK+390, "SUCCESS", 	BLU_LCD);
	}
//	msg24(220,190,0xff, "Return");

	print_thermal("\r%s", PrintSystem1);		wait_printing();
	MEASyear  = year;
	MEASmonth = month;
	MEASday   = day;
	MEAShour  = hour;
	MEASmin   = min;
	MEASsec   = sec;
	set_print_time();
	print_thermal("\r%s", charbuff);			wait_printing();
	print_thermal("\r%s", PrintSystem3);		wait_printing();
	sprintf(charbuff, "      V1:%4d (%4d~%4d)", (R_MIN+R_MAX)/2, LED_Range_MIN, LED_Range_MAX);
	print_thermal("\r\n%s", charbuff);
	sprintf(charbuff, "      V2:%4d (%4d~%4d)", (G_MIN+G_MAX)/2, LED_Range_MIN, LED_Range_MAX);
	print_thermal("\r\n%s", charbuff);
	sprintf(charbuff, "      V3:%4d (%4d~%4d)", (B_MIN+B_MAX)/2, LED_Range_MIN, LED_Range_MAX);
	print_thermal("\r\n%s", charbuff);
	if(chk_result!=OK){
		print_thermal("\r\n%s", PrintSystem4);	wait_printing();
	}else{
		print_thermal("\r\n%s", PrintSystem5);	wait_printing();
	}
	print_thermal("\r\n\n\n\n\n\n");			wait_printing();
}

/********************************************************************************************/
//void system_check(void){
//u16 i;	
//	printf0("\n *system_check");
//
//	draw_box(CABXS,CABYS,CABXE,CABYE,WHT,RAST1);
//	draw_box(SYBXS,SYBYS,SYBXE,SYBYE,WHT,RAST1);
//	LCDbmpxx(b_box6,   35,88+48+60+60+4);					
//
//	move_strip_info();
//	meas_time=0;
//	run_meas_adj(CABYS,CABYE);
//	span_curve();
//	get_start_strip_point();
//
//	if(BALANCE_AUTO){
//		printf0("\n *BALANCE_AUTO");
//		i=set_balance(0);
//		if(i){
//			move_strip_info();
//			meas_time=0;
//			run_meas_adj(CABYS,CABYE);
//			span_curve();
//			get_start_strip_point();
//		}
//	}else delay_ms(500);
//
//	for(i=0;i<MAX_PAD_NO;i++){		//	get_pad_data();
//		dataRED1[i] = whtRED;
//		dataGRN1[i] = whtGRN;
//		dataBLU1[i] = whtBLU;
//	}
//
//	run_meas_adj(SYBYS,SYBYE);
//	span_curve();
//	get_start_strip_point();
//
//	for(i=0;i<MAX_PAD_NO;i++){		//	get_pad_data();
//		dataRED1[i] = whtRED;
//		dataGRN1[i] = whtGRN;
//		dataBLU1[i] = whtBLU;
//	}
//	delay_ms(500);
//
//	out_check();
//}
/***************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
void set_print_time(void){
u32 w1;
u8  b1;
if (datePresentation == 0)
	{
		if (timePresentation == 0)
		{ 
			strcpy(charbuff, " 2000/01/01          AM 00:00:00\r\n");
			w1 = bin2bcdasc4(MEASyear);		charbuff[3] = w1 >> 8;	charbuff[4] = w1 >> 0;
			w1 = dateDelimiter;				charbuff[5] = w1;
			w1 = bin2bcdasc4(MEASmonth);	charbuff[6] = w1 >> 8;	charbuff[7] = w1 >> 0;
			w1 = dateDelimiter;				charbuff[8] = w1;
			w1 = bin2bcdasc4(MEASday);		charbuff[9] = w1 >> 8;	charbuff[10] = w1 >> 0;
			if (!MEAShour) b1 = 12;
			else if (MEAShour >= 12) {
				charbuff[21] = 'P';
				if (MEAShour > 12)	b1 = MEAShour - 12;
			}
			else b1 = MEAShour;
			w1 = bin2bcdasc4(b1);			charbuff[24] = w1 >> 8;	charbuff[25] = w1 >> 0;
			w1 = timeDelimiter;				charbuff[26] = w1;
			w1 = bin2bcdasc4(MEASmin);		charbuff[27] = w1 >> 8;	charbuff[28] = w1 >> 0;
			w1 = timeDelimiter;				charbuff[29] = w1;
			w1 = bin2bcdasc4(MEASsec);		charbuff[30] = w1 >> 8;	charbuff[31] = w1 >> 0;
		}
		else
		{
			strcpy(charbuff, " 2000/01/01           00:00:00\r\n");
			w1 = bin2bcdasc4(MEASyear);		charbuff[3] = w1 >> 8;	charbuff[4] = w1 >> 0;
			w1 = dateDelimiter;				charbuff[5] = w1;
			w1 = bin2bcdasc4(MEASmonth);	charbuff[6] = w1 >> 8;	charbuff[7] = w1 >> 0;
			w1 = dateDelimiter;				charbuff[8] = w1;
			w1 = bin2bcdasc4(MEASday);		charbuff[9] = w1 >> 8;	charbuff[10] = w1 >> 0;
			w1 = bin2bcdasc4(MEAShour);		charbuff[22] = w1 >> 8;	charbuff[23] = w1 >> 0;
			w1 = timeDelimiter;				charbuff[24] = w1;
			w1 = bin2bcdasc4(MEASmin);		charbuff[25] = w1 >> 8;	charbuff[26] = w1 >> 0;
			w1 = timeDelimiter;				charbuff[27] = w1;
			w1 = bin2bcdasc4(MEASsec);		charbuff[28] = w1 >> 8;	charbuff[29] = w1 >> 0;
		}

	}
	else
	{
		if (timePresentation == 0)
		{
			strcpy(charbuff, " 01/01/2000          AM 00:00:00\r\n");
			w1 = bin2bcdasc4(MEASyear);		charbuff[9] = w1 >> 8;	charbuff[10] = w1 >> 0;
			w1 = dateDelimiter;				charbuff[6] = w1;
			w1 = bin2bcdasc4(MEASmonth);	charbuff[4] = w1 >> 8;	charbuff[5] = w1 >> 0;
			w1 = dateDelimiter;				charbuff[3] = w1;
			w1 = bin2bcdasc4(MEASday);		charbuff[1] = w1 >> 8;	charbuff[2] = w1 >> 0;
			if (!MEAShour) b1 = 12;
			else if (MEAShour >= 12) {
				charbuff[21] = 'P';
				if (MEAShour > 12)	b1 = MEAShour - 12;
			}
			else b1 = MEAShour;
			w1 = bin2bcdasc4(b1);			charbuff[24] = w1 >> 8;	charbuff[25] = w1 >> 0;
			w1 = timeDelimiter;				charbuff[26] = w1;
			w1 = bin2bcdasc4(MEASmin);		charbuff[27] = w1 >> 8;	charbuff[28] = w1 >> 0;
			w1 = timeDelimiter;				charbuff[29] = w1;
			w1 = bin2bcdasc4(MEASsec);		charbuff[30] = w1 >> 8;	charbuff[31] = w1 >> 0;
		}
		else
		{
			strcpy(charbuff, " 01/01/2000           00:00:00\r\n");
			w1 = bin2bcdasc4(MEASyear);		charbuff[9] = w1 >> 8;	charbuff[10] = w1 >> 0;
			w1 = dateDelimiter;				charbuff[6] = w1;
			w1 = bin2bcdasc4(MEASmonth);	charbuff[4] = w1 >> 8;	charbuff[5] = w1 >> 0;
			w1 = dateDelimiter;				charbuff[3] = w1;
			w1 = bin2bcdasc4(MEASday);		charbuff[1] = w1 >> 8;	charbuff[2] = w1 >> 0;
			w1 = bin2bcdasc4(MEAShour);		charbuff[22] = w1 >> 8;	charbuff[23] = w1 >> 0;
			w1 = timeDelimiter;				charbuff[24] = w1;
			w1 = bin2bcdasc4(MEASmin);		charbuff[25] = w1 >> 8;	charbuff[26] = w1 >> 0;
			w1 = timeDelimiter;				charbuff[27] = w1;
			w1 = bin2bcdasc4(MEASsec);		charbuff[28] = w1 >> 8;	charbuff[29] = w1 >> 0;
		}
	}
}
/******************************************************************************/
void set_serial_data(void){
	
}
/******************************************************************************/
void run_fw_update(void){
}
/******************************************************************************/
void run_image_update(void){
}
/******************************************************************************/
#define USB_WAIT	12000
void fnb_update(u8 cmd) {									//2020-07-19 12:44����
	u16 i;
	flag_upload = 0;
	flag_second = 0;
	if ((cmd == FONT_UPDATE) || (cmd == BMP_UPDATE) || (cmd == BNF_UPDATE)) {
		for (i = 0; i < USB_WAIT; i++) {
			USB_HS_Process();
			if (USBFatReady()) {
				cmd = ExploreUSB(cmd);
				break;
			}
			delay_us(100);
		}
		if (i >= USB_WAIT) {
			//msg16(100, 440, WHT_LCD, "No USB Memory");
			//printf0("\r No USB Memory");
			delay_ms(2000);
		}
		else {
			clr_screen_box(100, 400, 680, 24);
			if (flag_upload & 0x02) move_FONT(); else delay_ms(100);
			msg24(100, 400, BLK_LCD, "Loop Count:%3d, File Count:%d", i, cmd);
			msg24(550, 400, RED_LCD, "[Please reboot]", i, cmd);
			printf0("\r USB Loop Count : %d, File Count : %d", i, cmd);
			if (flag_second) {
				msg24(518, 440, MGT_LCD, "[Write Second BMP]", i, cmd);
				printf0("\r ****** Write Second BMP ******");
			}
			if (flag_upload & 0x01) {
				if (!LANGUAGE_FLAG) move_BMP();
				else move_BMP_second();
			}
			else delay_ms(1000);
		}
	}

}
/******************************************************************************/
int prog_update(void){
int i, j;
	flag_upload = 0;
	for(i=0;i<USB_WAIT;i++){
		USB_HS_Process();
		if(USBFatReady()){
			j = FirmwareUSB();
			break;
		}
		delay_us(100);
	}		
	if(i>=USB_WAIT) flag_upload |= 0x80;
	return(j);
}
/******************************************************************************/
#define WHEAD	0x33
#define RHEAD	0x55
u8  rdata1, rdata2;

void send_chk_ref(void){
u8 b1, b2;
u16 i, w1;
u8 rstep;
	printf0("\r Send CHK_REF[%d]", CHK_REF);
	PORT_LEDhi(STPR );				//Hi:STCHK, Lo:Print
	delay_ms(10);
	while(rxw2!=rxr2);
	rstep = 0;
	for(i=0;i<5;i++){
						savcmd2(WHEAD);	b2  = WHEAD;
		b1=CHK_REF>>8;	savcmd2(b1);	b2 += b1;
		b1=CHK_REF>>0;	savcmd2(b1);	b2 += b1;
						savcmd2(b2);
		delay_ms(50);
		while(rxw2!=rxr2){
			b1 = getcmd2();
			if(rstep==0){
				if(b1==WHEAD){ 
					rstep=1; 
printf0("-0");
				}
			}else if(rstep==1){
				rdata1 = b1;
				rstep = 2;
printf0("-1");
			}else if(rstep==2){
				rdata2 = b1;
				rstep = 3;
printf0("-2");
			}else{
				b2 = WHEAD + rdata1 + rdata2;
printf0("-sum:%2x,%2x", b1,b2);
				if(b1==b2){
					w1 = rdata1;
					w1 = (w1<<8) + rdata2;
					if(w1==CHK_REF){
printf0(" Succes set CHK_REF");
						i = 5;
						break;
					}
				}
				rstep = 0;
			}
		}
	}
	PORT_LEDlo(STPR );				//Hi:STCHK, Lo:Print
}
/******************************************************************************/
short set_chk_ref(void){
u8 b1, b2;
u16 i, w1;
u8 rstep, rwait;
	printf0("\r Set CHK_REF");

	PORT_LEDhi(STPR );				//Hi:STCHK, Lo:Print
	delay_ms(10);
	while(rxw2!=rxr2);
	savcmd2(RHEAD);
	savcmd2(RHEAD);
	rstep = rwait = 0;
	for(i=0;i<1000;i++){
		delay_ms(1);
		if(rwait){ if(!(--rwait)) rstep = 0; }
		if(rxw2!=rxr2){
			b1 = getcmd2();
printf0(" *%2x", b1);
			if(rstep==0){
				if(b1==RHEAD){ 
					rstep=1; rwait=20; 
printf0("-0");
				}
			}else if(rstep==1){
				rdata1 = b1;
				rstep = 2;
printf0("-1");
			}else if(rstep==2){
				rdata2 = b1;
				rstep = 3;
printf0("-2");
			}else{
				b2 = RHEAD + rdata1 + rdata2;
printf0("-3");

				if(b1==b2){
					w1 = rdata1;
					w1 = (w1<<8) + rdata2;
					//printf3(" Write CHK_REF:%d", w1);
//					if((w1>MIN_REF)&&(w1<MAX_REF)){
					CHK_REF = w1;
					write24w(CHK_REF_ADR, CHK_REF);
					write_id_chksum();
					store_vars(CHK_REF_ADR, CHK_REF);	//
					printf3(" Write CHK_REF:%d", CHK_REF);
					send_chk_ref();
					break;
//					}
				}
				rstep = rwait = 0;
			}
		}
	}
	PORT_LEDlo(STPR );				//Hi:STCHK, Lo:Print
	if(i<100) return(OK);
	else return(NG);
}
/******************************************************************************/

void run_system_check(void){
u16 a;
u8  b1, msb=0;

	/*
	if(set_chk_ref()==OK)    out_line_16(RSCX_OFFSET, 146+(27*9), "OK",	BLU_LCD);		
	else		  		 out_line_16(RSCX_OFFSET-FAIL_OFFSET, 146+(27*9), "FAIL",	RED_LCD);
	sprintf(charbuff, "[%d]", CHK_REF);
	out_line_16(RSCX_OFFSET-FAIL_OFFSET+64, 146+(27*9), charbuff,	BLK_LCD);
	*/
	LCDbmpxx(bm_bm67,200,140);
	set_chk_ref();
	for(a=1;a<9;a++) erase_box0(RSCX_OFFSET-FAIL_OFFSET,146+(27*a),RSCX_OFFSET+32+FAIL_OFFSET,146+(27*a)+16);
	
	if(flag_table){
		out_line_16(RSCX_OFFSET, 146+(27*1), "OK",	BLU_LCD);	
		plateCheck = 0;
	}else{
		LCDbmpxx(bm_syschk_button, RSCX_OFFSET-YOFF_SYS_CHK, 140+(27*1));	
		out_line_16(RSCX_OFFSET-FAIL_OFFSET, 146+(27*1), "FAIL",	RED_LCD);
		plateCheck = 1;
	}
	delay_ms(500);

	if(flag_box){
		out_line_16(RSCX_OFFSET, 146+(27*2), "OK",	BLU_LCD);
		trayCheck = 0;
	}else{
		LCDbmpxx(bm_syschk_button, RSCX_OFFSET-YOFF_SYS_CHK, 140+(27*2));	
		out_line_16(RSCX_OFFSET-FAIL_OFFSET, 146+(27*2), "FAIL",	RED_LCD);
		trayCheck = 1;
	}
	move_strip();
	a = 0;
	while(run_stepa){ delay_ms(10); a++; }
	printf0("\r move strip delay:%d", a);
	
	//if((a>250)&&(a<280)) out_line_16(RSCX_OFFSET, 146+(27*3), "OK",	BLU_LCD);		//267
	if ((a > 330) && (a < 370)){
		out_line_16(RSCX_OFFSET, 146 + (27 * 3), "OK", BLU_LCD);		//267
		tableCheck = 0;
	}else{
		LCDbmpxx(bm_syschk_button, RSCX_OFFSET-YOFF_SYS_CHK, 140+(27*3));	
		out_line_16(RSCX_OFFSET-FAIL_OFFSET, 146+(27*3), "FAIL",	RED_LCD);
		tableCheck = 1;
	}
	delay_ms(500);
	
	push_strip();
	a = 0;
	while(run_stepb){ delay_ms(10);	a++; }
//	printf3("\r push strip delay:%d", a);
//	if((a>250)&&(a<280)) out_line_16(RSCX_OFFSET, 146+(27*4), "OK",	BLU_LCD);		//267
	if ((a > 330) && (a < 370)){
		out_line_16(RSCX_OFFSET, 146 + (27 * 4), "OK", BLU_LCD);		//267
		pushbarCheck = 0;
	}else{
		LCDbmpxx(bm_syschk_button, RSCX_OFFSET-YOFF_SYS_CHK, 140+(27*4));	
		out_line_16(RSCX_OFFSET-FAIL_OFFSET, 146+(27*4), "FAIL",	RED_LCD);
		pushbarCheck = 1;
	}
	delay_ms(500);

	dac_r = dac_g = dac_b = 0xFF;
	savcmd1(BIAS_CMD); 	rxw1 = rxr1 = 0;
	delay_us(200);	savcmd1(BIAS_CMD); 	
	delay_us(200);	savcmd1(BIAS_CMD); 	
	
	for(a=0;a<5000;a++){
		delay_ms(1);
		if(rxw1!=rxr1){
			b1 = getcmd1();
			if(!msb){
				if((b1==BIAS_RED)||(b1==BIAS_GRN)||(b1==BIAS_BLU)) msb = b1;
			}else{
				if((b1&0xF0)==BIAS_LOW){
					     if(msb==BIAS_RED) dac_r = b1&0x0F; 
					else if(msb==BIAS_GRN) dac_g = b1&0x0F;
					else if(msb==BIAS_BLU) dac_b = b1&0x0F;
				}
				msb = 0;
				if((dac_r<0x10)&&(dac_g<0x10)&&(dac_b<0x10)) break;
			}
		}		
	}
	//printf3("\r sensor move delay:%d", a);
	a /= 10;
//	if((a>190)&&(a<210)) out_line_16(RSCX_OFFSET, 146+(27*5), "OK",	BLU_LCD);		//1982
	if ((a > 130) && (a < 210)){
		out_line_16(RSCX_OFFSET, 146 + (27 * 5), "OK", BLU_LCD);		//1982
		sensorCheck = 0;
	}else{
		LCDbmpxx(bm_syschk_button, RSCX_OFFSET-YOFF_SYS_CHK, 140+(27*5));	
		out_line_16(RSCX_OFFSET-FAIL_OFFSET, 146+(27*5), "FAIL",	RED_LCD);
		sensorCheck = 1;
	}
	delay_ms(500);

	if(dac_r<0x10){
		out_line_16(RSCX_OFFSET, 146+(27*6), "OK",	BLU_LCD);		
		rCheck = 0;
	}else{
		LCDbmpxx(bm_syschk_button, RSCX_OFFSET-YOFF_SYS_CHK, 140+(27*6));	
		out_line_16(RSCX_OFFSET-FAIL_OFFSET, 146+(27*6), "FAIL",	RED_LCD);
		rCheck = 1;
	}
	delay_ms(500);

	if(dac_g<0x10){
		out_line_16(RSCX_OFFSET, 146+(27*7), "OK",	BLU_LCD);		
		gCheck = 0;
	}else{
		LCDbmpxx(bm_syschk_button, RSCX_OFFSET-YOFF_SYS_CHK, 140+(27*7));	
		out_line_16(RSCX_OFFSET-FAIL_OFFSET, 146+(27*7), "FAIL",	RED_LCD);
		gCheck = 1;
	}
	delay_ms(500);

	if(dac_b<0x10){
		out_line_16(RSCX_OFFSET, 146+(27*8), "OK",	BLU_LCD);		
		bCheck = 0;
	}else{
		LCDbmpxx(bm_syschk_button, RSCX_OFFSET-YOFF_SYS_CHK, 140+(27*8));	
		out_line_16(RSCX_OFFSET-FAIL_OFFSET, 146+(27*8), "FAIL",	RED_LCD);
		bCheck = 1;
	}
	checkFlag = 1;
}
/******************************************************************************/
#define SCX	200
#define SCDX 40
#define SCY	400
void init_system_check(void){
u16 a;
u8  b1, msb=0;
//	if(!flag_table)	out_line_16(SCX, SCY, "NG",	RED_LCD);	
//	delay_ms(500);
//	if(!flag_box)  	out_line_16(SCX+SCDX, SCY, "NG",	RED_LCD);
	
	move_strip();
	a = 0;
	while(run_stepa){ delay_ms(10); a++; }
	
//	if ((a > 330) && (a < 370)) out_line_16(SCX+(2*SCDX), SCY, "NG", RED_LCD);		//267
//	delay_ms(500);
	
	push_strip();
	a = 0;
	while(run_stepb){ delay_ms(10);	a++; }
//	if ((a > 330) && (a < 370)) out_line_16(SCX+(3*SCDX), SCY, "NG",	RED_LCD);
//	delay_ms(500);

	dac_r = dac_g = dac_b = 0xFF;
	savcmd1(BIAS_CMD); 	rxw1 = rxr1 = 0;
	delay_us(200);	savcmd1(BIAS_CMD); 	
	delay_us(200);	savcmd1(BIAS_CMD); 	
	
	for(a=0;a<5000;a++){
		delay_ms(1);
		if(rxw1!=rxr1){
			b1 = getcmd1();
			if(!msb){
				if((b1==BIAS_RED)||(b1==BIAS_GRN)||(b1==BIAS_BLU)) msb = b1;
			}else{
				if((b1&0xF0)==BIAS_LOW){
					     if(msb==BIAS_RED) dac_r = b1&0x0F; 
					else if(msb==BIAS_GRN) dac_g = b1&0x0F;
					else if(msb==BIAS_BLU) dac_b = b1&0x0F;
				}
				msb = 0;
				if((dac_r<0x10)&&(dac_g<0x10)&&(dac_b<0x10)) break;
			}
		}		
	}
	a /= 10;
//	if ((a > 130) && (a < 210)) out_line_16(SCX+(4*SCDX), SCY, "NG",	RED_LCD);
//	delay_ms(500);

//	if(dac_r<0x10)       out_line_16(SCX+(5*SCDX), SCY, "NG",	RED_LCD);
//	delay_ms(500);

//	if(dac_g<0x10)       out_line_16(SCX+(6*SCDX), SCY, "NG",	RED_LCD);
//	delay_ms(500);

//	if(dac_b<0x10)       out_line_16(SCX+(7*SCDX), SCY, "NG",	RED_LCD);
}
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
