#define	_COM_C_

#include <string.h>
#include "common\32f429_port.h"
#include "util\usart.h"
#include "util\tm_adc.h"
#include "util\eep.h"
#include "util\Flash.h"
#include "util\lcd.h" 
#include "util\para.h"
#include "sysadj.h"
#include "com.h"
#include "resm.h"
#include "meas.h"
#include "dsp.h"

void store_userid(void);
void store_strip_info(void);
void store_position(void);
void store_para(void);

//void com_sys_check(void);
void com_msg(char *fmt,...);

/*********************************************************************/
void store_userid(void){
u16 i;;
	printf0("\r store_userid with");
	i =          cmdbuff[0] - '0';
	i = (i*10) + cmdbuff[1] - '0';
	i = (i*10) + cmdbuff[2] - '0';
	i = (i*10) + cmdbuff[3] - '0';
	printf0(" : %d", i);

	if(i>MAX_RECORD){//QC
		printf0(" qc_no");
		if(i<=END_QC_NO){
			QC_NO = i;
			printf0(" qc_ no : %d", QC_NO);
			store_vars(QC_NO_ADR, QC_NO);
		}else printf0(" Error No. : %d", i);		
	}else{
		if(i>0){
			SEQ_NO = i;
			printf0(" seq_ no : %d", SEQ_NO);
			store_seqno();
		}else printf0(" Error No. : 0x00");		
	}
	
	for(i=0;i<MAX_USERID;i++) USER_ID[i] = cmdbuff[i+4];
	USER_ID[i] = 0x00;
	printf0("\r User ID:%s", USER_ID);
}
/*********************************************************************/
/*********************************************************************/
void store_sysid(u8 wflag){
u8 i, j=0;	
	printf0("\r store_sysid");
	for(i=0;i<MAX_SYSID;i++){
		SYS_ID[i] = cmdbuff[i]; 	
		j += SYS_ID[i]; 	
 	}
	SYS_ID[i++] = 0x00;	
	SYS_ID[i++] = j>>8;	
	SYS_ID[i++] = j>>0;	
	for(i=0;i<MAX_SYSID+3;i++) write24(SYSID_ADR+i, SYS_ID[i]);

	store_sflash(SYSTEM_RAM, SYSTEM_SIZE+2);					// ID55AA_ADR
	backup_intflash(SYSTEM_RAM, SYSTEM_SIZE);	
	if(wflag) write_vars('S');									// system_backup(ID55AA_ADR, ID_CHKSUM+2);
}


/*********************************************************************/
void store_strip_info(void){
u16 i,j;	
	printf0("\r store_Strip");
	com_msg("Change Strip" );
	j=0;
	for(i=0;i<STRIP_INFO_SIZE;i++){
		*(u8*)(EXT_BUFF+STRIP_INFO_RAM+i) = cmdbuff[i];
		j += cmdbuff[i];
	}
	j = ~j;
	*(u8*)(EXT_BUFF+STRIP_INFO_RAM+i) = j>>8;
	*(u8*)(EXT_BUFF+STRIP_INFO_RAM+i+1) = j;
	store_sflash(STRIP_INFO_RAM,STRIP_INFO_SIZE+2);
	backup_intflash(PFORM_RAM,PFORM_SIZE+2);
	write_vars('I');

	get_max_stripno();
	move_strip_info();
}

/*********************************************************************/
void store_position(void){
u16 i,j;	
	printf0("\r store_position : ");
	j=0;
	for(i=0;i<POSITION_SIZE;i++){
		*(u8*)(EXT_BUFF+POSITION_RAM+i) = cmdbuff[i];
		j += cmdbuff[i];
	}
	j = ~j;
	*(u8*)(EXT_BUFF+POSITION_RAM+i) = j>>8;
	*(u8*)(EXT_BUFF+POSITION_RAM+i+1) = j;
	store_sflash(POSITION_RAM,POSITION_SIZE+2);
	backup_intflash(POSITION_RAM, POSITION_SIZE+2);
	write_vars('p');
	move_position();
	for(j=0;j<16;j++) printf0("%4d,", StripPosition[j]);
}
/*********************************************************************/
void store_para(void){
u16 i,j;	
	printf0("\r Weight + store_para\r\n");

	j=0;
	for(i=0;i<PARA_SIZE;i++){
		//printf3("%c", cmdbuff[i]);
		*(u8*)(EXT_BUFF+PARA_RAM+i) = cmdbuff[i];
		j += cmdbuff[i];
	}
	j = ~j;
	*(u8*)(EXT_BUFF+PARA_RAM+i) = j>>8;
	*(u8*)(EXT_BUFF+PARA_RAM+i+1) = j;
	store_sflash(PARA_RAM,PARA_SIZE+2);
	backup_intflash(PARA_RAM,PARA_SIZE+2);
	write_vars('P');
	move_parameter();
}
/*********************************************************************/
void system_decode(u8 dat){
	com3dat = dat;
//savcmd0(com3dat);
	//printf0("%c", dat);
printf0("%2x",com3dat);
	if(!rx3_cnt){
		if(com3dat=='*'){ 
			COMCMD = com3dat;
			rx3_cnt  =1;  
			rx3_wait = RX3_WAIT;
//printf0("\r Start#%2x",com3dat);
		}else if((rx3_step==RX0_1st)&&(com3dat=='$')){ 
			rx3_step = RX0_2nd;
			flag_flash = 3; 
			printf0("\r Get1 $");
		}
	}else if(rx3_cnt==1){
//printf0("\r 1'st #%2x",com3dat);
		switch(com3dat){
			case COM_SYS_INFO:		                  
			case COM_SYS_ID_SET:		                  
			case COM_MEAS_RESULT:	                  
			case COM_USERID_SET:		                  
			case COM_OPERID_SET:		                  
			case COM_POSITION_READ:	                  
			case COM_POSITION_WRITE:	                  
			case COM_PARA_READ:		                  
			case COM_PARA_WRITE:	                  
			case COM_STRIP_INFO_READ:	                  
			case COM_STRIP_INFO_WRITE:	                  
			case COM_STRIP_NO_RD:	                  
			case COM_STRIP_NO_WR:	                  
			case COM_FULL_RAW_DATA:	                  
			case COM_TIME_READ:		                  
			case COM_TIME_WRITE:	                  
			case COM_VERSION_READ:	                  
			case COM_PFORM_READ:
			case COM_PFORM_WRITE:
			case COM_CAL_SEND:COMCMD = com3dat; rx3_cnt=2;	break;                          
			default: 			rx3_cnt=rx3_wait=0; 			break;
		}
	}else{
//printf0("\r 2'nd #%2x,%2d",com3dat,rx3_cnt);
		switch(COMCMD){
			case COM_SYS_INFO:			com_sys_info();			break;
			case COM_SYS_ID_SET:		com_sys_id_set();		break;
			case COM_MEAS_RESULT:		com_meas_result();		break;
			case COM_USERID_SET:		com_userid_set();		break;
			case COM_OPERID_SET:		com_operid_set();		break;
			case COM_POSITION_READ:		com_position_read();	break;
			case COM_POSITION_WRITE:	com_position_write();	break;
			case COM_PARA_READ:			com_para_read();		break;
			case COM_PARA_WRITE:		com_para_write();		break;
			case COM_STRIP_INFO_READ:	com_strip_info_read();	break;
			case COM_STRIP_INFO_WRITE:	com_strip_info_write();	break;
			case COM_STRIP_NO_RD:		com_strip_no_read();	break;
			case COM_STRIP_NO_WR:		com_strip_no_write();	break;
			case COM_FULL_RAW_DATA:		com_full_raw_data(); 	break;
			case COM_TIME_READ:			com_time_read();		break;
			case COM_TIME_WRITE:		com_time_write();		break;
			case COM_VERSION_READ:		com_version_read();		break;
			case COM_PFORM_READ:		com_pform_read();		break;
			case COM_PFORM_WRITE:		com_pform_write();		break;
			case COM_CAL_SEND:			com_cal_send();			break;
			//case COM_SYS_CHECK:			com_sys_check();		break;	
			default: rx3_cnt=rx3_wait=0; 						break;
		}
	}
}
/********************************************/
//                    123456789012

void com_sys_info(void){		
u32 i;
	if((rx3_cnt==2)&&(com3dat==URLF)){
		savcmd3('*');
		savcmd3(0x51);
		for(i=0;i<11;i++) savcmd3(SYS_ID[i]);
		for(i=0;i<5;i++) savcmd3(Version[i]);
		savcmd3(URLF);
		printf0("\r Send sys id.+Version");
		key_buzzer();
	}
	rx3_wait=rx3_cnt=0;
}
/********************************************/
void com_sys_id_set(void){
u32 i;
	if(rx3_cnt<(2+MAX_SYSID)){
		if(rx3_cnt>=2) cmdbuff[rx3_cnt-2]=com3dat;
		rx3_cnt++;
	}else if((com3dat==URLF)&&(rx3_cnt==(MAX_SYSID+2))){
		savcmd3('*');
		savcmd3(0x52);
		for(i=0;i<MAX_SYSID;i++) savcmd3(cmdbuff[i]);
		savcmd3(URLF);
		rx3_wait=rx3_cnt=0;
		key_buzzer();
		store_sysid(1);
	}else rx3_wait=rx3_cnt=0;
}
/********************************************/
void com_meas_result(void){
u16 i;
	if((rx3_cnt==2)&&(com3dat==URLF)){
printf0("\r Send Meas_result");		
		//StoreResult�ÿ� cmdbuff�� data�� ������ �ξ���.
		for(i=0;i<(RESULT_SIZE-1);i++){ savcmd3(cmdbuff[i]); }
		if((buffRESULT[0]=='*')&&(buffRESULT[1]==0x54)){
			for(i=0;i<RESULT_SIZE;i++){ savcmd3(buffRESULT[i]); }
		}else savcmd3(URLF);
		wait_tx3();
		key_buzzer();
	}
	rx3_wait=rx3_cnt=0;
}

/********************************************/
void com_userid_set(void){
	if(rx3_cnt<(2+22)){
		if(rx3_cnt>=2) cmdbuff[rx3_cnt-2]=com3dat;
		rx3_cnt++;
	}else if(com3dat==URLF){
		savcmd3('*');
		savcmd3(0x55);
		savcmd3(URLF);
		key_buzzer();
		store_userid();
		rx3_wait=rx3_cnt=0;
	}else rx3_wait=rx3_cnt=0;		
}
/********************************************/
void com_operid_set(void){
u16 i;
	if(rx3_cnt<(2+10)){
		if(rx3_cnt>=2) cmdbuff[rx3_cnt-2]=com3dat;
		rx3_cnt++;
	}else if(com3dat==URLF){
		savcmd3('*');
		savcmd3(0x56);
		savcmd3(URLF);
		key_buzzer();
//		store_operid();
		for(i=0;i<MAX_OPERID;i++) OPER_ID[i] = cmdbuff[i];
		OPER_ID[i++] = 0x00;
		fill_space(OPER_ID, MAX_OPERID);
		printf0("\r Oper ID:%s", OPER_ID);
		rx3_wait=rx3_cnt=0;
	}else rx3_wait=rx3_cnt=0;		
}

/********************************************/
void com_position_read(void){
printf0("\r POSITION_READ : %d", rx3_cnt);
	if((rx3_cnt==2)&&(com3dat==URLF)){
		send_position();	
		key_buzzer();
	}
	rx3_wait=rx3_cnt=0;
}
/********************************************/
void com_position_write(void){
	if(rx3_cnt<(2+POSITION_SIZE)){
		cmdbuff[rx3_cnt-2] = com3dat;
//savcmd0(com3dat);
		rx3_cnt++;
		rx3_wait=RX3_WAIT;
	}else if(com3dat==URLF){
		savcmd3('*');	savcmd3(0x62);	savcmd3(URLF);
		store_position();
		rx3_wait=rx3_cnt=0;
		key_buzzer();
	}else rx3_wait=rx3_cnt=0;		
}
/********************************************/
/********************************************/
void com_pform_read(void){
u16 i;	
	if((rx3_cnt==2)&&(com3dat==URLF)){
printf0("\r SEND PFORM_READ : %d", rx3_cnt);
		savcmd3('*');
		savcmd3(COM_PFORM_READ);
		for(i=0;i<PFORM_SIZE;i++){
			savcmd3( *(u8*)(EXT_BUFF+PFORM_RAM+i) );		
//			printf0("%c", *(u8*)(EXT_BUFF+PFORM_RAM+i) );		
		}
		savcmd3(URLF);	
		key_buzzer();
		wait_tx3();
		rx3_wait=rx3_cnt=0;
	}else rx3_wait=rx3_cnt=0;		
}
/********************************************/
void com_pform_write(void){
u16 i,j;
	if(rx3_cnt<(2+PFORM_SIZE)){
		if(rx3_cnt==2) printf0("\r Resive print from ");
		if(!(rx3_cnt&0xFF)) printf0("~");
		if(rx3_cnt>=2) cmdbuff[rx3_cnt-2] = com3dat;
		rx3_cnt++;
		rx3_wait=RX3_WAIT;
//printf0("%2x",com3dat);				//4044ea
	}else if(com3dat==URLF){
		printf0("\r Com print form write");
		savcmd3('*');	savcmd3(0x72);	savcmd3(URLF);
		j=0;
		for(i=0;i<PFORM_SIZE;i++){
			*(u8*)(EXT_BUFF+PFORM_RAM+i) = cmdbuff[i];
			j += cmdbuff[i];
		}
		j = ~j;
		*(u8*)(EXT_BUFF+PFORM_RAM+i+0) = j>>8;
		*(u8*)(EXT_BUFF+PFORM_RAM+i+1) = j;
		printf0("\t SumB:%4x", j);
		store_sflash(PFORM_RAM,PFORM_SIZE+2);
		backup_intflash(PFORM_RAM,PFORM_SIZE+2);
		write_vars('F');
		rx3_wait=rx3_cnt=0;
		key_buzzer();
		move_printform();
	}else rx3_wait=rx3_cnt=0;		
}
/********************************************/
void com_time_write(void){
	if(rx3_cnt<14){
		cmdbuff[rx3_cnt]=com3dat;
		rx3_cnt++;
	}else if((com3dat==URLF)&&(rx3_cnt==14)){
		tempyear = cmdbuff[2]-'0';
		tempyear = (tempyear*10) + (cmdbuff[3]-'0');
		tempmonth= cmdbuff[4]-'0';
		tempmonth= (tempmonth*10) + (cmdbuff[5]-'0');
		tempday  = cmdbuff[6]-'0';
		tempday  = (tempday*10) + (cmdbuff[7]-'0');
		temphour = cmdbuff[8]-'0';
		temphour = (temphour*10) + (cmdbuff[9]-'0');
		tempmin  = cmdbuff[10]-'0';
		tempmin  = (tempmin*10) + (cmdbuff[11]-'0');
		tempsec  = cmdbuff[12]-'0';
		tempsec  = (tempsec*10) + (cmdbuff[13]-'0');
		//printf0("\r Set Time : %2d-%2d-%2d,%2d:%2d:%2d", setyear, setmonth, setday, sethour, setmin, setsec);
		sendAll_RTC();		
		savcmd3('*');	
		savcmd3(COM_TIME_WRITE);	
		savcmd3(URLF);
		key_buzzer();
		rx3_wait=rx3_cnt=0;
		dsp_date();
	}else rx3_wait=rx3_cnt=0;		
}

/********************************************/
void com_version_read(void){
u16 i;
printf0("\r Version_READ : %d", rx3_cnt);
	if((rx3_cnt==2)&&(com3dat==URLF)){
printf0(" ==> Send Version : %d", rx3_cnt);
		savcmd3('*');
		savcmd3(COM_VERSION_READ);
		for(i=0;i<12;i++) savcmd3(Version[i]);
		savcmd3(URLF);
		key_buzzer();
		rx3_wait=rx3_cnt=0;
	}else rx3_wait=rx3_cnt=0;		
}

/********************************************/
void com_time_read(void){
u32 i;
	if((rx3_cnt==2)&&(com3dat==URLF)){
		printf0("\r SEND Time : %2d-%2d-%2d,%2d:%2d:%2d", year, month, day, hour, min, sec);
		savcmd3('*');
		savcmd3(COM_TIME_READ);
		i = bin2bcdasc4(year);	savcmd3(i>>8);	savcmd3(i);
		i = bin2bcdasc4(month);	savcmd3(i>>8);	savcmd3(i);
		i = bin2bcdasc4(day);	savcmd3(i>>8);	savcmd3(i);
		i = bin2bcdasc4(hour);	savcmd3(i>>8);	savcmd3(i);
		i = bin2bcdasc4(min);	savcmd3(i>>8);	savcmd3(i);
		i = bin2bcdasc4(sec);	savcmd3(i>>8);	savcmd3(i);
		savcmd3(URLF);
		key_buzzer();
		rx3_wait=rx3_cnt=0;
	}else rx3_wait=rx3_cnt=0;		
}

/********************************************/
void com_para_read(void){
	if((rx3_cnt==2)&&(com3dat==URLF)){
		send_para();
		key_buzzer();
	}
	rx3_wait=rx3_cnt=0;
}
/********************************************/
void com_para_write(void){
	if(rx3_cnt<(2+PARA_SIZE)){
		if(rx3_cnt>=2) cmdbuff[rx3_cnt-2]=com3dat;
		rx3_cnt++;
		rx3_wait=RX3_WAIT;
	}else if(com3dat==URLF){
		printf0("\r Com para write");
		savcmd3('*');	savcmd3(0x64);	savcmd3(URLF);
		store_para();
		rx3_wait=rx3_cnt=0;
		key_buzzer();
	}else rx3_wait=rx3_cnt=0;		
}
/********************************************/
void com_strip_info_read(void){
	if((rx3_cnt==2)&&(com3dat==URLF)){
		send_strip();	
		key_buzzer();
	}
	rx3_wait=rx3_cnt=0;
}
/********************************************/
void com_strip_info_write(void){
	if(rx3_cnt<(2+STRIP_INFO_SIZE)){
		if(rx3_cnt>=2) cmdbuff[rx3_cnt-2]=com3dat;
		rx3_cnt++;
		rx3_wait=RX3_WAIT;
	}else if(com3dat==URLF){
		printf0("\r Com_strip_write");
		savcmd3('*');	savcmd3(0x66);	savcmd3(URLF);
		store_strip_info();
		rx3_wait=rx3_cnt=0;
		key_buzzer();
	}else rx3_wait=rx3_cnt=0;		
}
/********************************************/
void com_strip_no_read(void){
u32 i;	
	if((rx3_cnt==2)&&(com3dat==URLF)){
		savcmd3('*');
		savcmd3(0x67);
		i = bin2hexasc4(STRIP_NO);
		savcmd3(i>>8);
		savcmd3(i>>0);
		savcmd3(URLF);
		key_buzzer();
	}
	rx3_wait=rx3_cnt=0;
}
/********************************************/
void com_strip_no_write(void){
	if(rx3_cnt<(2+2)){
		if(rx3_cnt>=2) cmdbuff[rx3_cnt-2]=com3dat;
		rx3_cnt++;
	}else if(com3dat==URLF){
		printf0("\r StripNO_write");
		savcmd3('*');	savcmd3(0x68);	savcmd3(URLF);
		rx3_wait=rx3_cnt=0;
		store_strip_no();
		key_buzzer();
	}else rx3_wait=rx3_cnt=0;		
}
/********************************************/
void com_full_raw_data(void){		//X3�Ͽ� ��ճ� ����Ÿ�� ������.
u32 i, j;
	if((rx3_cnt==2)&&(com3dat==URLF)){
		key_buzzer();
		savcmd3('*');
		savcmd3(0x69);
		for(i=0;i<MaxRawData;i++){
			j = bin2hexasc4(spanRED[i]);
			savcmd3(j>>24);	savcmd3(j>>16);	savcmd3(j>>8); savcmd3(j>>0);
			j = bin2hexasc4(spanGRN[i]);
			savcmd3(j>>24);	savcmd3(j>>16);	savcmd3(j>>8); savcmd3(j>>0);
			j = bin2hexasc4(spanBLU[i]);
			savcmd3(j>>24);	savcmd3(j>>16);	savcmd3(j>>8); savcmd3(j>>0);
			wait_tx3();
		}		
		savcmd3(URLF);
		key_buzzer();
	}
	rx3_wait=rx3_cnt=0;
}
/********************************************/
void send_position(void){
u16 k;
printf0("\r SEND POSITION : %d", rx3_cnt);
	savcmd3('*');
	savcmd3(COM_POSITION_READ);
	for(k=0;k<POSITION_SIZE;k++){ 
		savcmd3(*(u8*)(EXT_BUFF+POSITION_RAM+k)); 
		wait_tx3();
	}
	savcmd3(URLF);	
	rx3_wait=rx3_cnt=0;
}
/********************************************/
void send_para(void){
u16 j;
printf0("\r Send Para");
	savcmd3('*');
	savcmd3(COM_PARA_READ);
	for(j=0;j<PARA_SIZE;j++){
		savcmd3(*(u8*)(EXT_BUFF+PARA_RAM+j)); 
		wait_tx3();
	}
	savcmd3(URLF);	
	rx3_wait=rx3_cnt=0;
}
/********************************************/
void send_strip(void){
u16 i;
printf0("\r Send Strip");
	savcmd3('*');
	savcmd3(COM_STRIP_INFO_READ);
	for(i=0;i<STRIP_INFO_SIZE;i++){ 
		savcmd3(*(u8*)(EXT_BUFF+STRIP_INFO_RAM+i));
		wait_tx3();
	}
	savcmd3(URLF);
	rx3_wait=rx3_cnt=0;
}
/********************************************/
void com_cal_send(void){
	unsigned int i,k;
	for(i=0;i<MaxRawData;i++){	k = ref_red_buf[i];	savcmd3(k>>8);	savcmd3(k>>0);	}
	for(i=0;i<MaxRawData;i++){	k = ref_grn_buf[i];	savcmd3(k>>8);	savcmd3(k>>0);	}
	for(i=0;i<MaxRawData;i++){	k = ref_blu_buf[i];	savcmd3(k>>8);	savcmd3(k>>0);	}
}
/********************************************/
void com_msg(char *fmt,...)
{
	if(OUT_MSG) printf0(fmt);
}
/********************************************/
void send_meas_step(u8 step){
	/*savcmd3('*');				//STEP1,2,3,4,5 ????
	savcmd3(0x53);
	savcmd3(step);
	savcmd3(URLF);*/
}
/********************************************/
void chk_com_dly(void){
	if(rx3_wait){
		if(!(--rx3_wait)){
			printf0("\r Com3 Time over! [cmd:%c]", COMCMD);
			switch(COMCMD){
				case COM_SYS_INFO			:	printf0(" read system info.");	break;
				case COM_SYS_ID_SET			:	printf0(" write system info.");	break;
				case COM_MEAS_RESULT		:	printf0(" read meas. result");	break;
				case COM_USERID_SET			:	printf0(" write user ID.");		break;
				case COM_OPERID_SET			:	printf0(" write oper ID.");		break;
				case COM_TIME_WRITE			:	printf0(" write Time & date");	break;
				case COM_VERSION_READ		:	printf0(" Read version");		break;
				case COM_TIME_READ			:	printf0(" Read Time & date");	break;
				case COM_POSITION_READ		:	printf0(" Read Position");		break;
				case COM_POSITION_WRITE		:	printf0(" Write Position");		break;
				case COM_PARA_READ			:	printf0(" Read Parameter");		break;
				case COM_PARA_WRITE			:	printf0(" Write Parameter");	break;
				case COM_STRIP_INFO_READ	:	printf0(" Read Strip info.");	break;
				case COM_STRIP_INFO_WRITE	:	printf0(" Write Strip info.");	break;
				case COM_STRIP_NO_RD		:	printf0(" Read Strip No.");		break;
				case COM_STRIP_NO_WR		:	printf0(" Write Strip No.");	break;
				case COM_FULL_RAW_DATA		:	printf0(" Read Full Raw data");	break;
				//case COM_SYS_CHECK			:	printf0(" Run System Check");	break;
				case COM_CAL_SEND			:	printf0(" Send Calibration Data");	break;
				case COM_PFORM_READ			:	printf0(" Read Print form");	break;
				case COM_PFORM_WRITE		:	printf0(" Write Print form");	break;
				default:						printf0(" Known Command");		break;
			}
			printf0(" [cnt:%d]", rx3_cnt);
			COMCMD = rx3_cnt = 0;
		}
	}
}
/********************************************/
void fill_space(char *str, short cnt){
u16 i;
	for(i=0;i<cnt;i++){
		if((str[i]<'0')||(str[i]>'Z')||((str[i]>'9')&&(str[i]<'A'))) str[i] = ' ';	
	}
}
/********************************************/
void fill_space2(char* str, short cnt) {
	u16 i;
	for (i = 0; i < cnt; i++) {
		if ((str[i] < '!') || (str[i] > '~')) str[i] = ' ';
	}
}
/********************************************/
/********************************************/
/********************************************/
