#define	_EEP_C_

#include <string.h>
#include "common\32f429_port.h"
#include "util\usart.h"
#include "util\tm_adc.h"
#include "util\para.h"
#include "util\eep.h"
#include "util\lcd.h"
#include "touch.h"
#include "com.h"
#include "dsp.h"
#include "sysadj.h"
#include "resm.h"
#include "meas.h"

u16 init_err;
u16	sum_ext, sum_int;
u16	flag_backup;

void clr_sys_id(void);
short chk_sum_eep(void);
short chk_sum_ext_buff(u16 adr, u16 size);
short chk_sum_int_buff(u16 adr, u16 size);
void move_int2ext(u16 adr, u16 size);			//Int==>Ext
u8   asc2hex(u8 asc);
int write_intflash(u8 cmd);
u16 read_ram4asc(u16 pnt);
u16 read_seqno(void);

//=======================================================
void initEEP(void) {
	u16 i, j;
	short sum;
	printf0("\r Check Sflash memory");
	readFR4k(POSITION_RAM, POSITION_SIZE + 2);
	for (i = 0; i < POSITION_SIZE + 2; i++)  *(u8*)(EXT_BUFF + POSITION_RAM + i) = flash_read[i];
	readFR4k(STRIP_INFO_RAM, STRIP_INFO_SIZE + 2);
	for (i = 0; i < STRIP_INFO_SIZE + 2; i++)  *(u8*)(EXT_BUFF + STRIP_INFO_RAM + i) = flash_read[i];
	readFR4k(PARA_RAM, PARA_SIZE + 2);
	for (i = 0; i < PARA_SIZE + 2; i++)   *(u8*)(EXT_BUFF + PARA_RAM + i) = flash_read[i];
	readFR4k(PFORM_RAM, SECTOR_SIZE);
	for (i = 0; i < SECTOR_SIZE; i++)  *(u8*)(EXT_BUFF + PFORM_RAM + i) = flash_read[i];
	readFR4k(PFORM_RAM + SECTOR_SIZE, PFORM_SIZE + 2 - SECTOR_SIZE);
	for (i = 0; i < PFORM_SIZE + 2 - SECTOR_SIZE; i++)  *(u8*)(EXT_BUFF + PFORM_RAM + i + SECTOR_SIZE) = flash_read[i];

	readFR4k(SYSTEM_RAM, SYSTEM_SIZE + 2);									//External Serial
	for (i = 0; i < SYSTEM_SIZE + 0; i++) *(u8*)(EXT_BUFF + SYSTEM_RAM + i) = flash_read[i];
	for (i = 0; i < STORE_SIZE; i++) *(u8*)(INT_BUFF + i) = *(u8*)(STORE_ROM + i);		//Internal Backup

	flag_backup = 0;
	i = read24w(ID55AA_ADR);
	sum = chk_sum_ext_buff(ID55AA_ADR, ID_CHKSUM);

	if ((i == ID55AA) && (sum == OK)) {
		printf0("  Correct ID:%4x", i);
		i = *(u8*)(INT_BUFF + 0);
		i = (i << 8) + *(u8*)(INT_BUFF + 1);
		sum = chk_sum_int_buff(ID55AA_ADR, ID_CHKSUM);
		
		if ((i != ID55AA) || (sum != OK) || (sum_ext != sum_int)) {
			prt_msg("\r Backup System Data.");
			backup_intflash(ID55AA_ADR, SYSTEM_SIZE);
			flag_backup |= 0x0100;
		}

	}
	else {
		i = *(u8*)(INT_BUFF + 0);
		i = (i << 8) + *(u8*)(INT_BUFF + 1);
		sum = chk_sum_int_buff(ID55AA_ADR, ID_CHKSUM);
		if ((i == ID55AA) && (sum == OK)) {
			printf0("\r Restore System Data.");
			restore_sflash(SYSTEM_RAM, SYSTEM_SIZE + 2);		//ID55AA_ADR
		}
		else {
			printf0("\r Initialize System");
			key_buzzer3();
			default_id();
		}
	}

	sum = 0;
	for (i = 0; i < MAX_SYSID; i++) sum += SYS_ID[i];
	j = SYS_ID[i + 1];
	j = (j << 8) + SYS_ID[i + 2];
	if (sum != j) clr_sys_id();
	sum = 0;

	read_eep_all();
	//STRIP_NO = 16; 
	//write24w(STRIPNO_ADR, STRIP_NO); flag_backup=1;
	//sprintf(USER_ID, "1234567890");
	//sprintf(OPER_ID, "abcdefghij");

	printf0("\r ==================================");
	init_err = 0;
	chk_ini_position();
	chk_ini_strip();
	chk_ini_para();
	chk_ini_printform();
	if (init_err) {
		printf0("\r  [not initialized parameter:%d]", init_err);
		delay_ms(100 + (init_err * 50));
	}
	if (flag_backup) {
		prt_msg("\r flag_backup : 0x%4x", flag_backup);
		for (i = 0; i < 10; i++) {
			if (write_intflash('V') == OK) {
				prt_msg(" WriteSuccess!!!");
				break;
			}
			else prt_msg("\r *** WriteErro:%d ***", i);
		}
	}
	read_ref_data();
}

//=======================================================
short chk_sum_eep(void) {
	u16 w1, w2;
	w2 = 0;
	for (w1 = ID55AA_ADR; w1 < ID_CHKSUM; w1++) w2 += read24(w1);
	w2 = ~w2;
	w1 = read24w(ID_CHKSUM);
	if (w1 != w2) {
		prt_msg("\r Error chksum. SUM:%4x, EEP:%4x", w1, w2);
		return(NG);
	}
	else return(OK);
}

//=======================================================
void read_eep_all(void) {
	u16 i, j, k;
	printf0("\r ********  Read I.D. All  *********");
	for (i = 0; i < MAX_SYSID; i++) SYS_ID[i] = read24(SYSID_ADR + i);
	SYS_ID[i] = 0;
	//	prt_msg("\r SysID:%s", SYS_ID);
	j = 0;
	for (i = 0; i < OPERATOR_SIZE; i++) {
		oper_buff[i] = read24(OPERATOR_ADR + i);
		j += read24(OPERATOR_ADR + i);
	}
	k = read24(OPERATOR_ADR + i + 0);
	k = (k << 8) + read24(OPERATOR_ADR + i + 1);
	if (j != k) {
		for (i = 0; i < OPERATOR_SIZE; i++) oper_buff[i] = ' ';
		printf0("\r operator NG! %4x-%4x", j, k);
	}
	else printf0("\r operator OK!");
	oper_no = oper_cursor = oper_buff[OPERATOR_SIZE - 1] - '0';
	j = oper_cursor * (MAX_OPERID + 18);
	if((j+MAX_OPERID)>OPERATOR_SIZE) j = OPERATOR_SIZE - MAX_OPERID;//201222 추가
	for (i = 0; i < MAX_OPERID; i++) OPER_ID[i] = oper_buff[i + j];
	OPER_ID[i] = 0;

	clr_user_id();						//prt_msg("\r UserID:%s", USER_ID);

	STRIP_NO = read24w(STRIPNO_ADR) & 0xFF;
	prt_msg("\r StripNo:%d", STRIP_NO);
	if (STRIP_NO >= 19) {
		STRIP_NO = 0; write24w(STRIPNO_ADR, STRIP_NO);
		prt_msg("  Reset StripNo:%d", STRIP_NO);
		write_id_chksum();
	}

	UNIT_FLAG = read24w(UNIT_ADR) & 0xFF;
	if (UNIT_FLAG == UNIT_CONV) prt_msg("  unit:Conv.");
	else if (UNIT_FLAG == UNIT_SI) prt_msg("  unit:SI.");
	else prt_msg("  unit:AR.");

	if(UNIT_FLAG>2 || UNIT_FLAG == 0){
		UNIT_FLAG=1;
		write24w(UNIT_ADR, UNIT_FLAG);
	}

	COLOR_NO = 0;
	TURBI_NO = 0;

	SERIAL_FLAG = read24w(SERIAL_ADR);
	if(SERIAL_FLAG>8){
		SERIAL_FLAG=0;
		write24w(SERIAL_ADR, SERIAL_FLAG);
	}
	if (!SERIAL_FLAG)		prt_msg("\r Serial data : Raw Data");
	else if (SERIAL_FLAG == 1)	prt_msg("\r Serial data : Result");
	else 					prt_msg("\r Serial data : Print");

	SBAUD_FLAG = read24w(SBAUD_ADR);
	if(SBAUD_FLAG>2){
		SBAUD_FLAG=0;
		write24w(SBAUD_ADR, SBAUD_FLAG);
	}
	set_baud_usb();
	
	if (SBAUD_FLAG == 0) 	  prt_msg("  SBaudrate:9600");
	else if (SBAUD_FLAG == 1) prt_msg("  SBaudrate: 38400");
	else { SBAUD_FLAG = 2;  prt_msg("  SBaudrate:  115200"); }

	BAUD_FLAG = read24w(BAUD_ADR);
	if(BAUD_FLAG>2){
		BAUD_FLAG=0;
		write24w(BAUD_ADR, BAUD_FLAG);
	}
	set_baud_rs232();
	if (BAUD_FLAG == 0) 	  prt_msg("  Baudrate:9600");
	else if (BAUD_FLAG == 1) prt_msg("  Baudrate: 38400");
	else if (BAUD_FLAG == 2)  prt_msg("  Baudrate:  115200");

	SOUND_FLAG = read24w(SOUND_ADR);
	if(SOUND_FLAG>1){
		SOUND_FLAG=0;
		write24w(SOUND_ADR, SOUND_FLAG);
	}
	if (SOUND_FLAG) prt_msg("\r Sound:On");
	else prt_msg("\r Sound:Off");

	AUTO_FLAG = read24w(AUTO_ADR);;
	if(AUTO_FLAG>2){
		AUTO_FLAG=0;
		write24w(AUTO_ADR, AUTO_FLAG);
	}
	if (AUTO_FLAG == DATA_AUTO) prt_msg("  Data:Auto");
	else prt_msg("  Data:Manual");
	//==========================================
	PRINT_FLAG=read24w(PRINT_ADR);				//Auto Print
	if(PRINT_FLAG>2){
		PRINT_FLAG=0;
		write24w(PRINT_ADR, PRINT_FLAG);
	}

	PLUS_FLAG = read24w(PLUS_ADR);
	
	if(PLUS_FLAG>2){
		PLUS_FLAG=0;
		write24w(PLUS_ADR, PLUS_FLAG);
	}
	if (PLUS_FLAG) prt_msg("  Plus:Enable");
	else prt_msg("  Plus Disable");

	MARK_FLAG = read24w(MARK_ADR);
	if(MARK_FLAG>1){
		MARK_FLAG=0;
		write24w(MARK_ADR, MARK_FLAG);
	}
	if (MARK_FLAG) prt_msg("  Mark:Yes");
	else prt_msg("  Mark No");

	CHK_REF = read24w(CHK_REF_ADR);
	prt_msg("  CHK_REF:%d", CHK_REF);
	if(CHK_REF>MAX_REF) CHK_REF = CHK_REF_INIT;
	//==========================================

	QC_NO = read24w(QC_NO_ADR);
	prt_msg("\r QC_NO:%d", QC_NO);
	if ((QC_NO < SRT_QC_NO) || (QC_NO > END_QC_NO+1)) QC_NO = SRT_QC_NO;

	SEQ_NO = read_seqno();
	prt_msg("     SEQ_NO:%d", SEQ_NO);
	if (SEQ_NO > MAX_RECORD+1) {
		prt_msg("   Reset SEQ_NO:%d", SEQ_NO);
		SEQ_NO = 1;
	}
	datePresentation = read24w(DATEPRESENTATIONADR);
	if(datePresentation>1){
		datePresentation=0;
		write24w(DATEPRESENTATIONADR, datePresentation);
	}

	timePresentation = read24w(TIMEPRESENTATIONADR);
	if(timePresentation>1){
		timePresentation=0;
		write24w(TIMEPRESENTATIONADR, timePresentation);
	}

	dateDelimiter = read24w(DATEDELIMITERADR);
	if(dateDelimiter != '/' && dateDelimiter != '-' && dateDelimiter != ','){
		dateDelimiter = '/';
		write24w(DATEDELIMITERADR, dateDelimiter);
	}

	timeDelimiter = read24w(TIMEDELIMITERADR);
	if(timeDelimiter != ':' && timeDelimiter != '-' && timeDelimiter != ','){
		timeDelimiter=':';  
		write24w(TIMEDELIMITERADR, timeDelimiter);
	}

	LANGUAGE_FLAG = read24w(LANGUAGE_ADR);
	if(LANGUAGE_FLAG>1){
		LANGUAGE_FLAG=0;
		write24w(LANGUAGE_ADR, LANGUAGE_FLAG);
	}
	printf0("\r LANGUAGE_FLAG : %d ", LANGUAGE_FLAG);
	if (LANGUAGE_FLAG > 0) prt_msg(" *BMP:second");
	else prt_msg(" *BMP:english");

	sampleIDFlag = 1;
}
//=======================================================
void store_seqno(void) {
	u16 i;
	flash_buff[0] = SEQ_NO >> 8;
	flash_buff[1] = SEQ_NO >> 0;
	i = ~SEQ_NO;
	flash_buff[2] = i >> 8;
	flash_buff[3] = i >> 0;
	writeFR4k(SEQNO_ADR, 0x0010);
	prt_msg(" [Store SEQ NO.%2x%2x-%2x%2x]", flash_buff[0], flash_buff[1], flash_buff[2], flash_buff[3]);
	temp_seqno[measCnt] = SEQ_NO;
}
//=======================================================
u16 read_seqno(void) {
	u16 i;
	readFR4k(SEQNO_ADR, 0x0010);
	SEQ_NO = flash_read[0];
	SEQ_NO = (SEQ_NO << 8) + flash_read[1];
	i = flash_read[2];
	i = (i << 8) + flash_read[3];
	i = ~i;
	if (SEQ_NO != i) {
		prt_msg("\r Error! SEQ NO.%4x-%4x", SEQ_NO, i);
		SEQ_NO = 1;
	}
	return(SEQ_NO);
}
//=======================================================
void default_id(void) {
	u16 i;
	for (i = 0; i < SYSTEM_SIZE; i++) *(u8*)(INT_BUFF + SYSTEM_RAM + i) = 0;
	prt_msg("\r ***** Set default I.D. *****");
	write24w(ID55AA_ADR, ID55AA);
	SEQ_NO = 1;
	store_seqno();

	UNIT_FLAG = UNIT_CONV;
	write24w(UNIT_ADR, UNIT_FLAG);
	STRIP_NO = 0;
	write24w(STRIPNO_ADR, STRIP_NO);
	QC_NO = SRT_QC_NO;
	write24w(QC_NO_ADR, QC_NO);
	PRINT_FLAG = PRINT_ENABLE;
	write24w(PRINT_ADR, PRINT_FLAG);
	PLUS_FLAG = 0;
	write24w(PLUS_ADR, PLUS_FLAG);
	MARK_FLAG = 0;
	write24w(MARK_ADR, MARK_FLAG);
	CHK_REF = 0;
	write24w(CHK_REF_ADR, CHK_REF);
	LANGUAGE_FLAG = 0;
	write24w(LANGUAGE_ADR, LANGUAGE_FLAG);
	SOUND_FLAG = 1;
	write24w(SOUND_ADR, SOUND_FLAG);
	BAUD_FLAG = 0;						//115200
	write24w(BAUD_ADR, BAUD_FLAG);
	SBAUD_FLAG = 0;						//115200
	write24w(SBAUD_ADR, SBAUD_FLAG);
	COLOR_NO = 0;
	write24w(COLOR_ADR, COLOR_NO);
	TURBI_NO = 0;
	write24w(TURBI_ADR, TURBI_NO);
	SERIAL_FLAG = 0;
	write24w(SERIAL_ADR, SERIAL_FLAG);
	AUTO_FLAG = DATA_AUTO;
	write24w(AUTO_ADR, AUTO_FLAG);
	COLOR_NO = 0;
	write24w(COLOR_ADR, COLOR_NO);
	TURBI_NO = 0;
	write24w(TURBI_ADR, TURBI_NO);
	set_default_RGB();
	write24w(DAC_ADR, 0X7777);

	write_id_chksum();

	backup_intflash(ID55AA_ADR, SYSTEM_SIZE);
	flag_backup |= 0x0001;
}

void clr_sys_id(void) {
	u8 i;
	for (i = 0; i < MAX_SYSID; i++) charbuff[i] = ' ';
	store_sysid(0);
	flag_backup |= 0x0001;
	printf0(" *Clear SYS_ID");
}
void clr_user_id(void) {
	u8 i;
	for (i = 0; i < MAX_USERID; i++) charbuff[i] = ' ';
	USER_ID[MAX_USERID] = 0;
}

//=======================================================
void write_id_chksum(void) {
	u16 i, sum;
	sum = 0;
	for (i = ID55AA_ADR; i < ID_CHKSUM; i++) sum += read24(i);
	write24w(ID_CHKSUM, ~sum);
	store_sflash(SYSTEM_RAM, SYSTEM_SIZE + 2);
}

//=======================================================
short chk_sum_ext_buff(u16 addr, u16 size) {
	u16 i, j, sum;
	prt_msg(" Eadd:0x%x-0x%x", addr, size);
	sum = 0;
	for (i = addr; i < (addr + size); i++) sum += *(u8*)(EXT_BUFF + i);
	j = *(u8*)(EXT_BUFF + i);	i++;
	j = (j << 8) + *(u8*)(EXT_BUFF + i);	i++;
	sum = ~sum;
	sum_ext = sum;
	if (j != sum) {
		prt_msg(" *NGsum:%4x-%4x", sum, j);
		return(NG);
	}
	else return(OK);
}

//=======================================================
#define OK_INIT	0x55AA
short chk_sum_int_buff(u16 addr, u16 size) {
	u16 i, j, sum;

	prt_msg(" Iadd:0x%x-0x%x", addr, size);
	sum = 0;
	for (i = addr; i < (addr + size); i++) sum += *(u8*)(INT_BUFF + i);
	j = *(u8*)(INT_BUFF + i);	i++;
	j = (j << 8) + *(u8*)(INT_BUFF + i);	i++;
	printf0("\r chk_sum Test");
	if (j == sum) return(OK_INIT);
	sum = ~sum;
	sum_int = sum;
	if (j != sum) {
		prt_msg(" *NGsum:%4x-%4x", sum, j);
		return(NG);
	}
	else{
		return(OK);
	}
}

//=======================================================
#define XS_OFF	34
#define YS_OFF	390

void chk_ini_position(void) {
	u16 i, j;
	short sum;
	prt_msg("\r chk_position");
	sum = chk_sum_ext_buff(POSITION_RAM, POSITION_SIZE);			//Int.Flash
	if (sum == OK) {
		prt_msg(" OK");
		sum = chk_sum_int_buff(POSITION_RAM, POSITION_SIZE);		//Ext.Serial
		if ((sum != OK) || (sum_ext != sum_int)) {
			prt_msg("\r Backup Position Data.");
			backup_intflash(POSITION_RAM, POSITION_SIZE + 2);
			flag_backup |= 0x0200;
		}
	}
	else {
		prt_msg(" (No ext.)");
		sum = chk_sum_int_buff(POSITION_RAM, POSITION_SIZE);		//cmp with Internal
//sum=0;
		if (sum == OK) {
			prt_msg("\r Restore Position Data.");
			//	for(i=0;i<(POSITION_SIZE+2);i++) prt_msg(" %2x,",INT_BUFF[POSITION_RAM+i]);
			restore_sflash(POSITION_RAM, POSITION_SIZE + 2);
			//			prt_msg("\r Restore Curve Data. OK!!!");
		}
		else {
			prt_msg("\r ***** Please, set up Position data!!! *****");
			if (sum != OK_INIT) {
				j = 0;
				for (i = 0; i < POSITION_SIZE; i++) {
					*(u8*)(INT_BUFF + POSITION_RAM + i) = ini_position[i];
					j += ini_position[i];
				}
				*(u8*)(INT_BUFF + POSITION_RAM + i) = j >> 8;
				*(u8*)(INT_BUFF + POSITION_RAM + i + 1) = j;
				prt_msg("\t Init. Position[SumB:%4x]", j);
				flag_backup |= 0x0002;
#ifdef ERR_MSG
				msg16(XS_OFF, YS_OFF, BLU, "Initialize Position!!!");
#endif
				key_buzzer2();
			}
			else {
				prt_msg("  [Running with default Position]");
#ifdef ERR_MSG
				msg16(XS_OFF, YS_OFF, BLU, "Please, set up Position!!!");
#endif
				//				key_buzzer(); 
			}
			init_err++;
			move_int2ext(POSITION_RAM, POSITION_SIZE + 2);
		}
	}

	move_position();
}
//=======================================================
#define INI_STRIP_INFO_SIZE		STRIP_INFO_SIZE
void chk_ini_strip(void) {
	u16 i, j;
	short sum;
	prt_msg("\r chk_ini_strip");
	sum = chk_sum_ext_buff(STRIP_INFO_RAM, STRIP_INFO_SIZE);
	if (sum == OK) {
		prt_msg(" OK");
		sum = chk_sum_int_buff(STRIP_INFO_RAM, STRIP_INFO_SIZE);
		if ((sum != OK) || (sum_ext != sum_int)) {
			prt_msg("\r Backup Strip Data.");
			backup_intflash(STRIP_INFO_RAM, STRIP_INFO_SIZE + 2);
			flag_backup |= 0x0400;
		}
	}
	else {
		prt_msg(" (No ext.)");
		sum = chk_sum_int_buff(STRIP_INFO_RAM, STRIP_INFO_SIZE);
		if (sum == OK) {
			prt_msg("\r Restore Strip Data.");
			restore_sflash(STRIP_INFO_RAM, STRIP_INFO_SIZE + 2);
		}
		else {
			prt_msg("\r ***** Please, set up Strip data!!! *****");
			if (sum != OK_INIT) {
				j = 0;
				for (i = 0; i < INI_STRIP_INFO_SIZE; i++) {
					*(u8*)(INT_BUFF + STRIP_INFO_RAM + i) = ini_strip[i];
					j += ini_strip[i];
				}
				*(u8*)(INT_BUFF + STRIP_INFO_RAM + i) = j >> 8;
				*(u8*)(INT_BUFF + STRIP_INFO_RAM + i + 1) = j;
				prt_msg("\t Init.Strip[SumB:%4x]", j);
				flag_backup |= 0x0004;
#ifdef ERR_MSG
				msg16(XS_OFF, YS_OFF + 18, BLU, "Initialize strip!!!");
#endif
				key_buzzer2();
			}
			else {
				prt_msg("  [Running with default Strip]");
#ifdef ERR_MSG
				msg16(XS_OFF, YS_OFF + 18, BLU, "Please, set up strip!!!");
#endif
				//				key_buzzer(); 
			}
			init_err++;
			move_int2ext(STRIP_INFO_RAM, STRIP_INFO_SIZE + 2);
		}
	}
	get_max_stripno();
	move_strip_info();
}
//=======================================================
void chk_ini_para(void) {
	u16 i, j;
	short sum;
	prt_msg("\r chk_ini_para");
	sum = chk_sum_ext_buff(PARA_RAM, PARA_SIZE);
	if (sum == OK) {
		prt_msg(" OK");
		sum = chk_sum_int_buff(PARA_RAM, PARA_SIZE);
		if ((sum != OK) || (sum_ext != sum_int)) {
			prt_msg("\r Backup Para Data.");
			backup_intflash(PARA_RAM, PARA_SIZE + 2);
			flag_backup |= 0x0800;
		}
	}
	else {
		prt_msg(" (No ext.)");
		sum = chk_sum_int_buff(PARA_RAM, PARA_SIZE);
		if (sum == OK) {
			prt_msg("\r Restore Para Data.");
			restore_sflash(PARA_RAM, PARA_SIZE + 2);
		}
		else {
			prt_msg("\r ***** Please, set up Parameter!!! *****");
			//sum=0;
			if (sum != OK_INIT) {
				j = 0;
				for (i = 0; i < WEIGHT_SIZE; i++) {
					*(u8*)(INT_BUFF + PARA_RAM + i) = ini_weight[i];
					j += ini_weight[i];
				}
				for (i = 0; i < (20 * 41); i++) {
					*(u8*)(INT_BUFF + PARA_RAM + WEIGHT_SIZE + i) = ini_para[i];
					j += ini_para[i];
				}
				*(u8*)(INT_BUFF + PARA_RAM + WEIGHT_SIZE + i) = j >> 8;
				*(u8*)(INT_BUFF + PARA_RAM + WEIGHT_SIZE + i + 1) = j;
				prt_msg("\t Init.Para[SumB:%4x]", j);
				flag_backup |= 0x0008;
#ifdef ERR_MSG
				msg16(XS_OFF, YS_OFF + 36, BLU, "Initialize Para!!!");
#endif
				key_buzzer2();
			}
			else {
				prt_msg("  [Running with default Parameter]");
#ifdef ERR_MSG
				msg16(XS_OFF, YS_OFF + 36, BLU, "Please, set up Para!!!");
#endif
				//				key_buzzer(); 
			}
			init_err++;
			move_int2ext(PARA_RAM, PARA_SIZE + 2);
		}
	}
	move_parameter();
}

//====================================================================
void chk_ini_printform(void) {
	u16 i, j, k, m;
	short sum;
	prt_msg("\r chk PrintForm");
	sum = chk_sum_ext_buff(PFORM_RAM, PFORM_SIZE);
	//sum=NG;	
	if (sum == OK) {
		prt_msg(" OK");
		sum = chk_sum_int_buff(PFORM_RAM, PFORM_SIZE);
		if ((sum != OK) || (sum_ext != sum_int)) {
			prt_msg("\r Backup printform Data.");
			backup_intflash(PFORM_RAM, PFORM_SIZE + 2);
			flag_backup |= 0x1000;
		}
	}
	else {
		prt_msg(" (No ext.)");
		sum = chk_sum_int_buff(PFORM_RAM, PFORM_SIZE);
		//sum=0;
		if (sum == OK) {
			prt_msg("\r Restore printform Data.");
			restore_sflash(PFORM_RAM, PFORM_SIZE + 2);
		}
		else {
			prt_msg("\r ***** Please, set up print form data!!! *****");
			if (sum != OK_INIT) {
				m = PFORM_RAM;
				for (i = 0; i < 20; i++) {
					for (k = 0; k < 11; k++) { *(u8*)(INT_BUFF + m) = ar_printform[i][0][k]; m++; }
					for (j = 1; j <= 9; j++) { for (k = 0; k < 8; k++) { *(u8*)(INT_BUFF + m) = ar_printform[i][j][k]; m++; } }
				}
				for (i = 0; i < 20; i++) {
					for (k = 0; k < 11; k++) { *(u8*)(INT_BUFF + m) = si_printform[i][0][k]; m++; }
					for (j = 1; j <= 9; j++) { for (k = 0; k < 8; k++) { *(u8*)(INT_BUFF + m) = si_printform[i][j][k]; m++; } }
				}
				for (i = 0; i < 20; i++) {
					for (k = 0; k < 11; k++) { *(u8*)(INT_BUFF + m) = co_printform[i][0][k]; m++; }
					for (j = 1; j <= 9; j++) { for (k = 0; k < 8; k++) { *(u8*)(INT_BUFF + m) = co_printform[i][j][k]; m++; } }
				}

				j = 0;
				for (i = 0; i < PFORM_SIZE; i++) j += *(u8*)(INT_BUFF + PFORM_RAM + i);
				*(u8*)(INT_BUFF + PFORM_RAM + i) = j >> 8;
				*(u8*)(INT_BUFF + PFORM_RAM + i + 1) = j;
				prt_msg("\t Init.printform[SumB:%4x]", j);
				flag_backup |= 0x0010;
#ifdef ERR_MSG
				msg16(XS_OFF, YS_OFF + 54, BLU, "Initialize printform!!!");
#endif
				key_buzzer2();
			}
			else {
				prt_msg("  [Running with default Print form]");
#ifdef ERR_MSG
				msg16(XS_OFF, YS_OFF + 54, BLU, "Please, set up printform!!!");
#endif
				//				key_buzzer(); 
				//				for(i=0;i<PFORM_SIZE;i++) prt_msg("%c", EXT_BUFF[PFORM_RAM+i]);
			}
			init_err++;
			move_int2ext(PFORM_RAM, PFORM_SIZE + 2);
		}
	}
	move_printform();
}
/*********************************************************************
//====================================================================
//====================================================================
//====================================================================
*********************************************************************/
//====================================================================
// ����� �ý��ۺ����� �����޳��� �÷����� ����.
void write_vars(u8 cmd) {
	if (write_intflash('V') != OK) {
		if (write_intflash('V') != OK) {
			switch (cmd) {
			case 'S': printf0("\n ************** SYSTEM ID write NG *************");	break;
				//no use		case 'U': printf0("\n **************   USER ID write NG *************");	break;	
			case 'O': printf0("\n **************   OPER ID write NG *************");	break;
			case 'p': printf0("\n **************  Position write NG *************");	break;
			case 'N': printf0("\n **************  Strip No write NG *************");	break;
			case 'I': printf0("\n ************* Strip Info write NG *************");	break;
			case 'P': printf0("\n ************** Parameter write NG *************");	break;
			case 'F': printf0("\n ************* Print Form write NG *************");	break;
			}
		}
	}
}
/*******************************************************************************/
/*******************************************************************************/
/*******************************************************************************/
/*******************************************************************************/
u16 read_ram4asc(u16 pnt) {
	u16 dat4;
	dat4 = *(u8*)(EXT_BUFF + pnt + 0) - '0';
	dat4 = (dat4 * 10) + *(u8*)(EXT_BUFF + pnt + 1) - '0';
	dat4 = (dat4 * 10) + *(u8*)(EXT_BUFF + pnt + 2) - '0';
	dat4 = (dat4 * 10) + *(u8*)(EXT_BUFF + pnt + 3) - '0';
	return(dat4);
}
/*******************************************************************************/
u16 read_ram4(u16 pnt) {
	u16 dat4;
	dat4 = asc2hex(*(u8*)(EXT_BUFF + pnt + 0));
	dat4 = (dat4 << 4) + asc2hex(*(u8*)(EXT_BUFF + pnt + 1));
	dat4 = (dat4 << 4) + asc2hex(*(u8*)(EXT_BUFF + pnt + 2));
	dat4 = (dat4 << 4) + asc2hex(*(u8*)(EXT_BUFF + pnt + 3));
	return(dat4);
}
/*******************************************************************************/
void move_position(void) {
	u16 i, k, j;
	prt_msg("  move position....");
	k = POSITION_RAM;							// Position Data 
	for (i = 0; i < 16; i++) {
		j = asc2dec(*(u8*)(EXT_BUFF + k)); k++;
		j = (j * 10) + asc2dec(*(u8*)(EXT_BUFF + k)); k++;
		j = (j * 10) + asc2dec(*(u8*)(EXT_BUFF + k)); k++;
		j = (j * 10) + asc2dec(*(u8*)(EXT_BUFF + k)); k++;
		StripPosition[i] = j;
	}

		//for(j=0;j<16;j++) printf3("%4d,", StripPosition[j]);
}

/*******************************************************************************/
void move_strip_info(void) {
	int j, i;
	prt_msg("  move strip....no[%d]", STRIP_NO);
	if (STRIP_NO >= MAX_STRIPNO) {
		if (!MAX_STRIPNO) STRIP_NO = 0;
		else STRIP_NO = MAX_STRIPNO - 1;
	}
	for (j = 0; j < STRIP_OFFSET; j++) STRIP_INFO[j] = 0;
	i = STRIP_INFO_RAM + (STRIP_NO * STRIP_OFFSET) + 2;
	for (j = 0; j < STRIP_NAME_SIZE; j++) { strip_name[j] = *(u8*)(EXT_BUFF + i); i++; }
	strip_name[j] = 0;
	i = STRIP_INFO_RAM + (STRIP_NO * STRIP_OFFSET) + 2 + STRIP_NAME_SIZE;
	for (j = 0; j < (MAX_PAD_NO + 1); j++) { STRIP_INFO[j] = asc2hex(*(u8*)(EXT_BUFF + i)); i++; }
	if (STRIP_INFO[0] > MAX_PAD_NO) STRIP_INFO[0] = MAX_PAD_NO;

	prt_msg("\r move_strip_info-no:%d", STRIP_NO);
	prt_msg(" name:%s ", strip_name);
	for (j = 0; j < 15; j++) { 
		prt_msg(" %2x", STRIP_INFO[j]); 
		MEASstripinfo[j] = STRIP_INFO[j];
		delay_ms(20); 
	}
}
//==============================================================================
void get_strip_name(u16 tsno) {
	int j, i;
	i = STRIP_INFO_RAM + (tsno * STRIP_OFFSET) + 2;
	for (j = 0; j < STRIP_NAME_SIZE; j++) { buff_strip_name[j] = *(u8*)(EXT_BUFF + i); i++; }
	buff_strip_name[j] = 0;

}
//==============================================================================
void get_max_stripno(void) {
	u16 j;
	u8 b1, b2;
	for (j = 0; j < 20; j++) {
		b1 = *(u8*)(EXT_BUFF + STRIP_INFO_RAM + (j * STRIP_OFFSET));
		if ((b1 < '0') || (b1 > '9')) break;
		b2 = b1 - '0';
		b1 = *(u8*)(EXT_BUFF + STRIP_INFO_RAM + (j * STRIP_OFFSET) + 1);
		if ((b1 < '0') || (b1 > '9')) break;
		b2 = (b2 * 10) + (b1 - '0');
		if ((b2 == 0) || (b2 > 20)) break;
		b1 = *(u8*)(EXT_BUFF + STRIP_INFO_RAM + (j * STRIP_OFFSET) + 2);
		if (b1 == ' ') break;
	}
	MAX_STRIPNO = j;
	prt_msg("\r MAX_STRIPNO:%d", MAX_STRIPNO);
}
/*******************************************************************************/
void move_parameter(void) {		//including Weight Data
	u16 i, j, k, m;
	prt_msg("  move weight,");
	for (j = 0; j < WEIGHT_SIZE; j++)
	{
		weight[j] = *(u8*)(EXT_BUFF + PARA_RAM + j) - '0';
		//printf0("%d ", weight[j]);
	}
	//	for(j=0;j<WEIGHT_SIZE;j++) prt_msg(" %4d",weight[j]);	//delay_ms(500);
	prt_msg(" para....");
	i = PARA_RAM + WEIGHT_SIZE;								//�ʱ� 64bytes�� Weight Data.		

	for (j = 0; j < 20; j++)
	{
		PARA_NEW[j][0] = asc2dec(*(u8*)(EXT_BUFF + i));		// up or down
		i++;
		for (k = 1; k <= 10; k++) {
			//			PARA_NEW[j][k] = read_ram4(i); i+=4;
			m = asc2dec(*(u8*)(EXT_BUFF + i)); i++;
			m = (m * 10) + asc2dec(*(u8*)(EXT_BUFF + i)); i++;
			m = (m * 10) + asc2dec(*(u8*)(EXT_BUFF + i)); i++;
			m = (m * 10) + asc2dec(*(u8*)(EXT_BUFF + i)); i++;
			PARA_NEW[j][k] = m;
		}
	}
	//	for(j=0;j<20;j++){
	//		prt_msg("\r %d",PARA_NEW[j][0]);
	//		for(k=1;k<=10;k++) prt_msg(" %4d",PARA_NEW[j][k]);
	//		delay_ms(10);
	//	}
}

/*******************************************************************************/
void move_printform(void) {
	u16 i, j, k, m;
	prt_msg("  Move Print form");
	m = PFORM_RAM;
	for (i = 1; i < item_ram_size; i++) {
		for (k = 0; k < 11; k++) { ar_item_ram[i][0][k] = *(u8*)(EXT_BUFF + m); m++; }
		for (j = 1; j <= 9; j++) { for (k = 0; k < 8; k++) { ar_item_ram[i][j][k] = *(u8*)(EXT_BUFF + m); m++; } }
	}
	for (i = 1; i < item_ram_size; i++) {
		for (k = 0; k < 11; k++) { si_item_ram[i][0][k] = *(u8*)(EXT_BUFF + m); m++; }
		for (j = 1; j <= 9; j++) { for (k = 0; k < 8; k++) { si_item_ram[i][j][k] = *(u8*)(EXT_BUFF + m); m++; } }
	}
	for (i = 1; i < item_ram_size; i++) {
		for (k = 0; k < 11; k++) { co_item_ram[i][0][k] = *(u8*)(EXT_BUFF + m); m++; }
		for (j = 1; j <= 9; j++) { for (k = 0; k < 8; k++) { co_item_ram[i][j][k] = *(u8*)(EXT_BUFF + m); m++; } }
	}
#if	0
	for (i = 0; i < 20; i++) {
		printf0("\r MPF-ar:%2d ", i);
		for (k = 0; k < 11; k++) { printf0("%c", ar_item_ram[i][0][k]); delay_ms(1); }
		printf0(" ");
		for (j = 1; j <= 9; j++) { printf0(" "); for (k = 0; k < 8; k++) { printf0("%c", ar_item_ram[i][j][k]); delay_ms(1); } }
	}
	for (i = 0; i < 20; i++) {
		printf0("\r MPF-si:%2d ", i);
		for (k = 0; k < 11; k++) { printf0("%c", si_item_ram[i][0][k]); delay_ms(1); }
		printf0(" ");
		for (j = 1; j <= 9; j++) { printf0(" "); for (k = 0; k < 8; k++) { printf0("%c", si_item_ram[i][j][k]); delay_ms(1); } }
	}
	for (i = 0; i < 20; i++) {
		printf0("\r MPF-co:%2d ", i);
		for (k = 0; k < 11; k++) { printf0("%c", co_item_ram[i][0][k]); delay_ms(1); }
		printf0(" ");
		for (j = 1; j <= 9; j++) { printf0(" "); for (k = 0; k < 8; k++) { printf0("%c", co_item_ram[i][j][k]); delay_ms(1); } }
	}
#endif
}
/*******************************************************************************/
u16 back_parameter(void) {				//touch.c���� ���
	u32 ww;
	u16 i;
	u8  j, k, change, b1;
	prt_msg("\r back para....");
	change = 0;
	i = PARA_RAM + WEIGHT_SIZE;			//�ʱ� 20u8s�� Weight Data.		
	for (j = 0; j < 20; j++) {					//was 13, was 11
		ww = PARA_NEW[j][0];
		b1 = bin2hexasc4(ww);
		if (*(u8*)(EXT_BUFF + i) != b1) { *(u8*)(EXT_BUFF + i) = b1; change = 1; } i++;
		for (k = 1; k <= 10; k++) {
			ww = PARA_NEW[j][k];
			ww = bin2hexasc4(ww);
			b1 = (ww >> 24) & 0xFF; if (*(u8*)(EXT_BUFF + i) != b1) { *(u8*)(EXT_BUFF + i) = b1; change = 1; } i++;
			b1 = (ww >> 16) & 0xFF; if (*(u8*)(EXT_BUFF + i) != b1) { *(u8*)(EXT_BUFF + i) = b1; change = 1; } i++;
			b1 = (ww >> 8) & 0xFF; if (*(u8*)(EXT_BUFF + i) != b1) { *(u8*)(EXT_BUFF + i) = b1; change = 1; } i++;
			b1 = (ww >> 0) & 0xFF; if (*(u8*)(EXT_BUFF + i) != b1) { *(u8*)(EXT_BUFF + i) = b1; change = 1; } i++;
		}
		delay_ms(100);
	}
	//	change=0;
	return(change);
}

/********************************************************************************/
/***************     STRIP POSITION, PARAMETER, STRIP INFO.    ******************/
/********************************************************************************/
//====================================================================
void move_int2ext(u16 adr, u16 size) {			//Int==>Ext
	u16 i;
	for (i = 0; i < size; i++) {
		*(u8*)(EXT_BUFF + adr + i) = *(u8*)(INT_BUFF + adr + i);
	}
}
//====================================================================
void backup_intflash(u16 adr, u16 size) {		//Ext==>Int
	u16 i;
	for (i = 0; i < (size + 2); i++) {
		*(u8*)(INT_BUFF + adr + i) = *(u8*)(EXT_BUFF + adr + i);
	}
	flag_backup |= 0x8000;
}
//====================================================================
// ������ ���� �÷����� ����Ÿ�� �ܺ� �ø��� �÷����� ���
void restore_sflash(u16 adr, u16 size) {			//Int==>Ext
	u16 i;
	for (i = 0; i < size; i++)	*(u8*)(EXT_BUFF + adr + i) = *(u8*)(INT_BUFF + adr + i);
	store_sflash(adr, size);
}
//====================================================================
// �ܺ� �ø��� �÷��� ������
void store_sflash(u16 adr, u16 size) {
	u16 i;
	while (size > SECTOR_SIZE) {
		for (i = 0; i < SECTOR_SIZE; i++)	flash_buff[i] = *(u8*)(EXT_BUFF + adr + i);
		write_sflash(adr, SECTOR_SIZE);
		adr += SECTOR_SIZE;
		size -= SECTOR_SIZE;
	}
	for (i = 0; i < size; i++)	flash_buff[i] = *(u8*)(EXT_BUFF + adr + i);
	write_sflash(adr, size);
}
//====================================================================
void write_sflash(u32 adr, u16 size) {
	u16 i;
	for (i = 0; i < 10; i++) {
		writeFR4k(adr, size);
		if (cmpFR4k(adr, size) == OK) break;
	}
	if (i < 10) { prt_msg("\r Flash(ext) Write OK!"); }
	else { prt_msg("\r Flash(ext) Write NG!"); }
}
//====================================================================
/*********************************************************************/
void store_vars(u16 adr, u16 dat) {
	printf0("\r Store A:%d,D:%d", adr, dat);
	write24w(adr, dat);
	write_id_chksum();
	backup_intflash(SYSTEM_RAM, SYSTEM_SIZE + 2);
	write_vars('S');
}
/*********************************************************************/
void store_strip_no(void) {
	u32 i;
	i = cmdbuff[0] - '0';
	i = (i * 10) + cmdbuff[1] - '0';
	STRIP_NO = i;
	printf0(" No.%2d", STRIP_NO);
	printf0("\r store_strip_no:%d", STRIP_NO);

	write24w(STRIPNO_ADR, STRIP_NO);
	write_id_chksum();								// Ext buff ==> Sflash
	backup_intflash(SYSTEM_RAM, SYSTEM_SIZE);
	write_vars('N');
	//	get_max_stripno();
	//	move_strip_info();
}
/********************************************************************************/
void store_oper_buff(void) {
	u16 i, j;
	j = 0;
	for (i = 0; i < OPERATOR_SIZE; i++) {
		write24(OPERATOR_ADR + i, oper_buff[i]);
		j += oper_buff[i];
	}
	write24(OPERATOR_ADR + OPERATOR_SIZE + 0, j >> 8);
	write24(OPERATOR_ADR + OPERATOR_SIZE + 1, j >> 0);
	store_sflash(SYSTEM_RAM, SYSTEM_SIZE + 2);			// ID55AA_ADR
	backup_intflash(SYSTEM_RAM, SYSTEM_SIZE);
	write_vars('S');									// system_backup(ID55AA_ADR, ID_CHKSUM+2);
}
/********************************************************************************/
void store_pr_buff(void) {//parameter
/*u16 i=0, j=0;
u8  a, b;
	j = 0;
	for(a=0;a<15;a++){
		for(b=0;b<5;b++){
			write24(CHANGEPR_ADR+i, pr_buff[a][b]>>8); i++;
			write24(CHANGEPR_ADR+i, pr_buff[a][b]>>0); i++;
			j += pr_buff[a][b];
		}
	}
	write24(CHANGEPR_ADR+CHANGEPR_SIZE+0, j>>8);
	write24(CHANGEPR_ADR+CHANGEPR_SIZE+1, j>>0);
	printf0("\r pr-sum:%4x", j);
	store_sflash(SYSTEM_RAM, SYSTEM_SIZE+2);			// ID55AA_ADR
	backup_intflash(SYSTEM_RAM, SYSTEM_SIZE);
	write_vars('S');									// system_backup(ID55AA_ADR, ID_CHKSUM+2);
*/
	int i, j;
	u16 buff = 0;
	printf0("\r\nweight : ");
	for (i = 0; i < 20; i++)
	{
		cmdbuff[buff++] = weight[i] + '0';//weight 1����Ʈ 20��
//		printf3("%d", weight[i]);
	}

	for (i = 0; i < 20; i++)
	{
//		printf0("\r\nparameter%d : \r\n", i);
		cmdbuff[buff++] = PARA_NEW[i][0] + '0';//Up/Down Ȯ�� 1
//		printf3("%1d\n", PARA_NEW[i][0]);
		for (j = 1; j < 11; j++)
		{
			cmdbuff[buff++] = (TEMP_PARA_NEW[i][j] / 1000) + '0';
			cmdbuff[buff++] = (TEMP_PARA_NEW[i][j] / 100) - (TEMP_PARA_NEW[i][j] / 1000) * 10 + '0';
			cmdbuff[buff++] = (TEMP_PARA_NEW[i][j] / 10) - (TEMP_PARA_NEW[i][j] / 100) * 10 + '0';
			cmdbuff[buff++] = TEMP_PARA_NEW[i][j] % 10 + '0';//�Ķ���� 4�ڸ� * 10�� = 40 �� ���ڸ� ���ڷ� �ٲ㼭 �־�� ��
			PARA_NEW[i][j] = TEMP_PARA_NEW[i][j];
//			printf3("%4d\n", PARA_NEW[i][j]);
		}

	}
	store_para();
}
/********************************************************************************/
/**************   Store RESULT DATA    **********/
/********************************************************************************/
void store_result(u16 tmp_seqno) {
	u32	i, j, k, radr;
	prt_msg("\r *store result:%d", tmp_seqno);
	if ((!tmp_seqno) || (tmp_seqno > END_QC_NO)) {
		prt_msg("\r Result Write Error!!! %d", tmp_seqno);
		return;				//Do not store
	}
	else prt_msg(" - Result Write");

	i = 0;
	cmdbuff[i++] = '*';	cmdbuff[i++] = 'T';		//0x54									//  0, 1
	j = bin2bcdasc4(tmp_seqno);
	cmdbuff[i++] = j >> 24; cmdbuff[i++] = j >> 16; cmdbuff[i++] = j >> 8; cmdbuff[i++] = j >> 0;		//  2~5
	cmdbuff[i++] = '2'; cmdbuff[i++] = '0';													//  6, 7
	j = bin2bcdasc4(MEASyear); 	cmdbuff[i++] = j >> 8; cmdbuff[i++] = j;						//  8, 9
	j = bin2bcdasc4(MEASmonth);	cmdbuff[i++] = j >> 8; cmdbuff[i++] = j;						// 10,11
	j = bin2bcdasc4(MEASday); 	cmdbuff[i++] = j >> 8; cmdbuff[i++] = j;						// 12,13
	j = bin2bcdasc4(MEAShour); 	cmdbuff[i++] = j >> 8; cmdbuff[i++] = j;						// 14,15
	j = bin2bcdasc4(MEASmin); 	cmdbuff[i++] = j >> 8; cmdbuff[i++] = j;						// 16,17
	j = bin2bcdasc4(MEASsec); 	cmdbuff[i++] = j >> 8; cmdbuff[i++] = j;						// 18,19

	for (j = 0; j < MAX_OPERID; j++) 	cmdbuff[i++] = OPER_ID[j];								// 20~29		10[ea]
	if (tmp_seqno >= SRT_QC_NO) {															//QC����϶�
		for (j = 0; j < MAX_LOTNO; j++){
			cmdbuff[i++] = temp_lotno[measCnt][j];						// 30~47		18[ea]
		}
		for (j = MAX_LOTNO; j < MAX_USERID; j++){
			cmdbuff[i++] = MEASuserid[j];
		}
	}
	else {
		for (j = 0; j < MAX_USERID; j++){
			cmdbuff[i++] = tempSampleID[measCnt][j];						// 30~47		18[ea]
		}
	}
	//	}//200528
	j = bin2bcdasc4(temp_stripno[measCnt]);	cmdbuff[i++] = j >> 8; cmdbuff[i++] = j >> 0;		// 48,49		 2[ea]
	//get_strip_name(temp_stripno[measCnt]);
	for (j = 0; j < 10; j++)			cmdbuff[i++] = buff_strip_name[j];						// 50~59		10[ea]
	for (j = 0; j < (MAX_PAD_NO + 1); j++) cmdbuff[i++] = bin2hexasc(STRIP_INFO[j] & 0xFF);		// 60~74		15[ea]
	for (j = 0; j < MAX_PAD_NO; j++) 	cmdbuff[i++] = m_final_results[j] + '0';				// 75~88		14[ea]

	for (j = 0; j < MAX_PAD_NO; j++) {
		k = dataRED1[j];
		cmdbuff[i++] = k >> 8; cmdbuff[i++] = k >> 0;											// 89~116
		k = dataGRN1[j];
		cmdbuff[i++] = k >> 8; cmdbuff[i++] = k >> 0;											//117~144
		k = dataBLU1[j];
		cmdbuff[i++] = k >> 8; cmdbuff[i++] = k >> 0;											//145~172
	}
	printf0("\nErrorCode Save: %d\n", ErrCode);
	cmdbuff[i++] = ErrCode + '0';																//173
	cmdbuff[i++] = COLOR_NO + '0';														//174
	cmdbuff[i++] = TURBI_NO + '0';														//175
	cmdbuff[i++] = NEGPOS + '0';															//176
	cmdbuff[i++] = URLF;																//177		
	tmp_seqno -= 1;
	radr = tmp_seqno / 16;
	radr = RESULT_ADR + (radr * SECTOR_SIZE);
	j = (tmp_seqno & 0x0F) * RESULT_PAGE;
	prt_msg("\r SeqNo.%4d, RSA:0x%lx-%4x", tmp_seqno + 1, radr, j);
	readFR4k(radr, 0x1000);
	k = 0;
	for (i = 0; i < RESULT_SIZE; i++) {
		flash_read[j++] = cmdbuff[i];
		k += cmdbuff[i];
	}
	for (i = 0; i < SECTOR_SIZE; i++) flash_buff[i] = flash_read[i];
	k = ~k;
	flash_buff[j++] = k >> 8;																//178  MSB
	flash_buff[j++] = k & 0xff;															//179  CHKSUM		
	write_sflash(radr, SECTOR_SIZE);													//system_wr4k(radr);
}
/********************************************************************************/
/********************************************************************************/
u16 read_result(u16 sno, u16 rrd) {		// extROM ==> RAM, "meas.c"���� ���
	u32 radr;
	u16 i, j, w1, w2;
	sno -= 1;
	radr = sno / 16;
	radr = RESULT_ADR + (radr * SECTOR_SIZE);
	readFR4k(radr, SECTOR_SIZE);

	j = (sno & 0x0F) * RESULT_PAGE;
	for (i = 0; i < (RESULT_SIZE + 2); i++) {
		cmdbuff[i] = flash_read[j++];
	}
	w1 = 0;
	for (i = 0; i < RESULT_SIZE; i++) w1 += cmdbuff[i];
	w2 = cmdbuff[i++];
	w2 = (w2 << 8) + cmdbuff[i++];
	w2 = ~w2;
	if ((w1 == w2) && (cmdbuff[0] == '*')) {
		if (rrd & 1) restore_result();
		return(OK);
	}
	else return(NG);
}
/********************************************************************************/
//#define SRT_RGB_DATA		 89
#define PNT_ERR_CODE		173
#define ORG_ERR_CODE		257
void restore_result(void) {
	u16 i, j, p1, p2;
	u32 ww;
	for (i = 0; i < 7; i++) cmdbuff[i + ORG_ERR_CODE] = cmdbuff[i + PNT_ERR_CODE];
	p1 = PNT_ERR_CODE; p2 = ORG_ERR_CODE;
	for (i = 0; i < (MAX_PAD_NO * 3); i++) {
		p1 = p1 - 2;
		j = cmdbuff[p1];
		j = (j << 8) + cmdbuff[p1 + 1];
		ww = bin2bcdasc4(j);
		p2 = p2 - 4;
		cmdbuff[p2 + 0] = ww >> 24;
		cmdbuff[p2 + 1] = ww >> 16;
		cmdbuff[p2 + 2] = ww >> 8;
		cmdbuff[p2 + 3] = ww >> 0;
	}
}
/********************************************************************************/
u32	check_adr;

u16 check_result(u16 sno) {
	u32 radr;
	u16 i, j, w1, w2, k, m;
	sno -= 1;
	radr = sno / 16;
	radr = (radr * SECTOR_SIZE) + check_adr;				// RESULT_ADR
	readFR4k(radr, 0x1000);

	k = 0;
	m = 0x0001;
	for (i = 0; i < SECTOR_SIZE; i += RESULT_PAGE) {
		w1 = 0;
		for (j = 0; j < RESULT_SIZE; j++) w1 += flash_read[i + j];
		w2 = flash_read[i + j]; j++;
		w2 = (w2 << 8) + flash_read[i + j];
		w2 = ~w2;
		if ((w1 == w2) && (flash_read[i] == '*')) k |= m;
		m <<= 1;
	}
	return(k);
}
/********************************************************************************/
void make_index(void) {
	u16 i, j, k, m, a, b, cnt1, cnt2;
	u32 idx;
	printf0("\r\n Index ");
	cnt1 = cnt2 = 0;
	idx = TEST_INDEX;
	check_adr = RESULT_ADR;
	for (i = 1; i <= ALL_RECORD; i += 16) {												//END_QC_NO//200528 Start
		j = check_result(i);
		m = 0x0001;
		for (k = 0; k < 16; k++) {
			if ((i + k) > ALL_RECORD) break;											//END_QC_NO
			if (j & m) {
				if (i + k < SRT_QC_NO) cnt1++;											//Test data
				else cnt2++;													//QC data
				*(u8*)(idx) = 'I';
				a = (k * RESULT_PAGE) + 8;										//from  yy/mm/dd
				for (b = 1; b < 53; b++) { *(u8*)(idx + b) = flash_read[a++]; }			//until strip name
				*(u8*)(idx + b) = 'X';
			}
			else {
				*(u8*)(idx) = 0x00;
			}
			m <<= 1;
			idx += INDEX_PAGE;
		}
		if ((i & 0xFF) == 1) printf0(">");
	}
	sprintf(charbuff, "Test:%d, QC:%d", cnt1, cnt2);
	//out_line_32(220, 400, charbuff, CYN_LCD);
	key_buzzer();
	delay_ms(1000);
}

/********************************************************************************/
void add_index(void) {
	u16 b;
	u32 idx;
	printf0(" add Index ");
	if ((modex & 0xFF00) == MODE_QC)
	{
		idx = QC_NO - 1;
	}
	else
	{
		idx = SEQ_NO - 1;
	}
	idx = TEST_INDEX + (idx * INDEX_PAGE);
	*(u8*)(idx) = 'I';
	for (b = 1; b < 53; b++)
	{
		*(u8*)(idx + b) = cmdbuff[b + 7];				//from yy/mm/dd, until strip name
	}
	*(u8*)(idx + b) = 'X';


	if ((modex & 0xFF00) == MODE_QC)
	{
		idx = QC_NO - 1;
	}
	else
	{
		idx = SEQ_NO - 1;
	}
	idx = TEST_INDEX + ((idx)*INDEX_PAGE);

}
/********************************************************************************/
void del_index(u16 dno) {
	u32 idx;
	printf0(" delete Index : %d", dno);
	idx = dno - 1;
	idx = TEST_INDEX + (idx * INDEX_PAGE);
	*(u8*)(idx + 53) = 0x00;
}
/********************************************************************************/
u8 chk_index(u16 dno) {//200528 �������
	u32 idx, k;
	idx = dno - 1;
	idx = TEST_INDEX + (idx * INDEX_PAGE);
	if (*(u8*)(idx + 53) != 'X') {
		return(NG);
	}
	MEASseqno = dno;
	idx += 1;
	for (k = 0; k < 53; k++) cmdbuff[k + 8] = *(u8*)(idx + k);

	k = cmdbuff[8];		k = (k << 8) + cmdbuff[9];	MEASyear = asc2bin4(k);
	k = cmdbuff[10];	k = (k << 8) + cmdbuff[11];	MEASmonth = asc2bin4(k);
	k = cmdbuff[12];	k = (k << 8) + cmdbuff[13];	MEASday = asc2bin4(k);
	k = cmdbuff[14];	k = (k << 8) + cmdbuff[15];	MEAShour = asc2bin4(k);
	k = cmdbuff[16];	k = (k << 8) + cmdbuff[17];	MEASmin = asc2bin4(k);
	k = cmdbuff[18];	k = (k << 8) + cmdbuff[19];	MEASsec = asc2bin4(k);

	for (k = 0; k < MAX_OPERID; k++) {
		for (k = 0; k < MAX_OPERID; k++)
		{
			MEASoperid[k] = cmdbuff[20 + k];
		}
		MEASoperid[k] = 0;			//20~29
	}							//20~29

	if (test_type == GENERAL)
	{
		for (k = 0; k < MAX_USERID; k++)
		{
			MEASuserid[k] = cmdbuff[30 + k];
		}
		MEASuserid[k] = 0;			//30~47
	}
	else
	{
		for (k = 0; k < MAX_LOTNO; k++) buff_lot_no[k] = cmdbuff[30 + k];
		for (k = MAX_LOTNO; k < MAX_USERID; k++)
		{
			MEASuserid[k] = cmdbuff[30 + k];
		}
		MEASuserid[k] = 0;
	}
	k = cmdbuff[48];
	k = (k << 8) + cmdbuff[49];
	MEASstripno = asc2bin4(k);
	for (k = 0; k < MEAS_NAME_SIZE; k++)
	{
		MEASstripname[k] = cmdbuff[50 + k];
	}
	MEASstripname[k] = 0;		//50~59
	return(OK);
}
/********************************************************************************/
void delete_record(u16 rno) {			//2020-07-03 9:17����
	u32 radr, i;
	radr = (rno - 1) / 16;
	radr = RESULT_ADR + (radr * SECTOR_SIZE);
	i = ((rno - 1) & 0x0F) * RESULT_PAGE;
	prt_msg("\r Delete - SeqNo.%4d, RSA:0x%lx-%3x", rno, radr, i);
	readFR4k(radr, 0x1000);
	flash_read[i] = '#';
	for (i = 0; i < SECTOR_SIZE; i++) flash_buff[i] = flash_read[i];
	write_sflash(radr, SECTOR_SIZE);													//system_wr4k(radr);
}
/********************************************************************************/
/**************       Internal Flash                                   **********/
/********************************************************************************/
extern uint16_t FLASH_If_Erase(uint32_t Add);
// ������ ���� �÷��� ������ (64KB)
int write_intflash(u8 cmd) {
	u32	ww;
	prt_msg("\r Flash(Int) Memory Writing...");
	if (cmd == 'V') {
		FLASH_Unlock();
		//		FLASH_ClearFlag( FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR | FLASH_FLAG_RDERR );
		FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR | FLASH_FLAG_RDERR);
		prt_msg("\r **** Erase Flash ****");
		if (FLASH_If_Erase(STORE_ROM) == OK) {
			prt_msg("\t **** Write Flash(Int) ****");
			prt_msg("\r 0x%lx - 0x%lx", STORE_ROM, STORE_ROM + STORE_SIZE - 1);
			for (ww = 0; ww < STORE_SIZE; ww += 4) {
				FLASH_ProgramWord((STORE_ROM + ww), *(u32*)(INT_BUFF + ww));
			}
		}
		else {
			prt_msg("\t **** Erase Fail !!! ****");
			return(NG);
		}
		FLASH_Lock();
	}
	else {
		//		targetAddr = RESULT_ROM;				//������.
		//		data_size = 0x10000;
		return(NG);
	}
	return(OK);
}

/********************************************************************************/
/********************************************************************************/
//====================================================================
u8 read24(u16 adr) { return(*(u8*)(EXT_BUFF + adr)); }
//====================================================================
u16 read24w(u16 adr) {
	u16 w;
	w = *(u8*)(EXT_BUFF + adr + 0);
	w = (w << 8) + *(u8*)(EXT_BUFF + adr + 1);
	return(w);
}
//====================================================================
void write24(u16 adr, u8 dat) { *(u8*)(EXT_BUFF + adr + 0) = dat; }
//====================================================================
void write24w(u16 adr, u16 dat) {
	*(u8*)(EXT_BUFF + adr + 0) = (dat >> 8);
	*(u8*)(EXT_BUFF + adr + 1) = (dat & 0xff);
}
//====================================================================
u8 asc2hex(u8 asc) {
	if (asc <= '9') return(asc - '0');
	else return((asc - 'A') + 10);
}
//====================================================================
u8 asc2dec(u8 asc) {
	if ((asc >= '0') && (asc <= '9')) return(asc - '0');
	else return(0X00);
}
//====================================================================
void key_buzzer2(void) {
	key_buzzer();	delay_ms(150);
	key_buzzer();	delay_ms(150);
}

void key_buzzer3(void) {
	key_buzzer();	delay_ms(150);
	key_buzzer();	delay_ms(150);
	key_buzzer();	delay_ms(150);
}
/********************************************************************************/
/********************************************************************************/
/********************************************************************************/
#define SPAN_POSITION_SIZE			MaxRawData*2
void store_reference_rgb(void) {
	u16 i, j, k;
	printf0("\r Store span RED");
	k = 0;
	//#define TEST_REF_RED	6000
	//#define TEST_REF_GRN	7000
	//#define TEST_REF_BLU	8000
	//	for(i=0;i<MaxRawData;i++){ spanRED[i] = TEST_REF_RED; spanGRN[i] = TEST_REF_GRN; spanBLU[i] = TEST_REF_BLU;	}

	for (i = 0; i < SPAN_POSITION_SIZE; i += 2) {
		j = spanRED[i / 2];
		flash_buff[i + 0] = j >> 8;
		flash_buff[i + 1] = j >> 0;
		k += j;
	}

	
	printf0("[%4x]", k);
	flash_buff[i++] = k >> 8;
	flash_buff[i++] = k >> 0;
	writeFR4k(SPAN_RED_ADR, SPAN_POSITION_SIZE + 2);
	printf0("  span GRN");
	k = 0;
	for (i = 0; i < SPAN_POSITION_SIZE; i += 2) {
		j = spanGRN[i / 2];
		flash_buff[i + 0] = j >> 8;
		flash_buff[i + 1] = j >> 0;
		k += j;
	}
	printf0("[%4x]", k);
	flash_buff[i++] = k >> 8;
	flash_buff[i++] = k >> 0;
	writeFR4k(SPAN_GRN_ADR, SPAN_POSITION_SIZE + 2);
	printf0("  span BLU  ");
	k = 0;
	for (i = 0; i < SPAN_POSITION_SIZE; i += 2) {
		j = spanBLU[i / 2];
		flash_buff[i + 0] = j >> 8;
		flash_buff[i + 1] = j >> 0;
		k += j;
	}
	printf0("[%4x]", k);
	flash_buff[i++] = k >> 8;
	flash_buff[i++] = k >> 0;
	writeFR4k(SPAN_BLU_ADR, SPAN_POSITION_SIZE + 2);
}
/**************************************************************/
#define DEF_AVER_RED			6000
#define DEF_AVER_GRN			6000
#define DEF_AVER_BLU			6000

u16 get_white_high(u16* str) {
	u16 i, j = 0/*, k = 0*/;
	for (i = 0; i < 80; i++) {
		if (str[i] > j) {
			j = str[i];
			//k = i;
		}
	}
	//printf3(" W:%dat%d", j, k);
	return(j);
}

void read_ref_data(void) {
	u16 i, j, k;
	printf0("\r span RED");
	readFR4k(SPAN_RED_ADR, SPAN_POSITION_SIZE + 2);
	k = 0;
	for (i = 0; i < SPAN_POSITION_SIZE; i += 2) {
		j = flash_read[i + 0];
		j = (j << 8) + flash_read[i + 1];
		ref_red_buf[i / 2] = j;
		k += j;
	}
	j = flash_read[i++];
	j = (j << 8) + flash_read[i++];
	if (j != k) {
		printf0("=>NG[%4x/%4x]", j, k);
		for (i = 0; i < MaxRawData; i++) ref_red_buf[i] = DEF_AVER_RED;
	}
	else printf0(" ==> OK");
	WHITE_RED = get_white_high(ref_red_buf);
	//	WHITE_RED = ref_red_buf[StripPosition[0]];

	printf0("  GRN");
	readFR4k(SPAN_GRN_ADR, SPAN_POSITION_SIZE + 2);
	k = 0;
	for (i = 0; i < SPAN_POSITION_SIZE; i += 2) {
		j = flash_read[i + 0];
		j = (j << 8) + flash_read[i + 1];
		ref_grn_buf[i / 2] = j;
		k += j;
	}
	j = flash_read[i++];
	j = (j << 8) + flash_read[i++];
	if (j != k) {
		printf0("=>NG[%4x/%4x]", j, k);
		for (i = 0; i < MaxRawData; i++) ref_grn_buf[i] = DEF_AVER_GRN;
	}
	else printf0(" ==> OK");
	WHITE_GRN = get_white_high(ref_grn_buf);
	//	WHITE_GRN = ref_grn_buf[StripPosition[0]];

	printf0("  BLU");
	readFR4k(SPAN_BLU_ADR, SPAN_POSITION_SIZE + 2);
	k = 0;
	for (i = 0; i < SPAN_POSITION_SIZE; i += 2) {
		j = flash_read[i + 0];
		j = (j << 8) + flash_read[i + 1];
		ref_blu_buf[i / 2] = j;
		k += j;
	}
	j = flash_read[i++];
	j = (j << 8) + flash_read[i++];
	if (j != k) {
		printf0("=>NG[%4x/%4x]", j, k);
		for (i = 0; i < MaxRawData; i++) ref_blu_buf[i] = DEF_AVER_BLU;
	}
	else printf0(" ==> OK");
	WHITE_BLU = get_white_high(ref_blu_buf);
	//	WHITE_BLU = ref_blu_buf[StripPosition[0]];
}
/**************************************************************/
void ReadMemory(void){
	int startSEQ, endSEQ, i, j;
	j = 0;
	los_count = 0;
	if (test_type == GENERAL) {
		startSEQ = srt_seqno;
		endSEQ = end_seqno;
	}
	else {
		startSEQ = srt_qcno;
		endSEQ = end_qcno;
	}
//	printf3("s:%d e:%d\n", startSEQ, endSEQ);
	for (int i = 0; i < 5000; i++) {
		searchSEQ[i] = 0;
	}

	for (i = endSEQ; i >= startSEQ; i--) {
		if (chk_index(i) == OK) {
			if(cmp_userid() == OK && cmp_operid() == OK){
				if(!dateMemFlag || cmp_testdate()){

					searchSEQ[j++] = i;
					add_los_memory(searchSEQ[j - 1]);
				}
			}
		}
	}
	memCount = j-1;
}
