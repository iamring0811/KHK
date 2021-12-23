#define _RESM_C_

#include <stdio.h>
#include "stm32f4xx.h"
#include "Common\stm324x9i_eval.h"
#include "common\32f429_port.h"
#include "common\32f429_lcd.h"
#include "util\usart.h"			// include port.h
#include "util\lcd.h"
#include "util\eep.h"
#include "resm.h"
#include "touch.h"
#include "dsp.h"
#include "Meas.h"


#define CE24_HI()			PORT_LEDhi(CE24) 	//
#define CE24_LO()			PORT_LEDlo(CE24)	//

/************************************************************************/
void push_strip(void){
	run_stepa = 0;
	PORT_LEDhi(MTAB );						//Hi:MT-B
	flag_mtab = MT_B;
	run_stepb = TICK_STEPB;	dly_phsw2 = 0;
	inputCnt++;
}
/************************************************************************/
void move_strip(void){
	run_stepb = 0;
	PORT_LEDlo(MTAB );						//Lo:MT-A	
	flag_mtab = MT_A;
	run_stepa = TICK_STEPA;	dly_phsw3 = 0;
}
/************************************************************************/
void chk_table(void){
	if(flag_table){
		if(!PH_TBL){
			noStripFlag = 0;
			if((++table_delay)>=10){
				flag_table=0; 
				table_delay=0;
				if(modex >= MODE_MEAS && modex <= MODE_QC_lot && !plateFlag){
					plateFlag = 1;
					AddError(Err_PlateOut);
				}
			}
		}else table_delay=0;
	}else{
		if(PH_TBL){
			if((++table_delay)>=10){				//3[sec]
				flag_table=1; 
				table_delay=0;
			}
		}else table_delay=0;
	}
	if(flag_box){
		if(!PH_BOX){
			if((++box_delay)>=10){
				flag_box=0; 
				box_delay=0;
				if(modex >= MODE_MEAS && modex <= MODE_QC_lot && !trayFlag && !trayFullFlag){
					trayFlag = 1;
					AddError(Err_WasteOut);
				}
			}
			stripCnt = 0;
		}else box_delay=0;
	}else{
		if(PH_BOX){
			if((++box_delay)>=10){
				flag_box=1; 
				box_delay=0;
				trayFullFlag = 0;
			}
		}else box_delay=0;
	}
}
/************************************************************************/

/**************************************************************
 ************************ RTC(ds1302) *************************
 **************************************************************/

u8 ds_fault=0;
u8 bcd2bin2(u8 ch);
u8 bin2bcd2(u8 bin);

void Delay1302(unsigned int d1){
	d1 *= 16;
	while(d1){	d1--;  }
}

void send_clk02(void){
//CLK_TEST:
	set_clk02(); Delay1302(25);
	clr_clk02(); Delay1302(15);
//goto CLK_TEST;
}

void sendRTC(u8 cmd){
u8 b1;
	rtc_write();
	for(b1=0;b1<8;b1++){
		if(cmd&0x01) set_dat02();
		else clr_dat02();
		cmd	= cmd >> 1;
		Delay1302(10);
		send_clk02();
	}
	Delay1302(10);
}
u8 dataRTC(void){
u8 b1, b2;
	rtc_read();
	b2=0;
	for(b1=0;b1<8;b1++){
		b2 = b2	>> 1;
		if(read_dat02()) b2	|= 0x80;
		send_clk02();
	}
	rtc_write();
	return(b2);
}

void writeRTC(u8 add, u8 sd){
u8 b1;
	clr_clk02();			//clk low
	en1302(); Delay1302(10);	//rst pin enable(ds1302	on)
	b1 = (add<<1) |	0x80;
	sendRTC(b1);
	sendRTC(sd);
Delay1302(200);
	dn1302();
Delay1302(200);
}

u8 readRTC(u8 add){
u8 b1;
	clr_clk02();
	en1302(); Delay1302(10);
	b1 = (add<<1) |	0x81;
	sendRTC(b1);
//	set_dat02();
	b1 = dataRTC();
	Delay1302(200);
	dn1302();
	Delay1302(200);
	return(b1);
}

/********************************************************************/

#define	DS_SLEEP 0x80
#define	DS_1224	 0x80

void initRTC(void){
u8 b1;
	rtc_write();
	
	writeRTC(RTC_DSWP, 0x00);	 writeRTC(RTC_DSWP,	0x00);
//	writeRTC(RTC_TCRV, 0xA5);	 writeRTC(RTC_TCRV,	0xA5);
	writeRTC(RTC_TCRV, 0x00);	 writeRTC(RTC_TCRV,	0x00);

	b1=readRTC(RTC_SECOND);
	b1=readRTC(RTC_SECOND);
//printf0("\r DS_SLEEP:%2x", b1);	
	if((b1&DS_SLEEP)==DS_SLEEP){
		if((bcd2bin2(b1&0x7f))>59) writeRTC(RTC_SECOND,	0x00);
		else{
//			writeRTC(RTC_SECOND, b1&0x7f);
			b1 = b1&0x7f; if(b1<59) b1 += 1;
			writeRTC(RTC_SECOND, b1&0x7f);
		}
	}
	b1=readRTC(RTC_HOUR);
	if((b1&DS_1224)==DS_1224){
		if((bcd2bin2(b1)&0x7f)>23) writeRTC(RTC_HOUR, 0x00);
		else writeRTC(RTC_HOUR,	b1&0x7f);
	}
	b1=bcd2bin2( readRTC(RTC_DAY) );
	if(!b1||(b1>31)) writeRTC(RTC_DAY, 0x01);
	b1=bcd2bin2( readRTC(RTC_MONTH)	);
	if(!b1||(b1>12)) writeRTC(RTC_MONTH, 0x01);

	readAllRTC();
printf0(" [time:%4d-%2d-%2d,%2d-%2d-%2d]", year,month,day,hour,min,sec);	
}


void readAllRTC(void){
	year =bcd2bin2(readRTC(RTC_YEAR));				//BCDȮ�ιٶ�.
	month=bcd2bin2(readRTC(RTC_MONTH));
	day	 =bcd2bin2(readRTC(RTC_DAY));
	hour =bcd2bin2(readRTC(RTC_HOUR));
	min	 =bcd2bin2(readRTC(RTC_MINUTE));
	sec	 =bcd2bin2(readRTC(RTC_SECOND));
}

void readTime(void){
//u16 a, b, smh;
u16 a, smh;
	smh = 0;
	a = bcd2bin2(readRTC(RTC_SECOND));
	if(a!=sec){
		time_flag = 1;
		smh = 's';
		sec = a;
		a = bcd2bin2(readRTC(RTC_MINUTE));
		if(a!=min){
			smh = 'm';
			min = a;
			a = bcd2bin2(readRTC(RTC_HOUR));
			if(a!=hour){
				smh = 'h';
				hour = a;
				day	 =bcd2bin2(readRTC(RTC_DAY));
				month=bcd2bin2(readRTC(RTC_MONTH));
				year =bcd2bin2(readRTC(RTC_YEAR));				//BCDȮ�ιٶ�.
				dsp_date();
			}
		}
		ds_fault=0;
		dsp_time();
	}else{
		if((a&DS_SLEEP)==DS_SLEEP){
			if((++ds_fault)>4){
//				writeRTC(RTC_SECOND, bin2bcd2(a&0x7F));
				a = bin2bcd2(a&0x7F);	//if(a<59) a += 1;
				writeRTC(RTC_SECOND, a);
				ds_fault=0;
			}
		}else{
			if((++ds_fault)>7){
//				writeRTC(RTC_SECOND, bin2bcd2(a&0x7F));
				a = bin2bcd2(a&0x7F);	if(a<59) a += 1;
				writeRTC(RTC_SECOND, a);
				ds_fault=0;
			}
		}
	}
	if(smh){
		readAllRTC();
		time_copy(year, month, day, hour, min, sec);
//		msg16(50,14,BLK,top_str);
	}
}


void out_time(void){
	readAllRTC();
	time_copy(year, month, day, hour, min, sec);
//	msg16(50,14,BLK,top_str);
}

u8 bcd2bin2(u8 ch){
u8 a;
	a =	ch>>4; ch &= 0xf;
	return((a*10)+ch);
}
//BINARY�� 2�ڸ��� BCD CODE�� �ٲ۴�.
u8 bin2bcd2(u8 bin){
    bin %= 100;
    bin = ((bin/10)<<4) + (bin%10);
    return(bin);
}

///////////////////////////////rtc store
void sendAll_RTC(void){
	year  = tempyear;
	month = tempmonth;
	day	  =	tempday;
	hour  =	temphour;
	min	  =	tempmin;
	sec	  =	tempsec;
	writeRTC(RTC_SECOND, bin2bcd2(sec));
	writeRTC(RTC_MINUTE, bin2bcd2(min));
	writeRTC(RTC_HOUR,	 bin2bcd2(hour));
	writeRTC(RTC_DAY,	 bin2bcd2(day));
	writeRTC(RTC_MONTH,	 bin2bcd2(month));
	writeRTC(RTC_YEAR,	 bin2bcd2(year));
}
void test_rtc_init(void){
//	initRTC();
	setyear=19;
	setmonth=11;
	setday=12;
	sethour=13;
	setmin=19;
	setsec=20;
	sendAll_RTC();
}
void test_rtc(void){
	year =readRTC(RTC_YEAR);				//BCDȮ�ιٶ�.
	month=bcd2bin2(readRTC(RTC_MONTH));
	day	 =bcd2bin2(readRTC(RTC_DAY));
	hour =bcd2bin2(readRTC(RTC_HOUR));
	min	 =bcd2bin2(readRTC(RTC_MINUTE));
	sec	 =bcd2bin2(readRTC(RTC_SECOND));

	printf0("\r%d-Year ", year);
	printf0("%d-Month ", month);
	printf0("%d-Dat ", day);
	printf0("%d-Hour ", hour);
	printf0("%d-Min ", min);
	printf0("%d-Sec", sec);
}

/**************************************************************
**************************   F25L16PA   ************************
***************************************************************/

void sendFR(u8 cmd);
void delay_FR(u16 dly);
void sendWRSR(void);
void writeFR(u32 add, u8 wd);
void eraseFR(u32 add);
u8 readFR(u32 add);

void eraseFR4K(u32 add);
short blankFR4K(u32 add);
void read_stat(void);
void writeFR256(u32 add);

u8 Fstat;
u8 buff256[0x100];

//====================================================================
void test_flash(u8 tno){
u32 i;
u16 j;
	printf0("\r FlashMemoryTest");
	read_jedec();

	i = SYS_TEST_ADR;
	eraseFR(i);
	for(j=0;j<0x100;j++) buff256[j] = j+0xA0;	
	writeFR256(i);	
	for(j=0;j<0x100;j++){ if(readFR(i+j)!=((j+0xA0)&0xFF)) break; }

	if(j<0x100){			
		printf0("\r Flash Memory Error.1     ");
		for(j=0;j<0x100;j++){ 
			printf0(" %2x", readFR(i+j) );
			delay_ms(50);
		}
		for(j=0;j<tno;j++){
//			key_buzzer();
			delay_ms(300);
		}
	}else{
		printf0(" Pass First.");
		eraseFR(i);
		do{ read_stat(); }while(Fstat&0x01);
		for(j=0;j<0x100;j++) buff256[j] = j+0x40;	
		writeFR256(i);	

		for(j=0;j<0x100;j++){ if(readFR(i+j)!=((j+0x40)&0xFF)) break; }
		if(j<0x100){			
			printf0("\r Flash Memory Error.2   ");
			for(j=0;j<0x100;j++){ 
				printf0(" %2x", readFR(i+j) );
				delay_ms(50);
			}
			for(j=0;j<tno;j++){
//				key_buzzer();
				delay_ms(300);
			}
		}else{
			printf0(" FlashTestOK!!!");	
		}
	}
/************************************************	
//	printf0("\r erase SYSTEM_RAM");	
//	eraseFR(SYSTEM_RAM);
//	delay_ms(100);
//	printf0("\r erase POSITION_RAM");	
//	eraseFR(POSITION_RAM);
//	delay_ms(100);
//	printf0("\r erase STRIP_INFO_RAM");	
//	eraseFR(STRIP_INFO_RAM);
//	delay_ms(100);
//	printf0("\r erase PARA_RAM");	
//	eraseFR(PARA_RAM);
//	delay_ms(100);
//	printf0("\r erase PFORM_RAM");	
//	eraseFR(PFORM_RAM);
************************************************/
}

//====================================================================
void delay_FR(u16 dly){
	dly*=3;
	while(dly) dly--;
}
//====================================================================
void send_ck(void){	CK24_HI(); CK24_HI(); CK24_LO(); CK24_LO(); }
//====================================================================
void sendFR(u8 cmd){
	if(cmd&0x80) DI24_HI(); else DI24_LO(); send_ck();
	if(cmd&0x40) DI24_HI(); else DI24_LO(); send_ck();
	if(cmd&0x20) DI24_HI(); else DI24_LO(); send_ck();
	if(cmd&0x10) DI24_HI(); else DI24_LO(); send_ck();
	if(cmd&0x08) DI24_HI(); else DI24_LO(); send_ck();
	if(cmd&0x04) DI24_HI(); else DI24_LO(); send_ck();
	if(cmd&0x02) DI24_HI(); else DI24_LO(); send_ck();
	if(cmd&0x01) DI24_HI(); else DI24_LO(); send_ck();
}

//====================================================================
void sendWRSR(void){
	CE24_HI();
	CK24_LO();
	WP24_HI();
	delay_FR(10);

//	CE24_LO();	
//	sendFR(FWREN);
//	CE24_HI();
//	delay_FR(10);

	CE24_LO();	
	sendFR(FEWSR);
	CE24_HI();
	delay_FR(10);
	
	CE24_LO();
	sendFR(FWRSR);
	sendFR(0x00);
	CE24_HI();
}

//====================================================================
void writeFR(u32 add, u8 wd){
	CE24_HI();
	CK24_LO();
	WP24_HI();		//WP24=1;
	delay_FR(1);
	CE24_LO();	
	delay_FR(1);
	sendFR(FWREN);
	CE24_HI();
	delay_FR(1);
	CE24_LO();	
	delay_FR(1);
	sendFR(FBTPR);
	sendFR(add>>16);
	sendFR(add>>8);
	sendFR(add>>0);
	sendFR(wd);
	delay_us(10);
	WP24_LO();		//WP24=0;
	CE24_HI();
}
//====================================================================
void eraseFR(u32 add){
u16 i;
	rtc_write();
	add &= 0x1FF000;		//BlockSize = 4KB
	for(i=0;i<3;i++){
		eraseFR4K(add);
		do{ read_stat(); }while(Fstat&0x01);
		if(blankFR4K(add)==OK) break;
	}
	if(i>=3) printf0("\r Erase NG!!!(%d)", i);
}
//====================================================================
void eraseFR4K(u32 add){
	sendWRSR();
	CE24_HI();
	CK24_LO();
	WP24_HI();		//WP24=1;
	delay_FR(1);
	CE24_LO();	
	delay_FR(1);
	sendFR(FWREN);
	CE24_HI();
	delay_FR(1);
	CE24_LO();
	delay_FR(1);
	sendFR(FERASE);
	sendFR(add>>16);
	sendFR(add>>8);
	sendFR(add>>0);
	delay_us(10);
	WP24_LO();		//WP24=0;
	CE24_HI();
}

//====================================================================
void writeFR4k(u32 adr, u16 size){
u16 i, j;
	rtc_write();
	adr &= 0x1FF000;		//BlockSize = 4KB
	eraseFR(adr);
	delay_us(10);
	if(size>0x1000) size = 0x1000;
//	printf0("\r WRT:%lx,%d",adr,size);
	for(i=0;i<size; ){
//		printf0(" %2x", flash_buff[i] );
		for(j=0;j<0x100;j++) buff256[j] = flash_buff[i+j];
		i += 0x100;
		writeFR256(adr);
		adr += 0x100;
		delay_us(10);
	}	
}

//====================================================================
u8 readFR(u32 add){
u8 e1;	
	CE24_HI();
	CK24_LO();
	WP24_LO();		//WP24=0;
	CE24_LO();	
	sendFR(FREAD);
	sendFR(add>>16);
	sendFR(add>>8);
	sendFR(add>>0);
	e1=0;
	if(DO24) e1 |= 0x80; CK24_HI(); CK24_HI(); CK24_LO(); CK24_LO(); 
	if(DO24) e1 |= 0x40; CK24_HI(); CK24_HI(); CK24_LO(); CK24_LO(); 
	if(DO24) e1 |= 0x20; CK24_HI(); CK24_HI(); CK24_LO(); CK24_LO(); 
	if(DO24) e1 |= 0x10; CK24_HI(); CK24_HI(); CK24_LO(); CK24_LO(); 
	if(DO24) e1 |= 0x08; CK24_HI(); CK24_HI(); CK24_LO(); CK24_LO(); 
	if(DO24) e1 |= 0x04; CK24_HI(); CK24_HI(); CK24_LO(); CK24_LO(); 
	if(DO24) e1 |= 0x02; CK24_HI(); CK24_HI(); CK24_LO(); CK24_LO(); 
	if(DO24) e1 |= 0x01; CK24_HI(); CK24_HI(); CK24_LO(); CK24_LO(); 
	CE24_HI();
	return(e1);
}

//====================================================================
void readFR4k(u32 add, u16 size){
u16 j;
u8 e1;
	rtc_write();
	add &= 0x1FF000;		//BlockSize = 4KB
	CE24_HI();
	CK24_LO();
	WP24_LO();				//WP24=0;
	CE24_LO();
	sendFR(FREAD);
	sendFR(add>>16);
	sendFR(add>>8);
	sendFR(0x00);
	if(size>0x1000) size = 0x1000;
	for(j=0;j<size;j++){
		e1=0;
		if(DO24) e1 |= 0x80; CK24_HI(); CK24_HI(); CK24_LO(); CK24_LO(); 
		if(DO24) e1 |= 0x40; CK24_HI(); CK24_HI(); CK24_LO(); CK24_LO(); 
		if(DO24) e1 |= 0x20; CK24_HI(); CK24_HI(); CK24_LO(); CK24_LO(); 
		if(DO24) e1 |= 0x10; CK24_HI(); CK24_HI(); CK24_LO(); CK24_LO(); 
		if(DO24) e1 |= 0x08; CK24_HI(); CK24_HI(); CK24_LO(); CK24_LO(); 
		if(DO24) e1 |= 0x04; CK24_HI(); CK24_HI(); CK24_LO(); CK24_LO(); 
		if(DO24) e1 |= 0x02; CK24_HI(); CK24_HI(); CK24_LO(); CK24_LO(); 
		if(DO24) e1 |= 0x01; CK24_HI(); CK24_HI(); CK24_LO(); CK24_LO(); 
		flash_read[j] = e1;
//		WDTR = 0xFF;        				// watchdog timer reflash
	}
	CE24_HI();
}

//====================================================================
short cmpFR4k(u32 adr, u16 size){
u16 i;
//	printf0("\r CMP:%lx,%d",adr,size);
	readFR4k(adr, size);
//	for(i=0;i<size;i++) printf0(" %2x,%2x", flash_buff[i], flash_read[i]);
	for(i=0;i<size;i++) if(flash_buff[i]!=flash_read[i]) return(NG);
	return(OK);
}

/********************************************************************************/
/*************   STRIP POSITION, PARAMETER, STRIP INFO. RESULT DATA    **********/
/********************************************************************************/

short blankFR4K(u32 add){
u16 i;
	readFR4k(add,0x1000);
	for(i=0;i<0x1000;i++){
		if(flash_read[i]!=0xFF) return(NG);
	}
	return(OK);
}

//====================================================================
void writeFR256(u32 add){
u16 j;	
	add &= 0x1FFF00;		//PageSize = 0x100
	do{ read_stat(); }while(Fstat&0x01);
	CE24_HI();
	CK24_LO();
	WP24_HI();		//WP24=1;
	delay_FR(1);
	CE24_LO();	
	delay_FR(1);
	sendFR(FWREN);
	CE24_HI();
	delay_FR(1);
//	read_stat();	printf0("\r Stat2:%2x", Fstat);
	do{ read_stat(); }while(Fstat&0x01);
//	read_stat();	printf0("\r Stat21:%2x", Fstat);
	CE24_LO();	
	delay_FR(1);
	sendFR(FBTPR);
	sendFR(add>>16);
	sendFR(add>>8);
	sendFR(add>>0);
//	sendFR(wd);
	for(j=0;j<0x100;j++){
		delay_FR(10);
		sendFR(buff256[j]);
	}
//	read_stat();	printf0("\r Stat3:%2x", Fstat);
	delay_us(10);
	do{ read_stat(); }while(Fstat&0x01);
//	read_stat();	printf0("\r Stat4:%2x", Fstat);
	WP24_LO();		//WP24=0;
	CE24_HI();
}
//====================================================================
short read_jedec(void){
u32 e1,e2;
	rtc_write();
	CE24_HI();
	CK24_LO();
	WP24_LO();
	delay_FR(10);
	CE24_LO();	
	sendFR(FJDRD);
	delay_FR(1);
	
	e1=0;
	for(e2=0;e2<24;e2++){
		e1 <<= 1;
		if(DO24) e1 |= 0x01; CK24_HI(); CK24_HI(); CK24_LO(); CK24_LO(); 		
	}
	CE24_HI();
	printf0(" ,  Jedec:%lx", e1);
	if(e1==0x8C2015){
		printf0(" [F25L]");
		return(OK);
	}else if(e1==0xBF2541){
		printf0(" [S25V]");
		return(OK);
	}else{
		printf0(" [!NG!]");
		return(NG);
	}
}

//====================================================================
void read_stat(void){
u32 e1,e2;
	CE24_HI();
	CK24_LO();
//	WP24_LO();
	delay_FR(10);
	CE24_LO();	
	sendFR(FRDSR);
	delay_FR(1);
	
	e1=0;
	for(e2=0;e2<8;e2++){
		e1 <<= 1;
		if(DO24) e1 |= 0x01; CK24_HI(); CK24_HI(); CK24_LO(); CK24_LO(); 		
	}
	CE24_HI();
//	printf0("\r Stat:%lx", e1);
	Fstat=e1;
}

//====================================================================
//====================================================================
void delay_ms(u32 dly){
	delay_150=0;
	dly = (dly*100)/15;
	while(dly>delay_150);
}
//====================================================================
void delay_us(u16 dly){
	kdelay( dly*10 ); 
}
//====================================================================
//====================================================================
//====================================================================

