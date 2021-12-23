 #define	_MEAS_C_

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "stm32f4xx.h"
#include "Common\stm324x9i_eval.h"
#include "common\32f429_lcd.h"
#include "common\32f429_port.h"
#include "util\usart.h"
#include "util\lcd.h"
#include "util\eep.h"
#include "util\para.h"
#include "com.h"
#include "sysadj.h"
#include "resm.h"
#include "touch.h"
#include "dsp.h"
#include "meas.h"

void print_save(char *fmt,...); 
u16	print_point;
#define PRINT_BUFF_SIZE			0x400
u8	print_buff[PRINT_BUFF_SIZE];

void wait_analysis(u16 wsec);
void meas_double(void);
u16  put_result1(u16 i, u8 x, u8 y);
u16  put_arb(u16 i, u8 x, u8 c);
u16 put_albumin(u16 i, u8 x);
void chg_rgb_data(void);
u8 check_double(void);
u16 put_result2(u16 i, u8 x, u8 y);



#define pntLUC			0xA
#define pntPRO			0x8
#define pntALB			0xC
#define pntCRE			0xD
#define pntCAL			0x10		
#define RGB_DATA_SRT	65
#define STRIP_DATA_SRT	30+12

extern const u16 meas_dat1[764*3];
extern const u16 meas_dat2[764*3];

extern char s_None[2][6] = { "None", "None" };
extern char s_Yellow[2][8] = { "Yellow", "Yellow" };
extern char s_DKYellow[2][11] = { "DK Yellow", "DK Yellow" };
extern char s_Straw[2][7] = { "Straw", "Straw" };
extern char s_Amber[2][7] = { "Amber", "Amber" };
extern char s_Red[2][5] = { "Red", "Red" };
extern char s_Orange[2][8] = { "Orange", "Orange" };
extern char s_Green[2][7] = { "Green", "Green" };
extern char s_Other[2][7] = { "Other", "Other" };
extern char s_Clear[2][7] = { "Clear", "Clear" };
extern char s_SL_Cloudy[2][11] = { "SL Cloudy", "SL Cloudy" };
extern char s_Cloudy[2][8] = { "Cloudy", "Cloudy" };
extern char s_Turbid[2][8] = { "Turbid", "Turbid" };

//#define NO_MOTOR
//#define OUT_RCV


void draw_graph(void);
#define XOFF_GRAPH	10
#define YOFF_GRAPH	0
void draw_graph(void){
u16 i, j;
	LCD_Clear(GRY_LCD);
	for(i=0;i<MaxRawData;i++){
		j = buffRED1[i]/7;	if(j>479) j = 479;
		line_x(XOFF_GRAPH+i, YOFF_GRAPH+(479-j), 2, 2, RED_LCD);
		j = buffGRN1[i]/7;	if(j>479) j = 479;
		line_x(XOFF_GRAPH+i, YOFF_GRAPH+(479-j), 2, 2, GRN_LCD);
		j = buffBLU1[i]/7;	if(j>479) j = 479;
		line_x(XOFF_GRAPH+i, YOFF_GRAPH+(479-j), 2, 2, BLU_LCD);
	}
}

/*******************************************************************************/
#define IS_STRIP	120	//210607
void run_measure(void){
u16 a, msb;
u16	r_pnt, g_pnt, b_pnt, i;
u8 j, k=0;
#ifdef OUT_RCV
	u16	tp=0;
	u8 tstbuf[4584];
#endif
	//send_meas_step('1');
	for(i=0;i<MaxRawData;i++) buffRED1[i] = buffGRN1[i] = buffBLU1[i] = 0x00;
	ErrCode = NO_ERROR;
	if(inputCnt)inputCnt--;
	savcmd1(RUN_CMD); 	rxw1 = rxr1 = 0;
	delay_us(200);	savcmd1(RUN_CMD); 	
	delay_us(200);	savcmd1(RUN_CMD); 	
	r_pnt = g_pnt = b_pnt = 0;
	msb = 0xFF;
	a = 0;												//for NO_MOTOR
	while(motor_wait<MOTOR_DLY){
#ifndef NO_MOTOR		
		if(rxw1!=rxr1){
			a = getcmd1();
	#ifdef OUT_RCV
			if(tp<4584) tstbuf[tp++] = a;
	#endif
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
		delay_us(200);
		buffRED1[r_pnt++] = meas_dat1[a++];
		buffGRN1[g_pnt++] = meas_dat1[a++];
		buffBLU1[b_pnt++] = meas_dat1[a++];
#endif
		if((r_pnt>=MaxRawData)||(g_pnt>=MaxRawData)||(r_pnt>=MaxRawData)) break;
	}
	//send_meas_step('2');												// ������ ������ ��������.
	printf0("\r End measure : %d-%d-%d", r_pnt, g_pnt, b_pnt);
	if(motor_wait>=MOTOR_DLY){
		printf0("\t Time Over!!!");
		ErrCode = TIME_OVER;
	}else{
		if(r_pnt<MaxRawData) for(i=r_pnt;i<MaxRawData;i++) buffRED1[i] = buffRED1[i-1]; 
		if(g_pnt<MaxRawData) for(i=g_pnt;i<MaxRawData;i++) buffGRN1[i] = buffGRN1[i-1]; 
		if(b_pnt<MaxRawData) for(i=b_pnt;i<MaxRawData;i++) buffBLU1[i] = buffBLU1[i-1]; 
	}
#ifdef OUT_RCV
	draw_graph();
//	for(tp=0;tp<4584;tp++){	if(!(tp%6)) printf0("\r");	printf0("%2x,", tstbuf[tp]); delay_ms(20);	}
	for(tp=0;tp<MaxRawData;tp++){
		if(!(tp%5)) printf0("\r");
		printf0("%3x,", buffRED1[tp]);		delay_ms(10);
		printf0("%3x,", buffGRN1[tp]);		delay_ms(10);
		printf0("%3x,", buffBLU1[tp]);		delay_ms(10);
	}
	while(1);
#endif
	if(measCnt>maxTempCnt)maxTempCnt = measCnt;
	//201113S

	STRIP_NO = temp_stripno[measCnt];
	get_strip_name(STRIP_NO);
	move_strip_info();
	move_position();
	span_curve();					//multi3
	correction_curve();
	get_pad_data();
	calc_ratio2();					//if(meas_type) calc_ratio2();	else calc_ratio1();	
	conv_result();
	//send_meas_step('3');

	MEASyear = year;
	MEASmonth = month;
	MEASday = day;
	MEAShour = hour;
	MEASmin = min;
	MEASsec = sec;
	for(j=0;j<MAX_OPERID+1;j++)MEASoperid[j] = OPER_ID[j];
	for(j=0;j<STRIP_NAME_SIZE+1;j++)MEASstripname[j]=strip_name[j]=buff_strip_name[j];

	store_COLOR_NO=COLOR_NO=temp_colorno[measCnt];		// for print
	store_TURBI_NO=TURBI_NO=temp_turbino[measCnt];		// for print

		for(j=0;j<STRIP_INFO[0];j++){			
			if(dataRED1[j]>=IS_STRIP) k++;
			if(dataGRN1[j]>=IS_STRIP) k++;
			if(dataBLU1[j]>=IS_STRIP) k++;	
		}
		if(k==0){
			ErrCode = NO_STRIP;
			AddError(Err_NoStrip);
		}

	if(modex != MODE_RESULT_NOSTRIP)temp_modex = modex;
	if((modex&0xFF00)==MODE_QC){
		for(j=0;j<MAX_LOTNO;j++)MEASuserid[j] = temp_lotno[measCnt][j];
		fill_space(MEASuserid, 8);
		if (NEGPOS == 0){
			MEASuserid[8] = 'N';
			MEASuserid[9] = 'E';
			MEASuserid[10] = 'G';
			MEASuserid[11] = 'A';
			MEASuserid[12] = 'T';
			MEASuserid[13] = 'I';
			MEASuserid[14] = 'V';
			MEASuserid[15] = 'E';
		}
		else{
			MEASuserid[8] = 'P';
			MEASuserid[9] = 'O';
			MEASuserid[10] = 'S';
			MEASuserid[11] = 'I';
			MEASuserid[12] = 'T';
			MEASuserid[13] = 'I';
			MEASuserid[14] = 'V';
			MEASuserid[15] = 'E';
		}
		store_result(temp_seqno[measCnt]);	//Store Result
	}else{
		for (a = 0; a < MAX_USERID; a++){
			if(tempSampleID[measCnt][a] != NULL){
				MEASuserid[a] = tempSampleID[measCnt][a];
			}
			else{
				for(a=a;a<MAX_USERID;a++){
					MEASuserid[a] = NULL;
				}
			}
		}
		store_result(temp_seqno[measCnt]);	//Store Result
		add_los_test(temp_seqno[measCnt]);//200623

	}
	restore_result();

	MEASseqno = temp_seqno[measCnt];

	if(read_result(MEASseqno, 1) == OK) {					//result data ==> cmdbuff
		make_result();
		conv_result_store();	
		if(SERIAL_FLAG==1) send_result();
		else if (SERIAL_FLAG == 2) Send720();
		else if(SERIAL_FLAG != 0)SendYD(SERIAL_FLAG);
	}
	for (j = 0; j < PRINT_FLAG; j++) {
		print_result();
	}

	//if(!errorCnt)view_result_store(temp_seqno[measCnt]);//dsp_result_store();
	if((modex == MODE_TEST || modex == MODE_QC) && !errorCnt)view_result_store(temp_seqno[measCnt]);
	
	if(modex == MODE_QC || modex == MODE_TEST)dsp_seq();
	IncreseSampleID();
	measCnt++;
	if(tempMeasCnt < measCnt)tempMeasCnt++;
	if(measCnt>5000)measCnt = 0;
	if(tempMeasCnt > maxTempCnt)maxTempCnt = tempMeasCnt;
	add_index();
	if((modex&0xFF00)==MODE_QC){
		
		if(temp_seqno[measCnt]>END_QC_NO-10){
			key_buzzerF();
			tch_error();
		}
		if(QC_NO == END_QC_NO){
			QC_NO = END_QC_NO+1;		//210713 END
			returnFlag = 0;
			store_vars(QC_NO_ADR, QC_NO);
			if(modex != MODE_TEST_Error && modex != MODE_TEST_Error2 && modex != MODE_QC_Error && modex != MODE_QC_Error2){
				temp_modex2[errorCnt] = modex;
			}else{
				temp_modex2[errorCnt] = temp_modex2[errorCnt-1];
			}
			errorStack[errorCnt++] = Err_MemoryFull;			
			errorSwitch = 1;
		}
		else if(QC_NO < END_QC_NO){
			QC_NO = temp_seqno[measCnt];
			NEGPOS = temp_negpos[measCnt];
			store_vars(QC_NO_ADR, QC_NO);
			for(j=0;j<MAX_LOTNO;j++)LOT_NO[j]=temp_lotno[measCnt][j];
		}
	}else{
		
		if(temp_seqno[measCnt]>MAX_RECORD-50){
			key_buzzerF();
			tch_error();
		}
		if(SEQ_NO == END_SEQ_NO){
			SEQ_NO = END_SEQ_NO+1;		//210713 END
			returnFlag = 0;
			store_seqno();
			AddError(Err_MemoryFull);
		}
		else if(SEQ_NO < END_SEQ_NO){
			SEQ_NO = temp_seqno[measCnt];
			store_seqno();
		}
	}

	clr_user_id();
	key_clear();
	com_clear();
	send_meas_step('6');
	if(measCnt>maxTempCnt)barcodeFlag = 0;
	if(modex == MODE_TEST || modex == MODE_QC)dsp_los_table();
	if(++stripCnt >= StripCount && !trayFullFlag){
		trayFullFlag = 1;
		AddError(Err_WasteFull);
	}
	if(returnFlag)returnFlag--;
	//211116 Start
	STRIP_NO = read24w(STRIPNO_ADR);
	get_strip_name(STRIP_NO);
	move_strip_info();
	//211116 End
}
/*******************************************************************************/
void span_curve(void){
u32 i, j, k;
	for(i=0;i<5;i++){
		buffRED1[MaxRawData+i] = buffRED1[MaxRawData-1];
		buffGRN1[MaxRawData+i] = buffGRN1[MaxRawData-1];
		buffBLU1[MaxRawData+i] = buffBLU1[MaxRawData-1];
	}
	for(i=0;i<MaxRawData;i++){
		k=0; for(j=0;j<5;j++) k += buffRED1[i+j]; spanRED[i] = (k*3)/5;
		k=0; for(j=0;j<5;j++) k += buffGRN1[i+j]; spanGRN[i] = (k*3)/5;
		k=0; for(j=0;j<5;j++) k += buffBLU1[i+j]; spanBLU[i] = (k*3)/5;
	}
}

/*******************************************************************************/
void get_pad_data(void){
u16 i, j;
	for(i=0;i<MAX_PAD_NO;i++){				//STRIP_INFO[0]
		j= StripPosition[i+2];
		dataRED1[i] = spanRED[j];
		dataGRN1[i] = spanGRN[j];
		dataBLU1[i] = spanBLU[j];
	}
}
/*******************************************************************************/
/*******************************************************************************/
#define MAX_DEGREE		9
void calc_ratio2(void){
u8  degree;
u8  item;
u16 temp;
u8  i;
	for(i=0; i<MAX_PAD_NO; i++ ) m_final_results[i] = 0;
	for(i=0; i < STRIP_INFO[0]; i++ ){
		item=STRIP_INFO[i+1]-1;
		for(degree=1 ; degree <= MAX_DEGREE ; degree++)		// 1~9
		{
			if(PARA_NEW[item][0]==1){
				m_para_max =PARA_NEW[item][degree];
				m_para_min =PARA_NEW[item][degree+1];
				//printf0("\r\n %d:Up %d %d", item, m_para_min, m_para_max);

			}else{
				m_para_min =PARA_NEW[item][degree];
				m_para_max =PARA_NEW[item][degree+1];
				//printf0("\r\n %d:Down %d %d ", item, m_para_min, m_para_max);

			}
			if(weight[item]==1){					
				temp = dataRED1[i];
			}else if(weight[item]==2){				
				temp = dataGRN1[i];
			}else{
				temp = dataBLU1[i];
			}
			if((temp<=m_para_max)&&(temp>=m_para_min)) break;
		}  
		m_final_results[i] = degree-1;		//result;
	}
	printf0("\r Result2: ");
	for(item=0;item<STRIP_INFO[0];item++){
		printf0("%2x,", m_final_results[item]);
	}
}
/*******************************************************************************/
void send_rawdata(void){
u16 i, k;
	printf0("\r Send Rawdata\n");
	savcmd3('*');	
	savcmd3('N');
	for(i=0;i<MaxRawData;i++){	k = spanRED[i];	savcmd3(k>>8);	savcmd3(k>>0);	}
	for(i=0;i<MaxRawData;i++){	k = spanGRN[i];	savcmd3(k>>8);	savcmd3(k>>0);	}
	for(i=0;i<MaxRawData;i++){	k = spanBLU[i];	savcmd3(k>>8);	savcmd3(k>>0);	}
	savcmd3(URLF);	
}
/*******************************************************************************/
void send_printdata(void){
u16 i;
	printf0("\r Send Printdata\n");
	for(i=0;i<print_point;i++){
		savcmd3(print_buff[i]);
//		if((print_buff[i]==0x0d)||(print_buff[i]==0x0a)) delay_ms(10);
	}			
}
/*******************************************************************************/
void send_result(void){
u16 i;
	//StoreResult�ÿ� cmdbuff�� data�� ������ �ξ���.
	printf0("\r Send Result\n");
	//restore_result();
#ifdef TEST_RESULT
	for(i=0;i<RESULT_SIZE_ORG;i++){ 
		if(cmdbuff[i]==0) savcmd0(0x20);
		else savcmd0(cmdbuff[i]); 			//wait_tx3();
	}
#else
	for(i=0;i<RESULT_SIZE_ORG;i++){ 
		if(cmdbuff[i]==0xff) savcmd3(0x20);
		else if(cmdbuff[i]==0) savcmd3(0x20);
		else savcmd3(cmdbuff[i]); 
		wait_tx3(); 
//		delay_ms(50);
	}
#endif
	savcmd3('\n');
}	

/*******************************************************************************/
void wait_analysis(u16 sec){
	printf0("\r Stop Motor3");
//	RGB_ALL_OFF();
}
/*******************************************************************************/
u16 put_albumin(u16 i, u8 x){
u8 j;	
printf0("\r Put abnormal:%d", x);
	for(j=0;j<32;j++) buffRESULT[i++]=albumin_tb[x][j];
	buffRESULT[i++]=0x0d;
	buffRESULT[i++]=0x0a;
	tb_acr = x;
	return(i);
}
/*******************************************************************************/
void conv_result_store(void){
u16 i, j, item;	
u8 x, y;
	for(i=0;i<1024;i++)buffRESULT[i] = ' ';
	for(i=0;i<20;i++)temp_result[i] = 0;
	printf0("\r Final Result(conv result)"); wait_tx0();
	for(item=0;item<MAX_PAD_NO+1;item++) printf0("  %d", MEASstripinfo[item]);
	i=0;
	datPRO = datALB = datCRE = datCAL = -1;
	for(item=0; item < MEASstripinfo[0] ; item++ ){
		x = MEASstripinfo[item+1];
		y = MEAS_results[item];
		buffRESULT[i++]=' ';
		if(normal_range[x][y]==0){
			for(j=0;j<8;j++) buffRESULT[i++] = item_string[MEASstripinfo[item+1]][j];
			temp_result[item] = 0;
		}else{
			for(j=0;j<8;j++) buffRESULT[i++] = item_string1[MEASstripinfo[item+1]][j];
			if (/*PLUS_FLAG*/MARK_FLAG) {
				temp_result[item] = 1;
			}
			else {
				temp_result[item] = 0;
			}
		}			
		for(j=0;j<4;j++) buffRESULT[i++]=' ';
//		x = MEASstripinfo[item+1]; y=MEAS_results[item]+1;		//200527
		x = MEASstripinfo[item+1]; 
		y = MEAS_results[item]+1;
		i = put_arb(i, x, y);
		i = put_result1(i, x, y);
		if(x==pntPRO) datPRO = y;
		else if(x==pntALB) datALB = y;
		else if(x==pntCRE) datCRE = y;
		else if(x==pntCAL) datCAL = y;

		if(((MEAS_results[item]>0)&&(MEAS_results[item]<7))
							||(x==pntALB)||(x==pntCRE)||(x==pntCAL)){
			x = MEASstripinfo[item+1];
			i = put_result2(i, x, 0);
		}else{
			for(j=0;j<8;j++) buffRESULT[i++]=' ';
		}
		for(j=0;j<3;j++) buffRESULT[i++]=' ';
		buffRESULT[i++]=0x0d;
		buffRESULT[i++]=0x0a;
	}	
printf0("\r C:%d, A:%d, P:%d", datCRE, datALB, datPRO );
printf0(" L:%d", datCAL ); wait_tx0();
	if((datCRE>=0)&&(datALB>=0)){
		for(j=0;j<4;j++) buffRESULT[i++]=' ';
		for(j=0;j<5;j++) buffRESULT[i++]=text_AC[j];
		for(j=0;j<4;j++) buffRESULT[i++]=' ';
		x = alb_item[datCRE-1][datALB-1];
		if(!x) y = 4 * 8;								//retest
		else y = x * 8;
		i = put_arb( i, ACpnt, y/8 );
		//i = put_result1( i, ACpnt, y/8 );
		if (UNIT_FLAG == UNIT_SI) {
			for (j = 0; j < 8; j++) buffRESULT[i++] = alb_tb_text_si[datCRE - 1][datALB][j];
			for (j = 0; j < 8; j++) buffRESULT[i++] = alb_tb_text_si[datCRE - 1][0][j];
		}
		else {
			for (j = 0; j < 8; j++) buffRESULT[i++] = alb_tb_text[datCRE - 1][datALB][j];
			for (j = 0; j < 8; j++) buffRESULT[i++] = alb_tb_text[datCRE - 1][0][j];
		}
		//if(x) i = put_result2( i, 18, 0);
		//else for(j=0;j<8;j++) buffRESULT[i++]=' ';
		for(j=0;j<3;j++) buffRESULT[i++]=' ';
		buffRESULT[i++]=0x0d;
		buffRESULT[i++]=0x0a;
		for(j=0;j<8;j++) buffRESULT[i++]=' ';
		i = put_albumin(i, x);		
	}else if((datCRE>=0)&&(datPRO>=0)){
		for(j=0;j<4;j++) buffRESULT[i++]=' ';
		for(j=0;j<5;j++) buffRESULT[i++]=text_PC[j];
		for(j=0;j<4;j++) buffRESULT[i++]=' ';
		x = pro_item[datPRO-1][datCRE-1];
		if(!x) y = 4 * 8;		//retest
		else y = x * 8;
		i = put_arb( i, PCpnt, y/8 );
		i = put_result1( i, PCpnt, y/8 );
		for(j=0;j<3;j++) buffRESULT[i++]=' ';
		buffRESULT[i++]=0x0d;
		buffRESULT[i++]=0x0a;
		for(j=0;j<8;j++) buffRESULT[i++]=' ';
		i = put_albumin(i, x);		
	}
	if((datCRE>=0)&&(datCAL>=0)){
		for(x=0;x<4;x++) buffRESULT[i++]=' ';
		for(x=0;x<5;x++) buffRESULT[i++]=text_CC[j];
		for(x=0;x<4;x++) buffRESULT[i++]=' ';
		x = alb_item[datCRE-1][datCAL-1];
		if(!x) y = 4 * 8;		//retest
		else y = x * 8;
		i = put_arb( i, CCpnt, y/8 );
		i = put_result1( i, CCpnt, y/8 );
		if(x) i = put_result1( i, 14, 0 );
		else for(x=0;x<8;x++) buffRESULT[i++]=' ';
		for(x=0;x<3;x++) buffRESULT[i++]=' ';
		buffRESULT[i++]=0x0d;
		buffRESULT[i++]=0x0a;
		for(y=0;y<8;y++) buffRESULT[i++]=' ';
		i = put_albumin(i, x);		
	}
//������ ����ϳ�?	
	END_RESULT = i;
	buffRESULT[i++]=0;
	printf0("\r view conv_result\r");
	for(i=0;i<END_RESULT;i++) savcmd0(buffRESULT[i]);
}

/*******************************************************************************/
/*******************************************************************************/
/*******************************************************************************/
void conv_result(void){
u16 i, j, item;	
u8 x, y;
	for(i=0;i<MAX_buffRESULT;i++) buffRESULT[i] = ' ';
	printf0("\r Final Result(conv result)"); wait_tx0();
	for(item=0;item<MAX_PAD_NO+1;item++) printf0("  %d", STRIP_INFO[item]); wait_tx0();
	i=0;
	datPRO = datALB = datCRE = datCAL = -1;
	for(item=0; item < STRIP_INFO[0] ; item++ ){
		x = STRIP_INFO[item+1];
		y = m_final_results[item];
		buffRESULT[i++]=' ';
		if(normal_range[x][y]==0){
			for(j=0;j<8;j++) buffRESULT[i++] = item_string[STRIP_INFO[item+1]][j];
			temp_result[item] = 0;
		}else{
			for(j=0;j<8;j++) buffRESULT[i++] = item_string1[STRIP_INFO[item+1]][j];
			if (MARK_FLAG) {
				temp_result[item] = 1;
			}
			else {
				temp_result[item] = 0;
			}
		}			
		for(j=0;j<4;j++) buffRESULT[i++]=' ';
//		x = STRIP_INFO[item+1]; y=m_final_results[item]/*+1*/;//200515 200527
		x = STRIP_INFO[item+1]; 
		y = m_final_results[item]+1;
		i = put_arb(i, x, y);
		i = put_result1(i, x, y);
		if(x==pntPRO) datPRO = y;
		else if(x==pntALB) datALB = y;
		else if(x==pntCRE) datCRE = y;
		else if(x==pntCAL) datCAL = y;

		if(((m_final_results[item]>0)&&(m_final_results[item]<7))
							||(x==pntALB)||(x==pntCRE)||(x==pntCAL)){
			x = STRIP_INFO[item+1];
			i = put_result2(i, x, 0);
		}else{
			for(j=0;j<8;j++) buffRESULT[i++]=' ';
		}
		for(j=0;j<3;j++) buffRESULT[i++]=' ';
		buffRESULT[i++]=0x0d;
		buffRESULT[i++]=0x0a;
	}	
printf0("\r C:%d, A:%d, P:%d", datCRE, datALB, datPRO );
printf0(" L:%d", datCAL ); wait_tx0();
	if((datCRE>=0)&&(datALB>=0)){
		for(j=0;j<4;j++) buffRESULT[i++]=' ';
		for(j=0;j<5;j++) buffRESULT[i++]=text_AC[j];
		for(j=0;j<4;j++) buffRESULT[i++]=' ';
		x = alb_item[datCRE-1][datALB-1];
		if(!x) y = 4 * 8;		//retest
		else y = x * 8;
		i = put_arb( i, ACpnt, y/8 );
//		i = put_result1( i, ACpnt, y/8 );
//		if(x) i = put_result2( i, 18, 0 );
//		else for (x = 0; x < 8; x++) buffRESULT[i++] = ' ';
		if (UNIT_FLAG == UNIT_SI) {
			for (j = 0; j < 8; j++) buffRESULT[i++] = alb_tb_text_si[datCRE - 1][datALB][j];
			for (j = 0; j < 8; j++) buffRESULT[i++] = alb_tb_text_si[datCRE - 1][0][j];
		}
		else {
			for (j = 0; j < 8; j++) buffRESULT[i++] = alb_tb_text[datCRE - 1][datALB][j];
			for (j = 0; j < 8; j++) buffRESULT[i++] = alb_tb_text[datCRE - 1][0][j];
		}
		for(j=0;j<3;j++) buffRESULT[i++]=' ';
		buffRESULT[i++]=0x0d;
		buffRESULT[i++]=0x0a;
		for(j=0;j<8;j++) buffRESULT[i++]=' ';
		printf0("\n x:%d %d %d\n", datALB, datCRE, x);
		i = put_albumin(i, x);		
	}else if((datCRE>=0)&&(datPRO>=0)){
		for(j=0;j<4;j++) buffRESULT[i++]=' ';
		for(j=0;j<5;j++) buffRESULT[i++]=text_PC[j];
		for(j=0;j<4;j++) buffRESULT[i++]=' ';
		x = pro_item[datPRO-1][datCRE-1];
		if(!x) y = 4 * 8;		//retest
		else y = x * 8;
		i = put_arb( i, PCpnt, y/8 );
		i = put_result1( i, PCpnt, y/8 );
		for(j=0;j<3;j++) buffRESULT[i++]=' ';
		buffRESULT[i++]=0x0d;
		buffRESULT[i++]=0x0a;
		for(j=0;j<8;j++) buffRESULT[i++]=' ';
		i = put_albumin(i, x);		
	}
	if((datCRE>=0)&&(datCAL>=0)){
		for(j=0;j<4;j++) buffRESULT[i++]=' ';
		for(j=0;j<5;j++) buffRESULT[i++]=text_CC[j];
		for(j=0;j<4;j++) buffRESULT[i++]=' ';
		x = alb_item[datCRE-1][datCAL-1];
		if(!x) y = 4 * 8;		//retest
		else y = x * 8;
		i = put_arb( i, CCpnt, y/8 );
		i = put_result1( i, CCpnt, y/8 );
		if(x) i = put_result1( i, 14, 0 );
		else for(j=0;j<8;j++) buffRESULT[i++]=' ';
		for(j=0;j<3;j++) buffRESULT[i++]=' ';
		buffRESULT[i++]=0x0d;
		buffRESULT[i++]=0x0a;
		for (j=0;j<8;j ++) buffRESULT[i++]=' ';
		i = put_albumin(i, x);		
	} 
//������ ����ϳ�?	
	END_RESULT = i;
	buffRESULT[i++]=0;
	
	printf0("\r conv_result\r");
	for(i=0;i<END_RESULT;i++) savcmd0(buffRESULT[i]);
}

/*******************************************************************************/
u16 put_result1(u16 i, u8 x, u8 y){
u16 j;
	if(UNIT_FLAG==UNIT_SI){
		for(j=0;j<8;j++) buffRESULT[i++]=si_item_ram[x][y][j];
	}else if(UNIT_FLAG==UNIT_CONV){
		for(j=0;j<8;j++) buffRESULT[i++]=co_item_ram[x][y][j];
	}else{
		for(j=0;j<8;j++) buffRESULT[i++]=ar_item_ram[x][y][j];
	}
	return(i);
}
/*******************************************************************************/
u16 put_result2(u16 i, u8 x, u8 y){
u16 j;
	if(UNIT_FLAG==UNIT_SI){
		for(j=0;j<8;j++) buffRESULT[i++]=si_item_ram[x][y][j+3];
	}else if(UNIT_FLAG==UNIT_CONV){
		for(j=0;j<8;j++) buffRESULT[i++]=co_item_ram[x][y][j+3];
	}else{
		for(j=0;j<8;j++) buffRESULT[i++]=ar_item_ram[x][y][j+3];
	}
	return(i);
}
/*******************************************************************************/
u16  put_arb(u16 i, u8 x, u8 y){
u16 j;
	for(j=0;j<8;j++) buffRESULT[i++]=ar_item_ram[x][y][j];
	return(i);
}
/*******************************************************************************/
/******************* ������� ����ϱ� ���� ����Ÿ ���� ************************/
/*******************************************************************************/
void print_save(char *fmt,...){
char str[64];
u16 i = 0;
	va_list ap;
    va_start(ap,fmt);
    vsprintf(str,fmt,ap);
//	Uart_Send2(str);
	while(str[i]){
		if(print_point>=PRINT_BUFF_SIZE) break;
		print_buff[print_point++] = str[i++];
	}
	va_end(ap);
}
/*******************************************************************************/
void print_result(void){
u32 w1;
u16  b1;
	print_point = 0;
//	print_sound();
	print_thermal("\n");					//Line0
	print_save("\n");
	strcpy(charbuff,"= = = = SEQ# : 0000  = = = = = =");
	w1 = bin2bcdasc4(MEASseqno);				//w1 = bin2bcdasc4(SEQ_NO);
	charbuff[15] = w1 >> 24;
	charbuff[16] = w1>>16;
	charbuff[17] = w1>>8;
	charbuff[18] = w1>>0;
	print_thermal("\n%s", charbuff);
	print_save("\n%s", charbuff);
	if (datePresentation == 0)
	{
		if (timePresentation == 0)
		{ 
			strcpy(charbuff, " 2000/01/01          AM 00:00:00");
			w1 = bin2bcdasc4(MEASyear);		charbuff[3] = w1 >> 8;	charbuff[4] = w1 >> 0;
			w1 = dateDelimiter;				charbuff[5] = w1;
			w1 = bin2bcdasc4(MEASmonth);	charbuff[6] = w1 >> 8;	charbuff[7] = w1 >> 0;
			w1 = dateDelimiter;				charbuff[8] = w1;
			w1 = bin2bcdasc4(MEASday);		charbuff[9] = w1 >> 8;	charbuff[10] = w1 >> 0;
			if (!MEAShour) b1 = 12;
			else if (MEAShour >= 12) {
				charbuff[21] = 'P';
				if (MEAShour > 12){
					b1 = MEAShour - 12;
				}else{
					b1 = MEAShour;
				}
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
			strcpy(charbuff, " 2000/01/01           00:00:00");
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
			strcpy(charbuff, " 01/01/2000          AM 00:00:00");
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
			strcpy(charbuff, " 01/01/2000           00:00:00");
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
	print_thermal("\n%s", charbuff);
	print_save("\n%s", charbuff);	
	strcpy(charbuff, "                ");
	for(w1=0;w1<11;w1++) charbuff[w1+1] = MEASstripname[w1];
	print_thermal("\n%s", charbuff);	
	print_save("\n%s", charbuff);	
	strcpy(charbuff, " ID:                            ");
	if(MEASseqno>END_SEQ_NO){
		for(w1=0;w1<MAX_LOTNO;w1++) {
			if(MEASuserid[w1]==NULL){
				charbuff[w1+5] = ' ';
			}else{
				charbuff[w1+5] = MEASuserid[w1];
			}
		}
		charbuff[w1+5] = '-';
		for(w1=0;w1<8;w1++){
			charbuff[w1+5+MAX_LOTNO+1] = MEASuserid[w1+MAX_LOTNO];
		}
		charbuff[w1+5+MAX_LOTNO+1] = 0;
	}
	else{
		for(w1=0;w1<MAX_USERID;w1++) charbuff[w1+5] = MEASuserid[w1];
		charbuff[w1+5] = 0;
	}

	print_thermal("\n%s", charbuff);	
	print_save("\n%s", charbuff);	
//	print_thermal("\n");					//Line5
//	print_save("\n");						//Line5
	print_thermal("\n%s", "= = = = = = = = = = = = = = = = ");	
	print_save("\n%s", "= = = = = = = = = = = = = = = = ");	
	print_thermal("\n");					//Start Data
	print_save("\n");						//Start Data
#ifdef TEST_PRINT
	b1=0;
	for(w1=0;w1<END_RESULT;w1++){
		if(pt_size[b1]==1) savcmd0(buffRESULT[w1]);
		if((++b1)>=42) b1=0;
	}
#else
	b1=0;
	if (store_ErrCode == NO_STRIP) {
		print_thermal("\n%s\n\n", "            No Strip            ");
		print_save("\n%s\n\n", "            No Strip            ");
	}
	else {
		for (w1 = 0; w1 < END_RESULT; w1++) {
			if (pt_size[b1] == 1) {
				savcmd_print(buffRESULT[w1]);
				print_buff[print_point++] = buffRESULT[w1];
			}
			if ((++b1) >= 42) b1 = 0;
		}
	}
#endif
	print_thermal("%s", "= = = = = = = = = = = = = = = = ");	
	print_save("%s", "= = = = = = = = = = = = = = = = ");	
	print_thermal("\n Color : ");
	print_save("\n Color : ");
	switch(store_COLOR_NO){
		case 0:	print_thermal("NONE");		print_save("NONE");			break; //1	
		case 1: print_thermal("YELLOW");	print_save("YELLOW");		break; //2	
		case 2:	print_thermal("DK YELLOW");	print_save("DK YELLOW");	break; //3	
		case 3:	print_thermal("STRAW");		print_save("STRAW");		break; //4	
		case 4:	print_thermal("AMBER");		print_save("AMBER");		break; //5 
		case 5:	print_thermal("RED");		print_save("RED");			break; //6 
		case 6:	print_thermal("ORANGE");	print_save("ORANGE");		break; //7 
		case 7:	print_thermal("GREEN");		print_save("GREEN");		break; //8 
		default: print_thermal("OTHER");	 print_save("OTHER");		break; //9 
	}
	
	print_thermal("\n Turb. : ");
	print_save("\n Turb. : ");
	switch(store_TURBI_NO){
		case 0:	print_thermal("NONE");		print_save("NONE");			break; //1	
		case 1:	print_thermal("CLEAR");		print_save("CLEAR");		break; //2	
		case 2:	print_thermal("SL CLOUDY");	print_save("SL CLOUDY");	break; //3	
		case 3:	print_thermal("CLOUDY");	print_save("CLOUDY");		break; //4
		case 4:	print_thermal("TURBID");	print_save("TURBID");		break; //5 
		default: print_thermal("OTHER");	 print_save("OTHER");		break; //9 
	}
	print_thermal("\n Operator ID: %s",MEASoperid);		//??????????????
	print_save("\n Operator ID: %s",MEASoperid);		//??????????????
	print_thermal("\n S/N: %s",SYS_ID);		//??????????????
	print_save("\n S/N: %s",SYS_ID);		//??????????????
	
	print_thermal("\n%s", "= = = = = = = = = = = = = = = = ");	
	print_save("\n%s", "= = = = = = = = = = = = = = = = ");	
	print_thermal("\n\n\n\n\n");			//Line19
	print_save("\n\n");						//Line19
}

/*******************************************************************************/
void Send720(void) {
	u32 w1;
	u8 i, j;
	u8 acrFlag=0, alb=0, cre=0;

	strcpy(charbuff, "~JAN/01/2020 00:00:00     \r\n");
	switch (MEASmonth) {
	case 1:
		charbuff[1] = 'J';
		charbuff[2] = 'A';
		charbuff[3] = 'N';
		break;
	case 2:
		charbuff[1] = 'F';
		charbuff[2] = 'E';
		charbuff[3] = 'B';
		break;
	case 3:
		charbuff[1] = 'M';
		charbuff[2] = 'A';
		charbuff[3] = 'R';
		break;
	case 4:
		charbuff[1] = 'A';
		charbuff[2] = 'P';
		charbuff[3] = 'R';
		break;
	case 5:
		charbuff[1] = 'M';
		charbuff[2] = 'A';
		charbuff[3] = 'Y';
		break;
	case 6:
		charbuff[1] = 'J';
		charbuff[2] = 'U';
		charbuff[3] = 'N';
		break;
	case 7:
		charbuff[1] = 'J';
		charbuff[2] = 'U';
		charbuff[3] = 'L';
		break;
	case 8:
		charbuff[1] = 'A';
		charbuff[2] = 'U';
		charbuff[3] = 'G';
		break;
	case 9:
		charbuff[1] = 'S';
		charbuff[2] = 'E';
		charbuff[3] = 'P';
		break;
	case 10:
		charbuff[1] = 'O';
		charbuff[2] = 'C';
		charbuff[3] = 'T';
		break;
	case 11:
		charbuff[1] = 'N';
		charbuff[2] = 'O';
		charbuff[3] = 'V';
		break;
	case 12:
		charbuff[1] = 'D';
		charbuff[2] = 'E';
		charbuff[3] = 'C';
		break;
	}
	w1 = bin2bcdasc4(MEASday);		charbuff[5] = w1 >> 8;	charbuff[6] = w1 >> 0;
	w1 = bin2bcdasc4(MEASyear);		charbuff[10] = w1 >> 8;	charbuff[11] = w1 >> 0;
	w1 = bin2bcdasc4(MEAShour);		charbuff[13] = w1 >> 8;	charbuff[14] = w1 >> 0;
	w1 = bin2bcdasc4(MEASmin);		charbuff[16] = w1 >> 8;	charbuff[17] = w1 >> 0;
	w1 = bin2bcdasc4(MEASsec);		charbuff[19] = w1 >> 8;	charbuff[20] = w1 >> 0;
	fill_space2(charbuff, 26);
	for (i = 0; i < 28; i++) {
		savcmd3(charbuff[i]);
	}
	/////////////////////////////////////////////////////////////////////////////////Line Feed
	strcpy(charbuff, "Name:             Sex:    \r\n");
	for (i = 0; i < 28; i++) {
		savcmd3(charbuff[i]);
	}
	/////////////////////////////////////////////////////////////////////////////////Line Feed
	strcpy(charbuff, "Ward:             Age:    \r\n");
	for (i = 0; i < 28; i++) {
		savcmd3(charbuff[i]);
	}
	/////////////////////////////////////////////////////////////////////////////////Line Feed
	strcpy(charbuff, "Strip Name1   (00000)     \r\n");
	for (i = 0; i < 11; i++) {
		charbuff[i] = MEASstripname[i];
	}
	w1 = bin2bcdasc4(MEASseqno);				//w1 = bin2bcdasc4(SEQ_NO);
	charbuff[16] = w1 >> 24;
	charbuff[17] = w1 >> 16;
	charbuff[18] = w1 >> 8;
	charbuff[19] = w1 >> 0;
	
	fill_space2(charbuff, 26);

	for (i = 0; i < 28; i++) {
		savcmd3(charbuff[i]);
	}
	/////////////////////////////////////////////////////////////////////////////////Line Feed
	if (store_ErrCode == NO_STRIP) {
		strcpy(charbuff, "         No Strip         \r\n");
		for (i = 0; i < 28; i++) {
			savcmd3(charbuff[i]);
		}
	}
	else {
		for(w1 = 0; w1 < MEASstripinfo[0]; w1++){
			if(MEASstripinfo[w1+1] == 12){
				acrFlag++; 
				alb=MEAS_results[w1];
			}
			if(MEASstripinfo[w1+1] == 13){
				acrFlag++; 
				cre=MEAS_results[w1];
			}
			if(MEASstripinfo[w1+1] == 12){
				 strcpy(charbuff, "                         \r\n");
				j=0;
				for(i = 5; i<8; i++)charbuff[j++] = buffRESULT[42*w1 + i];
				for(i=0;i<4;i++)charbuff[j++] = ' ';
				if(UNIT_FLAG == UNIT_CONV) for(i = 0; i < 16 ; i++)charbuff[j++] = as720_table_cybow_conv[MEASstripinfo[w1+1]-1][MEAS_results[w1]][i];//7~14
				else for(i = 0; i < 16 ; i++)charbuff[j++] = as720_table_cybow_si[MEASstripinfo[w1+1]-1][MEAS_results[w1]][i];
				for(i = 0; i<2; i++)charbuff[j++] = ' ';
				for(i=0;i<27;i++)savcmd3(charbuff[i]);
			}
			else if(MEASstripinfo[w1+1] == 13 && UNIT_FLAG == UNIT_CONV){
				 strcpy(charbuff, "                         \r\n");
				j=0;
				for(i = 5; i<8; i++)charbuff[j++] = buffRESULT[42*w1 + i];
				for(i=0;i<4;i++)charbuff[j++] = ' ';
				if(UNIT_FLAG == UNIT_CONV) for(i = 0; i < 16 ; i++)charbuff[j++] = as720_table_cybow_conv[MEASstripinfo[w1+1]-1][MEAS_results[w1]][i];//7~14
				else for(i = 0; i < 16 ; i++)charbuff[j++] = as720_table_cybow_si[MEASstripinfo[w1+1]-1][MEAS_results[w1]][i];
				for(i = 0; i<2; i++)charbuff[j++] = ' ';
				for(i=0;i<27;i++)savcmd3(charbuff[i]);
			}
			else{
				strcpy(charbuff, "                          \r\n");
				j=0;
				for(i = 5; i<8; i++)charbuff[j++] = buffRESULT[42*w1 + i];
				for(i=0;i<4;i++)charbuff[j++] = ' ';
				if(UNIT_FLAG == UNIT_CONV) for(i = 0; i < 16 ; i++)charbuff[j++] = as720_table_cybow_conv[MEASstripinfo[w1+1]-1][MEAS_results[w1]][i];//7~14
				else for(i = 0; i < 16 ; i++)charbuff[j++] = as720_table_cybow_si[MEASstripinfo[w1+1]-1][MEAS_results[w1]][i];
				for(i = 0; i<3; i++)charbuff[j++] = ' ';
				for(i=0;i<28;i++)savcmd3(charbuff[i]);
			}
		}
	}
	if(MEASstripinfo[0] < 13){
		strcpy(charbuff, "                          \r\n");
		for (i = 0; i < 28; i++) {
			savcmd3(charbuff[i]);
		}
	}
	/////////////////////////////////////////////////////////////////////////////////Line Feed
	strcpy(charbuff, "(SN=        )             \r\n");	
	for (i = 0; i < 8; i++) {
		charbuff[i + 4] = SYS_ID[i];
	}
	fill_space2(charbuff, 26);
	for (i = 0; i < 28; i++) {
		savcmd3(charbuff[i]);
	}
	/////////////////////////////////////////////////////////////////////////////////Line Feed
	strcpy(charbuff, "ID(             )         \r\n");
	if(MEASseqno < SRT_QC_NO){
		int idLength;
		for(idLength=12;idLength>=0;idLength--){
		if(MEASuserid[idLength] != 0 && MEASuserid[idLength] != ' ')break;
	}idLength++;
	int k=0;
	for(int j=13-idLength;j<13;j++){
		charbuff[j + 3] = MEASuserid[k++];
	}
	fill_space2(charbuff, 26);
	}else{
		for(i=0;i<MAX_USERID-MAX_LOTNO-2;i++){
			charbuff[i + 8] = MEASuserid[i + 8];
		}
	}
	for (i = 0; i < 28; i++) {
		savcmd3(charbuff[i]);
	}
	/////////////////////////////////////////////////////////////////////////////////Line Feed
	strcpy(charbuff, "OP(00000000)LOT(00000000) \r\n");
		int operFlag = 0, lotFlag = 0, opLength = 0, lotLength = 0;
		
	for(opLength=7;opLength>=0;opLength--){
		if(MEASoperid[opLength] != 0 && MEASoperid[opLength] != ' ')break;
	}opLength++;
	for(i=0;i<opLength;i++){
			if(MEASoperid[i]<'0' || MEASoperid[i]>'9')operFlag=1;
		}
/*		for(i=0;i<8;i++){
			if(MEASoperid[i] == NULL || MEASoperid[i] == ' '){
				break;
			}else if(MEASoperid[i]<'0' || MEASoperid[i]>'9')operFlag=1;
		}*/
		//i= id length
		int k=0;
		if(operFlag){
			for(k=3;k<11;k++){
				charbuff[k] = ' ';
			}
		}
		k=0;
		for(int j=8-opLength;j<8;j++){
			charbuff[j + 3] = MEASoperid[k++];
		}
		if(MEASseqno>=SRT_QC_NO){//QC일 때
		for(lotLength=MAX_LOTNO-1;lotLength>=0;lotLength--){
			if(MEASuserid[lotLength] != 0 && MEASuserid[lotLength] != ' ')break;
		}lotLength++;
			for(i=0;i<lotLength;i++){
				if(MEASuserid[i]<'0' || MEASuserid[i]>'9')lotFlag=1;
			}
			//i= id length
			k=0;
			if(lotFlag){
				for(k=16;k<24;k++){
					charbuff[k] = ' ';
				}
			}
			k=0;
			for(int j=MAX_LOTNO-lotLength;j<MAX_LOTNO;j++){
				charbuff[j + 16] = MEASuserid[k++];
			}
		}
	if(acrFlag < 2 && (!store_COLOR_NO && !store_TURBI_NO)) charbuff[25] = '~';
	for (i = 0; i < 28; i++) {
		savcmd3(charbuff[i]);
	}
	/////////////////////////////////////////////////////////////////////////////////Line Feed
	if(acrFlag == 2){
		strcpy(charbuff, "A:C                       \r\n");
		if(UNIT_FLAG == UNIT_CONV){
			for(i=0;i<17;i++){
				charbuff[9+i] = acr_result_conv[acr_item[cre][alb]][i]; 
			}
		}else{
			for(i=0;i<17;i++){
				charbuff[9+i] = acr_result_si[acr_item[cre][alb]][i];
			}
		}
		for (i = 0; i < 28; i++) {
			savcmd3(charbuff[i]);
		}
		strcpy(charbuff, "                          \r\n");
		for(i=0;i<13;i++){
			charbuff[i] = acr_tb[acr_item[cre][alb]][i];
		}
		if(!store_COLOR_NO && !store_TURBI_NO) charbuff[25] = '~';
		for (i = 0; i < 28; i++) {
			savcmd3(charbuff[i]);
		}
	}
//////////////////////////////////////////////////////////////////////////////////////////////////
	if(store_COLOR_NO || store_TURBI_NO) {

	/////////////////////////////////////////////////////////////////////////////////Color
		strcpy(charbuff, "Color                     \r\n");
		switch(store_COLOR_NO){
			case 0:
				for(i=0;i<6;i++){
					charbuff[6+i] = s_None[LANGUAGE_FLAG][i];
				}
			break;
			case 1:
				for(i=0;i<8;i++){
					charbuff[6+i] = s_Yellow[LANGUAGE_FLAG][i];
				}
			break;
			case 2:
				for(i=0;i<11;i++){
					charbuff[6+i] = s_DKYellow[LANGUAGE_FLAG][i];
				}
			break;
			case 3:
				for(i=0;i<7;i++){
					charbuff[6+i] = s_Straw[LANGUAGE_FLAG][i];
				}
			break;
			case 4:
				for(i=0;i<7;i++){
					charbuff[6+i] = s_Amber[LANGUAGE_FLAG][i];
				}
			break;
			case 5:
				for(i=0;i<5;i++){
					charbuff[6+i] = s_Red[LANGUAGE_FLAG][i];
				}
			break;
			case 6:
				for(i=0;i<8;i++){
					charbuff[6+i] = s_Orange[LANGUAGE_FLAG][i];
				}
			break;
			case 7:
				for(i=0;i<7;i++){
					charbuff[6+i] = s_Green[LANGUAGE_FLAG][i];
				}
			break;
			case 8:
				for(i=0;i<7;i++){
					charbuff[6+i] = s_Other[LANGUAGE_FLAG][i];
				}
			break;
		} 
		fill_space2(charbuff, 26);
		for (i = 0; i < 28; i++) {
			savcmd3(charbuff[i]);
		}
	/////////////////////////////////////////////////////////////////////////////////Turbidity
		strcpy(charbuff, "Turb                     ~\r\n");
		switch(store_TURBI_NO){
			case 0:
				for(i=0;i<6;i++){
					charbuff[6+i] = s_None[LANGUAGE_FLAG][i];
				}
			break;
			case 1:
				for(i=0;i<7;i++){
					charbuff[6+i] = s_Clear[LANGUAGE_FLAG][i];
				}
			break;
			case 2:
				for(i=0;i<11;i++){
					charbuff[6+i] = s_SL_Cloudy[LANGUAGE_FLAG][i];
				}
			break;
			case 3:
				for(i=0;i<8;i++){
					charbuff[6+i] = s_Cloudy[LANGUAGE_FLAG][i];
				}
			break;
			case 4:
				for(i=0;i<8;i++){
					charbuff[6+i] = s_Turbid[LANGUAGE_FLAG][i];
				}
			break;
			case 5:
				for(i=0;i<7;i++){
					charbuff[6+i] = s_Other[LANGUAGE_FLAG][i];
				}
			break;
		} 
		fill_space2(charbuff, 26);
		for (i = 0; i < 28; i++) {
			savcmd3(charbuff[i]);
		}
	}
}

/*******************************************************************************/
void SendYD(u8 flag) {
	u32 w1;
	u8 i, j, k, numFlag;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////Date Start		
	if(flag < 6 ){
		strcpy(charbuff, "Date :MAR-19-2021 12:35:10\r\n");
		switch (MEASmonth) {
			case 1:
			charbuff[7] = 'J';
			charbuff[8] = 'A';
			charbuff[9] = 'N';
			break;
		case 2:
			charbuff[7] = 'F';
			charbuff[8] = 'E';
			charbuff[9] = 'B';
			break;
		case 3:
			charbuff[7] = 'M';
			charbuff[8] = 'A';
			charbuff[9] = 'R';
			break;
		case 4:
			charbuff[7] = 'A';
			charbuff[8] = 'P';
			charbuff[9] = 'R';
			break;
		case 5:
			charbuff[7] = 'M';
			charbuff[8] = 'A';
			charbuff[9] = 'Y';
			break;
		case 6:
			charbuff[7] = 'J';
			charbuff[8] = 'U';
			charbuff[9] = 'N';
			break;
		case 7:
			charbuff[7] = 'J';
			charbuff[8] = 'U';
			charbuff[9] = 'L';
			break;
		case 8:
			charbuff[7] = 'A';
			charbuff[8] = 'U';
			charbuff[9] = 'G';
			break;
		case 9:
			charbuff[7] = 'S';
			charbuff[8] = 'E';
			charbuff[9] = 'P';
			break;
		case 10:
			charbuff[7] = 'O';
			charbuff[8] = 'C';
			charbuff[9] = 'T';
			break;
		case 11:
			charbuff[7] = 'N';
			charbuff[8] = 'O';
			charbuff[9] = 'V';
			break;
		case 12:
			charbuff[7] = 'D';
			charbuff[8] = 'E';
			charbuff[9] = 'C';
			break;
		}
		w1 = bin2bcdasc4(MEASday);		charbuff[11] = w1 >> 8;	charbuff[12] = w1 >> 0;
		w1 = bin2bcdasc4(MEASyear);		charbuff[16] = w1 >> 8;	charbuff[17] = w1 >> 0;
		w1 = bin2bcdasc4(MEAShour);		charbuff[19] = w1 >> 8;	charbuff[20] = w1 >> 0;
		w1 = bin2bcdasc4(MEASmin);		charbuff[22] = w1 >> 8;	charbuff[23] = w1 >> 0;
		w1 = bin2bcdasc4(MEASsec);		charbuff[25] = w1 >> 8;	charbuff[26] = w1 >> 0;
		for (i = 0; i < 28; i++) {
			savcmd3(charbuff[i]);
		}
	}
	else{
		strcpy(charbuff, "Date :MAR-19-21 11:35:17 \r\n");
		switch (MEASmonth) {
			case 1:
			charbuff[7] = 'J';
			charbuff[8] = 'A';
			charbuff[9] = 'N';
			break;
		case 2:
			charbuff[7] = 'F';
			charbuff[8] = 'E';
			charbuff[9] = 'B';
			break;
		case 3:
			charbuff[7] = 'M';
			charbuff[8] = 'A';
			charbuff[9] = 'R';
			break;
		case 4:
			charbuff[7] = 'A';
			charbuff[8] = 'P';
			charbuff[9] = 'R';
			break;
		case 5:
			charbuff[7] = 'M';
			charbuff[8] = 'A';
			charbuff[9] = 'Y';
			break;
		case 6:
			charbuff[7] = 'J';
			charbuff[8] = 'U';
			charbuff[9] = 'N';
			break;
		case 7:
			charbuff[7] = 'J';
			charbuff[8] = 'U';
			charbuff[9] = 'L';
			break;
		case 8:
			charbuff[7] = 'A';
			charbuff[8] = 'U';
			charbuff[9] = 'G';
			break;
		case 9:
			charbuff[7] = 'S';
			charbuff[8] = 'E';
			charbuff[9] = 'P';
			break;
		case 10:
			charbuff[7] = 'O';
			charbuff[8] = 'C';
			charbuff[9] = 'T';
			break;
		case 11:
			charbuff[7] = 'N';
			charbuff[8] = 'O';
			charbuff[9] = 'V';
			break;
		case 12:
			charbuff[7] = 'D';
			charbuff[8] = 'E';
			charbuff[9] = 'C';
			break;
		}
		w1 = bin2bcdasc4(MEASday);		charbuff[11] = w1 >> 8;	charbuff[12] = w1 >> 0;
		w1 = bin2bcdasc4(MEASyear);		charbuff[14] = w1 >> 8;	charbuff[15] = w1 >> 0;
		w1 = bin2bcdasc4(MEAShour);		charbuff[17] = w1 >> 8;	charbuff[18] = w1 >> 0;
		w1 = bin2bcdasc4(MEASmin);		charbuff[20] = w1 >> 8;	charbuff[21] = w1 >> 0;
		w1 = bin2bcdasc4(MEASsec);		charbuff[23] = w1 >> 8;	charbuff[24] = w1 >> 0;		
		for (i = 0; i < 28; i++) {
			savcmd3(charbuff[i]);
		}																																																																																																																																																																					
	}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////Date End	
/////////////////////////////////////////////////////////////////////////////////////////////////////////////SEQ & ID Start
	int idLength, spCnt = 0;
	for(idLength=0;idLength<13;idLength++){
		if(MEASuserid[idLength]==' ')spCnt++;
		else if(MEASuserid[idLength] == NULL){
			break;
		}
	}if(idLength == spCnt)idLength = 0;
	if(!idLength){
		strcpy(charbuff, "ID_NO:0000-              \r\n");
		w1 = bin2bcdasc4(MEASseqno);				//w1 = bin2bcdasc4(SEQ_NO);
		charbuff[6] = w1 >> 24;
		charbuff[7] = w1 >> 16;
		charbuff[8] = w1 >> 8;
		charbuff[9] = w1 >> 0;
		
		for (i = 0; i < 27 ; i++) {
			savcmd3(charbuff[i]);
		}
	}else if(flag%3==0){	/////////////////////////////////////////1:ABCD
		strcpy(charbuff, "ID_NO:0000-                 \r\n");
		w1 = bin2bcdasc4(MEASseqno);				//w1 = bin2bcdasc4(SEQ_NO);
		charbuff[6] = w1 >> 24;
		charbuff[7] = w1 >> 16;
		charbuff[8] = w1 >> 8;
		charbuff[9] = w1 >> 0;
		j=0;
		if(MEASseqno>END_SEQ_NO){
			j=MAX_LOTNO;
			for(i=11;i<11+8;i++)charbuff[i]=MEASuserid[j++];
			charbuff[i]=0;
		}
		else{
			for(i=11;i<24;i++){
				charbuff[i]=MEASuserid[j++];
			}
		}
		fill_space2(charbuff, 28);

		for (i = 0; i < 30; i++) {
			savcmd3(charbuff[i]);
		}
	}
	else if(flag%3==1){	/////////////////////////////////////////2:---ABCD
		strcpy(charbuff, "ID_NO:0000-                 \r\n");
		w1 = bin2bcdasc4(MEASseqno);				//w1 = bin2bcdasc4(SEQ_NO);
		charbuff[6] = w1 >> 24;
		charbuff[7] = w1 >> 16;
		charbuff[8] = w1 >> 8;
		charbuff[9] = w1 >> 0;
		if(MEASseqno>END_SEQ_NO){
			j=MAX_LOTNO;
			for(i=19;i<27;i++)charbuff[i]=MEASuserid[j++];
			charbuff[i]=0;
		}
		else{
			k=0;
			for(j=13-idLength;j<13;j++){
				charbuff[j+14] = MEASuserid[k++];
			}
		}
		fill_space2(charbuff, 28);

		for (i = 0; i < 30; i++) {
			savcmd3(charbuff[i]);
		}
	}
	else if(flag%3==2){	/////////////////////////////////////////3:0001234
		for(j=0;j<idLength;j++){
			if(MEASuserid[j] >= '0' && MEASuserid[j] <= '9' ){
				numFlag = 1;
			}else{
				numFlag = 0;
				break;
			}
		}
		
		if(!numFlag || MEASseqno>END_SEQ_NO){
			strcpy(charbuff, "ID_NO:0000-                 \r\n");
			w1 = bin2bcdasc4(MEASseqno);				//w1 = bin2bcdasc4(SEQ_NO);
			charbuff[6] = w1 >> 24;
			charbuff[7] = w1 >> 16;
			charbuff[8] = w1 >> 8;
			charbuff[9] = w1 >> 0;
			if(MEASseqno>END_SEQ_NO){
				j=MAX_LOTNO;
				for(i=19;i<27;i++)charbuff[i]=MEASuserid[j++];
				charbuff[i]=0;
			}
			else{
				k=0;
				for(j=13-idLength;j<13;j++){
					charbuff[j+14] = MEASuserid[k++];
				}
			}
			fill_space2(charbuff, 28);

			for (i = 0; i < 30; i++) {
				savcmd3(charbuff[i]);
			}
		}else if(numFlag == 1){
			strcpy(charbuff, "ID_NO:0000-0000000000000 \r\n");
			w1 = bin2bcdasc4(MEASseqno);				//w1 = bin2bcdasc4(SEQ_NO);
			charbuff[6] = w1 >> 24;
			charbuff[7] = w1 >> 16;
			charbuff[8] = w1 >> 8;
			charbuff[9] = w1 >> 0;
			k=0;
			for(j=13-idLength;j<13;j++){
				charbuff[j+11] = MEASuserid[k++];
			}

			fill_space2(charbuff, 24);

			for (i = 0; i < 27; i++) {
				savcmd3(charbuff[i]);
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////SEQ & ID End

	/////////////////////////////////////////////////////////////////////////////////Line Feed
	/////////////////////////////////////////////////////////////////////////////////Line Feed
	if(flag < 6){
		strcpy(charbuff, "Ward: \r\n");
		for (i = 0; i < 8; i++) savcmd3(charbuff[i]);
		strcpy(charbuff, "NAME: \r\n");
		for (i = 0; i < 8; i++) savcmd3(charbuff[i]);
	}else{
		strcpy(charbuff, "Ward:                     \r\n");
		for (i = 0; i < 28; i++) savcmd3(charbuff[i]);
		strcpy(charbuff, "NAME:                     \r\n");
		for (i = 0; i < 28; i++) savcmd3(charbuff[i]);
	}
	/////////////////////////////////////////////////////////////////////////////////Line Feed

	if (store_ErrCode == NO_STRIP) {
		strcpy(charbuff, "         No Strip         \r\n");
		for (i = 0; i < 28; i++) {
			savcmd3(charbuff[i]);
		}
	}
	else {
		for(w1 = 0; w1 < MEASstripinfo[0]; w1++){
			if(flag < 6){
				strcpy(charbuff, "                          \r\n");
				j=0;
				for(i = 5; i < 8 ; i++)charbuff[j++] = buffRESULT[42*w1 + i];//item name//0~2
				for(i = 0; i < 4 ; i++)charbuff[j++] = ' ';//3~6
				for(i = 0; i < 8 ; i++)charbuff[j++] = yd_table_cybow[MEASstripinfo[w1+1]-1][MEAS_results[w1]+1][i];//7~14
				for(i = 0; i < 3 ; i++)charbuff[j++] = ' ';//15~17
				for(i = 0; i < 8 ; i++)charbuff[j++] = yd_table_cybow[MEASstripinfo[w1+1]-1][0][i];//result//18~25
				for(i = 0; i < 28; i++)savcmd3(charbuff[i]);
			}
			else{
				strcpy(charbuff, "                            \r\n");
				j=0;
				for(i = 5; i < 8 ; i++)charbuff[j++] = buffRESULT[42*w1 + i];//item name//0~2
				for(i = 0; i < 4 ; i++)charbuff[j++] = ' ';//3~6
				for(i = 0; i < 8 ; i++)charbuff[j++] = yd_table_cybow[MEASstripinfo[w1+1]-1][MEAS_results[w1]+1][i];//7~14
				for(i = 0; i < 5 ; i++)charbuff[j++] = ' ';//15~17
				for(i = 0; i < 8 ; i++)charbuff[j++] = yd_table_cybow[MEASstripinfo[w1+1]-1][0][i];//result//18~25
				for(i = 0; i < 30; i++)savcmd3(charbuff[i]);
			}
		}
	}
	strcpy(charbuff, "\r\n");
	//fill_space2(charbuff, 26);
	for (i = 0; i < 3; i++) {
		savcmd3(charbuff[i]);
	}
}
/*******************************************************************************/
void print_sound(void){
u16 i;
	for(i=0;i<20;i++){	
		key_buzzer();
		delay_ms(30);
		key_buzzer();
		delay_ms(20);
	}
	delay_ms(120);
//	BUZZ_LO();	
}

/*******************************************************************************/
//=============================================================
//no = 1 ~ MAX_RECORD
short view_result_store(u16 view_seqno){
	prt_msg("\r view_result_store SEQ.no:%4d", view_seqno);

	if((!view_seqno)||(view_seqno>END_QC_NO)){
		dsp_novalid();
		prt_msg("   Error ==> view_seqno");
		return(NG);
	}else{
		if(read_result(view_seqno, 0)==OK){						//result data ==> cmdbuff
			make_result();
			conv_result_store();
			dsp_result_store();
			return(OK);			
		}else{	
			dsp_novalid();
			return(NG);
		}
	}
}
//=============================================================
void make_result(void){
u32 k;
	k=cmdbuff[ 2];	k=(k<<8)+cmdbuff[ 3];	k=(k<<8)+cmdbuff[ 4];	k=(k<<8)+cmdbuff[ 5];
	cur_dsp_record_no = MEASseqno  = asc2bin4(k);
	k=cmdbuff[ 8];	k=(k<<8)+cmdbuff[ 9];	MEASyear  = asc2bin4(k);
	k=cmdbuff[10];	k=(k<<8)+cmdbuff[11];	MEASmonth = asc2bin4(k);
	k=cmdbuff[12];	k=(k<<8)+cmdbuff[13];	MEASday   = asc2bin4(k);
	k=cmdbuff[14];	k=(k<<8)+cmdbuff[15];	MEAShour  = asc2bin4(k);
	k=cmdbuff[16];	k=(k<<8)+cmdbuff[17];	MEASmin   = asc2bin4(k);
	k=cmdbuff[18];	k=(k<<8)+cmdbuff[19];	MEASsec   = asc2bin4(k);

	for(k=0;k<MAX_OPERID;k++) MEASoperid[k] = cmdbuff[20+k];		MEASoperid[k] = 0;			//20~29
	for(k=0;k<MAX_USERID;k++) MEASuserid[k] = cmdbuff[30+k];		MEASuserid[k] = 0;			//30~47
	k=cmdbuff[48];	k=(k<<8)+cmdbuff[49];	MEASstripno = asc2bin4(k);
	for(k=0;k<MEAS_NAME_SIZE;k++) MEASstripname[k] = cmdbuff[50+k];	MEASstripname[k] = 0;		//50~59
	for(k=0;k<(MAX_PAD_NO+1);k++) MEASstripinfo[k] = hexasc2bin_1(cmdbuff[60+k]);				//60~74
	MEASstripinfo[k] = 0;
	for(k=0;k<MAX_PAD_NO;k++) MEAS_results[k] = hexasc2bin_1(cmdbuff[75+k]);					//75~89
	k = 0;
	store_ErrCode = cmdbuff[257]-'0';
	store_COLOR_NO = cmdbuff[258]-'0';
	store_TURBI_NO = cmdbuff[259]-'0';
	store_NEGPOS = cmdbuff[260]-'0';
}
//=============================================================
void delete_result_store(u16 ssno, u16 eeno){
u32 i, j, cnt;
u32 radr;
	if((!ssno)||(ssno>END_QC_NO)) return;
	if((!eeno)||(eeno>END_QC_NO)) return;
	if(ssno<=eeno){
		printf0("\r Delete result store: ", ssno);
		cnt = eeno-ssno;
		cnt += 1;
	}else{
		printf0("\r Number Error!");
		cnt = 0;
	}
		
	while(cnt){
//		erase_box( 139, 428, 139+120, 428+40);
		radr = (((ssno-1)/16)*SECTOR_SIZE) + RESULT_ADR;
		readFR4k(radr, 0x1000);
		j = ((ssno-1)&0x0F)*RESULT_PAGE;
		for(i=0;i<RESULT_PAGE;i++) flash_read[j] = 0x00;
		for(i=0;i<SECTOR_SIZE;i++) flash_buff[i] = flash_read[i];
		write_sflash(radr, SECTOR_SIZE);
		printf0(" %4d,", ssno);
		delay_ms(100);
//		LCDbmpxx(b_del3, 139,428);					
//		delay_ms(150);
//		j = put_seqno(ssno);
//		msg20(130+j,92,BLK,top_str);	
		ssno++;
		if(ssno>END_QC_NO) break;
		cnt--;
	}
}
//=============================================================
//=============================================================
int send_result_store(u16 ssno, u16 eeno){
int k, cnt;
u16 w1, w2;
	if((!ssno)||(ssno>END_QC_NO)) return(NG);
	if((!eeno)||(eeno>END_QC_NO)) return(NG);
	if(ssno<eeno){
		cnt = (eeno-ssno)+1;
	}else if(ssno==eeno){
		cnt = 1;
	}else{
//		cnt = (eeno+MAX_RECORD)-ssno;
		cnt = 0;
		printf0("\r Number Error!");
	}
		
	while(cnt){
		erase_box( 139, 428, 139+120, 428+40);
		if(read_result(ssno, 1)==OK){
			printf0("\r *Success(SendResult) No:%4d,Sum:%4x\n", ssno,w1);		
#ifdef TEST_RESULT
			for(k=0;k<RESULT_SIZE_ORG;k++) savcmd0(cmdbuff[k]); //wait_tx0();
#else
			for(k=0;k<RESULT_SIZE_ORG;k++){ savcmd3(cmdbuff[k]);	/*wait_tx3();*/ }
#endif
		}else{
			printf0("\r *Error!(SendResult) No:%4d,Sum:%4x,SAV:%4x", ssno, w1, w2);	//wait_tx0();	
#ifdef TEST_RESULT
			printf0("\r");
			for(k=0;k<RESULT_SIZE;k++) printf0("0"); //wait_tx0();
#else
			for(k=0;k<RESULT_SIZE;k++){ savcmd3('0'); /*wait_tx3();*/ }
#endif
		}
		delay_ms(100);
//		LCDbmpxx(b_send3, 139,428);					
		wait_tx0();
		wait_tx3();
		delay_ms(150);
//		LCDbmpxx(b_start3, 20,88);					
//		k = put_seqno(ssno);
//		msg20(130+j,92,BLK,top_str);	
		cnt--;
		ssno++;
//		if(ssno>MAX_RECORD) ssno=1;
	}
	delay_ms(1000);
	return(OK);
}
//=============================================================
//=============================================================
#define ASC	0
#define DESC 1
#define POS_GAP 52
#define RANGE_GAP 10
#define REF_POS	90
#define AC_R 50
#define AC_G 50
#define AC_B 15
#define AC_START	270
#define AC_END		300
#define AC12_R	25
#define AC12_G	25
#define AC12_B	25


void correction_curve(void){
	u16 i, wht_r, wht_g, wht_b;
	u16 minR=10000, minPos = 0, maxPos = 0, grade = 0;
	u32 maxR = 0, maxG = 0, maxB = 0;
	u32 minR2 = 10000, minG2 = 10000, minB2 = 10000, minPos2 = 0;
	u16 ref;
	u8 item1, item2;
	//u8 REF_POS = (StripPosition[15]>150?1:0);//1 = true, 0 = false
	uint64_t ww;
	unsigned int k;
	wht_r = spanRED[StripPosition[0]];
	printf0("\r RED:%4d", wht_r);
	if(!wht_r) wht_r = 10000;
	wht_g = spanGRN[StripPosition[0]];
	printf0("   GRN:%4d", wht_g);
	if(!wht_g) wht_g = 10000;
	wht_b = spanBLU[StripPosition[0]];
	printf0("   BLU:%4d", wht_b);
	if(!wht_b) wht_b = 10000;
	WHITE_RED = ref_red_buf[StripPosition[0]];
	WHITE_GRN = ref_grn_buf[StripPosition[0]];
	WHITE_BLU = ref_blu_buf[StripPosition[0]];
	
	if(modex != MODE_MAINTANENCE_cali && SERIAL_FLAG == 0){
		for(i=0;i<MaxRawData;i++){	k = spanRED[i];	savcmd3(k>>8);	savcmd3(k>>0);	}
		for(i=0;i<MaxRawData;i++){	k = spanGRN[i];	savcmd3(k>>8);	savcmd3(k>>0);	}
		for(i=0;i<MaxRawData;i++){	k = spanBLU[i];	savcmd3(k>>8);	savcmd3(k>>0);	}
	}
	item1 = *(u8*)(EXT_BUFF+STRIP_INFO_RAM+33*STRIP_NO+2+16+1);
	item2 = *(u8*)(EXT_BUFF+STRIP_INFO_RAM+33*STRIP_NO+2+16+2);
	if(modex != MODE_MAINTANENCE_cali &&  item1 == item2){
		for(i=StripPosition[3]-RANGE_GAP;i<StripPosition[4]+RANGE_GAP;i++){			
			if(weight[0]==1 && maxR<spanRED[i]){
				maxR = spanRED[i];
				maxPos = i;
			}else if(weight[0]==2 && maxG<spanGRN[i]){
				maxG = spanGRN[i];
				maxPos = i;
			}else if(maxB<spanBLU[i]){
				maxB = spanBLU[i];
				maxPos = i;
			}
		}
/*		for(i=1;i<13;i++){
			if(maxPos>StripPosition[2+i] && maxPos<StripPosition[2+i]+POS_GAP){
				grade = i;
			}
		}*/grade = 1;
		//ref = maxPos-POS_GAP*(grade+1);
		ref = maxPos-(POS_GAP*1.5);
		for(i=0;i<13;i++){
			StripPosition[2+i] = ref + POS_GAP*i/* + POS_GAP/2*/;
			if(weight[i]==2)StripPosition[2+i] -= 2;
			else if(weight[i]!=1)StripPosition[2+i] -= 3;
			if(StripPosition[2+i]>=MaxRawData)StripPosition[2+i] = ref;

		}
		if(SERIAL_FLAG == 0){
			for(i=0;i<13;i++){
					k = StripPosition[2+i];
					savcmd3(k>>8);
					savcmd3(k>>0);
			}
			savcmd3(StripPosition[15]>>8);
			savcmd3(StripPosition[15]>>0);
		}

	}else if(modex != MODE_MAINTANENCE_cali){
		for(i=StripPosition[3]-RANGE_GAP;i<MaxRawData-RANGE_GAP;i++){
			
			if(minR>spanRED[i]){
				minR = spanRED[i];
				minPos = i;
			}if(maxR<spanRED[i]){
				maxR = spanRED[i];
			}if(maxG<spanGRN[i]){
				maxG = spanGRN[i];
			}if(maxB<spanBLU[i]){
				maxB = spanBLU[i];
			}
			if(i>AC_START && i<AC_END){
				if(minR2>spanRED[i]){
					minR2 = spanRED[i];
					minPos2 = i;
				}if(minG2>spanGRN[i]){
					minG2 = spanGRN[i];
				}if(minB2>spanBLU[i]){
					minB2 = spanBLU[i];
				}
			}
		}
		//clr_screen_box(360, 16, 250, 16);
		maxR = minR2*100 / maxR;
		maxG = minG2*100 / maxG;
		maxB = minB2*100 / maxB; 
		//sprintf(charbuff, "%d %d %d %d", maxR, maxG, maxB, minPos);
		//out_line_16(360, 16, charbuff, BLK_LCD);
		
		if(maxR < AC12_R && maxG < AC12_G && maxB < AC12_B){//211108 12AC Recognition.//ACR 이미지 변경 필요
			if(minPos == minPos2)minPos -= 80;
			minR = spanRED[minPos];
			for(i=0;i<20;i++){
				if(*(u8*)(EXT_BUFF+STRIP_INFO_RAM+33*i+2+16)=='C'){
					STRIP_NO = i;
					get_strip_name(STRIP_NO);
					move_strip_info();
					break;
				}
			}
			//sprintf(charbuff, "%d %s", STRIP_NO, buff_strip_name);
			//out_line_16(120, 16, charbuff, BLK_LCD);

		}else if(maxR > AC_R && maxG <AC_G && maxB < AC_B){//211108 2AC Recognition.
			if(minPos == minPos2)minPos -= 80;
			minR = spanRED[minPos];
			for(i=0;i<20;i++){

				if(*(u8*)(EXT_BUFF+STRIP_INFO_RAM+33*i+2+16)=='2' && (*(u8*)(EXT_BUFF+STRIP_INFO_RAM+33*i+2+16+1)=='C' || *(u8*)(EXT_BUFF+STRIP_INFO_RAM+33*i+2+16+2)=='C')){
					STRIP_NO = i;
					get_strip_name(STRIP_NO);
					move_strip_info();
					break;
				}
			}
//			sprintf(charbuff, "%d %s", STRIP_NO, buff_strip_name);
//			out_line_16(120, 16, charbuff, BLK_LCD);

		}
		for(i=1;i<13;i++){
			if(minPos>StripPosition[2+i]-26 && minPos<StripPosition[2+i]+26){
				grade = i;
			}
		}
		ref = minPos-52*grade;
		for(i=0;i<13;i++){

			StripPosition[2+i] = ref + POS_GAP*i;
			if(weight[i]==2)StripPosition[2+i] -= 2;
			else if(weight[i]!=1)StripPosition[2+i] -= 3;
			if(StripPosition[2+i]>=MaxRawData)StripPosition[2+i] = ref;

		}
		if(modex != MODE_MAINTANENCE_cali && SERIAL_FLAG == 0){
			for(i=0;i<13;i++){
					k = StripPosition[2+i];
					savcmd3(k>>8);
					savcmd3(k>>0);
			}
		}
		if(modex != MODE_MAINTANENCE_cali && SERIAL_FLAG == 0){
			savcmd3(StripPosition[15]>>8);
			savcmd3(StripPosition[15]>>0);
		}
	}

	/*for(i=0;i<MaxRawData;i++){
		ww = spanRED[i];
		spanRED[i] = (((ww * WHITE_RED) * 1000) / wht_r) / ref_red_buf[763];
		ww = spanGRN[i];
		spanGRN[i] = (((ww * WHITE_GRN) * 1000) / wht_g) / ref_grn_buf[763];
		ww = spanBLU[i];
		spanBLU[i] = (((ww * WHITE_BLU) * 1000) / wht_b) / ref_blu_buf[763];
	}*/
	for(i=0;i<MaxRawData;i++){
		ww = spanRED[i];
		spanRED[i] = (((ww*WHITE_RED)*1000)/wht_r)/ref_red_buf[i];
		ww = spanGRN[i];
		spanGRN[i] = (((ww*WHITE_GRN)*1000)/wht_g)/ref_grn_buf[i];
		ww = spanBLU[i];
		spanBLU[i] = (((ww*WHITE_BLU)*1000)/wht_b)/ref_blu_buf[i];
	}
}


//=============================================================
void IncreseSampleID(void){
	int i;
	u8 numFlag = 0, decimal = 0, tempDecimal = 0;
	if(modex >= MODE_TEST && modex < MODE_QC && AUTO_FLAG && tempSampleID[measCnt][0] != ' ' &&tempSampleID[measCnt+1][0] == ' '){

		for(i=0;i<MAX_USERID;i++){
			if(tempSampleID[measCnt][i]>='0' && tempSampleID[measCnt][i] <='9'){
				//printf3("%c", tempSampleID[measCnt][i]);
				numFlag = 1;
			}
			else if(tempSampleID[measCnt][i] == NULL){
				tempDecimal = decimal = i-1;
				//printf3("\r\n%d\r\n", tempDecimal);
				i=MAX_USERID;

			}
			else{
				numFlag = 0;
				//printf3("\r\nNot Space\r\n");
				i=MAX_USERID;
			}
		}
		if(numFlag){
			for(i=0;i<MAX_USERID;i++)tempSampleID[measCnt+1][i]=tempSampleID[measCnt][i];
			while(1){
				if(decimal==0){
					if(tempSampleID[measCnt][decimal] == '9'){
						if(tempDecimal < MAX_USERID){
							tempSampleID[measCnt+1][0] = '1';
							tempSampleID[measCnt+1][tempDecimal+1] = '0';
						}else{
							for(i=0;i<MAX_USERID;i++)tempSampleID[measCnt+1][i] = '0';
						}
					}else{
						tempSampleID[measCnt+1][0] = tempSampleID[measCnt+1][0] + 1;
					}
					break;
				}
				else{
					if(tempSampleID[measCnt][decimal] != '9'){
						tempSampleID[measCnt+1][decimal] = tempSampleID[measCnt][decimal] + 1; 
						break;
					}
					else if(tempSampleID[measCnt][decimal] == '9'){
						tempSampleID[measCnt+1][decimal] = '0';
						decimal--;
					}
				}
			}
		}
	}
}
//=============================================================
void AddError(u16 ErrorCode){
	if(ErrorCode != errorStack[errorCnt-1]){
		if(modex != MODE_TEST_Error && modex != MODE_TEST_Error2 && modex != MODE_QC_Error && modex != MODE_QC_Error2){
			temp_modex2[errorCnt] = modex;
		}else{
			temp_modex2[errorCnt] = temp_modex2[errorCnt-1];
		}
		errorStack[errorCnt++] = ErrorCode;
		if(!errorSwitch)errorSwitch = 1;
	}
}
//=============================================================
//=============================================================
