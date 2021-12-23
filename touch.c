#define  _TOUCH_C_

#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include "common\32f429_port.h"
#include "common\32f429_lcd.h"
#include "util\eep.h"
#include "util\usart.h"	 
#include "util\lcd.h"
#include "util\tm_adc.h"
#include "util\Flash.h"
#include "USB\usb_file.h"
#include "sysadj.h"
#include "resm.h"
#include "touch.h"
#include "meas.h"
#include "dsp.h"
static  char s_Conv[2][7] = { "Conv.", "Conv." };
static  char s_SI[2][6] = { "S.I.", "S.I." };

extern void fill_space(char *str, short cnt);

u16 tcode;
 
u16 tch_check(const u16 *tb, u8 tno);

void tch_mem_delete(void);
void delete_memory_all(void);


/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
void key_decode(void){
	switch(modex&0x0FFF){
		case MODE_HOME:					tch_home();					break;
		case MODE_MEMORY:				tch_memory();				break;
		case MODE_MEM_date_set:			tch_mem_set();				break;
		case MODE_MEM_type:				tch_mem_set();				break;
		case MODE_MEM_seq:				tch_mem_set();				break;
		case MODE_MEM_sid:				tch_mem_set();				break;
		case MODE_MEM_oid:				tch_mem_set();				break;
		case MODE_MEM_view:				tch_mem_view();				break;
		case MODE_MEM_delete:			tch_mem_delete();			break;
		case MODE_MEM_print:			
		case MODE_MEM_upload:			tch_printupload();			break;

		case MODE_MEAS:					tch_measure();				break;
		case MODE_MEAS_Error:			tch_error_confirm();		break;
		case MODE_MEAS_Error2:			tch_printupload();			break;

		case MODE_TEST:					tch_test();					break;
		case MODE_TEST_Error:			tch_error_confirm();		break;
		case MODE_TEST_Error2:			tch_printupload();			break;
		case MODE_TEST_Return:			tch_test_return();			break;
		case MODE_TEST_seq:				tch_test_set();				break;
		case MODE_TEST_strip:			tch_test_set_strip();		break;
		case MODE_TEST_sid:				tch_test_set();				break;
		case MODE_TEST_color:			tch_test_set_color();		break;
		case MODE_TEST_trubi:			tch_test_set_turbi();		break;
		
		case MODE_QC:					tch_QC();					break;
		case MODE_QC_Error:				tch_error_confirm();		break;
		case MODE_QC_Error2:			tch_printupload();			break;
		case MODE_QC_Return:			tch_QC_return();			break;
		case MODE_QC_seq:				tch_qc_set();				break;
		case MODE_QC_strip:				tch_qc_set_strip();			break;
		case MODE_QC_negpos:			tch_qc_set();				break;
		case MODE_QC_lot:				tch_qc_set();				break;
		
		case MODE_SET:					tch_setting();				break;
		case MODE_datetime:				tch_datetime();				break;
		case MODE_date_set:				tch_date_set();				break;
		case MODE_time_set:				tch_time_set();				break;
//result unit
//system setting
		case MODE_oper_set:				tch_oper_set();				break;
		case MODE_oper_new:				tch_oper_new();				break;
		case MODE_strip_set:			tch_strip_set();			break;
		case MODE_input_password:		tch_input_password();		break;
		case MODE_change_PR:			tch_change_pr_set();		break;
		case MODE_digit_PR:				tch_digit_pr();				break;
		case MODE_system_chk:			tch_system_chk();			break;
		case MODE_system_chk_confirm:	tch_syschk_confirm();		break;
		case MODE_MAINTANENCE_password:	tch_maintanence_password();	break;
		case MODE_MAINTANENCE:			tch_maintanence();			break;
		case MODE_MAINTANENCE_cali:		tch_maintanence_cali();		break;
		case MODE_MAINTANENCE_serial:	tch_maintanence_serial();	break;
		case MODE_MAINTANENCE_fw:		tch_maintanence_fw();		break;
		case MODE_MAINTANENCE_image:	tch_maintanence_image();	break;
		case MODE_MAINTANENCE_font:		tch_maintanence_font();		break;
		case MODE_MAINTANENCE_Reset:	tch_maintanence_Reset();	break;
		case MODE_MAINTANENCE_Confirm:	tch_maintanence_Confirm();	break;

		case MODE_result_unit:			tch_unit();					break;
		case MODE_system_set:			tch_sysset();				break;
		case MODE_system_set_Return:	tch_sysset_return();		break;
		case MODE_system_set_Restart:	tch_sysset_Restart();		break;
	}
}
/*****************************************************************************/
void tch_home(void){
	tcode=tch_check(tch_home_tb, tch_home_no);
	if(tcode==0){								//memory
		key_buzzer();
		temp_test_type = test_type = GENERAL;
		temp_search_theme = search_theme = NUMBER_SEARCH;
		mem_point = 0;
		end_seqno = (SEQ_NO-1<SRT_SEQ_NO?SRT_SEQ_NO:SEQ_NO - 1);
		//end_seqno = END_SEQ_NO;
		srt_seqno = SRT_SEQ_NO;
		end_qcno = (QC_NO-1<SRT_QC_NO?SRT_QC_NO:QC_NO - 1);
		//end_qcno = END_QC_NO;
		srt_qcno = SRT_QC_NO;
		buff_oper_id[0] = NULL;
		buff_user_id[0] = NULL;
		buff_lot_no[0] = NULL;
		tempDateDelimiter = dateDelimiter;
		tempDatePresentation = datePresentation;
		tempTimeDelimiter = timeDelimiter;
		tempTimePresentation = timePresentation;
		tempyear = year;
		tempmonth = month;
		tempday = day;
		setyear = tempyear;
		setmonth = tempmonth;
		setday = tempday;
		dateMemFlag = 0;
		temp_QC=temp_QC2=0;
		modex = MODE_MEMORY;
		ReadMemory();
		dsp_memory();
	}else if(tcode==1){							//measure
		key_buzzer();
		buff_oper_id[0] = NULL;
		buff_user_id[0] = NULL;
		buff_lot_no[0] = NULL;
		for(int i=0;i<errorStackSize;i++){
			errorStack[i] = 0;
			temp_modex2[i] = 0;
		}
		errorCnt = 0;
		inputCnt = 0;
		noStripFlag = 0;
		dsp_measure();
		modex = MODE_MEAS;
	}else if(tcode==2){							//setting
		key_buzzer();
		dsp_setting();
		modex = MODE_SET;
	}
}
/*****************************************************************************/
void tch_setting(void){
	tcode=tch_check(tch_setting_tb, tch_setting_no);	
printf0(" tcode:%d", tcode);	
	if(tcode==0){								//home
		key_buzzer();
		dsp_home();
		modex = MODE_HOME;
	}else if(tcode==1){							//back
		key_buzzer();
		dsp_home();
		modex = MODE_HOME;
	}else if(tcode==2){							//clock
		key_buzzer();
		tempDateDelimiter = dateDelimiter;
		tempDatePresentation = datePresentation;
		tempTimeDelimiter = timeDelimiter;
		tempTimePresentation = timePresentation;
		tempyear = year;
		tempmonth = month;
		tempday = day;
		temphour = hour;
		tempmin = min;
		tempsec = sec;
		dsp_datetime();
		modex = MODE_datetime;
	}else if(tcode==3){							//unit
		key_buzzer();
		temp_UNIT_FLAG = UNIT_FLAG;
		temp_PLUS_FLAG = PLUS_FLAG;
		temp_MARK_FLAG = MARK_FLAG;
		dsp_unit();
		modex = MODE_result_unit;
	}else if(tcode==4){							//system setting
		key_buzzer();
		dsp_sysset();
		modex = MODE_system_set;
	}else if(tcode==5){							//operater 
		key_buzzer();
		temp_modex = MODE_SET;
		oper_cursor = cursor = oper_no;
		dsp_oper_set();
		modex = MODE_oper_set;
	}else if(tcode==6){							//sys_check
		key_buzzer();
		checkFlag = 0;
		dsp_system_check();
		modex = MODE_system_chk;
	}else if(tcode==7){							//change%R
		key_buzzer();
		password[0] = '0';
		password[1] = '0';
		password[2] = '0';
		password[3] = '0';
		password[4] = NULL;

		cursor = cursor_set = 0;
		modex = MODE_input_password;
		dsp_input_password();

	}
}

/*****************************************************************************/
extern void USB_HS_Init (void);
void tch_system_chk(void){
	tcode=tch_check(tch_syschk_tb, tch_syschk_no);
printf0(" tcode:%d", tcode);	
	if(tcode==0){								//home
		key_buzzer();
		dsp_home();
		modex = MODE_HOME;
	}else if(tcode==1){							//back
		key_buzzer();
		dsp_setting();
		modex = MODE_SET;
	}else if(tcode==2){							//start check
		key_buzzer();
		if(read_strip_flag){
//			if(!SOUND_FLAG)key_buzzerF();
//			tch_error();
			AddError(Err_PlateOut);
		}else{
			run_system_check();
		}
	}else if(tcode==3 && plateCheck){
		key_buzzer();
		LCDbmpxx(bm_popup1_top, 250, 185);
		LCDbmpxx(bm_popup1_mid, 250, 185+50);
		LCDbmpxx(bm_popup2_bottom, 250, 185+50+70);
		modex = MODE_system_chk_confirm;
	}else if(tcode==4 && trayCheck){
		key_buzzer();
		LCDbmpxx(bm_popup2_top, 250, 185);
		LCDbmpxx(bm_popup2_mid, 250, 185+50);
		LCDbmpxx(bm_popup2_bottom, 250, 185+50+70);
		modex = MODE_system_chk_confirm;
	}else if(tcode==5 && tableCheck){
		key_buzzer();
		LCDbmpxx(bm_popup7_top, 250, 185);
		LCDbmpxx(bm_popup7_mid, 250, 185+50);
		LCDbmpxx(bm_popup2_bottom, 250, 185+50+70);
		modex = MODE_system_chk_confirm;
	}else if(tcode==6 && pushbarCheck){
		key_buzzer();
		LCDbmpxx(bm_popup8_top, 250, 185);
		LCDbmpxx(bm_popup8_mid, 250, 185+50);
		LCDbmpxx(bm_popup2_bottom, 250, 185+50+70);
		modex = MODE_system_chk_confirm;
	}else if(tcode==7 && sensorCheck){
		key_buzzer();
		LCDbmpxx(bm_popup9_top, 250, 185);
		LCDbmpxx(bm_popup8_mid, 250, 185+50);
		LCDbmpxx(bm_popup2_bottom, 250, 185+50+70);
		modex = MODE_system_chk_confirm;
	}else if((tcode == 8 && rCheck) ||(tcode == 9 && gCheck) ||(tcode == 10 && bCheck)){
		key_buzzer();
		LCDbmpxx(bm_popup10_top, 250, 185);
		LCDbmpxx(bm_popup8_mid, 250, 185+50);
		LCDbmpxx(bm_popup2_bottom, 250, 185+50+70);
		modex = MODE_system_chk_confirm;
	}else {
//		tch_error();
	}
}
/*****************************************************************************/
void tch_maintanence_cali(void){
	tcode=tch_check(tch_syschk_serial_tb, tch_syschk_serial_no);
printf0(" tcode:%d", tcode);	
	if(tcode==0){								//home
		key_buzzer();
		dsp_home();
		modex = MODE_HOME;
	}else if(tcode==1){							//back
		key_buzzer();
		dsp_maintanence();
		modex = MODE_MAINTANENCE;
	}
}
/*****************************************************************************/
void tch_maintanence_serial(void){
	tcode=tch_check(tch_syschk_serial_tb, tch_syschk_serial_no);
printf0(" tcode:%d", tcode);	
	if(tcode==0){								//home
		key_buzzer();
		dsp_home();
		modex = MODE_HOME;
	}else if(tcode==1){							//back
		key_buzzer();
		dsp_maintanence();
		modex = MODE_MAINTANENCE;
	}else if(tcode<11){							//Type
		key_buzzer();
		dsp_serial_data_change(temp_SERIAL_FLAG%3, temp_SERIAL_FLAG/3, tcode-2);
		temp_SERIAL_FLAG = tcode-2;
	}else if(tcode==11){						//Save
		key_buzzer();
		SERIAL_FLAG = temp_SERIAL_FLAG;
		write24w(SERIAL_ADR, SERIAL_FLAG);
		write_id_chksum();
		dsp_maintanence();
		modex = MODE_MAINTANENCE;
	}
}
/*****************************************************************************/
#define BOOT_ADDRESS	(uint32_t) 0x08000000
typedef void (*fptr)(void);
fptr jump_to_app;
uint32_t jump_addr;

void tch_maintanence_fw(void){
int i;
	tcode=tch_check(tch_syschk_fw_tb, tch_syschk_fw_no);
printf0(" tcode:%d", tcode);	
	if(tcode==0){								//home
		key_buzzer();
		dsp_home();
		modex = MODE_HOME;
	}else if(tcode==1){							//back
		key_buzzer();
		dsp_maintanence();
		modex = MODE_MAINTANENCE;
	}else if(tcode==2){							//confirm
		key_buzzer();
		printf0("\r Up grade firmware from USB Memory");
//		USB_Init();
		if(!first_USB_HS) USB_HS_Init();
		i = prog_update();
		if(!(flag_upload&0x04)){
			USB_HS_Init();
			delay_ms(100);
			i = prog_update();
		}
		if(flag_upload&0x04){
			sprintf(charbuff, "Update Firmware, file size=%ld", i);
			out_line_32(100, 360, charbuff, BLU_LCD);
			printf0("\r Update Firmware:%d", i);
			delay_ms(100);
			*(__IO uint32_t*) (TIM10_BASE+0x28) = 0x33CC;
			*(__IO uint32_t*) (TIM10_BASE+0x2C) = 0x55AA;
			*(__IO uint32_t*) (TIM11_BASE+0x28) = i>>16;
			*(__IO uint32_t*) (TIM11_BASE+0x2C) = i>>0;
			RCC->CIR = 0x00000000;							// Disable all interrupts 
			jump_addr = *(__IO uint32_t*) (BOOT_ADDRESS + 4);
			jump_to_app = (fptr) jump_addr;
			__set_MSP(*(__IO uint32_t*) BOOT_ADDRESS);
			jump_to_app();
		}else{
			if(flag_upload&0x80){
				//out_line_32(288, 360, "No USB Memory!", RED_LCD);
				//printf0("\r No USB Memory");
				LCDbmpxx(bm_popup11_top, 250, 185);
				LCDbmpxx(bm_popup9_mid, 250, 185+50);
				LCDbmpxx(bm_popup2_bottom, 250, 185+50+70);
			}else{				
				//out_line_32(224, 360, "Fail Firmware Reading!", RED_LCD);
				//printf0("\r Fail Firmware Reading!");
				LCDbmpxx(bm_popup12_top, 250, 185);
				LCDbmpxx(bm_popup10_mid, 250, 185+50);
				LCDbmpxx(bm_popup2_bottom, 250, 185+50+70);
			}
			key_buzzer();	delay_ms(300);
			key_buzzer();	delay_ms(300);
			key_buzzer();	delay_ms(700);
			modex = MODE_MAINTANENCE_Confirm;
			//dsp_maintanence();
			//modex = MODE_MAINTANENCE;
		}
	}else if(tcode==3){							//exit
		key_buzzer();
		dsp_maintanence();
		modex = MODE_MAINTANENCE;
	}
}
/*****************************************************************************/
void tch_maintanence_image(void){
	tcode=tch_check(tch_syschk_serial_tb, tch_syschk_serial_no);
printf0(" tcode:%d", tcode);	
	if(tcode==0){								//home
		key_buzzer();
		dsp_home();
		modex = MODE_HOME;
	}else if(tcode==1){							//back
		key_buzzer();
		dsp_maintanence();
		modex = MODE_MAINTANENCE;
	}	
}
/*****************************************************************************/
void tch_maintanence_font(void){
	tcode=tch_check(tch_syschk_serial_tb, tch_syschk_serial_no);
printf0(" tcode:%d", tcode);	
	if(tcode==0){								//home
		key_buzzer();
		dsp_home();
		modex = MODE_HOME;
	}else if(tcode==1){							//back
		key_buzzer();
		dsp_maintanence();
		modex = MODE_MAINTANENCE;
	}	
}
/*****************************************************************************/
void tch_datetime(void) {

	tcode = tch_check(tch_datetime_tb, tch_datetime_no);
	printf0(" tcode:%d", tcode);
	if (tcode == 0) {								//home
		key_buzzer();
		dsp_home();
		modex = MODE_HOME;
	}
	else if (tcode == 1) {							//back
		key_buzzer();
		//200511 Start
		tempDatePresentation = read24w(DATEPRESENTATIONADR);
		tempTimePresentation = read24w(TIMEPRESENTATIONADR);

		tempDateDelimiter = read24w(DATEDELIMITERADR);
		tempTimeDelimiter = read24w(TIMEDELIMITERADR);
		//200511 End
		dsp_setting();
		modex = MODE_SET;
	}
	else if (tcode == 2) {							//date
		key_buzzer();
		setyear = tempyear;
		setmonth = tempmonth;
		setday = tempday;
		dsp_date_set();
		modex = MODE_date_set;
		cursor = 0;
	}
	else if (tcode == 3) {							//time
		key_buzzer();
		sethour = temphour;
		setmin = tempmin;
		setsec = tempsec;
		if (sethour > 11 && tempTimePresentation==0) {
			timeFlag = 1;
			sethour -= 12;
		}
		else timeFlag = 0;
		dsp_time_set();
		modex = MODE_time_set;
		cursor = 0;
	}
	else if (tcode == 4) {							//save
	   //200511 Start
		key_buzzer();
		sendAll_RTC();
		datePresentation = tempDatePresentation;
		dateDelimiter = tempDateDelimiter;
		timePresentation = tempTimePresentation;
		timeDelimiter = tempTimeDelimiter;

		write24w(DATEPRESENTATIONADR, datePresentation);
		write24w(TIMEPRESENTATIONADR, timePresentation);
		write24w(DATEDELIMITERADR, dateDelimiter);
		store_vars(TIMEDELIMITERADR, timeDelimiter);

		dsp_setting();
		modex = MODE_SET;
		//200511 End
	}
	else if (tcode == 5){						//YYYYMMDD
		key_buzzer();
		tempDatePresentation = 0;
		LCDbmpxx(bm_calinder_l, 65, 120);
		sprintf(buff_date, "20%2d%c%2d%c%2d", tempyear, tempDateDelimiter, tempmonth, tempDateDelimiter, tempday);
		fill_zero(buff_date);
		out_line_16(158, 132, buff_date, BLK_LCD);

		LCDbmpxx(bm_ymd1_w, 110, 166);
		LCDbmpxx(bm_ymd2_g, 224, 166);
	}
	else if (tcode == 6){						//DDMMYYYY
		key_buzzer();
		tempDatePresentation = 1;
		LCDbmpxx(bm_calinder_l, 65, 120);
		sprintf(buff_date, "%2d%c%2d%c20%2d", tempday, tempDateDelimiter, tempmonth, tempDateDelimiter, tempyear);
		fill_zero(buff_date);
		out_line_16(158, 132, buff_date, BLK_LCD);

		LCDbmpxx(bm_ymd1_g, 110, 166);
		LCDbmpxx(bm_ymd2_w, 224, 166);
	}
	else if (tcode == 7){						//Date /
		key_buzzer();
		tempDateDelimiter = '/';
		LCDbmpxx(bm_calinder_l, 65, 120);
		if (tempDatePresentation == 0){
			sprintf(buff_date, "20%2d%c%2d%c%2d", tempyear, tempDateDelimiter, tempmonth, tempDateDelimiter, tempday);
			fill_zero(buff_date);
			out_line_16(158, 132, buff_date, BLK_LCD);
		}
		else if (tempDatePresentation == 1){
			sprintf(buff_date, "%2d%c%2d%c20%2d", tempday, tempDateDelimiter, tempmonth, tempDateDelimiter, tempyear);
			fill_zero(buff_date);
			out_line_16(158, 132, buff_date, BLK_LCD);
		}
		LCDbmpxx(bm_slush_w, 110, 212);
		LCDbmpxx(bm_hyphen_g, 224, 212);
		LCDbmpxx(bm_comma_g, 338, 212);
	}
	else if (tcode == 8){						//Date -
		key_buzzer();
		tempDateDelimiter = '-';
		LCDbmpxx(bm_calinder_l, 65, 120);
		if (tempDatePresentation == 0){
			sprintf(buff_date, "20%2d%c%2d%c%2d", tempyear, tempDateDelimiter, tempmonth, tempDateDelimiter, tempday);
			fill_zero(buff_date);
			out_line_16(158, 132, buff_date, BLK_LCD);
		}
		else if (tempDatePresentation == 1){
			sprintf(buff_date, "%2d%c%2d%c20%2d", tempday, tempDateDelimiter, tempmonth, tempDateDelimiter, tempyear);
			fill_zero(buff_date);
			out_line_16(158, 132, buff_date, BLK_LCD);
		}
		LCDbmpxx(bm_slush_g, 110, 212);
		LCDbmpxx(bm_hyphen_w, 224, 212);
		LCDbmpxx(bm_comma_g, 338, 212);
	}
	else if (tcode == 9){						//Date ,
		key_buzzer();
		tempDateDelimiter = ',';
		LCDbmpxx(bm_calinder_l, 65, 120);
		if (tempDatePresentation == 0){
			sprintf(buff_date, "20%2d%c%2d%c%2d", tempyear, tempDateDelimiter, tempmonth, tempDateDelimiter, tempday);
			fill_zero(buff_date);
			out_line_16(158, 132, buff_date, BLK_LCD);
		}
		else if (tempDatePresentation == 1){
			sprintf(buff_date, "%2d%c%2d%c20%2d", tempday, tempDateDelimiter, tempmonth, tempDateDelimiter, tempyear);
			fill_zero(buff_date);
			out_line_16(158, 132, buff_date, BLK_LCD);
		}
		LCDbmpxx(bm_slush_g, 110, 212);
		LCDbmpxx(bm_hyphen_g, 224, 212);
		LCDbmpxx(bm_comma_w, 338, 212);
	}
	else if (tcode == 10){						//12hr
		key_buzzer();
		tempTimePresentation = 0;
		LCDbmpxx(bm_clock_l, 65, 120 + 160);
		if (temphour == 0)       sprintf(buff_time, "AM%2d%c%2d%c%2d", temphour+12, tempTimeDelimiter, tempmin, tempTimeDelimiter, tempsec);
		else if (temphour < 12)       sprintf(buff_time, "AM%2d%c%2d%c%2d", temphour, tempTimeDelimiter, tempmin, tempTimeDelimiter, tempsec);
		else if (temphour == 12) sprintf(buff_time, "PM%2d%c%2d%c%2d", temphour, tempTimeDelimiter, tempmin, tempTimeDelimiter, tempsec);
		else              sprintf(buff_time, "PM%2d%c%2d%c%2d", temphour - 12, tempTimeDelimiter, tempmin, tempTimeDelimiter, tempsec);
		fill_zero(buff_time);
		out_line_16(150, 132 + 160, buff_time, BLK_LCD);
		LCDbmpxx(bm_12h_w, 110, 166 + 160);
		LCDbmpxx(bm_24h_g, 224, 166 + 160);
	}
	else if (tcode == 11){						//24hr
		key_buzzer();
		tempTimePresentation = 1;
		LCDbmpxx(bm_clock_l, 65, 120 + 160);
		sprintf(buff_time, "%2d%c%2d%c%2d", temphour, tempTimeDelimiter, tempmin, tempTimeDelimiter, tempsec);
		fill_zero(buff_time);
		out_line_16(160, 132 + 160, buff_time, BLK_LCD);
		LCDbmpxx(bm_12h_g, 110, 166 + 160);
		LCDbmpxx(bm_24h_w, 224, 166 + 160);
	}
	else if (tcode == 12){						//Time :
		key_buzzer();
		tempTimeDelimiter = ':';
		LCDbmpxx(bm_clock_l, 65, 120 + 160);
		LCDbmpxx(bm_colon_w, 110, 212 + 160);
		LCDbmpxx(bm_hyphen_g, 224, 212 + 160);
		LCDbmpxx(bm_comma_g, 338, 212 + 160);

		if (tempTimePresentation == 0){
			if (temphour < 12)       sprintf(buff_time, "AM%2d%c%2d%c%2d", temphour, tempTimeDelimiter, tempmin, tempTimeDelimiter, tempsec);
			else if (temphour == 12) sprintf(buff_time, "PM%2d%c%2d%c%2d", temphour, tempTimeDelimiter, tempmin, tempTimeDelimiter, tempsec);
			else              sprintf(buff_time, "PM%2d%c%2d%c%2d", temphour - 12, tempTimeDelimiter, tempmin, tempTimeDelimiter, tempsec);
			fill_zero(buff_time);
			out_line_16(150, 132 + 160, buff_time, BLK_LCD);
		}
		else{
			sprintf(buff_time, "%2d%c%2d%c%2d", temphour, tempTimeDelimiter, tempmin, tempTimeDelimiter, tempsec);
			fill_zero(buff_time);
			out_line_16(160, 132 + 160, buff_time, BLK_LCD);
		}
	}
	else if (tcode == 13){						//Time -
		key_buzzer();
		tempTimeDelimiter = '-';
		LCDbmpxx(bm_clock_l, 65, 120 + 160);
		LCDbmpxx(bm_colon_g, 110, 212 + 160);
		LCDbmpxx(bm_hyphen_w, 224, 212 + 160);
		LCDbmpxx(bm_comma_g, 338, 212 + 160);
		if (tempTimePresentation == 0){
			if (temphour < 12)       sprintf(buff_time, "AM%2d%c%2d%c%2d", temphour, tempTimeDelimiter, tempmin, tempTimeDelimiter, tempsec);
			else if (temphour == 12) sprintf(buff_time, "PM%2d%c%2d%c%2d", temphour, tempTimeDelimiter, tempmin, tempTimeDelimiter, tempsec);
			else              sprintf(buff_time, "PM%2d%c%2d%c%2d", temphour - 12, tempTimeDelimiter, tempmin, tempTimeDelimiter, tempsec);
			fill_zero(buff_time);
			out_line_16(150, 132 + 160, buff_time, BLK_LCD);
		}
		else{
			sprintf(buff_time, "%2d%c%2d%c%2d", temphour, tempTimeDelimiter, tempmin, tempTimeDelimiter, tempsec);
			fill_zero(buff_time);
			out_line_16(160, 132 + 160, buff_time, BLK_LCD);
		}

	}
	else if (tcode == 14){						//Time ,
		key_buzzer();
		tempTimeDelimiter = ',';
		LCDbmpxx(bm_clock_l, 65, 120 + 160);
		LCDbmpxx(bm_colon_g, 110, 212 + 160);
		LCDbmpxx(bm_hyphen_g, 224, 212 + 160);
		LCDbmpxx(bm_comma_w, 338, 212 + 160);
		if (tempTimePresentation == 0){
			if (temphour < 12)       sprintf(buff_time, "AM%2d%c%2d%c%2d", temphour, tempTimeDelimiter, tempmin, tempTimeDelimiter, tempsec);
			else if (temphour == 12) sprintf(buff_time, "PM%2d%c%2d%c%2d", temphour, tempTimeDelimiter, tempmin, tempTimeDelimiter, tempsec);
			else              sprintf(buff_time, "PM%2d%c%2d%c%2d", temphour - 12, tempTimeDelimiter, tempmin, tempTimeDelimiter, tempsec);
			fill_zero(buff_time);
			out_line_16(150, 132 + 160, buff_time, BLK_LCD);
		}
		else{
			sprintf(buff_time, "%2d%c%2d%c%2d", temphour, tempTimeDelimiter, tempmin, tempTimeDelimiter, tempsec);
			fill_zero(buff_time);
			out_line_16(160, 132 + 160, buff_time, BLK_LCD);
		}
	}
}
/*****************************************************************************/
void tch_date_set(void){
	tcode=tch_check(tch_date_set_tb, tch_date_set_no);
printf0(" D-tcode:%d", tcode);	
	if(tcode==0){										//home
		key_buzzer();
		dsp_home();
		modex = MODE_HOME;
	}else if(tcode==1 || tcode==15){						//close
		key_buzzer();
		dsp_datetime();
		modex = MODE_datetime;
		dsp_date();
	}else if(tcode<=11){								//digit
		key_buzzer();
		if(tcode<11) tcode--;					//'1'~'9'
		else tcode = 0;							//'0'
		if (cursor <= 5) {
			if (tempDatePresentation == 0) {
				switch (cursor) {
				case 0:	setyear = (setyear % 10) + (tcode * 10);	
					if (setyear < 21)setyear = 21;
					break;
				case 1:	
					setyear = ((setyear / 10) * 10) + tcode;
					if (setyear > 100)setyear = 99;
					if (setyear < 21)setyear = 21;
					break;
				case 2:	setmonth = (setmonth % 10) + (tcode * 10);
					if(setmonth>12)setmonth = 12;
					break;
				case 3:	
					setmonth = ((setmonth / 10) * 10) + tcode;	
					if (setmonth > 12)setmonth = 12;
					if (setmonth == 0)setmonth = 1;
					break;
				case 4:	setday = (setday % 10) + (tcode * 10);	break;
				case 5:	
					setday = ((setday / 10) * 10) + tcode;	
					dayInMonth(setyear, setmonth);
					if (setday > dayCount)setday = dayCount;
					if (setday == 0)setday = 1;
					break;
				}
			}
			else {
				switch (cursor) {
				case 0:	setday = (setday % 10) + (tcode * 10);	break;
				case 1:	
					setday = ((setday / 10) * 10) + tcode;	
					dayInMonth(setyear, setmonth);
					if (setday > dayCount)setday = dayCount;
					if (setday == 0)setday = 1;
					break;
				case 2:	setmonth = (setmonth % 10) + (tcode * 10);
					if(setmonth>12)setmonth = 12;
					break;
				case 3:	
					setmonth = ((setmonth / 10) * 10) + tcode;	
					if (setmonth > 12)setmonth = 12;
					if (setmonth == 0)setmonth = 1;
					break;
				case 4:	setyear = (setyear % 10) + (tcode * 10);	
					if(setyear < 21)setyear = 21;
				break;
				case 5:	
					setyear = ((setyear / 10) * 10) + tcode;	
					if(setyear < 21)setyear = 21;
					else if (setyear > 100)setyear = 99;
					break;
				}
			}
			dayInMonth(setyear, setmonth);
			if (setday > dayCount)setday = dayCount;
			if (setday == 0)setday = 1;

			cursor++;
			if (cursor == 6)cursor = 0;
			LCDbmpxx(bm_12040_w,485,66);
			if (tempDatePresentation == 0) {
				sprintf(buff_date, "20%2d%c%2d%c%2d", setyear, tempDateDelimiter, setmonth, tempDateDelimiter, setday);
				fill_zero(buff_date);
				if (cursor > 3) {
					dsp_cursor_mem_date(cursor + 4);
				}
				else if (cursor > 1) {
					dsp_cursor_mem_date(cursor + 3);
				}
				else {
					dsp_cursor_mem_date(cursor+2);
				}
				out_line_16(491, 78, buff_date, BLK_LCD);
			}
			else {
				sprintf(buff_date, "%2d%c%2d%c20%2d", setday, tempDateDelimiter, setmonth, tempDateDelimiter, setyear);
				fill_zero(buff_date);
				if (cursor > 3) {
					dsp_cursor_mem_date(cursor + 4);
				}
				else if (cursor > 1) {
					dsp_cursor_mem_date(cursor + 1);
				}
				else {
					dsp_cursor_mem_date(cursor);
				}
				out_line_16(491, 78, buff_date, BLK_LCD);
			}

		}/*
		else {
			cursor = 0;
			LCDbmpxx(bm_calinder_l, 448, 66);
			if (tempDatePresentation == 0){
				dsp_cursor_mem_date(cursor+2);
			}
			else {
				dsp_cursor_mem_date(cursor);
			}
			out_line_16(496, 78, buff_date, BLK_LCD);
		}*/
	}else if(tcode==13){						//back
		key_buzzer();
		if(cursor) cursor--;
		LCDbmpxx(bm_12040_w,485,66);
		if (tempDatePresentation == 0) {
			sprintf(buff_date, "20%2d%c%2d%c%2d", setyear, tempDateDelimiter, setmonth, tempDateDelimiter, setday);
			fill_zero(buff_date);
			if (cursor > 3) {
				dsp_cursor_mem_date(cursor + 4);
			}
			else if (cursor > 1) {
				dsp_cursor_mem_date(cursor + 3);
			}
			else {
				dsp_cursor_mem_date(cursor + 2);
			}
			out_line_16(491, 78, buff_date, BLK_LCD);
		}
		else {
			sprintf(buff_date, "%2d%c%2d%c20%2d", setday, tempDateDelimiter, setmonth, tempDateDelimiter, setyear);
			fill_zero(buff_date);
			if (cursor > 3) {
				dsp_cursor_mem_date(cursor + 4);
			}
			else if (cursor > 1) {
				dsp_cursor_mem_date(cursor + 1);
			}
			else {
				dsp_cursor_mem_date(cursor);
			}
			out_line_16(491, 78, buff_date, BLK_LCD);
		}
	}else if(tcode==14){						//save
		key_buzzer();
		tempyear  = setyear;
		tempmonth = setmonth;
		tempday	  =	setday;
		dsp_datetime();
		modex = MODE_datetime;
		//dsp_date();
	}
}
/*****************************************************************************/
void tch_time_set(void){
	tcode=tch_check(tch_time_set_tb, tch_time_set_no);
	printf0(" T-tcode:%d", tcode);	
	if(tcode==0){										//home
		key_buzzer();
		dsp_home();
		modex = MODE_HOME;
	}else if(tcode==1 || tcode == 15){						//close
		key_buzzer();
		dsp_datetime();
		modex = MODE_datetime;
	}else if (tcode <= 11) {								//digit
		key_buzzer();
		if (tcode < 11) tcode--;					//'1'~'9'
		else tcode = 0;							//'0'
		if (cursor < 6) {
			switch (cursor) {
			case 0:	
				sethour = (sethour % 10) + (tcode * 10);
				/*if(tempTimePresentation == 0){
					if (sethour > 12 && sethour < 24) {
						timeFlag = 1;
						sethour -= 12;
					}
					else if (sethour >= 24) {
						timeFlag = 1;
						sethour = 11;
					}
				}else{
					if(sethour>23)sethour = 23;
				}*/
				break;
			case 1:	
				sethour = ((sethour / 10) * 10) + tcode;	
				if (tempTimePresentation == 0) {
					if (sethour == 0) {
						timeFlag = 0;
						sethour = 12;
					}
					else if (sethour > 12 && sethour < 24) {
						timeFlag = 1;
						sethour -= 12;
					}
					else if (sethour >= 24) {
						timeFlag = 1;
						sethour = 11;
					}
				}
				else if(sethour>23){
					sethour = 23;
				}
				break;
			case 2:	setmin = (setmin % 10) + (tcode * 10);	
				if(setmin>59)setmin=59;
				break;
			case 3:
				setmin = ((setmin / 10) * 10) + tcode;	
				if (setmin >= 60)setmin = 59;
				break;
			case 4:	setsec = (setsec % 10) + (tcode * 10);
				if(setsec>59)setsec=59;
				break;
			case 5:	
				setsec = ((setsec / 10) * 10) + tcode;	
				if (setsec >= 60)setsec = 59;
				break;
			}
			
			if (cursor < 5) {
				cursor++;
			}
			else {
				cursor = 0;
			}
//			LCDbmpxx(bm_clock_l, 448, 66);
			LCDbmpxx(bm_12040_w,488,66);
			if (tempTimePresentation == 0) {
				if (timeFlag == 0) {
					sprintf(buff_time, "AM%2d%c%2d%c%2d", sethour, tempTimeDelimiter, setmin, tempTimeDelimiter, setsec);
				}
				else {
					sprintf(buff_time, "PM%2d%c%2d%c%2d", (sethour == 0 ? 12 : sethour), tempTimeDelimiter, setmin, tempTimeDelimiter, setsec);
				}
				fill_zero(buff_time);
				out_line_16(490, 78, buff_time, BLK_LCD);
				if (cursor > 3) {
					dsp_cursor_mem_time(cursor + 4);
				}
				else if (cursor > 1) {
					dsp_cursor_mem_time(cursor + 3);
				}
				else {
					dsp_cursor_mem_time(cursor + 2);
				}
			}
			else {
				sprintf(buff_time, "%2d%c%2d%c%2d", sethour, tempTimeDelimiter, setmin, tempTimeDelimiter, setsec);
				fill_zero(buff_time);
				out_line_16(505, 78, buff_time, BLK_LCD);
				if (cursor > 3) {
					dsp_cursor_mem_time(cursor+4);
				}
				else if (cursor > 1) {
					dsp_cursor_mem_time(cursor+3);
				}
				else {
					dsp_cursor_mem_time(cursor+2);
				}
			}

		}
	}
	else if (tcode == 12) {//AM/PM
		key_buzzer();
		if (!tempTimePresentation) {
			if (timeFlag) {
				timeFlag = 0;
			}
			else {
				timeFlag = 1;
			}
		}else{
			if (sethour >= 12) {
				sethour -= 12;
			}
			else {
				sethour += 12;
			}
		}
		LCDbmpxx(bm_12040_w,488,66);
		if (!tempTimePresentation) {
			if (!timeFlag) {
				sprintf(buff_date, "AM%2d%c%2d%c%2d", (sethour==0?12:sethour), timeDelimiter, setmin, timeDelimiter, setsec);
			}
			else {
				sprintf(buff_date, "PM%2d%c%2d%c%2d", (sethour==0?12:sethour), timeDelimiter, setmin, timeDelimiter, setsec);
			}
			fill_zero(buff_date);
			out_line_16(490, 78, buff_date, BLK_LCD);
			if (cursor > 3) {
				dsp_cursor_mem_time(cursor + 4);
			}
			else if (cursor > 1) {
				dsp_cursor_mem_time(cursor + 3);
			}
			else {
				dsp_cursor_mem_time(cursor + 2);
			}
		}else {
			sprintf(buff_date, "%2d%c%2d%c%2d", sethour, timeDelimiter, setmin, timeDelimiter, setsec);
			fill_zero(buff_date);
			out_line_16(505, 78, buff_date, BLK_LCD);
			if (cursor > 3) {
				dsp_cursor_mem_time(cursor + 4);
			}
			else if (cursor > 1) {
				dsp_cursor_mem_time(cursor + 3);
			}
			else {
				dsp_cursor_mem_time(cursor + 2);
			}
		}
	}else if(tcode==13){						//back
		key_buzzer();
		if(cursor) cursor--;
		LCDbmpxx(bm_12040_w,488,66);
		if (tempTimePresentation == 0) {
			if (timeFlag == 0) {
				sprintf(buff_time, "AM%2d%c%2d%c%2d", sethour, timeDelimiter, setmin, timeDelimiter, setsec);
			}
			else {
				sprintf(buff_time, "PM%2d%c%2d%c%2d", (sethour == 0 ? 12 : sethour), timeDelimiter, setmin, timeDelimiter, setsec);
			}
			fill_zero(buff_time);
			out_line_16(490, 78, buff_time, BLK_LCD);
			if (cursor > 3) {
				dsp_cursor_mem_time(cursor + 4);
			}
			else if (cursor > 1) {
				dsp_cursor_mem_time(cursor + 3);
			}
			else {
				dsp_cursor_mem_time(cursor + 2);
			}
		}
		else {
			sprintf(buff_time, "%2d%c%2d%c%2d", sethour, timeDelimiter, setmin, timeDelimiter, setsec);
			fill_zero(buff_time);
			out_line_16(505, 78, buff_time, BLK_LCD);
			if (cursor > 3) {
				dsp_cursor_mem_time(cursor + 4);
			}
			else if (cursor > 1) {
				dsp_cursor_mem_time(cursor + 3);
			}
			else {
				dsp_cursor_mem_time(cursor + 2);
			}
		}
	}else if(tcode==14){						//save
		key_buzzer();
		if(tempTimePresentation == 0){
			if(sethour > 12)temphour = sethour;
			//else if (sethour == 12 && timeFlag == 0)temphour = 0;
			else if(sethour == 12){
				if(!timeFlag)temphour = 0;
				else temphour = 12;
			}
			else  temphour = sethour + (timeFlag * 12);
		}else{
			temphour = sethour;
		}
		tempmin	  =	(setmin>59?tempmin:setmin);
		tempsec	  =	(setsec>59?tempsec:setsec);
		
		dsp_datetime();
		modex = MODE_datetime;
	}
}
/*****************************************************************************/
void tch_oper_set(void){
u16 i, j;
	tcode=tch_check(tch_oper_tb, tch_oper_no);
printf0(" op-set:%d", tcode);	
	if(tcode<1){										//home
		key_buzzer();
		dsp_home();
		modex = MODE_HOME;
	}else if(tcode==1){									//back
		key_buzzer();
		if (temp_modex == MODE_SET)
		{
			dsp_setting();
			modex = MODE_SET;
		}
		else if(temp_modex == MODE_MEAS)
		{
			dsp_measure();
			modex = MODE_MEAS;
		}
	}else if(tcode==2){									//up
		key_buzzer();
		if(oper_cursor>0){ oper_cursor--; 	dsp_oper_info(); }
	}else if(tcode==3){									//dn
		key_buzzer();
		if(oper_cursor<7){ oper_cursor++; 	dsp_oper_info(); }
	}else if(tcode==4){									//new id
		key_buzzer();
		cursor_set = 0;
		oper_name[0] = 0;
		dsp_oper_new();
		modex = MODE_oper_new;
	}else if(tcode==5){									//log in
		key_buzzer();
		oper_no = oper_cursor;
		oper_buff[OPERATOR_SIZE - 1] = oper_no + '0';
//		write24(OPERATOR_ADR + OPERATOR_SIZE, oper_no+'0');
		//printf3("\r OperCursor:%d", oper_no);
		i = oper_cursor * (MAX_OPERID+18);
		for(j=0;j<MAX_OPERID;j++) OPER_ID[j] = oper_buff[i+j];
		OPER_ID[j] = 0;
		store_oper_buff();
		LCDbmpxx(bm_opid,60,125);
		LCDbmpxx(bm_200_w,60+120,125);
		erase_box0(182, 130, 245+80+34, 115+16+2);
		out_line_16(182, 130, OPER_ID, BLK_LCD);		//
	}else if(tcode==6){									//delete
		key_buzzer();
		i = oper_cursor * (MAX_OPERID+18);
		for(j=0;j<(MAX_OPERID+18);j++) oper_buff[i+j] = ' ';
		dsp_oper_set();
		store_oper_buff();
	}
	else if (tcode <= 14)
	{
		key_buzzer();
		oper_cursor = tcode - 7;
		dsp_oper_info();
	}
}
/*****************************************************************************/
void tch_oper_new(void){
u16 i, j;
	tcode=tch_check(tch_memset_tb, tch_memset_no);
	//printf3(" oper_new:%d", tcode);	
	if((tcode>=4)&&(tcode<tch_memset_no)) key_buzzer();
	if(tcode<1){										//home
		key_buzzer();
		dsp_home();
		modex = MODE_HOME;
	}else if(tcode==1){									//back
		key_buzzer();
		modex = MODE_oper_set;
		dsp_oper_set();
	}else if(tcode==10){								//SAVE
		for (i = 0; i < 8; i++) {
			j = i * (MAX_OPERID + 18);
			if (oper_buff[j] == ' ') {
				oper_cursor = i;
				break;
			}
		}
		//printf3("\nL:%d\n", oper_cursor);
		j = oper_cursor * (MAX_OPERID+18);
		fill_space(oper_name, MAX_OPERID);
		for(i=0;i<MAX_OPERID;i++) oper_buff[j++] = oper_name[i];
		if (datePresentation) {
			if (timePresentation){
				sprintf(charbuff, "%2d%c%2d%c%2d  %2d%c%2d%c%2d", day, dateDelimiter, month, dateDelimiter, year, hour, timeDelimiter, min, timeDelimiter, sec);
				fill_zero(charbuff);
				charbuff[8] = ' ';
				charbuff[9] = ' ';
			}
			else {
				if (hour < 12) {
					sprintf(charbuff, "%2d%c%2d%c%2dAM%2d%c%2d%c%2d", day, dateDelimiter, month, dateDelimiter, year, hour, timeDelimiter, min, timeDelimiter, sec);
				}
				else if (hour == 12) {
					sprintf(charbuff, "%2d%c%2d%c%2dPM%2d%c%2d%c%2d", day, dateDelimiter, month, dateDelimiter, year, hour, timeDelimiter, min, timeDelimiter, sec);
				}
				else {
					sprintf(charbuff, "%2d%c%2d%c%2dPM%2d%c%2d%c%2d", day, dateDelimiter, month, dateDelimiter, year, hour-12, timeDelimiter, min, timeDelimiter, sec);
				}
				fill_zero(charbuff);
			}
		}
		else {
			if (timePresentation) {
				sprintf(charbuff, "%2d%c%2d%c%2d  %2d%c%2d%c%2d", year, dateDelimiter, month, dateDelimiter, day, hour, timeDelimiter, min, timeDelimiter, sec);
				fill_zero(charbuff);
				charbuff[8] = ' ';
				charbuff[9] = ' ';
			}
			else {
				if (hour < 12) {
					sprintf(charbuff, "%2d%c%2d%c%2dAM%2d%c%2d%c%2d", year, dateDelimiter, month, dateDelimiter, day, hour, timeDelimiter, min, timeDelimiter, sec);
				}
				else if (hour == 12) {
					sprintf(charbuff, "%2d%c%2d%c%2dPM%2d%c%2d%c%2d", year, dateDelimiter, month, dateDelimiter, day, hour, timeDelimiter, min, timeDelimiter, sec);
				}
				else {
					sprintf(charbuff, "%2d%c%2d%c%2dPM%2d%c%2d%c%2d", year, dateDelimiter, month, dateDelimiter, day, hour-12, timeDelimiter, min, timeDelimiter, sec);
				}
				fill_zero(charbuff);
			}
		}

		for(i=0;i<18;i++) oper_buff[j++] = charbuff[i];		
		modex = MODE_oper_set;
		dsp_oper_set();
		store_oper_buff();
	}else if(tcode==11){							//CLOSE
//		cancel_oper_set();
		modex = MODE_oper_set;
		dsp_oper_set();
	}else if(tcode==12){							//'<'
		if(cursor_set){
			cursor_set--;
			oper_name[cursor_set] = NULL;
			dsp_oper_name();
//			dsp_cursor_oper(cursor_set);
		}else tch_error();
	}else if(tcode==13){							//Clear
		cursor_set=0;
		oper_name[cursor_set] = NULL;
		dsp_oper_name();
//		dsp_cursor_oper(cursor_set);
	}else if(tcode==14){							//SpaceBar
		if(cursor_set<MAX_OPERID){
			oper_name[cursor_set++] = ' ';
			oper_name[cursor_set] = NULL;
			dsp_oper_name();
//			if(cursor_set<MAX_OPERID) dsp_cursor_oper(cursor_set);
		}else tch_error();
	}else if(tcode<tch_memset_no){				//0~~9, A~Z
		if(cursor_set<MAX_OPERID){
			tcode -= 15;
			if(tcode<=9) tcode += '0';
			else tcode = (tcode-10) + 'A';
			oper_name[cursor_set++] = tcode;
			oper_name[cursor_set] = NULL;
			dsp_oper_name();
//printf0(" cs:%d", cursor_set);			
//			if(cursor_set<MAX_OPERID) dsp_cursor_oper(cursor_set);
		}else tch_error();
	}
}
/*****************************************************************************/
void tch_strip_set(void){
	tcode=tch_check(tch_strip_tb, tch_strip_no);
printf0(" st-set:%d", tcode);
	if(tcode==0){										//home
		key_buzzer();
		dsp_home();
		modex = MODE_HOME;
	}else if(tcode==1){									//back
		key_buzzer();
		modex = MODE_MEAS;
		dsp_measure();
	}else if(tcode==2){									//up
		key_buzzer();
		if(cursor){	cursor--;	dsp_strip_set(); }
	}else if(tcode==3){									//down
		key_buzzer();
		if(cursor<(MAX_STRIPNO-12)){ cursor++;	dsp_strip_set(); }
	}else if(tcode<tch_strip_no){
		key_buzzer();
		STRIP_NO = (tcode-4)+cursor;
printf0(" stripNO:%d", STRIP_NO);	
		move_strip_info();
		dsp_measure();
		modex = MODE_MEAS;
		store_vars(STRIPNO_ADR, STRIP_NO);
	}
}
/*****************************************************************************/

void tch_change_pr_set(void){
	tcode=tch_check(tch_change_tb, tch_change_no);
printf0(" pr-set:%d", tcode);	
	if(tcode<1){										//home
		key_buzzer();
		dsp_home();
		modex = MODE_HOME;
	}else if(tcode==1){									//back
		key_buzzer();
		dsp_setting();
		modex = MODE_SET;
	}else if(tcode<16){
		key_buzzer();
		cursor = tcode - 2;
		dsp_change_PR();
	}else if(tcode<tch_change_no){
		key_buzzer();
		LCDbmpxx(bm_digit_box,92,55);
		//LCDbmpxx(bm_digit_box2,92+252,55);
		
		tmp_line = tcode - 16;
		erase_box0(467+150, 134+(27*(tmp_line+1)), 467+150+70, 134+(27*(tmp_line+1))+24);
		modex = MODE_digit_PR;
		tmp_digit=0;
	}	
}
/*****************************************************************************/
void tch_digit_pr(void){
	tcode=tch_check(tch_dgtpr_tb, tch_dgtpr_no);
printf0(" pr-dgt:%d", tcode);	
	if(tcode==0){										//home
		key_buzzer();
		dsp_home();
		modex = MODE_HOME;
	}else if(tcode==1 || tcode == 14){									//back
		key_buzzer();
		dsp_base_bar();
		LCDbmpxx(bm_change_pr2,40,58);
		dsp_change_PR();
		modex = MODE_change_PR;
	}else if(tcode<12){
		key_buzzer();
		if(tcode==11) tcode = 0;
		else tcode -= 1;
		if (tmp_digit == 1300)tmp_digit = 0;
		tmp_digit = (tmp_digit*10) + tcode;
		tmp_digit %= 10000;
		if (tmp_digit > 1300)tmp_digit = 1300;
		erase_box0(467+150, 134+(27*(tmp_line+1)), 467+150+70, 134+(27*(tmp_line+1))+24);
		if (tmp_digit > 1000){
			sprintf(charbuff, "%4d", tmp_digit);
		}else{
			sprintf(charbuff, "%3d", tmp_digit);
		}
		out_line_24(467+150, 134+(27*(tmp_line+1)), charbuff, BLK_LCD);
	}else if(tcode==12){
		key_buzzer();
		tmp_digit = tmp_digit / 10;
		erase_box0(467+150, 134+(27*(tmp_line+1)), 467+150+60, 134+(27*(tmp_line+1))+24);
		sprintf(charbuff,"%3d", tmp_digit);
		out_line_24(467+150, 134+(27*(tmp_line+1)), charbuff, BLK_LCD);
	}else if(tcode==13){
		key_buzzer();
		TEMP_PARA_NEW[cursor][tmp_line+1] = tmp_digit;
		dsp_base_bar();
		LCDbmpxx(bm_change_pr2,40,58);
		dsp_change_PR();
		modex = MODE_change_PR;
		store_pr_buff();
	}
}		
/*****************************************************************************/

void tch_memory(void) {
	u16 i;
	tcode = tch_check(tch_memory_tb, tch_memory_no);
	printf0(" tcode:%d", tcode);
	if (tcode <= 1) {				//Home, Back																//home,back
		key_buzzer();																			//������
		dsp_home();																				//HOMEȭ�� Display
		modex = MODE_HOME;																		//Modex�� HOME���� ����
	}
	else if (tcode == 2) {			//Test Type
		key_buzzer();																			//������
		if(test_type == QC_MEMORY){
			test_type = GENERAL;																	//Test Type�� GENERAL�� ����
			temp_test_type = GENERAL;																//Temp Test Type�� GENERAL�� ����
		}
		else{
			test_type = QC_MEMORY;																	//Test Type�� GENERAL�� ����
			temp_test_type = QC_MEMORY;																//Temp Test Type�� GENERAL�� ����
			temp_QC = 0;
		}
		mem_point = 0;
		search_theme = NUMBER_SEARCH;															//�˻� ������ SEQ �˻����� ����
		dateMemFlag = 0;
		setyear = tempyear = year;
		setmonth = tempmonth = month;
		setday = tempday = day;
		for(i=0;i<MAX_OPERID;i++)buff_oper_id[i] = NULL;																	//�˻��� Operator ID �ʱ�ȭ
		for(i=0;i<MAX_USERID;i++)buff_user_id[i] = NULL;																	//�˻��� Sample ID �ʱ�ȭ
		for(i=0;i<MAX_LOTNO;i++)buff_lot_no[i] = NULL;																	//�˻��� Lot No. �ʱ�ȭ
		ReadMemory();
		dsp_memory();																			//Memory ȭ�� ����
	}
	else if (tcode == 3) {			//Test date
		key_buzzer();
		backup_mem_set();
		modex = MODE_MEM_date_set;
		dateMemFlag = 1;
		dsp_mem_set();
		cursor = 0;
		if (tempDatePresentation == 0) {
			dsp_cursor_mem_date2(cursor+2);
		}else{
			dsp_cursor_mem_date2(cursor);		
		}
	}
	else if (tcode == 4) {			//SEQ
		key_buzzer();																			//������
		backup_mem_set();																		//���� ������ Sample ID, Operator ID, Test Type ���?
		modex = MODE_MEM_seq;																	//Modex�� Memory SEQ �������� ����
		dsp_mem_set();																			//Memory Setȭ�� Display
		cursor_set = 0;																			//Cursor �ʱ�ȭ
		dsp_cursor_mem_seq(cursor_set);															//SEQ�κ� Ŀ�� Display
	}
	else if (tcode == 5) {				//Sample id(TEST) or ALL Sample(QC)
		key_buzzer();																			//������
		if (test_type == QC_MEMORY)	{								//Test Type�� QC�� ���?	QC Sample ������ All�� ǥ��
			search_theme = NUMBER_SEARCH;														//�˻� ������ SEQ�� ����
			for(i=0;i<MAX_USERID;i++)USER_ID[i] = buff_user_id[i] = NULL;
			mem_point = 0;
			temp_QC = 0;
			ReadMemory();
			dsp_memory();																		//Memory ȭ�� ����
		}
		else{
			modex = MODE_MEM_sid;
			backup_mem_set();
			dsp_mem_set();
			cursor_set = 0;
			dsp_cursor_mem_sid(cursor_set);
		}
	}
	else if (tcode == 6) {																		//sample id(TEST) or Negative Sample(QC)
		key_buzzer();
		if (test_type == QC_MEMORY) {
			search_theme = SAMPLE_SEARCH;
			USER_ID[8] = buff_user_id[8] = 'N';
			USER_ID[9] = buff_user_id[9] = 'E';
			USER_ID[10] = buff_user_id[10] = 'G';
			USER_ID[11] = buff_user_id[11] = 'A';
			USER_ID[12] = buff_user_id[12] = 'T';
			USER_ID[13] = buff_user_id[13] = 'I';
			USER_ID[14] = buff_user_id[14] = 'V';
			USER_ID[15] = buff_user_id[15] = 'E';
			mem_point = 0;
			temp_QC = 1;
			ReadMemory();
			dsp_memory();
		}
		else {
			modex = MODE_MEM_sid;
			backup_mem_set();
			dsp_mem_set();
			cursor_set = 0;
			dsp_cursor_mem_sid(cursor_set);
		}
	}
	else if (tcode == 7) {																		//sample id(TEST) or Positive Sample(QC)
		key_buzzer();
		if (test_type == QC_MEMORY)	{
			search_theme = SAMPLE_SEARCH;
			USER_ID[8] = buff_user_id[8] = 'P';
			USER_ID[9] = buff_user_id[9] = 'O';
			USER_ID[10] = buff_user_id[10] = 'S';
			USER_ID[11] = buff_user_id[11] = 'I';
			USER_ID[12] = buff_user_id[12] = 'T';
			USER_ID[13] = buff_user_id[13] = 'I';
			USER_ID[14] = buff_user_id[14] = 'V';
			USER_ID[15] = buff_user_id[15] = 'E';
			mem_point = 0;
			temp_QC = 2;
			ReadMemory();
			dsp_memory();
		}
		else{
			modex = MODE_MEM_sid;
			backup_mem_set();
			dsp_mem_set();
			cursor_set = 0;
			dsp_cursor_mem_sid(cursor_set);
		}
	}
	else if (tcode == 8) {																		//operator ID
		key_buzzer();
		modex = MODE_MEM_oid;
		backup_mem_set();
		dsp_mem_set();
		cursor_set = 0;
		dsp_cursor_mem_oid(cursor_set);
	}
	else if (tcode == 9) {																		//Search List Up

		key_buzzer();
		if(mem_point-memoryLine >= 0){
			mem_point -= memoryLine;
		}
		else{
			mem_point = 0;
		}
		dsp_mem_line8();
	}
	else if (tcode == 10) {																		//Search List Down
		key_buzzer();
		if (mem_point <= memCount-8){
			mem_point += memoryLine;
		}
//		printf3("m:%d\n", mem_point);
	dsp_mem_line8();
}
	else if (tcode == 11) {																		//Print Search List
		key_buzzer();
		if(memCount<0){
			tch_error();
		}else{
			modex = MODE_MEM_print;
			dsp_mem_printupload();
		}
	}
	else if (tcode == 12) {																		//Upload Search List
		key_buzzer();
		if(memCount<0){
			tch_error();
		}else{

			modex = MODE_MEM_upload;
			dsp_mem_printupload();
		}
	}
	else if (tcode == 13)																	//Delete All
	{
		key_buzzer();
		delete_memory_all();
	}
	else if (tcode <= 21) {																	//Search List (1-8 Line)
		i = searchSEQ[tcode - 14 + mem_point];
		if(i>0 && read_result(i, 1) == OK) {					//result data ==> cmdbuff
			key_buzzer();
			modex = MODE_MEM_view;
			cur_dsp_record_no = i;
			make_result();
			conv_result_store();

			dsp_result_store();
			LCDbmpxx(bm_topbar,0,480-50);
			LCDbmpxx(bm_print2, 400, 438);
			LCDbmpxx(bm_upload, 550, 438);
//			LCDbmpxx(bm_delete, 690, 438);
			dsp_date();
			dsp_time();		}else{
			key_buzzer();
			delay_ms(200);
			key_buzzer();
		}
	}
}
/*****************************************************************************/
void tch_mem_view(void){
//u32 radr, i;
	tcode=tch_check(tch_memview_tb, tch_memview_no);
printf0(" mem view tcode:%d", tcode);	
	if(tcode<1){								//home
		key_buzzer();
		modex = MODE_HOME;
		dsp_home();
	}else if(tcode==1){							//back
		key_buzzer();
		modex = MODE_MEMORY;
		dsp_memory();
	}else if(tcode==2){							//print
		key_buzzer();
		correction_curve();
		make_result();
		print_result(); 
	}else if(tcode==3){							//upload
		key_buzzer();
		if(SERIAL_FLAG<=1) send_result();
		else if (SERIAL_FLAG == 2) Send720();
		else SendYD(SERIAL_FLAG);
	}else if(tcode==4){							//delete
	/*	
		key_buzzer();	
		radr = (cur_dsp_record_no-1) / 16;
		radr = RESULT_ADR + (radr * SECTOR_SIZE);
		i = ((cur_dsp_record_no-1)&0x0F)*RESULT_PAGE;
		prt_msg("\r Delete - SeqNo.%4d, RSA:0x%lx-%3x", cur_dsp_record_no, radr, i);
		readFR4k(radr, 0x1000);
		flash_read[i] = '#';
		for(i=0;i<SECTOR_SIZE;i++) flash_buff[i] = flash_read[i];	
		write_sflash(radr, SECTOR_SIZE);													//system_wr4k(radr);
		del_index(cur_dsp_record_no);
		modex = MODE_MEMORY;
		dsp_memory();
	*/
	}
	else if (tcode <= 16)
	{
		key_buzzer();
		if (searchSEQ[mem_point + (tcode - 5)] != 0) {
			MEASseqno = cur_dsp_record_no = searchSEQ[mem_point + (tcode - 5)];
			//printf3("\r\nView:%d\r\n", searchSEQ[mem_point + (tcode - 5)]);
			if (read_result(MEASseqno, 1) == OK) {					//result data ==> cmdbuff
				key_buzzer();
				make_result();
				conv_result_store();
				dsp_result_store();
			}
		}
	}
	else if (tcode == 17)//up
	{
		modex = MODE_MEM_view;
		key_buzzer();
		if (mem_point > 12) {
				key_buzzer();
				mem_point -= 12;
		}
		else{
			mem_point = 0;
		}
		//ReadLos12();
		//printf3("\r\nlc:%d mp:%d\r\n", los_count, mem_point);
		dsp_los_table_store();
	}
	else if (tcode == 18)//down
	{
		modex = MODE_MEM_view;
		key_buzzer();
		if (los_count < LOSCOUNT) {
			key_buzzer();
			mem_point = 0;
		}
		else if (los_count - 12 > mem_point) {
			mem_point += 12;
		}
		//ReadLos12();
		//printf3("\r\nlc:%d mp:%d\r\n", los_count, mem_point);
		dsp_los_table_store();
	}
}
/*****************************************************************************/
void tch_mem_set(void) {
	tcode = tch_check(tch_memset_tb, tch_memset_no);
	printf0(" mem_set:%d", tcode);
	if(tcode<1){								//home
		key_buzzer();
		modex = MODE_HOME;
		dsp_home();
	}else if(tcode==1 || tcode==11){					//close
		key_buzzer();
		cancel_mem_set();
		modex = MODE_MEMORY;
		dsp_memory();
	}else if (tcode == 2) {								//test date
		key_buzzer();
		modex = MODE_MEM_date_set;
		dateMemFlag = 1;
		dsp_mem_set();
		//clr_cursor_mem_set();
		cursor_set = 0;
		if(!datePresentation)dsp_cursor_mem_date2(cursor_set+2);
		else dsp_cursor_mem_date2(cursor_set);
	}
	else if (tcode == 3) {							//seq
		key_buzzer();
		if (modex != MODE_MEM_seq) {
			modex = MODE_MEM_seq;
			dsp_mem_set();
			//clr_cursor_mem_set();
			cursor_set = 0;
			dsp_cursor_mem_seq(cursor_set);
		}
	}
	else if (tcode == 4) {							//oper id
		key_buzzer();
		if (modex != MODE_MEM_oid) {
			modex = MODE_MEM_oid;
			dsp_mem_set();
			//clr_cursor_mem_set();
			cursor_set = 0;
			dsp_cursor_mem_oid(cursor_set);
		}
	}
	else if (tcode <= 7) {							//sample id
		key_buzzer();
		modex = MODE_MEM_sid;
		key_buzzer();
		if (temp_test_type == GENERAL) {
			dsp_mem_set();
			//clr_cursor_mem_set();
			cursor_set = 0;
			dsp_cursor_mem_sid(cursor_set);
		}
		else {
			if (tcode == 5) {					//All
				temp_QC2 = 0;
				search_theme = NUMBER_SEARCH;
			}
			else if (tcode == 6) {			//Neg
				temp_QC2 = 1;
				search_theme = SAMPLE_SEARCH;
			}
			else {								//Pos
				temp_QC2 = 2;
				search_theme = SAMPLE_SEARCH;
			}
			dsp_mem_set();
		}
	}
	else if (tcode <= 9) {												// Test Type
		key_buzzer();
		modex = MODE_MEM_type;
		if(tcode == 8) temp_test_type = GENERAL;
		else temp_test_type = QC_MEMORY;

		if (temp_test_type == GENERAL){
			sprintf(buff_seqno, "%4d-%4d", srt_seqno, end_seqno);
		}
		else {
			sprintf(buff_seqno, "%4d-%4d", srt_qcno, end_qcno);
		}
		fill_zero(buff_seqno);
		dsp_mem_set();
	}
	else if (tcode == 10) {							//SAVE
		key_buzzer();
		enter_mem_set();
		modex = MODE_MEMORY;
		ReadMemory();
		dsp_memory();
	}
	else if (tcode < tch_memset_no) {		//'<'	//CLEAR		//SPACEBAR		//TEXT
		key_buzzer();
		if (modex == MODE_MEM_seq) alpa_mem_seq(tcode);
		else if (modex == MODE_MEM_sid && temp_test_type == GENERAL) alpa_mem_sid(tcode);
		else if (modex == MODE_MEM_oid) alpa_mem_oid(tcode);
		else if (modex == MODE_MEM_date_set) alpa_mem_date_set(tcode);
	}
}
/*****************************************************************************/
void alpa_mem_seq(u8 cod){
	if(cod==12){									//'<'
		if(cursor_set){
			cursor_set--;
			if(cursor_set==4) cursor_set=3;
			//clr_cursor_mem_set();
			dsp_mem_set_seqno();
			dsp_cursor_mem_seq(cursor_set);
		}else tch_error();
	}else if(cod==13){							//Clear
		if (temp_test_type == GENERAL){
			sprintf(buff_seqno, "%4d-%4d", SRT_SEQ_NO, (SEQ_NO==SRT_SEQ_NO?SRT_SEQ_NO:SEQ_NO-1));
			//sprintf(buff_seqno, "%4d-%4d", SRT_SEQ_NO, END_SEQ_NO);
		}
		else {
			sprintf(buff_seqno, "%4d-%4d", SRT_QC_NO, (QC_NO==SRT_QC_NO?SRT_QC_NO:QC_NO-1));
			//sprintf(buff_seqno, "%4d-%4d", SRT_QC_NO, END_QC_NO);
		}
		fill_zero(buff_seqno);
		dsp_mem_set_seqno();
		cursor_set=0;
		dsp_cursor_mem_seq(cursor_set);
	}else if(cod==14){							//SpaceBar
		tch_error();
	}else if(cod<=24){
		cod -= 15;
		buff_seqno[cursor_set] = cod + '0';
		if(cursor_set<8) cursor_set++;
		if(cursor_set==4) cursor_set=5;
		dsp_mem_set_seqno();
		dsp_cursor_mem_seq(cursor_set);
	}else tch_error();
}
/*****************************************************************************/
void alpa_mem_sid(u8 cod){
	if(cod==12){									//'<'
		if(cursor_set){
			cursor_set--;
			buff_user_id[cursor_set] = NULL;
			dsp_mem_set_sid();
			dsp_cursor_mem_sid(cursor_set);
		}else tch_error();
	}else if(cod==13){							//Clear
		cursor_set=0;
		buff_user_id[cursor_set] = NULL;
		dsp_mem_set_sid();
		dsp_cursor_mem_sid(cursor_set);
	}else if(cod==14){							//SpaceBar
		if(cursor_set<MAX_USERID){
			buff_user_id[cursor_set++] = ' ';
			buff_user_id[cursor_set] = NULL;
			dsp_mem_set_sid();
			dsp_cursor_mem_sid(cursor_set);
		}else tch_error();
	}else{										//0~~9, A~Z
		if(cursor_set<MAX_USERID){
			cod -= 15;
			if(cod<=9) cod += '0';
			else cod = (cod-10)+'A';
			buff_user_id[cursor_set++] = cod;
			buff_user_id[cursor_set] = NULL;
			dsp_mem_set_sid();
			if(cursor_set<MAX_USERID) dsp_cursor_mem_sid(cursor_set);
		}else tch_error();
	}
}
/*****************************************************************************/
void alpa_mem_oid(u8 cod){
	if(cod==12){									//'<'
		if(cursor_set){
			cursor_set--;
			buff_oper_id[cursor_set] = NULL;
			dsp_mem_set_oid();
			dsp_cursor_mem_oid(cursor_set);
		}else tch_error();
	}else if(cod==13){							//Clear
		cursor_set=0;
		buff_oper_id[cursor_set] = NULL;
		dsp_mem_set_oid();
		dsp_cursor_mem_oid(cursor_set);
	}else if(cod==14){							//SpaceBar
		if(cursor_set<MAX_USERID){
			buff_oper_id[cursor_set++] = ' ';
			buff_oper_id[cursor_set] = NULL;
			dsp_mem_set_oid();
			dsp_cursor_mem_oid(cursor_set);
		}else tch_error();
	}else{										//0~~9, A~Z
		if(cursor_set<MAX_OPERID){
			cod -= 15;
			if(cod<=9) cod += '0';
			else cod = (cod-10)+'A';
			buff_oper_id[cursor_set++] = cod;
			buff_oper_id[cursor_set] = NULL;
			dsp_mem_set_oid();
			if(cursor_set<MAX_OPERID) dsp_cursor_mem_oid(cursor_set);
		}else tch_error();
	}
}
/*****************************************************************************/
void alpa_mem_date_set(u8 cod){
	dateMemFlag = 1;
	if(cod==12){									//'<'
		if(cursor_set){
			 cursor_set--;
			if (datePresentation == 0) {
				sprintf(buff_date, "20%2d%c%2d%c%2d", setyear, tempDateDelimiter, setmonth, tempDateDelimiter, setday);
				fill_zero(buff_date);
				if (cursor_set > 3) {
					dsp_cursor_mem_date2(cursor_set + 4);
				}
				else if (cursor_set > 1) {
					dsp_cursor_mem_date2(cursor_set + 3);
				}
				else {
					dsp_cursor_mem_date2(cursor_set + 2);
				}
				dsp_mem_date_set();
				dsp_cursor_mem_date2(cursor_set+2);
			}
			else {
				sprintf(buff_date, "%2d%c%2d%c20%2d", setday, tempDateDelimiter, setmonth, tempDateDelimiter, setyear);
				fill_zero(buff_date);
				if (cursor_set > 3) {
					dsp_cursor_mem_date2(cursor_set + 4);
				}
				else if (cursor_set > 1) {
					dsp_cursor_mem_date2(cursor_set + 1);
				}
				else {
					dsp_cursor_mem_date2(cursor_set);
				}
				dsp_mem_date_set();
				dsp_cursor_mem_date2(cursor_set);
			}

		}else tch_error();
	}else if(cod==13){							//Clear
		cursor_set=0;
		setyear = tempyear = year;
		setmonth = tempmonth = month;
		setday = tempday = day;
		modex = MODE_MEM_type;
		dateMemFlag = 0;
		dsp_mem_set();
		//dsp_cursor_mem_date2(cursor_set);
	}else if(cod==14){							//SpaceBar
		tch_error();
	}else if(cod<=24){
		cod-=15;
		if (cursor_set <= 5) {
			if (datePresentation == 0) {
				switch (cursor_set) {
				case 0:	setyear = (setyear % 10) + (cod * 10);	break;
				case 1:	
					setyear = ((setyear / 10) * 10) + cod;
					if (setyear > 100)setyear = 99;
					if (setyear < 21)setyear = 21;
					break;
				case 2:	setmonth = (setmonth % 10) + (cod * 10);	break;
				case 3:	
					setmonth = ((setmonth / 10) * 10) + cod;	
					if (setmonth > 12)setmonth = 12;
					if (setmonth == 0)setmonth = 1;
					break;
				case 4:	setday = (setday % 10) + (cod * 10);	break;
				case 5:	
					setday = ((setday / 10) * 10) + cod;	
					dayInMonth(setyear, setmonth);
					if (setday > dayCount)setday = dayCount;
					if (setday == 0)setday = 1;
					break;
				}
			}
			else {
				switch (cursor_set) {
				case 0:	setday = (setday % 10) + (cod * 10);	break;
				case 1:	
					setday = ((setday / 10) * 10) + cod;	
					dayInMonth(setyear, setmonth);
					if (setday > dayCount)setday = dayCount;
					if (setday == 0)setday = 1;
					break;
				case 2:	setmonth = (setmonth % 10) + (cod * 10);	break;
				case 3:	
					setmonth = ((setmonth / 10) * 10) + cod;	
					if (setmonth > 12)setmonth = 12;
					if (setmonth == 0)setmonth = 1;
					break;
				case 4:	setyear = (setyear % 10) + (cod * 10);	break;
				case 5:	
					setyear = ((setyear / 10) * 10) + cod;	
					if(setyear < 20)setyear = 20;
					else if (setyear > 100)setyear = 99;
					break;
				}
			}
			dayInMonth(setyear, setmonth);
			if (setday > dayCount)setday = dayCount;
			if (setday == 0)setday = 1;

			cursor_set++;
			if (cursor_set == 6){
				cursor_set = 0;
				dateMemFlag = 1;
			}
			dsp_mem_date_set();
			if (datePresentation == 0) {
				sprintf(buff_date, "20%2d%c%2d%c%2d", setyear, tempDateDelimiter, setmonth, tempDateDelimiter, setday);
				fill_zero(buff_date);
				if (cursor_set > 3) {
					dsp_cursor_mem_date2(cursor_set + 4);
				}
				else if (cursor_set > 1) {
					dsp_cursor_mem_date2(cursor_set + 3);
				}
				else {
					dsp_cursor_mem_date2(cursor_set+2);
				}
			}
			else {
				sprintf(buff_date, "%2d%c%2d%c20%2d", setday, tempDateDelimiter, setmonth, tempDateDelimiter, setyear);
				fill_zero(buff_date);
				if (cursor_set > 3) {
					dsp_cursor_mem_date2(cursor_set + 4);
				}
				else if (cursor_set > 1) {
					dsp_cursor_mem_date2(cursor_set + 1);
				}
				else {
					dsp_cursor_mem_date2(cursor_set);
				}
			}
		}
	}else tch_error();
}
/*****************************************************************************/

void tch_error(void){
	delay_ms(200);
	key_buzzerF();
}		
/*****************************************************************************/
void tch_measure(void){
	u16 i;
	tcode=tch_check(tch_measure_tb, tch_measure_no);
	printf0(" tcode:%d", tcode);	
	if(tcode<=1){								//home,back
		key_buzzer();
		dsp_home();
		modex = MODE_HOME;
	}else if(tcode==2){							//operator id
		key_buzzer();
		temp_modex = MODE_MEAS;
		modex = MODE_oper_set;
		oper_cursor = oper_no;
		dsp_oper_set();
	}else if(tcode==3){							//strip
		key_buzzer();
		cursor = 0;
		dsp_strip_set();
		modex = MODE_strip_set;
	}else if(tcode==4){							//Unit
		key_buzzer();
		erase_box0(102,230,102+48+48,230+24);

		if (UNIT_FLAG == UNIT_SI) {
			UNIT_FLAG = UNIT_CONV;
			out_line_24(102, 230, s_Conv[LANGUAGE_FLAG], BLK_LCD);
		}
		else {
			UNIT_FLAG = UNIT_SI;
			out_line_24(102, 230, s_SI[LANGUAGE_FLAG], BLK_LCD);
		}
		write24w(UNIT_ADR, UNIT_FLAG);
		write_id_chksum();
	}else if(tcode==5){							//Mark plus
		key_buzzer();
		erase_box0(102,281,102+36+36,281+24);
		if(!PLUS_FLAG){
			PLUS_FLAG=1;
			out_line_24(102, 281, "On", BLK_LCD);
		}else{
			PLUS_FLAG=0;
			out_line_24(102, 281, "Off", BLK_LCD);
		}
		write24w(PLUS_ADR, PLUS_FLAG);
		write_id_chksum();
	}else if(tcode==6){							//Mark positive
		key_buzzer();
		erase_box0(102,332,102+36+36,332+24);
		if(!MARK_FLAG){
			MARK_FLAG = 1;
			out_line_24(102, 332, "On", BLK_LCD);	
		}else{
			MARK_FLAG = 0;
			out_line_24(102, 332, "Off", BLK_LCD);	
		}
		write24w(MARK_ADR, MARK_FLAG);
		write_id_chksum();
		store_vars(MARK_ADR, MARK_FLAG);	//RAM �� EEP Save
	}else if(tcode==7){							//test
		key_buzzer();
		measFlag = 0;
		if(trayFullFlag){
			AddError(Err_WasteFull);
		}else if(!PH_BOX){
			stripCnt = 0;
			if(!trayFlag){
				trayFlag = 1;
				AddError(Err_WasteOut);
			}
		}else if(!PH_TBL || read_strip_flag){
			AddError(Err_PlateOut);
		}else if(SEQ_NO == END_SEQ_NO+1){
			AddError(Err_MemoryFull);
		}else{
			if(SEQ_NO>=END_SEQ_NO-50)fullFlag = 1;
			init_test_set();
			dsp_test();
			for(int a=0;a<15;a++){
				LCDbmpxx(bm_strip_no, 317+32*a, 431);
			}
			for(i=0;i<MAX_buffRESULT;i++) buffRESULT[i]=0;
			modex = MODE_TEST;
			flag_motor_run = 1;

		}
#ifdef	SELF_TEST
		flag_self = 5;
printf0(" flag_self:%d", flag_self);	
#endif				

	}else if(tcode==8){							//QC
		key_buzzer();
		measFlag = 1;//0=test, 1=QC
		if(trayFullFlag){
			AddError(Err_WasteFull);
		}else if(!PH_BOX){
			stripCnt = 0;
			if(!trayFlag){
				trayFlag = 1;
				AddError(Err_WasteOut);
			}
		}else if(!PH_TBL || read_strip_flag){
			AddError(Err_PlateOut);
		}else if(QC_NO == END_QC_NO+1){
			AddError(Err_MemoryFull);
		}
		else{
			NEGPOS = 0;
			if(QC_NO>=END_QC_NO-10)fullFlag = 1;
			init_qc_set();
			for(i=0;i<MAX_buffRESULT;i++) buffRESULT[i]=0;
			dsp_qc();
			for(int a=0;a<15;a++){
				LCDbmpxx(bm_strip_no, 317+32*a, 431);
			}
			modex = MODE_QC;
			flag_motor_run = 1;
		}
	}
}
/*****************************************************************************/
/*********************  QQQQQQQQQQQQQQQQQQQQQQQQ  ****************************/
void tch_QC(void) {
	tcode = tch_check(tch_QC_tb, tch_QC_no);
	if (tcode == 0) {								//home
		if(returnFlag){
			LCDbmpxx(bm_popup14_top, 250, 185);
			LCDbmpxx(bm_popup12_mid, 250, 185+50);
			LCDbmpxx(bm_popup3_bottom, 250, 185+50+70);
			modex = MODE_QC_Return;
		}else{
			key_buzzer();
			dsp_home();
			modex = MODE_HOME;
			flag_motor_run = 0;
			flag_self = 0;
		}
 	}
	else if (tcode == 1) {							//back
		if (!motor_step) {
			key_buzzer();
			if(returnFlag){
				LCDbmpxx(bm_popup14_top, 250, 185);
				LCDbmpxx(bm_popup12_mid, 250, 185+50);
				LCDbmpxx(bm_popup3_bottom, 250, 185+50+70);
				modex = MODE_QC_Return;
			}else{
				dsp_measure();
				modex = MODE_MEAS;
				flag_motor_run = 0;
				flag_self = 0;

			}
		}
	}
	else if (tcode == 2) {							//list of samples
		key_buzzer();
	}
	else if (tcode == 3) {							//up
		key_buzzer();
		if (los_cursor > 0) {
			los_cursor--;
			dsp_los_table();
		}
	}
	else if (tcode == 4) {							//down
		key_buzzer();
//		if (los_cursor < (los_count - 12)) {
		if(maxTempCnt>los_cursor+12){
			los_cursor++;
			dsp_los_table();
		}
	}else if (tcode == 5) {							//SEQ
		key_buzzer();
		prt_msg("\r MODE_QC_seq");
		modex = MODE_QC_negpos;//MODE_QC_seq;
		dsp_qc_set();
//		cursor_set = 0;
//		dsp_cursor_qc_seq(cursor_set);
	}
	else if (tcode == 6) {							//strip
/*		key_buzzer();
		prt_msg("\r MODE_QC_strip");
		modex = MODE_QC_strip;
		dsp_qc_set();
		cursor_set = temp_stripno[measCnt];
		dsp_qc_set_strip();*/
		key_buzzer();
		modex = MODE_QC_negpos;//MODE_QC_seq;
		dsp_qc_set();
	}
	else if (tcode == 7) {							//neg/pos
		key_buzzer();
		prt_msg("\r negpos");
		
		if (NEGPOS == 0)
		{
			NEGPOS = temp_negpos[measCnt] = 1;
			tempSampleID[measCnt][8] = buff_user_id[8] = 'P';
			tempSampleID[measCnt][9] = buff_user_id[9] = 'O';
			tempSampleID[measCnt][10] = buff_user_id[10] = 'S';
			tempSampleID[measCnt][11] = buff_user_id[11] = 'I';
			tempSampleID[measCnt][12] = buff_user_id[12] = 'T';
			tempSampleID[measCnt][13] = buff_user_id[13] = 'I';
			tempSampleID[measCnt][14] = buff_user_id[14] = 'V';
			tempSampleID[measCnt][15] = buff_user_id[15] = 'E';
		}
		else
		{
			NEGPOS = temp_negpos[measCnt] = 0;
			tempSampleID[measCnt][8] = buff_user_id[8] = 'N';
			tempSampleID[measCnt][9] = buff_user_id[9] = 'E';
			tempSampleID[measCnt][10] = buff_user_id[10] = 'G';
			tempSampleID[measCnt][11] = buff_user_id[11] = 'A';
			tempSampleID[measCnt][12] = buff_user_id[12] = 'T';
			tempSampleID[measCnt][13] = buff_user_id[13] = 'I';
			tempSampleID[measCnt][14] = buff_user_id[14] = 'V';
			tempSampleID[measCnt][15] = buff_user_id[15] = 'E';
		}
		dsp_qc();
	}
	else if (tcode == 8) {							//Negative
		key_buzzer();
		NEGPOS = temp_negpos[measCnt] = 0;
		tempSampleID[measCnt][8] = buff_user_id[8] = 'N';
		tempSampleID[measCnt][9] = buff_user_id[9] = 'E';
		tempSampleID[measCnt][10] = buff_user_id[10] = 'G';
		tempSampleID[measCnt][11] = buff_user_id[11] = 'A';
		tempSampleID[measCnt][12] = buff_user_id[12] = 'T';
		tempSampleID[measCnt][13] = buff_user_id[13] = 'I';
		tempSampleID[measCnt][14] = buff_user_id[14] = 'V';
		tempSampleID[measCnt][15] = buff_user_id[15] = 'E';
		dsp_qc();
	}
	else if (tcode == 9) {							//Positive
		key_buzzer();
		NEGPOS = temp_negpos[measCnt] = 1;
		tempSampleID[measCnt][8] = buff_user_id[8] = 'P';
		tempSampleID[measCnt][9] = buff_user_id[9] = 'O';
		tempSampleID[measCnt][10] = buff_user_id[10] = 'S';
		tempSampleID[measCnt][11] = buff_user_id[11] = 'I';
		tempSampleID[measCnt][12] = buff_user_id[12] = 'T';
		tempSampleID[measCnt][13] = buff_user_id[13] = 'I';
		tempSampleID[measCnt][14] = buff_user_id[14] = 'V';
		tempSampleID[measCnt][15] = buff_user_id[15] = 'E';
		dsp_qc();
	}
	else if (tcode == 10) {							//lot no
		key_buzzer();
		prt_msg("\r MODE_QC_lot");
		modex = MODE_QC_lot;
		dsp_qc_set();
		cursor_set = 0;
	}
	else if (tcode <= 22)//los
	{
	//200602
		key_buzzer();
		modex = MODE_QC;
		MEASseqno = cur_dsp_record_no = temp_seqno[tcode - 11 + los_cursor];
		printf0("\r\nView:%4d\r\n", temp_seqno[tcode - 11 + los_cursor]);
		if (read_result(MEASseqno, 1) == OK) {					//result data ==> cmdbuff
			key_buzzer();
			test_type = QC_MEMORY;
			make_result();
			conv_result_store();
			dsp_result_store();
		}
		if(QC_NO<=MEASseqno){
			if(maxTempCnt>=los_cursor+tcode-11) tempMeasCnt = los_cursor+tcode-11;
			else if(maxTempCnt==0)tempMeasCnt = 0;
			else tempMeasCnt = maxTempCnt;
			modex = MODE_QC_lot;
			dsp_qc_set();
		}
	}
}
/*****************************************************************************/
void tch_QC_return(void){
	tcode=tch_check(tch_mem_printsend_tb, tch_mem_printsend_no);
	if(tcode==0){								//home
		key_buzzer();
//		dsp_home();
//		modex = MODE_HOME;
	}else if(tcode==1){							//back
		key_buzzer();
		dsp_qc();
		modex = MODE_QC;
		
	}else if(tcode==2){							//confirm
		key_buzzer();
		modex = MODE_MEAS;
		dsp_measure();
		LCDbmpxx(bm_topbar, 0, 480 - 50);
		flag_motor_run = 0;
		flag_self = 0;
		dsp_date();
		dsp_time();
	}else if(tcode==3){							//cancel
		key_buzzer();
		modex = MODE_QC;
		dsp_qc();

	}
}
void tch_qc_set(void) {
	tcode = tch_check(tch_qcset_tb, tch_qcset_no);
	printf0(" tcode:%d", tcode);
	if (tcode == 0) {								//home
		if (!motor_step) {
			key_buzzer();
			dsp_home();
			modex = MODE_HOME;
			flag_motor_run = 0;
			flag_self = 0;
		}
	}
	else if (tcode == 1) {							//back
		key_buzzer();
		cancel_qc_set();
		modex = MODE_QC;
		dsp_qc();
	}else if (tcode == 2) {								//seq
/*		if (modex != MODE_QC_seq) {
			key_buzzer();
			modex = MODE_QC_seq;
			dsp_qc_set();
			cursor_set = 0;
			dsp_cursor_test_seq(cursor_set);
		}*/
	}else if (tcode == 3) {							//seq up
		key_buzzer();
		if (tempMeasCnt < 99 && temp_seqno2[tempMeasCnt] < END_QC_NO) {
			tempMeasCnt++;
			temp_seqno2[tempMeasCnt] = temp_seqno2[tempMeasCnt-1]+1;
			//if(tempMeasCnt>maxTempCnt)maxTempCnt = tempMeasCnt;
			dsp_qc_set();
			//if (modex == MODE_QC_seq) dsp_cursor_qc_seq(cursor_set);
		}
		else tch_error();
	}else if (tcode == 4) {							//seq dn
		key_buzzer();
		if (tempMeasCnt > measCnt && temp_seqno2[tempMeasCnt] > SRT_QC_NO) {
			tempMeasCnt--;
			temp_seqno2[tempMeasCnt] = temp_seqno2[tempMeasCnt+1]-1;
			dsp_qc_set();
			//if (modex == MODE_QC_seq) dsp_cursor_qc_seq(cursor_set);
		}
		else tch_error();
	}else if (tcode == 5) {							//strip
		/*if (modex != MODE_QC_strip) {
			key_buzzer();
			modex = MODE_QC_strip;
			dsp_qc_set();
			cursor_set = temp_stripno2[tempMeasCnt];
			dsp_qc_set_strip();
		}*/
	}else if (tcode == 6){							//neg/pos
		key_buzzer();
		if (!temp_negpos2[tempMeasCnt]) temp_negpos2[tempMeasCnt] = 1;
		else temp_negpos2[tempMeasCnt] = 0;
		modex = MODE_QC_negpos;
		dsp_qc_set();
	}
	else if (tcode == 7){		//Negative
		key_buzzer();
		temp_negpos2[tempMeasCnt] = 0;
		modex = MODE_QC_negpos;
		tempSampleID2[tempMeasCnt][8] = 'N';
		tempSampleID2[tempMeasCnt][9] = 'E';
		tempSampleID2[tempMeasCnt][10] = 'G';
		tempSampleID2[tempMeasCnt][11] = 'A';
		tempSampleID2[tempMeasCnt][12] = 'T';
		tempSampleID2[tempMeasCnt][13] = 'I';
		tempSampleID2[tempMeasCnt][14] = 'V';
		tempSampleID2[tempMeasCnt][15] = 'E';
		dsp_qc_set();
	}
	else if (tcode == 8){		//Positive
		key_buzzer();
		temp_negpos2[tempMeasCnt] = 1;
		modex = MODE_QC_negpos;
		tempSampleID2[tempMeasCnt][8] = 'P';
		tempSampleID2[tempMeasCnt][9] = 'O';
		tempSampleID2[tempMeasCnt][10] = 'S';
		tempSampleID2[tempMeasCnt][11] = 'I';
		tempSampleID2[tempMeasCnt][12] = 'T';
		tempSampleID2[tempMeasCnt][13] = 'I';
		tempSampleID2[tempMeasCnt][14] = 'V';
		tempSampleID2[tempMeasCnt][15] = 'E';
		dsp_qc_set();
	}
	else if (tcode == 9) {							//lot no
		if (modex != MODE_QC_lot) {
			modex = MODE_QC_lot;
			key_buzzer();
			dsp_qc_set();
		}
	}
	else if (tcode == 11) {							//SAVE
		key_buzzer();
		printf0("\n Save Start \n");
		modex = MODE_QC;
		enter_qc_set();
	}else if (tcode == 12) {							//CLOSE
		key_buzzer();
		cancel_qc_set();
		modex = MODE_QC;
		dsp_qc();
	}else {	//'<'	//CLEAR		//SPACEBAR		//TEXT
		key_buzzer();
		/*if (modex == MODE_QC_seq) digit_qc_seq(tcode);
		else */if (modex == MODE_QC_lot) alpa_qc_lot(tcode);
	}
}
/*****************************************************************************/
void digit_qc_seq(u16 cod){
u16 i;
//	printf0("*%d",cod);
	if(cod==11){									//'<'
		if(cursor_set>1){
			cursor_set--;
			clr_cursor_qc_set();
			dsp_cursor_qc_seq(cursor_set);
		}else tch_error()  ;
	}else if(cod==12){							//Clear
		temp_seqno2[tempMeasCnt] = SRT_QC_NO;
		dsp_qc_set_seqno();
		cursor_set=1;
		dsp_cursor_qc_seq(cursor_set);
	}else if(cod==13){							//SpaceBar
		tch_error();
	}else if(cod<=23){
		cod -= 14;
		switch(cursor_set){
			case 0:	cod *= 1000; i =  (temp_seqno2[tempMeasCnt]/1000)*1000; 		break;
			case 1:	cod *=  100; i = ((temp_seqno2[tempMeasCnt]%1000)/100)*100; 	break;
			case 2:	cod *=   10; i = ((temp_seqno2[tempMeasCnt]%100)/10)*10;	 	break;
			case 3:	cod *=    1; i =  (temp_seqno2[tempMeasCnt]%10);			 	break;
		}
		temp_seqno2[tempMeasCnt] += cod - i;
		if(temp_seqno2[tempMeasCnt]<SRT_QC_NO)temp_seqno2[tempMeasCnt] = SRT_QC_NO;
		if(temp_seqno2[tempMeasCnt]>END_QC_NO)temp_seqno2[tempMeasCnt] = END_QC_NO;
		dsp_qc_set_seqno();
		if(cursor_set<3) cursor_set++;
		dsp_cursor_qc_seq(cursor_set);
	}else tch_error();
}
/*****************************************************************************/
void alpa_qc_lot(u16 cod){
	u16 i, j;
	if(cod==13){									//'<'
		if(cursor_set){
			cursor_set--;
			for(i=tempMeasCnt;i<1000;i++)temp_lotno2[i][cursor_set] = NULL;
			dsp_qc_set_lot();
			clr_cursor_qc_set();
			dsp_cursor_qc_lot(cursor_set);
		}else tch_error();
	}else if(cod==14){							//Clear
		for(i=tempMeasCnt;i<1000;i++){
			for(j=0;j<MAX_LOTNO;j++)temp_lotno2[i][j] = NULL;
		}
		dsp_qc_set_lot();
		dsp_cursor_qc_lot(cursor_set);
	}else if(cod==15){							//SpaceBar
		if(cursor_set<MAX_LOTNO){
			for(i=tempMeasCnt;i<1000;i++)temp_lotno2[i][cursor_set] =  ' ';
			cursor_set++;
			for(i=tempMeasCnt;i<1000;i++)temp_lotno2[i][cursor_set] =  NULL;
			if(cursor_set>=MAX_LOTNO) cursor_set--;
			dsp_qc_set_lot();
			dsp_cursor_qc_lot(cursor_set);
		}else tch_error();
	}else{										//0~~9, A~Z
		if(cursor_set<MAX_LOTNO){
			cod -= 16;
			if(cod<=9) cod += '0';
			else cod = (cod-10)+'A';
			if(cod<='Z'){
				for(i=tempMeasCnt;i<1000;i++)temp_lotno2[i][cursor_set] = cod;
				cursor_set++;
				for(i=tempMeasCnt;i<1000;i++){
					for(j=cursor_set;j<MAX_LOTNO;j++)temp_lotno2[i][j] = NULL;
				}
				if(cursor_set>=MAX_LOTNO) cursor_set--;
				dsp_qc_set_lot();
				dsp_cursor_qc_lot(cursor_set);
			}
		}else tch_error();
	}
}
/*****************************************************************************/
void tch_qc_wait(void) {
	tcode = tch_check(tch_qcwait_tb, tch_qcwait_no);
	printf0(" tcode:%d", tcode);
	if (tcode == 0) {								//home
		if (!motor_step) {
			key_buzzer();
			dsp_home();
			modex = MODE_HOME;
			flag_motor_run = 0;
			flag_self = 0;
		}
	}
	else if (tcode == 1) {							//back
		if (!motor_step) {
			key_buzzer();
			cancel_qc_set();
			modex = MODE_QC;
			dsp_qc();
		}
	}	
	if (tcode == 2) {								//seq
/*		key_buzzer();
		modex = MODE_QC_seq;
		cursor_set = 0;
		dsp_cursor_qc_seq(cursor_set);*/
	}
	else if (tcode == 3) {							//strip
		key_buzzer();
		modex = MODE_QC_strip;
		cursor_set = temp_stripno2[tempMeasCnt];
		dsp_qc_set_strip();
	}
	else if (tcode == 4) {							//neg/pos
		key_buzzer();
		modex = MODE_QC_negpos;
		if (!temp_negpos2[tempMeasCnt]) temp_negpos2[tempMeasCnt] = 1;
		else temp_negpos2[tempMeasCnt] = 0;
		dsp_qc_set_negpos();
	}
	else if (tcode == 5) {							//negative
		key_buzzer();
		modex = MODE_QC_negpos;
		temp_negpos2[tempMeasCnt] = 0;
		tempSampleID2[tempMeasCnt][8] = 'N';
		tempSampleID2[tempMeasCnt][9] = 'E';
		tempSampleID2[tempMeasCnt][10] = 'G';
		tempSampleID2[tempMeasCnt][11] = 'A';
		tempSampleID2[tempMeasCnt][12] = 'T';
		tempSampleID2[tempMeasCnt][13] = 'I';
		tempSampleID2[tempMeasCnt][14] = 'V';
		tempSampleID2[tempMeasCnt][15] = 'E';
		dsp_qc_set_negpos();
	}
	else if (tcode == 6) {							//Positive
		key_buzzer();
		modex = MODE_QC_negpos;
		temp_negpos2[tempMeasCnt] = 1;
		tempSampleID2[tempMeasCnt][8] = 'P';
		tempSampleID2[tempMeasCnt][9] = 'O';
		tempSampleID2[tempMeasCnt][10] = 'S';
		tempSampleID2[tempMeasCnt][11] = 'I';
		tempSampleID2[tempMeasCnt][12] = 'T';
		tempSampleID2[tempMeasCnt][13] = 'I';
		tempSampleID2[tempMeasCnt][14] = 'V';
		tempSampleID2[tempMeasCnt][15] = 'E';
		dsp_qc_set_negpos();
	}
	else if (tcode == 7) {							//lot no
		modex = MODE_QC_lot;
		key_buzzer();
		dsp_qc_set_lot();
	}
	else if (tcode == 8) {							//SAVE
		key_buzzer();
		modex = MODE_QC;
		enter_qc_set();
	}
	else if (tcode == 9) {							//CLOSE
		key_buzzer();
		cancel_qc_set();
		modex = MODE_QC;
		dsp_qc();
	}else if (tcode == 10) {							//seq up
		key_buzzer();
		if (tempMeasCnt < 99 && temp_seqno2[tempMeasCnt] < END_QC_NO) {
			tempMeasCnt++;
			temp_seqno2[tempMeasCnt] = temp_seqno2[tempMeasCnt-1]+1;
			//if(tempMeasCnt>maxTempCnt)maxTempCnt = tempMeasCnt;
			dsp_qc_set();
			//if (modex == MODE_QC_seq) dsp_cursor_qc_seq(cursor_set);
		}
		else tch_error();
	}
	else if (tcode == 11) {							//seq dn
		key_buzzer();
		if (tempMeasCnt > measCnt && temp_seqno2[tempMeasCnt] > SRT_QC_NO) {
			tempMeasCnt--;
			temp_seqno2[tempMeasCnt] = temp_seqno2[tempMeasCnt+1]-1;
			dsp_qc_set();
			//if (modex == MODE_QC_seq) dsp_cursor_qc_seq(cursor_set);
		}
		else tch_error();
	}
}
/*****************************************************************************/
void tch_qc_set_strip(void) {
	if (flag_dsp_strip) {
		tcode = tch_check(tch_qcset_strip_tb, tch_qcset_strip_no);
		printf0(" tcode:%d,fds:%d", tcode, flag_dsp_strip);
		if (tcode <= 8) {								//strip
			key_buzzer();
			tcode += cursor_set;
			if (tcode < MAX_STRIPNO) {
				cursor_set = temp_stripno2[tempMeasCnt] = tcode;
				dsp_qc_set();
				flag_dsp_strip = 0;
			}
			else tch_error();
		}
		else if (tcode == 9) {							//up
			key_buzzer();
			if (cursor_set) {
				cursor_set--;
				dsp_qc_set_strip();
			}
			else tch_error();
		}
		else if (tcode == 10) {						//down
			key_buzzer();
			if (cursor_set < (MAX_STRIPNO - 1)) {
				cursor_set++;
				dsp_qc_set_strip();
			}
			else tch_error();
		}
		else if (tcode == 11) {						//SAVE
			key_buzzer();
			modex = MODE_QC;
			enter_qc_set();
		}
		else if (tcode == 12) {						//CLOSE
			key_buzzer();
			cancel_qc_set();
			modex = MODE_QC;
			dsp_qc();
		}
		else if (tcode == 13)						//SEQ
		{
/*			key_buzzer();
			modex = MODE_QC_seq;
			dsp_qc_set();
			cursor_set = 0;
			dsp_cursor_qc_seq(cursor_set);*/
		}
		else if (tcode == 14)						//SEQ No. Up
		{
			key_buzzer();
			dsp_qc_set();
		if (tempMeasCnt < 99 && temp_seqno2[tempMeasCnt] < END_QC_NO) {
				tempMeasCnt++;
				temp_seqno2[tempMeasCnt] = temp_seqno2[tempMeasCnt-1]+1;
				//if(tempMeasCnt>maxTempCnt)maxTempCnt = tempMeasCnt;
				dsp_qc_set_seqno();
				//if (modex == MODE_QC_seq) dsp_cursor_qc_seq(cursor_set);
			}
			else tch_error();
		}
		else if (tcode == 15)						//SEQ No. Down
		{
			key_buzzer();
			dsp_qc_set();
		if (tempMeasCnt > measCnt && temp_seqno2[tempMeasCnt] > SRT_QC_NO) {
				tempMeasCnt--;
				temp_seqno2[tempMeasCnt] = temp_seqno2[tempMeasCnt+1]-1;
				dsp_qc_set_seqno();
				//if (modex == MODE_QC_seq) dsp_cursor_qc_seq(cursor_set);
			}
			else tch_error();
		}
		else if (tcode == 16)						//Color �� Lot No.
		{
			key_buzzer();
			dsp_qc_set();
			modex = MODE_QC_lot;
			key_buzzer();
			dsp_qc_set_lot();
		}
		else if (tcode == 17)						//Turbidity
		{
			//Not available
		}
		else if (tcode == 18)						//Sample ID1 �� QC Left
		{
			key_buzzer();
			dsp_qc_set();
			modex = MODE_QC_negpos;
			temp_negpos2[tempMeasCnt] = 0;
			tempSampleID2[tempMeasCnt][8] = 'N';
			tempSampleID2[tempMeasCnt][9] = 'E';
			tempSampleID2[tempMeasCnt][10] = 'G';
			tempSampleID2[tempMeasCnt][11] = 'A';
			tempSampleID2[tempMeasCnt][12] = 'T';
			tempSampleID2[tempMeasCnt][13] = 'I';
			tempSampleID2[tempMeasCnt][14] = 'V';
			tempSampleID2[tempMeasCnt][15] = 'E';
			dsp_qc_set_negpos();
		}
		else if (tcode == 19)						//Sample ID2 �� QC Right
		{
			key_buzzer();
			dsp_qc_set();
			modex = MODE_QC_negpos;
			temp_negpos2[tempMeasCnt] = 1;
			tempSampleID2[tempMeasCnt][8] = 'P';
			tempSampleID2[tempMeasCnt][9] = 'O';
			tempSampleID2[tempMeasCnt][10] = 'S';
			tempSampleID2[tempMeasCnt][11] = 'I';
			tempSampleID2[tempMeasCnt][12] = 'T';
			tempSampleID2[tempMeasCnt][13] = 'I';
			tempSampleID2[tempMeasCnt][14] = 'V';
			tempSampleID2[tempMeasCnt][15] = 'E';
			dsp_qc_set_negpos();
		}
	}
	else {
		tch_qc_wait();
	}
}
/*****************************************************************************/
/*********************  TTTTTTTTTTTTTTTTTTTTTTTT  ****************************/
/*********************  TTTTTTTTTTTTTTTTTTTTTTTT  ****************************/
/*********************  TTTTTTTTTTTTTTTTTTTTTTTT  ****************************/
/*****************************************************************************/
void tch_test(void) {
	tcode = tch_check(tch_test_tb, tch_test_no);
	if (tcode == 0) {								//home
		key_buzzer();
		if (!motor_step) {
			if(returnFlag){
				key_buzzer();
				modex = MODE_TEST_Return;
				LCDbmpxx(bm_popup14_top, 250, 185);
				LCDbmpxx(bm_popup12_mid, 250, 185+50);
				LCDbmpxx(bm_popup3_bottom, 250, 185+50+70);
			}else{
				dsp_home();
				modex = MODE_HOME;
				flag_motor_run = 0;
				flag_self = 0;
			}
		}
	}
	else if (tcode == 1) {							//back
		
		if (!motor_step) {
			if(returnFlag){
				key_buzzer();
				modex = MODE_TEST_Return;
				LCDbmpxx(bm_popup14_top, 250, 185);
				LCDbmpxx(bm_popup12_mid, 250, 185+50);
				LCDbmpxx(bm_popup3_bottom, 250, 185+50+70);
			}else{
				key_buzzer();
				dsp_measure();
				LCDbmpxx(bm_topbar, 0, 480 - 50);
				dsp_date();
				dsp_time();
				modex = MODE_MEAS;
				flag_motor_run = 0;
				flag_self = 0;
			}
		}
	}
	else if (tcode == 2) {							//
		key_buzzer();
	}
	else if (tcode == 3) {							//up
		key_buzzer();
		if (los_cursor > 0) {
			los_cursor--;
			dsp_los_table();
		}
	}
	else if (tcode == 4) {							//down
		key_buzzer();
		//if (measCnt>los_cursor+12) {
		if(maxTempCnt>los_cursor+12){
			los_cursor++;
			dsp_los_table();
		}
	}
	else if (tcode == 5) {							//SEQ
		key_buzzer();
		prt_msg("\r MODE_TEST_seq");
		modex = MODE_TEST_sid;//MODE_TEST_seq;
		dsp_test_set();
	}
	else if (tcode <= 7) {							//USER ID
		key_buzzer();
		prt_msg("\r MODE_TEST_sid");
		modex = MODE_TEST_sid;
		buff_user_id[0] = NULL;
		dsp_test_set();
		cursor_set = 0;
		dsp_cursor_test_sid(cursor_set);
	}
	else if (tcode == 8) {							//color
		key_buzzer();
		prt_msg("\r MODE_TEST_color");
		modex = MODE_TEST_color;
		dsp_test_set();
		dsp_test_set_color();
	}
	else if (tcode == 9) {							//trub
		key_buzzer();
		prt_msg("\r MODE_TEST_trubi");
		modex = MODE_TEST_trubi;
		dsp_test_set();
		dsp_test_set_turbi();
	}
	else if (tcode <= 21){//list of samples
		key_buzzer();
		MEASseqno = cur_dsp_record_no = temp_seqno[los_cursor+tcode-10];
		if (read_result(MEASseqno, 1) == OK) {					//result data ==> cmdbuff
			key_buzzer();
			make_result();
			conv_result_store();
			dsp_result_store();
		}
		if(SEQ_NO<=MEASseqno){
			if(maxTempCnt>=los_cursor+tcode-10) tempMeasCnt = los_cursor+tcode-10;
			else if(maxTempCnt==0)tempMeasCnt = 0;
			else tempMeasCnt = maxTempCnt;
			modex = MODE_TEST_sid;
			dsp_test_set();
		}
	}
}

/*****************************************************************************/

void tch_test_return(void){
	tcode=tch_check(tch_mem_printsend_tb, tch_mem_printsend_no);
	if(tcode==0){								//home
		key_buzzer();
//		dsp_home();
//		modex = MODE_HOME;
	}else if(tcode==1){							//back
		key_buzzer();
		dsp_test();
		modex = MODE_TEST;
	}else if(tcode==2){							//confirm
		key_buzzer();
		modex = MODE_MEAS;
		dsp_measure();
		LCDbmpxx(bm_topbar, 0, 480 - 50);
		flag_motor_run = 0;
		flag_self = 0;
		dsp_date();
		dsp_time();
	}else if(tcode==3){							//cancel
		key_buzzer();
		dsp_test();
		modex = MODE_TEST;
	}
}

void tch_test_set(void) {
	tcode = tch_check(tch_testset_tb, tch_testset_no);
	printf0(" tcode:%d", tcode);
	if (tcode == 0) {								//home
		key_buzzer();
		if (!motor_step) {
			dsp_home();
			modex = MODE_HOME;
			flag_motor_run = 0;
			flag_self = 0;
		}
	}else if (tcode == 10 || tcode == 1) {							//CLOSE
		key_buzzer();
		cancel_test_set();
		modex = MODE_TEST;
		dsp_test();
	}else if (tcode == 2) {								//seq
/*		if (modex != MODE_TEST_seq) {
			key_buzzer();
			modex = MODE_TEST_seq;
			cursor_set = 0;
			dsp_test_set();
			dsp_cursor_test_seq(cursor_set);
		}*/
	}
	else if (tcode == 3) {							//seq up
		key_buzzer();
		if (temp_seqno2[tempMeasCnt] < MAX_RECORD && tempMeasCnt < 999) {
			tempMeasCnt++;
			temp_seqno2[tempMeasCnt] = temp_seqno2[tempMeasCnt-1]+1;
			//printf3("%d %d", tempMeasCnt, maxTempCnt);
			//if(tempMeasCnt>maxTempCnt)maxTempCnt = tempMeasCnt;
			dsp_test_set();
			/*if (modex == MODE_TEST_seq){
				 dsp_cursor_test_seq(cursor_set);
			}else */if (modex == MODE_TEST_sid){
				 dsp_cursor_test_sid(cursor_set);
			}
		}
		else if(tempMeasCnt == 999)	{
			key_buzzer();
			tempMeasCnt = 0;
		}
		else tch_error();
	}
	else if (tcode == 4) {							//seq dn
		key_buzzer();
		if (temp_seqno2[tempMeasCnt] > 1 && tempMeasCnt>measCnt) {
			tempMeasCnt--;
			temp_seqno2[tempMeasCnt] = temp_seqno2[tempMeasCnt+1]-1;
			dsp_test_set();
			/*if (modex == MODE_TEST_seq){
				dsp_cursor_test_seq(cursor_set);
			}else */if (modex == MODE_TEST_sid){
				 dsp_cursor_test_sid(cursor_set);
			}
		}
		else tch_error();
	}
/*	else if (tcode == 3) {							//strip
		if (modex != MODE_TEST_strip) {
			modex = MODE_TEST_strip;
			key_buzzer();
			cursor_set = temp_stripno2[tempMeasCnt];
			dsp_test_set();
			dsp_test_set_strip();
		}
	}*/
	else if (tcode <= 6) {							//sample id
		if (modex != MODE_TEST_sid) {
			modex = MODE_TEST_sid;
			key_buzzer();
			dsp_test_set();
			cursor_set = 0;
			dsp_cursor_test_sid(cursor_set);
		}
	}
	else if (tcode == 7) {							//color
		if (modex != MODE_TEST_color) {
			modex = MODE_TEST_color;
			key_buzzer();
			dsp_test_set();
			dsp_test_set_color();
		}
	}
	else if (tcode == 8) {							//turbi
		if (modex != MODE_TEST_trubi) {
			modex = MODE_TEST_trubi;
			key_buzzer();
			dsp_test_set();
			dsp_test_set_turbi();
		}
	}
	else if (tcode == 9) {							//SAVE
		key_buzzer();
		modex = MODE_TEST;
		enter_test_set();
		//dsp_test();
	}

	else {	//'<'	//CLEAR		//SPACEBAR		//TEXT
		key_buzzer();
		/*if (modex == MODE_TEST_seq) digit_test_seq(tcode);
		else */if (modex == MODE_TEST_sid) alpa_test_sid(tcode);
	}
}
/*****************************************************************************/
void digit_test_seq(u16 cod){
u16 i;
//	printf0("*%d",cod);
	if(cod==9){									//'<'
		if(cursor_set){
			cursor_set--;
			clr_cursor_test_set();
			dsp_cursor_test_seq(cursor_set);
		}else tch_error();
	}else if(cod==10){							//Clear
		temp_seqno2[tempMeasCnt] = 0;
		dsp_test_set_seqno();
		cursor_set=0;
		dsp_cursor_test_seq(cursor_set);
	}else if(cod==11){							//SpaceBar
		tch_error();
	}else if(cod<=21){
		cod -= 12;
		switch(cursor_set){
			case 0:	cod *= 1000; i =  (temp_seqno2[tempMeasCnt]/1000)*1000; 		break;
			case 1:	cod *=  100; i = ((temp_seqno2[tempMeasCnt]%1000)/100)*100; 	break;
			case 2:	cod *=   10; i = ((temp_seqno2[tempMeasCnt]%100)/10)*10;	 	break;
			case 3:	cod *=    1; i =  (temp_seqno2[tempMeasCnt]%10);			 	break;
		}
		temp_seqno2[tempMeasCnt] += cod - i;
		sprintf(buff_seqno, "%4d", temp_seqno2[tempMeasCnt]);
		fill_zero(buff_seqno);
		dsp_test_set_seqno();
		if(cursor_set<3) cursor_set++;
		dsp_cursor_test_seq(cursor_set);
	}else tch_error();
}
/*****************************************************************************/
void alpa_test_sid(u16 cod){
	if(cod==11){									//'<'
		if(cursor_set){
			cursor_set--;
			tempSampleID2[tempMeasCnt][cursor_set] = NULL;
			dsp_test_set_sid();
			clr_cursor_test_set();
			dsp_cursor_test_sid(cursor_set);
		}else tch_error();
	}else if(cod==12){							//Clear
		tempSampleID2[tempMeasCnt][cursor_set] = NULL;
		cursor_set=0;
		tempSampleID2[tempMeasCnt][cursor_set] = NULL;
		dsp_test_set_sid();
		dsp_cursor_test_sid(cursor_set);
	}else if(cod==13){							//SpaceBar
		if(cursor_set<MAX_USERID){
			tempSampleID2[tempMeasCnt][cursor_set++] = ' ';
			tempSampleID2[tempMeasCnt][cursor_set] = NULL;
			dsp_test_set_sid();
			if(cursor_set<MAX_USERID) dsp_cursor_test_sid(cursor_set);
		}else tch_error();
	}else{										//0~~9, A~Z
		if(cursor_set<MAX_USERID){
			cod -= 14;
			if(cod<=9) cod += '0';
			else cod = (cod-10)+'A';
			if(cod<='Z'){
				printf0("\n%d:%c\n", cursor_set, cod);
				tempSampleID2[tempMeasCnt][cursor_set++] = cod;
				tempSampleID2[tempMeasCnt][cursor_set] = NULL;
				dsp_test_set_sid();
				if(cursor_set<MAX_USERID) dsp_cursor_test_sid(cursor_set);
			}
		}else tch_error();
	}
}
/*****************************************************************************/
void tch_test_wait(void){
	tcode=tch_check(tch_testwait_tb, tch_testwait_no);
printf0(" tcode:%d", tcode);	
	if (tcode == 0) {								//home
		key_buzzer();
		if (!motor_step) {
			dsp_home();
			modex = MODE_HOME;
			flag_motor_run = 0;
			flag_self = 0;
		}
	}else if(tcode==2){								//seq
		/*key_buzzer();
		modex = MODE_TEST_seq;
		dsp_test_set_seqno();
		cursor_set=0;
		dsp_cursor_test_seq(cursor_set);*/
	}else if(tcode==3){							//strip
/*		key_buzzer();
		modex = MODE_TEST_strip;
		cursor_set = temp_stripno[tempMeasCnt];
		dsp_test_set_strip();*/
	}else if(tcode==4){							//sample id
		modex = MODE_TEST_sid;
		key_buzzer();
		dsp_test_set_sid();
		cursor_set=0;
		dsp_cursor_test_sid(cursor_set);
	}else if(tcode==5){							//color
		modex = MODE_TEST_color;
		key_buzzer();
		dsp_test_set_color();
	}else if(tcode==6){							//turbi
		modex = MODE_TEST_trubi;
		key_buzzer();
		dsp_test_set_turbi();
	}else if(tcode==7){							//SAVE
		key_buzzer();
		enter_test_set();
		modex = MODE_TEST;
		dsp_test();
	}else if(tcode == 1 || tcode==8){							//CLOSE
		key_buzzer();
		cancel_test_set();
		modex = MODE_TEST;
		dsp_test();
	}else if (tcode == 9) {							//seq up
		key_buzzer();
		if (temp_seqno2[tempMeasCnt] < MAX_RECORD && tempMeasCnt < 999) {
			tempMeasCnt++;
			temp_seqno2[tempMeasCnt] = temp_seqno2[tempMeasCnt-1]+1;
			//if(tempMeasCnt>maxTempCnt)maxTempCnt = tempMeasCnt;
			dsp_test_set();
		}
		else if(tempMeasCnt == 999)	{
			key_buzzer();
			tempMeasCnt = 0;
		}
		else tch_error();
	}
	else if (tcode == 10) {							//seq dn
		key_buzzer();
		if (temp_seqno2[tempMeasCnt] > 1 && tempMeasCnt > measCnt) {
			tempMeasCnt--;
			temp_seqno2[tempMeasCnt] = temp_seqno2[tempMeasCnt+1]-1;
			dsp_test_set();
		}
		else tch_error();
	}
}
/*****************************************************************************/
void tch_test_set_strip(void) {
	if (flag_dsp_strip) {
		tcode = tch_check(tch_testset_strip_tb, tch_testset_strip_no);
		printf0(" tcode:%d,fds:%d", tcode, flag_dsp_strip);
		if (tcode == 0) {								//home
			key_buzzer();
			if (!motor_step) {
				dsp_home();
				modex = MODE_HOME;
				flag_motor_run = 0;
				flag_self = 0;
			}
		}
		else if (tcode == 1 || tcode == 14) {						//Back, CLOSE
			key_buzzer();
			cancel_test_set();
			modex = MODE_TEST;
			dsp_test();
		}else if (tcode <= 10) {								//strip
			key_buzzer();
			tcode -= 2;
			tcode += cursor_set;
			if (tcode < MAX_STRIPNO) {
				cursor_set = temp_stripno2[tempMeasCnt] = tcode;
				flag_dsp_strip = 0;
				dsp_test_set();
			}
			else tch_error();
		}
		else if (tcode == 11) {							//up
			key_buzzer();
			if (cursor_set) {
				cursor_set--;
				dsp_test_set_strip();
			}
			else tch_error();
		}
		else if (tcode == 12) {						//down
			key_buzzer();
			if (cursor_set < (MAX_STRIPNO - 1)) {
				cursor_set++;
				dsp_test_set_strip();
			}
			else tch_error();
		}
		else if (tcode == 13) {						//SAVE
			key_buzzer();
			enter_test_set();
			modex = MODE_TEST;
			dsp_test();
		}
		else if (tcode == 15)					//SEQ
		{/*
			key_buzzer();
			modex = MODE_TEST_seq;
			dsp_test_set();
			cursor_set = 0;
			dsp_cursor_test_seq(cursor_set);
		*/}
		else if (tcode == 16)					//SEQ Up
		{
			key_buzzer();

			if (temp_seqno2[tempMeasCnt] < MAX_RECORD && tempMeasCnt < 999 ) {
				tempMeasCnt++;
				temp_seqno2[tempMeasCnt] = temp_seqno2[tempMeasCnt-1]+1;
				//if(tempMeasCnt>maxTempCnt)maxTempCnt = tempMeasCnt;
				dsp_test_set();
				dsp_test_set_strip();

			}
			else tch_error();
		}
		else if (tcode == 17)					//SEQ Down
		{
			key_buzzer();

			if (temp_seqno2[tempMeasCnt] > 1 && tempMeasCnt > measCnt) {
				tempMeasCnt--;
				temp_seqno2[tempMeasCnt] = temp_seqno2[tempMeasCnt+1]-1;
				dsp_test_set();
				dsp_test_set_strip();
			}
			else tch_error();
		}
		else if (tcode == 18)					//Color
		{
			modex = MODE_TEST_color;
			key_buzzer();
			dsp_test_set();
		}
		else if (tcode == 19)					//Turbidity
		{
			modex = MODE_TEST_trubi;
			key_buzzer();
			dsp_test_set();
			dsp_test_set_turbi();
		}
		else if (tcode <= 21)						//Sample ID
		{
			modex = MODE_TEST_sid;
			key_buzzer();
			dsp_test_set();
			cursor_set = 0;
			dsp_cursor_test_sid(cursor_set);
		}
	}
	else {
		tch_test_wait();
	}
}
/*****************************************************************************/
void tch_test_set_color(void) {
 	if (flag_dsp_color) {
		tcode = tch_check(tch_testset_color_tb, tch_testset_color_no);
		if (tcode == 0) {								//home
			key_buzzer();
			if (!motor_step) {
				dsp_home();
				modex = MODE_HOME;
				flag_motor_run = 0;
				flag_self = 0;
			}
		}else if (tcode == 1 || tcode == 12) {						//Back, CLOSE
			key_buzzer();
			cancel_test_set();
			modex = MODE_TEST;
			dsp_test();
		}else if (tcode == 11) {									//SAVE
			key_buzzer();
			enter_test_set();
			modex = MODE_TEST;
			dsp_test();
		}
		else if (tcode <= 10) {								//color
			key_buzzer();
			temp_colorno2[tempMeasCnt] = tcode-2;
			flag_dsp_color = 0;
			dsp_test_set();
		}
		else if (tcode == 11)					//SEQ
		{
			/*key_buzzer();
			modex = MODE_TEST_seq;
			dsp_test_set();
			cursor_set = 0;
			dsp_cursor_test_seq(cursor_set);
		*/}
		else if (tcode == 14)					//SEQ Up
		{
			key_buzzer();
			if (temp_seqno2[tempMeasCnt] < MAX_RECORD && tempMeasCnt <999) {
				tempMeasCnt++;
				temp_seqno2[tempMeasCnt] = temp_seqno2[tempMeasCnt-1]+1;
				//if(tempMeasCnt>maxTempCnt)maxTempCnt = tempMeasCnt;
				dsp_test_set();
				dsp_test_set_color();
			}
			else tch_error();
		}
		else if (tcode == 15)					//SEQ Down
		{
			key_buzzer();
			if (temp_seqno2[tempMeasCnt] > 1 && tempMeasCnt > measCnt) {
				tempMeasCnt--;
				temp_seqno2[tempMeasCnt] = temp_seqno2[tempMeasCnt+1]-1;
				dsp_test_set();
				dsp_test_set_color();
			}
			else tch_error();
		}
		/*else if (tcode == 14)					//Strip
		{
			key_buzzer();
			modex = MODE_TEST_strip;
			cursor_set = temp_stripno2[tempMeasCnt];
			dsp_test_set();
		}*/
		else if (tcode == 17)					//Turbidity
		{
			modex = MODE_TEST_trubi;
			key_buzzer();
			dsp_test_set();
			dsp_test_set_turbi();
		}
		else if (tcode == 18)						//Sample ID
		{
			modex = MODE_TEST_sid;
			key_buzzer();
			dsp_test_set();
			cursor_set = 0;
			dsp_cursor_test_sid(cursor_set);
		}
	}
	else {
		tch_test_wait();
	}
}
/*****************************************************************************/
void tch_test_set_turbi(void) {
	if (flag_dsp_turbi) {
		tcode = tch_check(tch_testset_turbi_tb, tch_testset_turbi_no);
		printf0(" turbi code:%d,fds:%d", tcode, flag_dsp_turbi);
		if (tcode == 0) {								//home
			key_buzzer();
			if (!motor_step) {
				dsp_home();
				modex = MODE_HOME;
				flag_motor_run = 0;
				flag_self = 0;
			}
		}else if (tcode == 11) {									//SAVE
			key_buzzer();
			enter_test_set();
			modex = MODE_TEST;
			dsp_test();
		}
		else if (tcode == 1 || tcode == 12) {							//CLOSE
			key_buzzer();
			cancel_test_set();
			modex = MODE_TEST;
			dsp_test();
		}
		else if (tcode <= 10) {								//turbi
			key_buzzer();
			temp_turbino2[tempMeasCnt] = tcode-2;
			flag_dsp_turbi = 0;
			dsp_test_set();
		}
		else if (tcode == 13)					//SEQ
		{/*
			key_buzzer();
			modex = MODE_TEST_seq;
			cursor_set = 0;
			dsp_test_set();
			dsp_cursor_test_seq(cursor_set);
		*/}
		else if (tcode == 14)					//SEQ Up
		{
			key_buzzer();
			dsp_test_set();
			if (temp_seqno2[tempMeasCnt] < MAX_RECORD && tempMeasCnt < 999) {
				tempMeasCnt++;
				temp_seqno2[tempMeasCnt] = temp_seqno2[tempMeasCnt-1]+1;
				//if(tempMeasCnt>maxTempCnt)maxTempCnt = tempMeasCnt;
				dsp_test_set();
			}
			else tch_error();
		}
		else if (tcode == 15)					//SEQ Down
		{
			key_buzzer();
			dsp_test_set();
			if (temp_seqno2[tempMeasCnt] > 1 && tempMeasCnt > measCnt) {
				tempMeasCnt--;
				temp_seqno2[tempMeasCnt] = temp_seqno2[tempMeasCnt+1]-1;
				dsp_test_set();
			}
			else tch_error();
		}
		else if (tcode == 17)					//Color
		{
			modex = MODE_TEST_color;
			key_buzzer();
			dsp_test_set();
			dsp_test_set_color();
		}
		else if (tcode == 18)						//Sample ID
		{
			modex = MODE_TEST_sid;
			key_buzzer();
			dsp_test_set();
			cursor_set = 0;
			dsp_cursor_test_sid(cursor_set);
		}
	}
	else {
		tch_test_wait();
	}
}
/*****************************************************************************/
/*****************************************************************************/
void key_clear(void){	tchy=tchx=0; }
/*****************************************************************************/
u16 tch_check(const u16 *tb, u8 tno){
u16 a, b, c;
s16 x, y;
	x=tchx; y=tchy;
//	printf3("\r x:%4d, y:%4d", x, y);

	for(a=0;a<tno;a++){
		b = tb[(a*4)+0];
		c = tb[(a*4)+1];
		if((x>b)&&(x<c)){
			b = tb[(a*4)+2];
			c = tb[(a*4)+3];
			if((y>b)&&(y<c)) break;
		}
	}
	return(a);	
}
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
u8	push_tch, pull_tch;
#define MIN_THY			50
#define MIN_THX			25
#define MIN_PME			5
#define TCH_DLY			5		// was 5
extern u16 yp_pre, xp_pre;
void tch_read(void){
u16 xx, yy;	
	if(push_tch<TCH_DLY){
		if((yp_adc>MIN_THY)&&(xp_adc>MIN_THX)){
			if(yp_adc>(yp_pre+MIN_PME)) push_tch = 0;
			else if(yp_adc<(yp_pre-MIN_PME)) push_tch = 0;
			if(xp_adc>(xp_pre+MIN_PME)) push_tch = 0;
			else if(xp_adc<(xp_pre-MIN_PME)) push_tch = 0;
			yp_pre = yp_adc;
			xp_pre = xp_adc;
			if(xp_adc>900) xx = 0;	else xx = 900 - xp_adc;
			if(yp_adc>900) yy = 0;	else yy = 900 - yp_adc;
			if((++push_tch)>=TCH_DLY){
				tchx = xx;
				tchy = yy;
				printf0("\r>x%4d,y%4d,", tchx, tchy );
				pull_tch=0;
				/*if(!PCB_TEST||!RUN_MEAS)*/ key_decode();
				yp_pre = xp_pre = 0;
//			}else{
//				printf0("\r X%4d,Y%4d,", xx, yy );
			}
			
		}else push_tch = yp_pre = xp_pre = 0;
	}else{
		if(yp_adc<MIN_THY){
			if((++pull_tch)>=TCH_DLY) push_tch = pull_tch = 0;
		}else pull_tch=0;
	}
}

/*****************************************************************************/
void dbg_wait(char *str){
u8 kk, ps = 0;
	printf0(str); 
	push_tch=0;
	for(;;){ 
		tch_read(); 
		if(push_tch>=3) break; 
	} 
//	key_buzzer();
	yp_adc=0;
	for(;;){ 
		delay_ms(10);
		if(yp_adc<MIN_THY){
			if((++ps)>=5) break;
		}else{ 
			ps=0; yp_adc=0; 
		}
		if((++kk)>10){ printf0(" %d", yp_adc); kk=0; }
	}
	push_tch=0;
}
void tch_unit(void) {
	tcode = tch_check(tch_unit_tb, tch_unit_no);
	printf0(" tcode:%d", tcode);
	if (tcode == 0){								//home
		key_buzzer();
		dsp_home();
		modex = MODE_HOME;
	}
	else if (tcode == 1){							//back
		key_buzzer();
		dsp_setting();
		modex = MODE_SET;
	}
	else if (tcode == 2){							//save
		key_buzzer();
		dsp_setting();
		UNIT_FLAG = temp_UNIT_FLAG;
		PLUS_FLAG = temp_PLUS_FLAG;
		MARK_FLAG = temp_MARK_FLAG;
		write24w(UNIT_ADR, UNIT_FLAG);
		write24w(PLUS_ADR, PLUS_FLAG);
		store_vars(MARK_ADR, MARK_FLAG);	//RAM �� EEP Save
		modex = MODE_SET;
	}
	else if (tcode == 3){			//unit conv
		temp_UNIT_FLAG = UNIT_CONV;
		LCDbmpxx(bm_conv_w, 160, 150);
		LCDbmpxx(bm_si_g, 270, 150);
		key_buzzer();
	}
	else if (tcode == 4){			//unit S.I.
		temp_UNIT_FLAG = UNIT_SI;
		LCDbmpxx(bm_conv_g, 160, 150);
		LCDbmpxx(bm_si_w, 270, 150);
		key_buzzer();
	}
	else if (tcode == 5){			//plus On
		temp_PLUS_FLAG = 1;
		LCDbmpxx(bm_on_w, 160, 250);
		LCDbmpxx(bm_off_g, 270, 250);
		key_buzzer();
	}
	else if (tcode == 6){			//plus Off
		temp_PLUS_FLAG = 0;
		LCDbmpxx(bm_on_g, 160, 250);
		LCDbmpxx(bm_off_w, 270, 250);
		key_buzzer();
	}
	else if (tcode == 7){			//mark On
		temp_MARK_FLAG = 1;
		LCDbmpxx(bm_on_w, 160, 350);
		LCDbmpxx(bm_off_g, 270, 350);
		key_buzzer();
	}
	else if (tcode == 8){		//mark Off
	temp_MARK_FLAG = 0;
		LCDbmpxx(bm_on_g, 160, 350);
		LCDbmpxx(bm_off_w, 270, 350);
		key_buzzer();
	}
}
/*****************************************************************************/
/*****************************************************************************/

void tch_sysset(void) {
	tcode = tch_check(tch_sysset_tb, tch_sysset_no);
	printf0(" tcode:%d", tcode);
	if (tcode == 0) {								//home
		key_buzzer();

		if(PRINT_FLAG != read24w(PRINT_ADR))returnFlag = 1;
		if(BAUD_FLAG != read24w(BAUD_ADR))returnFlag = 1;
		if(SBAUD_FLAG != read24w(SBAUD_ADR))returnFlag = 1;
		if(SOUND_FLAG != read24w(SOUND_ADR))returnFlag = 1;
		if(LANGUAGE_FLAG != read24w(LANGUAGE_ADR))returnFlag = 1;
		if(sampleIDFlag != read24w(SAMPLE_ADR))returnFlag = 1;
		if(AUTO_FLAG != read24w(AUTO_ADR))returnFlag = 1;

		if(returnFlag){
			temp_modex = MODE_HOME;
			modex = MODE_system_set_Return;
			LCDbmpxx(bm_popup15_top, 250, 185);
			LCDbmpxx(bm_popup13_mid, 250, 185+50);
			LCDbmpxx(bm_popup3_bottom, 250, 185+50+70);
		}
		else{
//		PRINT_FLAG = read24w(PRINT_ADR);
//		BAUD_FLAG = read24w(BAUD_ADR);
//		SBAUD_FLAG = read24w(SBAUD_ADR);
//		SOUND_FLAG = read24w(SOUND_ADR);
//		LANGUAGE_FLAG = read24w(LANGUAGE_ADR);
//		sampleIDFlag = read24w(SAMPLE_ADR);
//		AUTO_FLAG = read24w(AUTO_ADR);

			dsp_home();
			modex = MODE_HOME;
		}
	}
	else if (tcode == 1) {							//back
		key_buzzer();
		
		if(PRINT_FLAG != read24w(PRINT_ADR))returnFlag = 1;
		if(BAUD_FLAG != read24w(BAUD_ADR))returnFlag = 1;
		if(SBAUD_FLAG != read24w(SBAUD_ADR))returnFlag = 1;
		if(SOUND_FLAG != read24w(SOUND_ADR))returnFlag = 1;
		if(LANGUAGE_FLAG != read24w(LANGUAGE_ADR))returnFlag = 1;
		if(sampleIDFlag != read24w(SAMPLE_ADR))returnFlag = 1;
		if(AUTO_FLAG != read24w(AUTO_ADR))returnFlag = 1;

		if(returnFlag){
			temp_modex = MODE_SET;
			modex = MODE_system_set_Return;
			LCDbmpxx(bm_popup15_top, 250, 185);
			LCDbmpxx(bm_popup13_mid, 250, 185+50);
			LCDbmpxx(bm_popup3_bottom, 250, 185+50+70);
		}
		else{
			PRINT_FLAG = read24w(PRINT_ADR);
			BAUD_FLAG = read24w(BAUD_ADR);
			SBAUD_FLAG = read24w(SBAUD_ADR);
			SOUND_FLAG = read24w(SOUND_ADR);
			LANGUAGE_FLAG = read24w(LANGUAGE_ADR);
			sampleIDFlag = read24w(SAMPLE_ADR);
			AUTO_FLAG = read24w(AUTO_ADR);
			dsp_setting();
			modex = MODE_SET;
		}
	}
	else if (tcode == 2) {							//save
		key_buzzer();
		if(LANGUAGE_FLAG != read24w(LANGUAGE_ADR)){
			languageFlag = 1;
		}else{
			languageFlag = 0;
		}
		if(languageFlag){
			modex = MODE_system_set_Restart;
			dsp_fw_update();
		}else{
			
			write24w(PRINT_ADR, PRINT_FLAG);
			write24w(BAUD_ADR, BAUD_FLAG);
			write24w(SBAUD_ADR, SBAUD_FLAG);
			write24w(SOUND_ADR, SOUND_FLAG);
			write24w(LANGUAGE_ADR, LANGUAGE_FLAG);
			write24w(SAMPLE_ADR, sampleIDFlag);
			store_vars(AUTO_ADR, AUTO_FLAG);
			set_baud_rs232();
			set_baud_usb();

			dsp_setting();
			modex = MODE_SET;
		}
			

	}
	else if (tcode == 3) {							//Print 1
		LCDbmpxx(bm_1_w, 110, 130);
		LCDbmpxx(bm_2_g, 220, 130);
		LCDbmpxx(bm_0_g, 330, 130);
		PRINT_FLAG = 1;
		key_buzzer();
	}
	else if (tcode == 4){							//Print 2
		LCDbmpxx(bm_1_g, 110, 130);
		LCDbmpxx(bm_2_w, 220, 130);
		LCDbmpxx(bm_0_g, 330, 130);
		PRINT_FLAG = 2;
		key_buzzer();
	}
	else if (tcode == 5){							//Print 0
		LCDbmpxx(bm_1_g, 110, 130);
		LCDbmpxx(bm_2_g, 220, 130);
		LCDbmpxx(bm_0_w, 330, 130);
		PRINT_FLAG = 0;
		key_buzzer();
	}
	else if (tcode == 6){							//USB 9600
		LCDbmpxx(bm_9600_w, 110, 180);
		LCDbmpxx(bm_38400_g, 220, 180);
		LCDbmpxx(bm_115200_g, 330, 180);
		SBAUD_FLAG = 0;
		key_buzzer();
	}
	else if (tcode == 7){							//USB 38400
		LCDbmpxx(bm_9600_g, 110, 180);
		LCDbmpxx(bm_38400_w, 220, 180);
		LCDbmpxx(bm_115200_g, 330, 180);
		SBAUD_FLAG = 1;
		key_buzzer();
	}
	else if (tcode == 8){							//USB 115200
		LCDbmpxx(bm_9600_g, 110, 180);
		LCDbmpxx(bm_38400_g, 220, 180);
		LCDbmpxx(bm_115200_w, 330, 180);
		SBAUD_FLAG = 2;
		key_buzzer();
	}
	else if (tcode == 9){							//Serial 9600
		LCDbmpxx(bm_9600_w, 110, 230);
		LCDbmpxx(bm_38400_g, 220, 230);
		LCDbmpxx(bm_115200_g, 330, 230);
		BAUD_FLAG = 0;
		key_buzzer();
	}
	else if (tcode == 10){							//Serial 38400
		LCDbmpxx(bm_9600_g, 110, 230);
		LCDbmpxx(bm_38400_w, 220, 230);
		LCDbmpxx(bm_115200_g, 330, 230);
		BAUD_FLAG = 1;
		key_buzzer();
	}
	else if (tcode == 11){							//Serial 115200
		LCDbmpxx(bm_9600_g, 110, 230);
		LCDbmpxx(bm_38400_g, 220, 230);
		LCDbmpxx(bm_115200_w, 330, 230);
		BAUD_FLAG = 2;
		key_buzzer();
	}
	else if (tcode == 12){							//Sound On
		LCDbmpxx(bm_on_w, 110, 280);
		LCDbmpxx(bm_off_g, 220, 280);
		SOUND_FLAG = 1;
		key_buzzer();
	}
	else if (tcode == 13){							//Sound Off
		LCDbmpxx(bm_on_g, 110, 280);
		LCDbmpxx(bm_off_w, 220, 280);
		SOUND_FLAG = 0;
		key_buzzer();
	}
	else if (tcode == 14){							//Language 1
		LCDbmpxx(bm_lang1_w, 110, 330);
		LCDbmpxx(bm_lang2_g, 220, 330);
		LANGUAGE_FLAG = 0;
		key_buzzer();
	}
	else if (tcode == 15){							//Language 2
		LCDbmpxx(bm_lang1_g, 110, 330);
		LCDbmpxx(bm_lang2_w, 220, 330);
		LANGUAGE_FLAG = 1;
		key_buzzer();
	}
	else if (tcode == 16){							//Reset On
		LCDbmpxx(bm_on_w, 110, 380);
		LCDbmpxx(bm_off_g, 220, 380);
		AUTO_FLAG = 1;
		key_buzzer();
/*
		LCDbmpxx(bm_on_w, 110, 380);
		LCDbmpxx(bm_off_g, 220, 380);
		resetFlag = 1;
		key_buzzer();
		*/
	}
	else if (tcode == 17) {							//Reset Off
		LCDbmpxx(bm_on_g, 110, 380);
		LCDbmpxx(bm_off_w, 220, 380);
		AUTO_FLAG = 0;
		key_buzzer();
/*		
		LCDbmpxx(bm_on_g, 110, 380);
		LCDbmpxx(bm_off_w, 220, 380);
		resetFlag = 0;
		key_buzzer();
		*/
	}
	else if (tcode == 18){							//Sample ID On
	}
	else if (tcode == 19){							//Sample ID Off
	}
	else if (tcode == 20){							//Auto ID On
	}
	else if (tcode == 21){							//Auto ID Off	
	}
	else if (tcode == 22){							//Maintanence
		key_buzzer();
		modex = MODE_MAINTANENCE_password;
		cursor = cursor_set = 0;
		dsp_maintanence_password();
	}
	else {
		key_buzzer();
	}
}

void tch_sysset_return(void){
	tcode=tch_check(tch_mem_printsend_tb, tch_mem_printsend_no);
	if(tcode==0){								//home
		key_buzzer();
		//dsp_home();
		//modex = MODE_HOME;
	}else if(tcode==1){							//back
		key_buzzer();
		dsp_sysset();
		modex = MODE_system_set;
	}else if(tcode==2){							//confirm
		key_buzzer();
		PRINT_FLAG = read24w(PRINT_ADR);
		BAUD_FLAG = read24w(BAUD_ADR);
		SBAUD_FLAG = read24w(SBAUD_ADR);
		SOUND_FLAG = read24w(SOUND_ADR);
		LANGUAGE_FLAG = read24w(LANGUAGE_ADR);
		sampleIDFlag = read24w(SAMPLE_ADR);
		AUTO_FLAG = read24w(AUTO_ADR);
		if(temp_modex == MODE_HOME){
			dsp_home();
			modex = temp_modex;	
		}else{
			dsp_setting();
			modex = MODE_SET;
		}
	}else if(tcode==3){							//cancel
		key_buzzer();
		dsp_sysset();
		modex = MODE_system_set;
	}
}
/*****************************************************************************/
void tch_maintanence(void){
	tcode = tch_check(tch_maintanence_tb, tch_maintanence_no);
	printf0(" tcode:%d", tcode);
	if (tcode == 0){				//home
		key_buzzer();
		dsp_home();
		modex = MODE_HOME;
	}
	else if (tcode == 1){			//back
		key_buzzer();
		dsp_sysset();
		modex = MODE_system_set;
	}
	else if (tcode == 2){			//calibration
		key_buzzer();
		
		if(read_strip_flag){
			temp_modex2[errorCnt] = modex;
			errorStack[errorCnt++] = Err_PlateOut;			
			errorSwitch = 1;
		}else{
			modex = MODE_MAINTANENCE_cali;
			run_calibration();
		}
	}
	else if (tcode == 3){			//Firmware Update
		key_buzzer();
		modex = MODE_MAINTANENCE_fw;
		dsp_fw_update();
	}
	else if (tcode == 4){			//Image Update
		key_buzzer();
		dsp_image_update();
		modex = MODE_MAINTANENCE_image;
		msg24(100, 60, BLU_LCD, "Up grade PICTUREs from USB Memory");
		if (!first_USB_HS) USB_HS_Init();
		else { key_buzzer();	delay_ms(500); }
		fnb_update(BMP_UPDATE);
		if (!(flag_upload & 0x01)) {
			USB_HS_Init();
			delay_ms(100);
			fnb_update(BMP_UPDATE);
		}
		if(flag_upload&0x80){
			//out_line_32(288, 360, "No USB Memory!", RED_LCD);
			//printf0("\r No USB Memory");
			LCDbmpxx(bm_popup11_top, 250, 185);
			LCDbmpxx(bm_popup9_mid, 250, 185+50);
			LCDbmpxx(bm_popup2_bottom, 250, 185+50+70);
			key_buzzer();	delay_ms(300);
			key_buzzer();	delay_ms(300);
			key_buzzer();
			modex = MODE_MAINTANENCE_Confirm;
		}else if (!(flag_upload & 0x01)) {
			LCDbmpxx(bm_popup12_top, 250, 185);
			LCDbmpxx(bm_popup10_mid, 250, 185+50);
			LCDbmpxx(bm_popup2_bottom, 250, 185+50+70);
			//out_line_24(100, 350, "Fail Image Reading!", RED_LCD);
			key_buzzer();	delay_ms(300);
			key_buzzer();	delay_ms(300);
			key_buzzer();
			modex = MODE_MAINTANENCE_Confirm;
		}
		if(flag_upload & 0x01){
			RCC->CIR = 0x00000000;							// Disable all interrupts 
			jump_addr = *(__IO uint32_t*) (BOOT_ADDRESS + 4);
			jump_to_app = (fptr) jump_addr;
			__set_MSP(*(__IO uint32_t*) BOOT_ADDRESS);
			jump_to_app();
		}
	}
	else if (tcode == 5)			//Font Update
	{
		dsp_font_update();
		modex = MODE_MAINTANENCE_font;
		msg24(100, 60, BLU_LCD, "Up grade FONTs from USB Memory");
		//		USB_Init();
		if (!first_USB_HS) USB_HS_Init();
		else { key_buzzer();	delay_ms(500); }
		fnb_update(FONT_UPDATE);
		if (!(flag_upload & 0x02)) {
			USB_HS_Init();
			delay_ms(100);
			fnb_update(FONT_UPDATE);
		}
		if(flag_upload&0x80){
			//out_line_32(288, 360, "No USB Memory!", RED_LCD);
			//printf0("\r No USB Memory");
			LCDbmpxx(bm_popup11_top, 250, 185);
			LCDbmpxx(bm_popup9_mid, 250, 185+50);
			LCDbmpxx(bm_popup2_bottom, 250, 185+50+70);
			key_buzzer();	delay_ms(300);
			key_buzzer();	delay_ms(300);
			key_buzzer();
			modex = MODE_MAINTANENCE_Confirm;
		}else if (!(flag_upload & 0x02)) {
			//out_line_24(100, 350, "Fail Font Reading!", RED_LCD);
			LCDbmpxx(bm_popup12_top, 250, 185);
			LCDbmpxx(bm_popup10_mid, 250, 185+50);
			LCDbmpxx(bm_popup2_bottom, 250, 185+50+70);
			key_buzzer();	delay_ms(300);
			key_buzzer();	delay_ms(300);
			key_buzzer();
			modex = MODE_MAINTANENCE_Confirm;
		}
	}
	else if (tcode == 6)			//Serial Data
	{
		key_buzzer();
		temp_SERIAL_FLAG = SERIAL_FLAG;
		dsp_serial_data();
		modex = MODE_MAINTANENCE_serial;
	}
	else if (tcode == 7){			//Reset
		key_buzzer();
		modex = MODE_MAINTANENCE_Reset;
		dsp_fw_update();
	}
	else if (tcode == 8) {			//Save
		key_buzzer();

	}
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
void tch_mem_delete(void){
u32 radr, i;
	tcode=tch_check(tch_mem_printsend_tb, tch_mem_printsend_no);
printf0(" mem delete tcode:%d", tcode);	
//return;
	if(tcode==0){								//home
		key_buzzer();
		dsp_home();
		modex = MODE_HOME;
	}else if(tcode==1){							//back
		key_buzzer();
		dsp_memory();
		modex = MODE_MEMORY;
	}else if(tcode==2){							//confirm
		key_buzzer();
		for(i=0;i<SECTOR_SIZE;i++) flash_buff[i] = 0xFF;	
		if(test_type==GENERAL){
			//out_line_32(220-40, 345, "Deleting Data", BLK_LCD);
			printf0("\n Erase 1 ~ 4992"); 
			draw_box(250, 185+50+70+30, 550, 185+50+70+30+5, White);
			//LCDbmpxx(bm_popup2_bottom, 250, 185+50+70);
			for(i=0;i<(MAX_RECORD-8);i+=16){
				radr = RESULT_ADR + ((i/16) * SECTOR_SIZE);
				write_sflash(radr, SECTOR_SIZE);				//system_wr4k(radr);
				if(!(i%16)){
//					printf0(" %d,", i); 
//					out_line_32(220+60+152+(20*(i/480)), 345, ">", BLK_LCD);
					draw_box(250+(i/16)*300/312, 185+50+70+30, 250+((i/16)+1)*300/312, 185+50+70+30+5, RED_LCD);
//					draw_box(250, 185+50+70+30, 250+(300*(i/480)), 185+50+70+30+5, Red);
				}
			}
			radr = TEST_INDEX;
			for(i=0;i<=MAX_RECORD;i++){
				*(u8*)(radr) = 0x00;
				radr += INDEX_PAGE;
				del_index(i);
			}			
			printf0(" * Erase 4993 ~ 5000"); 
			for (i = 4993; i <= MAX_RECORD; i++) {
				delete_record(i);
			}
			srt_seqno = end_seqno = SEQ_NO = 1;
			store_seqno();
			
		}else{
			//out_line_32(220-24, 345, "Deleting QC", BLK_LCD);
			printf0("\n Erase QC"); 
			for(i=SRT_QC_NO;i<=END_QC_NO;i++){
				delete_record(i);
				del_index(i);
//				if(!((i-SRT_QC_NO)%10)){
					//out_line_32(220+44+132+(20*((i-SRT_QC_NO)/10)), 345, ">", BLK_LCD);
					if(i!=END_QC_NO)draw_box(250+(i-SRT_QC_NO-1)*3, 185+50+70+30, 250+(i-SRT_QC_NO-1)*3+3, 185+50+70+30+5, RED_LCD);
//				}
			}
			srt_qcno = end_qcno = QC_NO = SRT_QC_NO;
			store_vars(QC_NO_ADR, QC_NO);
		}
		key_buzzer();
		dsp_memory();
		fullFlag = 0;
		modex = MODE_MEMORY;
	}else if(tcode==3){							//exit
		key_buzzer();
		dsp_memory();
		modex = MODE_MEMORY;
	}
}
/*****************************************************************************/
void delete_memory_all(void){
	modex = MODE_MEM_delete;
	dsp_fw_update();
	do{
		if(MS10){
			tch_read();
			MS10=0;
		}		
	}while(modex==MODE_MEM_delete);
	memCount = -1;
}
/*****************************************************************************/
void update_bmp(void) {
	if (DIPSW_BMP) {
		dsp_image_update();
		msg24(100, 60, BLU_LCD, "Up grade PICTUREs from USB Memory");
		if (!first_USB_HS) USB_HS_Init();
		else { key_buzzer();	delay_ms(500); }
		fnb_update(BMP_UPDATE);
		if (!(flag_upload & 0x01)) {
			USB_HS_Init();
			delay_ms(100);
			fnb_update(BMP_UPDATE);
		}
		if (!(flag_upload & 0x01)) {
			out_line_24(100, 350, "Fail Image Reading!", RED_LCD);
			key_buzzer();	delay_ms(300);
			key_buzzer();	delay_ms(300);
			key_buzzer();	delay_ms(300);
		}
		else {
			delay_ms(900);
		}
	}
}
/*****************************************************************************/
void dayInMonth(u16 year, u8 month) {
	year += 2000;
	switch (month) {
	case 1: case 3: case 5: case 7: case 8: case 10: case 12:
		dayCount = 31;
		break;
	case 4: case 6: case 9: case 11:
		dayCount = 30;
		break;
	case 2:
		if (!(year % 400))dayCount = 29;
		else if (!(year % 100))dayCount = 28;
		else if (!(year % 4))dayCount = 29;
		else dayCount = 28;
		break;
	}
}
/*****************************************************************************/
void tch_printupload(void){
	u16 i;
	tcode=tch_check(tch_mem_printsend_tb, tch_mem_printsend_no);
	if(tcode==0){								//home
		key_buzzer();
		dsp_home();
		modex = MODE_HOME;
	}else if(tcode==1){							//back
		key_buzzer();
		dsp_memory();
		modex = MODE_MEMORY;
	}else if(tcode==2){							//confirm
		key_buzzer();
		if(modex == MODE_MEM_print){
			for (i = 0; i < memCount+1; i++) {
				if (read_result(searchSEQ[i], 1) == OK) {
					make_result();
					conv_result_store();
					print_result();//200716
					delay_ms(2750);
				}
			}
		}
		else if(modex == MODE_MEM_upload){
			for (i = 0; i < memCount+1; i++) {
				if (read_result(searchSEQ[i], 1) == OK) {
					make_result();
					conv_result_store();
					if(SERIAL_FLAG<=1) send_result();
					else if (SERIAL_FLAG == 2) Send720();
					else SendYD(SERIAL_FLAG);
					delay_ms(500);
				}
			}
		}else if(modex & 0x002){
			flag_motor_run = 0;
			flag_self = 0;
			if(measFlag){
				temp_test_type = test_type = QC_MEMORY;
				for(i=0;i<MAX_USERID;i++)USER_ID[i] = buff_user_id[i] = NULL;
				mem_point = 0;
				temp_QC = 0;
			
			}else{
				temp_test_type = test_type = GENERAL;
			}
			temp_search_theme = search_theme = NUMBER_SEARCH;
			mem_point = 0;
			end_seqno = (SEQ_NO-1<SRT_SEQ_NO?SRT_SEQ_NO:SEQ_NO - 1);
			//end_seqno = END_SEQ_NO;
			srt_seqno = SRT_SEQ_NO;
			end_qcno = (QC_NO-1<SRT_QC_NO?SRT_QC_NO:QC_NO - 1);
			//end_qcno = END_QC_NO;
			srt_qcno = SRT_QC_NO;
			buff_oper_id[0] = NULL;
			buff_user_id[0] = NULL;
			buff_lot_no[0] = NULL;
			tempDateDelimiter = dateDelimiter;
			tempDatePresentation = datePresentation;
			tempTimeDelimiter = timeDelimiter;
			tempTimePresentation = timePresentation;
			tempyear = year;
			tempmonth = month;
			tempday = day;
			setyear = tempyear;
			setmonth = tempmonth;
			setday = tempday;
			dateMemFlag = 0;
			temp_QC=temp_QC2=0;
			modex = MODE_MEMORY;
			ReadMemory();
		}
		key_buzzer();
		dsp_memory();
		modex = MODE_MEMORY;
	}else if(tcode==3){							//cancel
		key_buzzer();
//		if(modex == MODE_MEAS_Error2){
		if(modex & 0x002){
			for(int i = 0; i<errorStackSize;i++){
				errorStack[i] = 0;
				temp_modex2[i] = 0;
			}
			/*
			if(SEQ_NO == END_SEQ_NO+1){
				modex = MODE_TEST;
				init_test_set();
				dsp_test();
			}else{
				modex = MODE_QC;
				init_qc_set();
				dsp_qc();
			}*/
			errorCnt = 0;
			noStripFlag = 0;
			flag_motor_run = 0;
			flag_self = 0;
			modex = MODE_MEAS;
			dsp_measure();
//		}
//		else if(modex == MODE_TEST_Error2){
//			for(int i = 0; i<errorStackSize;i++){
//				errorStack[i] = 0;
//				temp_modex2[i] = 0;
//			}
//			modex = MODE_TEST;
//			init_test_set();
//			dsp_test();
//		}
//		else if(modex == MODE_QC_Error2){
//			for(int i = 0; i<errorStackSize;i++){
//				errorStack[i] = 0;
//				temp_modex2[i] = 0;
//			}
//			modex = MODE_QC;
//			init_qc_set();
//			dsp_qc();
		}else{
			dsp_memory();
			modex = MODE_MEMORY;
		}
	}
}
/*****************************************************************************/
void tch_input_password(void){
	tcode=tch_check(tch_input_password_tb, tch_input_password_no);
	if(tcode==0){								//home
		key_buzzer();
		dsp_home();
		modex = MODE_HOME;
	}else if(tcode==1 || tcode==15){							//back, close
		key_buzzer();
		dsp_setting();
		modex = MODE_SET;
	}else if(tcode<=11){								//digit
		key_buzzer();
		if(tcode<11) tcode--;					//'1'~'9'
		else tcode = 0;							//'0'
		password[cursor] = tcode+'0';
		LCDbmpxx(bm_12040_w,468-126,66);
		out_line_32(471-116, 70, password, BLK_LCD);
		if (cursor < 3)cursor++;
		else cursor = 0;
		dsp_cursor_password(cursor);
	}else if(tcode==13){						//back
		key_buzzer();
		if(cursor) cursor--;
		LCDbmpxx(bm_12040_w,468-126,66);
		out_line_32(471-116, 70, password, BLK_LCD);
		dsp_cursor_password(cursor);
	}else if(tcode==14){						//save
		key_buzzer();
		if(password[0] == '1' && password[1] == '8' && password[2] == '8' && password[3] == '2'){
			cursor = 0;
			u8 i, j;
			for (i = 0; i < 20; i++)
			{
				for (j = 0; j < 10; j++)
				{
					TEMP_PARA_NEW[i][j] = PARA_NEW[i][j];
				}
			}
			dsp_change_PR();
			modex = MODE_change_PR;
		}else{
			tch_error();
			dsp_setting();
			modex = MODE_SET;
		}

	}
}
/*****************************************************************************/
void tch_maintanence_password(void){
	tcode=tch_check(tch_input_password_tb, tch_input_password_no);
	if(tcode==0){								//home
		key_buzzer();
		dsp_home();
		modex = MODE_HOME;
	}else if(tcode==1 || tcode==15){							//back, close
		key_buzzer();
		dsp_sysset();
		modex = MODE_system_set;
	}else if(tcode<=11){								//digit
		key_buzzer();
		if(tcode<11) tcode--;					//'1'~'9'
		else tcode = 0;							//'0'
		password[cursor] = tcode+'0';
		LCDbmpxx(bm_12040_w,468-126,66);
		out_line_32(471-116, 70, password, BLK_LCD);
		if (cursor < 3)cursor++;
		else cursor = 0;
		dsp_cursor_password(cursor);
	}else if(tcode==13){						//back
		key_buzzer();
		if(cursor) cursor--;
		LCDbmpxx(bm_12040_w,468-126,66);
		out_line_32(471-116, 70, password, BLK_LCD);
		dsp_cursor_password(cursor);
	}else if(tcode==14){						//save
		key_buzzer();
		if(password[0] == '2' && password[1] == '8' && password[2] == '8' && password[3] == '1'){
			cursor = 0;
			dsp_maintanence();
			modex = MODE_MAINTANENCE;
		}else{
			tch_error();
			dsp_sysset();
			modex = MODE_system_set;
		}

	}
}
/*****************************************************************************/
void tch_maintanence_Reset(void){
	tcode=tch_check(tch_syschk_fw_tb, tch_syschk_fw_no);
	if(tcode==0){								//home
		key_buzzer();
		dsp_home();
		modex = MODE_HOME;
	}else if(tcode==1){							//back
		key_buzzer();
		dsp_maintanence();
		modex = MODE_MAINTANENCE;
	}else if(tcode==2){							//confirm
		key_buzzer();
		PRINT_FLAG = 1;
		BAUD_FLAG = 0;
		SBAUD_FLAG = 0;
		SOUND_FLAG = 1;
		LANGUAGE_FLAG = 0;
		sampleIDFlag = 1;
		AUTO_FLAG = 0;
		write24w(PRINT_ADR, PRINT_FLAG);
		write24w(BAUD_ADR, BAUD_FLAG);
		write24w(SBAUD_ADR, SBAUD_FLAG);
		write24w(SOUND_ADR, SOUND_FLAG);
		write24w(LANGUAGE_ADR, LANGUAGE_FLAG);
		write24w(SAMPLE_ADR, sampleIDFlag);
		store_vars(AUTO_ADR, AUTO_FLAG);
		set_baud_rs232();
		set_baud_usb();

		RCC->CIR = 0x00000000;							// Disable all interrupts 
		jump_addr = *(__IO uint32_t*) (BOOT_ADDRESS + 4);
		jump_to_app = (fptr) jump_addr;
		__set_MSP(*(__IO uint32_t*) BOOT_ADDRESS);
		jump_to_app();
	}else if(tcode==3){							//exit
		key_buzzer();
		dsp_maintanence();
		modex = MODE_MAINTANENCE;
	}
}
/*****************************************************************************/
void tch_error_confirm(void){
	tcode=tch_check(tch_confirm_tb, tch_confirm_no);
	if(tcode<=2){							//confirm
		key_buzzer();
		if(errorStack[0] == Err_WasteOut){
			trayFlag = 0;
		}else if(errorStack[0] == Err_PlateOut){
			plateFlag = 0;
		}
		for(int i = 0; i<errorCnt-1;i++){
			errorStack[i] = errorStack[i+1];
			temp_modex2[i] = temp_modex2[i+1];
		}
		errorStack[errorCnt] = 0;
		temp_modex2[errorCnt] = 0;
		errorCnt--;
		if(temp_modex2[0] == MODE_TEST || temp_modex2[0] == MODE_QC){
			dsp_los_table();
			dsp_seq();
			if(measCnt>0){
				view_result_store(temp_seqno[measCnt-1]);
			}else{
				dsp_item_tb();
			}
		}else if(temp_modex2[0] == MODE_system_chk){
			dsp_system_check();
		}
		if(!errorCnt){
			modex = temp_modex2[0];
			if(modex < MODE_TEST){
				dsp_measure();
			}else if(modex>MODE_TEST && modex<MODE_QC){
				dsp_test_set();
			}else if(modex>MODE_QC && modex<=MODE_QC_lot){
				dsp_qc_set();
			}else if(modex == MODE_MAINTANENCE){
				dsp_maintanence();
			}
		}else{
			errorSwitch = 1;
		}
	}
}
/*****************************************************************************/
void tch_sysset_Restart(void){
	tcode=tch_check(tch_mem_printsend_tb, tch_mem_printsend_no);
	if(tcode==0){								//home
		key_buzzer();
	}else if(tcode==1){							//back
		key_buzzer();
		modex = MODE_system_set;
		dsp_sysset();
	}else if(tcode==2){							//confirm
		key_buzzer();

		write24w(PRINT_ADR, PRINT_FLAG);
		write24w(BAUD_ADR, BAUD_FLAG);
		write24w(SBAUD_ADR, SBAUD_FLAG);
		write24w(SOUND_ADR, SOUND_FLAG);
		write24w(LANGUAGE_ADR, LANGUAGE_FLAG);
		write24w(SAMPLE_ADR, sampleIDFlag);
		store_vars(AUTO_ADR, AUTO_FLAG);
		set_baud_rs232();
		set_baud_usb();

		RCC->CIR = 0x00000000;							// Disable all interrupts 
		jump_addr = *(__IO uint32_t*) (BOOT_ADDRESS + 4);
		jump_to_app = (fptr) jump_addr;
		__set_MSP(*(__IO uint32_t*) BOOT_ADDRESS);
		jump_to_app();
	}else if(tcode==3){							//cancel
		key_buzzer();
		modex = MODE_system_set;
		dsp_sysset();
	}
}
void tch_syschk_confirm(void){
	tcode=tch_check(tch_confirm_tb, tch_confirm_no);
	if(tcode<=2){							//confirm
		key_buzzer();
		modex = MODE_system_chk;
		dsp_system_check();
	}
}

void tch_maintanence_Confirm(void){
	tcode=tch_check(tch_confirm_tb, tch_confirm_no);
	if(tcode<=2){							//confirm
		key_buzzer();
		modex = MODE_MAINTANENCE;
		dsp_maintanence();
	}
}
