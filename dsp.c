#define  _DSP_C_

#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include "common\32f429_lcd.h"
#include "common\32f429_port.h"
#include "util\usart.h"
#include "util\lcd.h"
#include "util\eep.h"
#include "util\Para.h"
#include "resm.h"
#include "meas.h"
#include "touch.h"
#include "language.h"



void dsp_color_no(u16 cno, u16 xp);
void dsp_trubi_no(u16 tno, u16 xp);
void make_seqno2dgtstr(u16 seqno);
void dsp_base_bar(void);


void type_pr_name(void);

u8	backup_user_id[MAX_USERID+3];
u8	backup_oper_id[MAX_OPERID+3];
u8	backup_test_type;
#include "dsp.h"
u8	backup_dateMemFlag;
//static  char s_Conv[2][7] = { "Conv.1", "Conv.2" };
//static  char s_SI[2][6] = { "S.I.1", "S.I.2" };
/*****************************************************************************/
void dsp_home(void){
	LCDbmpxx(bm_topbar,0,0);
	clr_screen();
	LCDbmpxx(bm_memory,110,145);
	LCDbmpxx(bm_measure,320,145);
	LCDbmpxx(bm_setting,530,145);
	LCDbmpxx(bm_topbar,0,480-50);
	dsp_date();
	
	dsp_time();

}
/*****************************************************************************/
void dsp_date(void) {
	LCDbmpxx(bm_timebar, 6, 480 - 35);
	if (datePresentation)
	{
		sprintf(charbuff, "%2d%c%2d%c20%2d", day, dateDelimiter, month, dateDelimiter, year);
	}
	else
	{
		sprintf(charbuff, "20%2d%c%2d%c%2d", year, dateDelimiter, month, dateDelimiter, day);
	}
	fill_zero(charbuff);
	out_line_16(30, 480 - 32, charbuff, BLK_LCD);
}
/*****************************************************************************/
void dsp_time(void) {
	LCDbmpxx(bm_timebar, 160, 480 - 35);
	if (timePresentation == 1){
		sprintf(charbuff, "%2d%c%2d%c%2d", hour, timeDelimiter, min, timeDelimiter, sec);
		fill_zero(charbuff);
		out_line_16(190, 480 - 32, charbuff, BLK_LCD);
	}
	else{
		if (hour == 0) sprintf(charbuff, "AM%2d%c%2d%c%2d", hour+12, timeDelimiter, min, timeDelimiter, sec);
		else if (hour < 12)       sprintf(charbuff, "AM%2d%c%2d%c%2d", hour, timeDelimiter, min, timeDelimiter, sec);
		else if (hour == 12) sprintf(charbuff, "PM%2d%c%2d%c%2d", hour, timeDelimiter, min, timeDelimiter, sec);
		else if (hour > 12)sprintf(charbuff, "PM%2d%c%2d%c%2d", hour - 12, timeDelimiter, min, timeDelimiter, sec);
		fill_zero(charbuff);
		out_line_16(180, 480 - 32, charbuff, BLK_LCD);
	}

}

/*****************************************************************************/
void dsp_setting(void){
	dsp_base_bar();
	LCDbmpxx(bm_house,45,5);
	LCDbmpxx(bm_back,720,5);
	LCDbmpxx(bm_date_time,110,75);
	LCDbmpxx(bm_result_unit,320,75);
	LCDbmpxx(bm_system_set,530,75);
	LCDbmpxx(bm_operater_info,110,255);
	LCDbmpxx(bm_system_chk,320,255);
	LCDbmpxx(bm_change_pr,530,255);
	returnFlag = 0;
}
/*****************************************************************************/
void dsp_datetime(void) {
	clr_screen();
	LCDbmpxx(bm_date_time2, 40, 58);
	LCDbmpxx(bm_calinder_l, 65, 120);

	if (!tempDatePresentation){
		LCDbmpxx(bm_ymd1_w, 110, 166);
		LCDbmpxx(bm_ymd2_g, 224, 166);
		sprintf(charbuff, "20%2d%c%2d%c%2d", tempyear, tempDateDelimiter, tempmonth, tempDateDelimiter, tempday);
		fill_zero(charbuff);
		out_line_16(158, 132, charbuff, BLK_LCD);

	}
	else{
		LCDbmpxx(bm_ymd1_g, 110, 166);
		LCDbmpxx(bm_ymd2_w, 224, 166);
		sprintf(charbuff, "%2d%c%2d%c20%2d", tempday, tempDateDelimiter, tempmonth, tempDateDelimiter, tempyear);
		fill_zero(charbuff);
		out_line_16(158, 132, charbuff, BLK_LCD);
	}
	if (tempDateDelimiter == '/'){
		LCDbmpxx(bm_slush_w, 110, 212);
		LCDbmpxx(bm_hyphen_g, 224, 212);
		LCDbmpxx(bm_comma_g, 338, 212);

	}
	else if (tempDateDelimiter == '-'){
		LCDbmpxx(bm_slush_g, 110, 212);
		LCDbmpxx(bm_hyphen_w, 224, 212);
		LCDbmpxx(bm_comma_g, 338, 212);
	}
	else if (tempDateDelimiter == ','){
		LCDbmpxx(bm_slush_g, 110, 212);
		LCDbmpxx(bm_hyphen_g, 224, 212);
		LCDbmpxx(bm_comma_w, 338, 212);
	}
	LCDbmpxx(bm_clock_l, 65, 120 + 160);
	if (tempTimePresentation==0){
		if (temphour == 0)sprintf(charbuff, "AM%2d%c%2d%c%2d", temphour + 12, tempTimeDelimiter, tempmin, tempTimeDelimiter, tempsec);
		else if (temphour < 12)       sprintf(charbuff, "AM%2d%c%2d%c%2d", temphour, tempTimeDelimiter, tempmin, tempTimeDelimiter, tempsec);
		else if (temphour == 12) sprintf(charbuff, "PM%2d%c%2d%c%2d", temphour, tempTimeDelimiter, tempmin, tempTimeDelimiter, tempsec);
		else sprintf(charbuff, "PM%2d%c%2d%c%2d", temphour - 12, tempTimeDelimiter, tempmin, tempTimeDelimiter, tempsec);
		fill_zero(charbuff);
		out_line_16(150, 132 + 160, charbuff, BLK_LCD);
	}
	else{
		sprintf(charbuff, "%2d%c%2d%c%2d", temphour, tempTimeDelimiter, tempmin, tempTimeDelimiter, tempsec);
		fill_zero(charbuff);
		out_line_16(160, 132 + 160, charbuff, BLK_LCD);
	}

	if (!tempTimePresentation){
		LCDbmpxx(bm_12h_w, 110, 166 + 160);
		LCDbmpxx(bm_24h_g, 224, 166 + 160);
	}
	else{
		LCDbmpxx(bm_12h_g, 110, 166 + 160);
		LCDbmpxx(bm_24h_w, 224, 166 + 160);
	}
	if (tempTimeDelimiter == ':'){
		LCDbmpxx(bm_colon_w, 110, 212 + 160);
		LCDbmpxx(bm_hyphen_g, 224, 212 + 160);
		LCDbmpxx(bm_comma_g, 338, 212 + 160);
	}
	else if (tempTimeDelimiter == '-'){
		LCDbmpxx(bm_colon_g, 110, 212 + 160);
		LCDbmpxx(bm_hyphen_w, 224, 212 + 160);
		LCDbmpxx(bm_comma_g, 338, 212 + 160);
	}
	else if (tempTimeDelimiter == ','){
		LCDbmpxx(bm_colon_g, 110, 212 + 160);
		LCDbmpxx(bm_hyphen_g, 224, 212 + 160);
		LCDbmpxx(bm_comma_w, 338, 212 + 160);
	}

	LCDbmpxx(bm_save, 690, 438);
}
/*****************************************************************************/
void dsp_time_set(void){
	LCDbmpxx(bm_timenum,400,58);
	//LCDbmpxx(bm_clock_l,448,66);
	LCDbmpxx(bm_clock_s,448,66);
	LCDbmpxx(bm_12040_w,488,66);
	if (tempTimePresentation == 0) {
		if(sethour == 0 && timeFlag == 1){
			sprintf(buff_time, "PM12%c%2d%c%2d", tempTimeDelimiter, setmin, tempTimeDelimiter, setsec);
		}else if (sethour == 0) {
			sprintf(buff_time, "AM%2d%c%2d%c%2d", sethour+12, tempTimeDelimiter, setmin, tempTimeDelimiter, setsec);
		}else if (sethour < 12 && timeFlag == 0) {
			sprintf(buff_time, "AM%2d%c%2d%c%2d", sethour, tempTimeDelimiter, setmin, tempTimeDelimiter, setsec);
		}else if (sethour == 12) {
			sprintf(buff_time, "PM%2d%c%2d%c%2d", sethour, tempTimeDelimiter, setmin, tempTimeDelimiter, setsec);
		}else {
			sprintf(buff_time, "PM%2d%c%2d%c%2d", sethour /*- 12*/, tempTimeDelimiter, setmin, tempTimeDelimiter, setsec);
		}
		fill_zero(buff_time);
		out_line_16(490, 78, buff_time, BLK_LCD);
		dsp_cursor_mem_time(2);
	}
	else
	{
		sprintf(buff_time, "%2d%c%2d%c%2d", sethour, tempTimeDelimiter, setmin, tempTimeDelimiter, setsec);
		fill_zero(buff_time);
		out_line_16(505, 78, buff_time, BLK_LCD);
		dsp_cursor_mem_time(2);
	}
}
void dsp_date_set(void){
	LCDbmpxx(bm_digit_box,400,58);
	//LCDbmpxx(bm_calinder_l,448,66);
	LCDbmpxx(bm_calinder_s,445,66);
	LCDbmpxx(bm_12040_w,485,66);
	if (tempDatePresentation == 0) {
		sprintf(charbuff, "20%2d%c%2d%c%2d", setyear, tempDateDelimiter, setmonth, tempDateDelimiter, setday);
		fill_zero(charbuff);
		out_line_16(491, 78, charbuff, BLK_LCD);
		dsp_cursor_mem_date(2);
	}
	else {
		sprintf(charbuff, "%2d%c%2d%c20%2d", setday, tempDateDelimiter, setmonth, tempDateDelimiter, setyear);
		fill_zero(charbuff);
		out_line_16(491, 78, charbuff, BLK_LCD);
		dsp_cursor_mem_date(0);
	}
}
/*****************************************************************************/
void dsp_base_bar(void){
	LCDbmpxx(bm_topbar,0,0);
	LCDbmpxx(bm_house,45,5);
	LCDbmpxx(bm_back,720,5);
	clr_screen();
	LCDbmpxx(bm_topbar,0,480-50);
	dsp_date();
	dsp_time();
}
/*****************************************************************************/

void dsp_system_check(void){
	char serialID[24];
	dsp_base_bar();
	LCDbmpxx(bm_system_chk2,45,60);
	LCDbmpxx(bm_bm67,200,140);
	LCDbmpxx(bm_start,690,438);
	sprintf(serialID, "S/N : %s", SYS_ID);
	out_line_16(10, 480-50-16-16, serialID, BLK_LCD);
	out_line_16(10, 480-50-16, Version, BLK_LCD);
	if(checkFlag){
		if(!plateCheck){
			out_line_16(RSCX_OFFSET, 146+(27*1), "OK",	BLU_LCD);	
		}else{
			LCDbmpxx(bm_syschk_button, RSCX_OFFSET-YOFF_SYS_CHK, 140+(27*1));
			out_line_16(RSCX_OFFSET-FAIL_OFFSET, 146+(27*1), "FAIL",	RED_LCD);
		}
		if(!trayCheck){
			out_line_16(RSCX_OFFSET, 146+(27*2), "OK",	BLU_LCD);
		}else{
			LCDbmpxx(bm_syschk_button, RSCX_OFFSET-YOFF_SYS_CHK, 140+(27*2));
			out_line_16(RSCX_OFFSET-FAIL_OFFSET, 146+(27*2), "FAIL",	RED_LCD);
		}
		if (!tableCheck){
			out_line_16(RSCX_OFFSET, 146 + (27 * 3), "OK", BLU_LCD);		//267
		}else{
			LCDbmpxx(bm_syschk_button, RSCX_OFFSET-YOFF_SYS_CHK, 140+(27*3));
			out_line_16(RSCX_OFFSET-FAIL_OFFSET, 146+(27*3), "FAIL",	RED_LCD);
		}
		if (!pushbarCheck){
			out_line_16(RSCX_OFFSET, 146 + (27 * 4), "OK", BLU_LCD);		//267
		}else{
			LCDbmpxx(bm_syschk_button, RSCX_OFFSET-YOFF_SYS_CHK, 140+(27*4));
			out_line_16(RSCX_OFFSET-FAIL_OFFSET, 146+(27*4), "FAIL",	RED_LCD);
		}
		if (!sensorCheck){
			out_line_16(RSCX_OFFSET, 146 + (27 * 5), "OK", BLU_LCD);		//1982
		}else{
			LCDbmpxx(bm_syschk_button, RSCX_OFFSET-YOFF_SYS_CHK, 140+(27*5));
			out_line_16(RSCX_OFFSET-FAIL_OFFSET, 146+(27*5), "FAIL",	RED_LCD);
		}
	
		if(!rCheck){
			out_line_16(RSCX_OFFSET, 146+(27*6), "OK",	BLU_LCD);		
		}else{
			LCDbmpxx(bm_syschk_button, RSCX_OFFSET-YOFF_SYS_CHK, 140+(27*6));
			out_line_16(RSCX_OFFSET-FAIL_OFFSET, 146+(27*6), "FAIL",	RED_LCD);
		}
		if(!gCheck){
			out_line_16(RSCX_OFFSET, 146+(27*7), "OK",	BLU_LCD);		
		}else{
			LCDbmpxx(bm_syschk_button, RSCX_OFFSET-YOFF_SYS_CHK, 140+(27*7));
			out_line_16(RSCX_OFFSET-FAIL_OFFSET, 146+(27*7), "FAIL",	RED_LCD);
		}
		if(!bCheck){
			out_line_16(RSCX_OFFSET, 146+(27*8), "OK",	BLU_LCD);		
		}else{
			LCDbmpxx(bm_syschk_button, RSCX_OFFSET-YOFF_SYS_CHK, 140+(27*8));
			out_line_16(RSCX_OFFSET-FAIL_OFFSET, 146+(27*8), "FAIL",	RED_LCD);
		}
	}
}
/*****************************************************************************/
/*****************************************************************************/
void dsp_serial_data(void){
	u16 i, j, x[3] = {130, 320, 510}, y[3] = {145, 215, 285};
	dsp_base_bar();
	LCDbmpxx(bm_serial_title, 20, 58);
	for(i=0;i<3;i++){
		for(j=0;j<3;j++){
			if(temp_SERIAL_FLAG == i*3+j){
				LCDbmpxx(tb_serial_w[i*3+j], x[j], y[i]);
			}
			else{
				LCDbmpxx(tb_serial_g[i*3+j], x[j], y[i]);
			}
		}
	}
	LCDbmpxx(bm_save, 690, 438);
}
/******************************************************************************/
void dsp_serial_data_change(u16 x, u16 y, u8 type){
	u16 x2[3] = {130, 320, 510}, y2[3] = {145, 215, 285};
	LCDbmpxx(tb_serial_g[temp_SERIAL_FLAG], x2[x], y2[y]);
	LCDbmpxx(tb_serial_w[type], x2[type%3], y2[type/3]);
}
/*****************************************************************************/
void dsp_fw_update(void){

	switch(modex){
		case MODE_MAINTANENCE_fw:
			dsp_base_bar();
			out_line_48(220, 135, "Are you sure?", RED_LCD);
			draw_box(100,220,350,320, CYN_LCD);
			draw_box(110,230,340,310, BLU_LCD);
			out_line_32(195, 255, "Yes", WHT_LCD);
			draw_box(450,220,700,320, CYN_LCD);	
			draw_box(460,230,690,310, BLU_LCD);	
			out_line_32(540, 255, "Exit", WHT_LCD);
		break;
		case MODE_MEM_delete:
			LCDbmpxx(bm_popup13_top, 250, 185);
 			LCDbmpxx(bm_popup11_mid, 250, 185+50);
 			LCDbmpxx(bm_popup3_bottom, 250, 185+50+70);
		break;
		case MODE_system_set_Restart:
			LCDbmpxx(bm_popup16_top, 250, 185);
 			LCDbmpxx(bm_popup14_mid, 250, 185+50);
 			LCDbmpxx(bm_popup3_bottom, 250, 185+50+70);
		break;
		case MODE_MAINTANENCE_Reset:
			LCDbmpxx(bm_popup16_top, 250, 185);
 			LCDbmpxx(bm_popup15_mid, 250, 185+50);
 			LCDbmpxx(bm_popup3_bottom, 250, 185+50+70);
		break;
	}
}
/*****************************************************************************/
void dsp_image_update(void){
	dsp_base_bar();

}
/*****************************************************************************/
void dsp_font_update(void){
	dsp_base_bar();

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
void dsp_test_type_set(void){
	if (temp_test_type == GENERAL) {
		LCDbmpxx(bm_general_w, 540, 96);
		LCDbmpxx(bm_qc_g, 540, 126);
	}
	else {
		LCDbmpxx(bm_general_g, 540, 96);
		LCDbmpxx(bm_qc_w, 540, 126);
	}
}
/*****************************************************************************/
void dsp_test_type(void){

	if (test_type == GENERAL)
	{
		LCDbmpxx(bm_general_w, 660, 116);
	}
	else
	{
		LCDbmpxx(bm_qc_w, 660, 116);
	}
}
/*****************************************************************************/
void dsp_memory(void){
	clr_screen();
	LCDbmpxx(bm_topbar,0,480-50);
	LCDbmpxx(bm_house, 45, 5);
	LCDbmpxx(bm_back, 720, 5);
	LCDbmpxx(bm_memory2, 20, 58);
////////////////////////////////////////////////////////////////////////////////Test Date
	LCDbmpxx(bm_testdate, 20, 116);
	LCDbmpxx(bm_200_w, 140, 116);
	if(dateMemFlag){
		if (datePresentation == 0) {
			sprintf(charbuff, "20%2d%c%2d%c%2d", tempyear, dateDelimiter, tempmonth, dateDelimiter, tempday);
			fill_zero(charbuff);
			out_line_16(150, 120, charbuff, BLK_LCD);
		}
		else {
			sprintf(charbuff, "%2d%c%2d%c20%2d", tempday, dateDelimiter, tempmonth, dateDelimiter, tempyear);
			fill_zero(charbuff);
			out_line_16(150, 120, charbuff, BLK_LCD);
		}
	}
////////////////////////////////////////////////////////////////////////////////SEQ
		LCDbmpxx(bm_seq, 380, 116);
		LCDbmpxx(bm_200_w, 500, 116);

	if (test_type == GENERAL){
		sprintf(buff_seqno, "%4d-%4d", srt_seqno, end_seqno);
	}
	else {
		sprintf(buff_seqno, "%4d-%4d", srt_qcno, end_qcno);
	}
	fill_zero(buff_seqno);
	out_line_16(510, 120, buff_seqno, BLK_LCD);
////////////////////////////////////////////////////////////////////////////////Test Type
	dsp_test_type();
////////////////////////////////////////////////////////////////////////////////Sample ID or QC Type
	if (test_type == QC_MEMORY){//QCȭ�� ǥ��
		if(temp_QC == 1){
			LCDbmpxx(bm_all_g, 380, 148);
			LCDbmpxx(bm_neg_w, 514, 148);
			LCDbmpxx(bm_pos_g, 648, 148);
		}else if(temp_QC == 2){
			LCDbmpxx(bm_all_g, 380, 148);
			LCDbmpxx(bm_neg_g, 514, 148);
			LCDbmpxx(bm_pos_w, 648, 148);
		}else{
			LCDbmpxx(bm_all_w, 380, 148);
			LCDbmpxx(bm_neg_g, 514, 148);
			LCDbmpxx(bm_pos_g, 648, 148);
		}
	}
	else{//�Ϲ�ȭ�� ǥ��
		LCDbmpxx(bm_samid, 380, 148);
		LCDbmpxx(bm_280_w, 500, 148);
		out_line_16(510, 152, buff_user_id, BLK_LCD);
	}
///////////////////////////////////////////////////////////////////////////////////////////////////////Operator ID
	LCDbmpxx(bm_opid, 20, 148);
	LCDbmpxx(bm_200_w, 140, 148);
	out_line_16(150, 152, buff_oper_id, BLK_LCD);

///////////////////////////////////////////////////////////////////////////////////////////////////////Search
	dsp_mem_line8();
	LCDbmpxx(bm_print2, 400, 438);
	LCDbmpxx(bm_upload, 550, 438);
	LCDbmpxx(bm_delete, 690, 438);
	dsp_date();
	dsp_time();

}
/*****************************************************************************/
void make_seqno_dgt(void){
	u16 temp_sseq, temp_eseq;
	if (test_type == GENERAL) {
		srt_seqno = buff_seqno[0] - '0';
		srt_seqno = (srt_seqno * 10) + (buff_seqno[1] - '0');
		srt_seqno = (srt_seqno * 10) + (buff_seqno[2] - '0');
		srt_seqno = (srt_seqno * 10) + (buff_seqno[3] - '0');
		if (srt_seqno > MAX_RECORD) srt_seqno = MAX_RECORD;
		else if (!srt_seqno) srt_seqno = 1;
		end_seqno = buff_seqno[5] - '0';
		end_seqno = (end_seqno * 10) + (buff_seqno[6] - '0');
		end_seqno = (end_seqno * 10) + (buff_seqno[7] - '0');
		end_seqno = (end_seqno * 10) + (buff_seqno[8] - '0');
		if (end_seqno > MAX_RECORD) end_seqno = MAX_RECORD;
		else if (!end_seqno) end_seqno = 1;
		if (srt_seqno > end_seqno) {
			temp_sseq = end_seqno;
			temp_eseq = srt_seqno;
			srt_seqno = temp_sseq;
			end_seqno = temp_eseq;
		}
	}
	else {
		srt_qcno = buff_seqno[0] - '0';
		srt_qcno = (srt_qcno * 10) + (buff_seqno[1] - '0');
		srt_qcno = (srt_qcno * 10) + (buff_seqno[2] - '0');
		srt_qcno = (srt_qcno * 10) + (buff_seqno[3] - '0');
		if (srt_qcno > END_QC_NO) srt_qcno = END_QC_NO;
		else if (srt_qcno < SRT_QC_NO) srt_qcno = SRT_QC_NO;
		end_qcno = buff_seqno[5] - '0';
		end_qcno = (end_qcno * 10) + (buff_seqno[6] - '0');
		end_qcno = (end_qcno * 10) + (buff_seqno[7] - '0');
		end_qcno = (end_qcno * 10) + (buff_seqno[8] - '0');
		if (end_qcno > END_QC_NO) end_qcno = END_QC_NO;
		else if (end_qcno < SRT_QC_NO) end_qcno = SRT_QC_NO;
		if (srt_qcno > end_qcno) {
			temp_sseq = end_qcno;
			temp_eseq = srt_qcno;
			srt_qcno = temp_sseq;
			end_qcno = temp_eseq;
		}
	}
}
/*****************************************************************************/
void dsp_mem_line8(void){
	LCDbmpxx(bm_bm79, 4, 180);
	if (test_type == GENERAL){
		LCDbmpxx(bm_memory_list_title_general, 4, 180);
	}
	else{
		LCDbmpxx(bm_memory_list_title_qc, 4, 180);
	}
	if (search_theme == NUMBER_SEARCH){
		dsp_mem_number8();
	}
	else if (search_theme == SAMPLE_SEARCH) {
		dsp_mem_sample8();
	}
	else {
		dsp_mem_oper8();
	}
}
/*****************************************************************************/
void dsp_mem_number8(void) {
	int i, j, k;
	char viewID[14];
	for (i = 0; i < memoryLine; i++) {
		if (chk_index(searchSEQ[mem_point + i]) == OK) {
			out_line_16(5, 213 + (i * 27), MEASoperid, BLK_LCD);
			if (test_type == GENERAL) {
				k=0;
				for(j=0;j<18;j++){
					if(MEASuserid[j] != ' ' && MEASuserid[j] != NULL)k++;
				}
				if(k>16){
					for(j=0;j<5;j++){
						viewID[j] = MEASuserid[j];
					}
					viewID[5] = '.';
					viewID[6] = '.';
					viewID[7] = '.';
					for(j=k;j>k-5;j--){
						viewID[12 + (j - k)] = MEASuserid[j-1];
					}
					viewID[13] = NULL;				
					out_line_16(193, 213 + (i * 27), viewID, BLK_LCD);
				}else
				{
					out_line_16(193, 213 + (i * 27), MEASuserid, BLK_LCD);
				}
			}
			else {
				out_line_16(193, 213 + (i * 27), buff_lot_no, BLK_LCD);
			}
			make_seqno2dgtstr(searchSEQ[mem_point + i]);
			out_line_16(412, 213 + (i * 27), charbuff, BLK_LCD);
			if (datePresentation) {
				sprintf(charbuff, "%2d%c%2d%c20%2d", MEASday, dateDelimiter, MEASmonth, dateDelimiter, MEASyear);
			}
			else {
				sprintf(charbuff, "20%2d%c%2d%c%2d", MEASyear, dateDelimiter, MEASmonth, dateDelimiter, MEASday);
			}
			fill_zero(charbuff);
			out_line_16(478, 213 + (i * 27), charbuff, BLK_LCD);
			out_line_16(623, 213 + (i * 27), MEASstripname, BLK_LCD);
		}
	}
}
/*****************************************************************************/
void dsp_mem_oper8(void) {
	int i, j, k;
	char viewID[14];
	for(i=0; i< memoryLine; i++){
		if (chk_index(searchSEQ[mem_point + i]) == OK) {
			if (cmp_operid() == OK) {
				out_line_16(5, 213 + (i * 27), MEASoperid, BLK_LCD);
				if (test_type == GENERAL) {
					k=0;
					for(j=0;j<18;j++){
						if(MEASuserid[j] != ' ' && MEASuserid[j] != NULL)k++;
					}
					if(k>16){
						for(j=0;j<5;j++){
							viewID[j] = MEASuserid[j];
						}
						viewID[5] = '.';
						viewID[6] = '.';
						viewID[7] = '.';
						for(j=k;j>k-5;j--){
							viewID[12 + (j - k)] = MEASuserid[j-1];
						}
						viewID[13] = NULL;				
						out_line_16(193, 213 + (i * 27), viewID, BLK_LCD);
					}else{
						out_line_16(193, 213 + (i * 27), MEASuserid, BLK_LCD);
					}
				}
				else {
					out_line_16(193, 213 + (i * 27), buff_lot_no, BLK_LCD);
				}
				make_seqno2dgtstr(searchSEQ[mem_point + i]);
				out_line_16(412, 213 + (i * 27), charbuff, BLK_LCD);
				if (datePresentation) {
					sprintf(charbuff, "%2d%c%2d%c20%2d", MEASday, dateDelimiter, MEASmonth, dateDelimiter, MEASyear);
				}
				else {
					sprintf(charbuff, "20%2d%c%2d%c%2d", MEASyear, dateDelimiter, MEASmonth, dateDelimiter, MEASday);
				}
				fill_zero(charbuff);
				out_line_16(478, 213 + (i * 27), charbuff, BLK_LCD);
				out_line_16(623, 213 + (i * 27), MEASstripname, BLK_LCD);
			}
		}
	}
}
/*****************************************************************************/
void dsp_mem_sample8(void) {			//user
	int i, j, k;
	char viewID[14];
	for (i = 0; i < memoryLine; i++) {
		if (chk_index(searchSEQ[mem_point + i]) == OK) {
			if (cmp_userid() == OK) {
				out_line_16(5, 213 + (i * 27), MEASoperid, BLK_LCD);
				if (test_type == GENERAL) {
					k=0;
					for(j=0;j<18;j++){
						if(MEASuserid[j] != ' ' && MEASuserid[j] != NULL)k++;
					}
					if(k>16){
						for(j=0;j<5;j++){
							viewID[j] = MEASuserid[j];
						}
						viewID[5] = '.';
						viewID[6] = '.';
						viewID[7] = '.';
						for(j=k;j>k-5;j--){
							viewID[12 + (j - k)] = MEASuserid[j-1];
						}
						viewID[13] = NULL;				
						out_line_16(193, 213 + (i * 27), viewID, BLK_LCD);
					}else{
						out_line_16(193, 213 + (i * 27), MEASuserid, BLK_LCD);
					}
				}
				else {
					out_line_16(193, 213 + (i * 27), buff_lot_no, BLK_LCD);
				}
				make_seqno2dgtstr(searchSEQ[mem_point + i]);
				out_line_16(412, 213 + (i * 27), charbuff, BLK_LCD);
				if (datePresentation) {
					sprintf(charbuff, "%2d%c%2d%c20%2d", MEASday, dateDelimiter, MEASmonth, dateDelimiter, MEASyear);
				}
				else {
					sprintf(charbuff, "20%2d%c%2d%c%2d", MEASyear, dateDelimiter, MEASmonth, dateDelimiter, MEASday);
				}
				fill_zero(charbuff);
				out_line_16(478, 213 + (i * 27), charbuff, BLK_LCD);
				out_line_16(623, 213 + (i * 27), MEASstripname, BLK_LCD);
			}
		}
	}
}
/*****************************************************************************/
short cmp_testdate(void) {
	u8 a, b, c;
	char testdate[7], searchdate[7];
	sprintf(testdate, "%2d%2d%2d", MEASyear, MEASmonth, MEASday);
	sprintf(searchdate, "%2d%2d%2d", tempyear, tempmonth, tempday);

	for (a = 0; a < 6; a++) {
		b = testdate[a];
		c = searchdate[a];
		if (b != c) return(NG);
	}
	//printf3("%s %s\n", testdate, searchdate);
	return(OK);
}
/*****************************************************************************/
short cmp_operid(void) {
	u8 a, b, c;
	for (a = 0; a < MAX_OPERID; a++) {
		b = buff_oper_id[a];
		if ((b < ' ') || (b > '~')) return(OK);
		c = MEASoperid[a];
		if ((c >= 'a') && (c <= 'z')) c = (c - 'a') + 'A';
		if (b != c) return(NG);
	}
	return(OK);
}
/*****************************************************************************/
short cmp_userid(void) {
	u8 a, b, c;
	u16 d;
	if(test_type == QC_MEMORY)d = 8;
	else d = MAX_USERID;
	for (a = 0; a < d; a++) {
		if(test_type == QC_MEMORY){
			b = buff_user_id[a+8];
			if ((b < ' ') || (b > '~')) return(OK);
			c = MEASuserid[a+8];
		}else{
			b = buff_user_id[a];
			if ((b < ' ') || (b > '~')) return(OK);
			c = MEASuserid[a];
		}
		if ((c >= 'a') && (c <= 'z')) c = (c - 'a') + 'A';
		if (b != c) return(NG);
	}
	return(OK);
}
/*****************************************************************************/
void backup_mem_set(void){
u16 i;
	for(i=0;i<MAX_USERID+1;i++)	backup_user_id[i] = buff_user_id[i];
	for(i=0;i<MAX_OPERID+1;i++)	backup_oper_id[i] = buff_oper_id[i];
	backup_test_type = temp_test_type;
	backup_dateMemFlag = dateMemFlag;
	temp_QC2 = temp_QC;
}
void cancel_mem_set(void){
u16 i;	
	for(i=0;i<MAX_USERID+1;i++)	buff_user_id[i] = backup_user_id[i];
	for(i=0;i<MAX_OPERID+1;i++)	buff_oper_id[i] = backup_oper_id[i];
	setyear = tempyear;
	setmonth = tempmonth;
	setday = tempday;
	temp_test_type = backup_test_type;
	dateMemFlag = backup_dateMemFlag;
	temp_QC2 = temp_QC;
}
void enter_mem_set(void){
u16 i;	
	for(i=0;i<MAX_USERID;i++) USER_ID[i] = buff_user_id[i];
	test_type = temp_test_type;
	make_seqno_dgt();											//buff_seq_str ==> srt,end_seqno
	tempyear = setyear;
	tempmonth = setmonth;
	tempday = setday;
	mem_point = 0;
	temp_QC = temp_QC2;

	if (test_type == GENERAL){
		if(modex == MODE_MEM_sid){
			search_theme = SAMPLE_SEARCH;
		}else{
			search_theme = NUMBER_SEARCH;
		}
	}else{		
		if(temp_QC == 1) {
			search_theme = SAMPLE_SEARCH;
			USER_ID[8] = buff_user_id[8] = 'N';
			USER_ID[9] = buff_user_id[9] = 'E';
			USER_ID[10] = buff_user_id[10] = 'G';
			USER_ID[11] = buff_user_id[11] = 'A';
			USER_ID[12] = buff_user_id[12] = 'T';
			USER_ID[13] = buff_user_id[13] = 'I';
			USER_ID[14] = buff_user_id[14] = 'V';
			USER_ID[15] = buff_user_id[15] = 'E';
		}
		else if(temp_QC == 2){
			search_theme = SAMPLE_SEARCH;
			USER_ID[8] = buff_user_id[8] = 'P';
			USER_ID[9] = buff_user_id[9] = 'O';
			USER_ID[10] = buff_user_id[10] = 'S';
			USER_ID[11] = buff_user_id[11] = 'I';
			USER_ID[12] = buff_user_id[12] = 'T';
			USER_ID[13] = buff_user_id[13] = 'I';
			USER_ID[14] = buff_user_id[14] = 'V';
			USER_ID[15] = buff_user_id[15] = 'E';
		}else{
			search_theme = NUMBER_SEARCH;
			for(u8 i=0;i<MAX_USERID;i++)buff_user_id[i]=NULL;
		}
	}

}
/*****************************************************************************/
void dsp_mem_set(void){
	printf0("\r dsp memory set");
	LCDbmpxx(bm_keyboard,80,58);
	cursor_set = 0;
	dsp_mem_date_set();
	dsp_test_type_set();
	dsp_mem_set_seqno();	
	dsp_mem_set_oid();
	dsp_mem_set_sid();
}
/*****************************************************************************/
void dsp_mem_set_sid(void){
	if (modex == MODE_MEM_sid) {
		if (temp_test_type == GENERAL) {
			LCDbmpxx(bm_samid, 90, 156);
			LCDbmpxx(bm_360_w, 90+120, 156);
			
			out_line_16(220, 160, buff_user_id, BLK_LCD);
		}
		else {
			if (temp_QC2 == 0) {
				LCDbmpxx(bm_all_w, 90, 156);//80+10, 134+14
				LCDbmpxx(bm_neg_g, 224, 156);
				LCDbmpxx(bm_pos_g, 358, 156);
			}
			else if (temp_QC2 == 1) {
				LCDbmpxx(bm_all_g, 90, 156);
				LCDbmpxx(bm_neg_w, 224, 156);
				LCDbmpxx(bm_pos_g, 358, 156);
			}
			else if (temp_QC2 == 2) {
				LCDbmpxx(bm_all_g, 90, 156);
				LCDbmpxx(bm_neg_g, 224, 156);
				LCDbmpxx(bm_pos_w, 358, 156);
			}
		}
	}else {
		if (temp_test_type == GENERAL) {
			LCDbmpxx(bm_samid, 90, 156);
			LCDbmpxx(bm_360_g, 90+120, 156);
			out_line_16(220, 160, buff_user_id, BLK_LCD);
		}
		else {
			if (temp_QC2 == 0) {
				LCDbmpxx(bm_all_w, 90, 156);//80+10, 134+14
				LCDbmpxx(bm_neg_g, 224, 156);
				LCDbmpxx(bm_pos_g, 358, 156);
			}
			else if (temp_QC2 == 1) {
				LCDbmpxx(bm_all_g, 90, 156);
				LCDbmpxx(bm_neg_w, 224, 156);
				LCDbmpxx(bm_pos_g, 358, 156);
			}
			else if (temp_QC2 == 2) {
				LCDbmpxx(bm_all_g, 90, 156);
				LCDbmpxx(bm_neg_g, 224, 156);
				LCDbmpxx(bm_pos_w, 358, 156);
			}
		}
	}
}
void dsp_mem_set_seqno(void){
	LCDbmpxx(bm_seq,90, 96);
	if(modex == MODE_MEM_seq){
		LCDbmpxx(bm_200_w,210, 96);
	}
	else{
		LCDbmpxx(bm_200_g,210, 96);
	}
	out_line_16(220, 100, buff_seqno, BLK_LCD);
}
void dsp_mem_set_oid(void){
	if(modex==MODE_MEM_oid){
		 LCDbmpxx(bm_opid,90, 126);
		 LCDbmpxx(bm_200_w,210, 126);
	}else{
		LCDbmpxx(bm_opid,90, 126);
		LCDbmpxx(bm_200_g,210, 126);
	}
	out_line_16(220, 130, buff_oper_id, BLK_LCD);
}

/*****************************************************************************/
void dsp_measure(void){
	clr_screen();
	//	LCDbmpxx(bm_topbar,0,0);
	dsp_base_bar();
	LCDbmpxx(bm_picture5,30,115);
	out_line_24(102, 128, OPER_ID, BLK_LCD);
	out_line_24(102, 179, strip_name, BLK_LCD);
	if(UNIT_FLAG==UNIT_CONV)        out_line_24(102, 230, s_Conv[LANGUAGE_FLAG], BLK_LCD);
	else if(UNIT_FLAG==UNIT_SI) out_line_24(102, 230, s_SI[LANGUAGE_FLAG], BLK_LCD);
	if(PLUS_FLAG) out_line_24(102, 281, "On", BLK_LCD);
	else	      out_line_24(102, 281, "Off", BLK_LCD);
	if(MARK_FLAG) out_line_24(102, 332, "On", BLK_LCD);	
	else              out_line_24(102, 332, "Off", BLK_LCD);	
	LCDbmpxx(bm_test,390,145);
	LCDbmpxx(bm_QC,600,145);
	returnFlag = 0 ;
	fullFlag = 0;
	errorSwitch = 0;
	for(int i = 0;i<errorStackSize;i++)errorStack[i] = 0;
	for(int i = 0;i<errorStackSize;i++)temp_modex2[i] = 0;
	
}
/*****************************************************************************/
void dsp_oper_set(void){
	dsp_base_bar();
	LCDbmpxx(bm_operater_info2,40,58);
	LCDbmpxx(bm_opid,60,125);
	LCDbmpxx(bm_200_w,60+120,125);
	out_line_16(182, 130, OPER_ID, BLK_LCD);		//	dsp_oper_name();
	dsp_oper_info();
	LCDbmpxx(bm_newid,390,438);
	LCDbmpxx(bm_loigin,540,438);
	LCDbmpxx(bm_delete,690,438);	
}
/*****************************************************************************/
void dsp_oper_info(void){
u16 a, b, c = 0;//a= ��, b= ID ĭ��, c=���� �ڸ�
	LCDbmpxx(bm_bm64,10,170);
	for(a=0;a<8;a++){
		if (oper_cursor == a) {
			out_line_24(10, 200 + (27 * a), ">", BLU_LCD);
			for (b = 0; b < MAX_OPERID; b++) charbuff[b] = oper_buff[c++]; charbuff[b] = 0x00;
			out_line_24(26, 199 + (27 * a), charbuff, BLU_LCD);
			for (b = 0; b < 8; b++) charbuff[b] = oper_buff[c++]; charbuff[b] = 0x00;
			out_line_24(26 + 375, 199 + (27 * a), charbuff, BLU_LCD);
			for (b = 0; b < 10; b++) charbuff[b] = oper_buff[c++]; charbuff[b] = 0x00;
			out_line_24(26 + 375 + 155, 199 + (27 * a), charbuff, BLU_LCD);
		}
		else {
//			out_line_24(70, 200 + (27 * a), ">", BLU_LCD);
			for (b = 0; b < MAX_OPERID; b++) charbuff[b] = oper_buff[c++]; charbuff[b] = 0x00;
			out_line_24(26, 199 + (27 * a), charbuff, BLK_LCD);
			for (b = 0; b < 8; b++) charbuff[b] = oper_buff[c++]; charbuff[b] = 0x00;
			out_line_24(26 + 375, 199 + (27 * a), charbuff, BLK_LCD);
			for (b = 0; b < 10; b++) charbuff[b] = oper_buff[c++]; charbuff[b] = 0x00;
			out_line_24(26 + 375 + 155, 199 + (27 * a), charbuff, BLK_LCD);
		}
	}	
}
/*****************************************************************************/
void dsp_oper_new(void){
u16 i;
	printf0("\r dsp operator new");
	LCDbmpxx(bm_keyboard,80,58);
	LCDbmpxx(bm_opid,120,110);
	LCDbmpxx(bm_200_w,120+120,110);
//	out_line_16(245, 115, "1234567890", BLK_LCD);		
	for(i=0;i<MAX_OPERID;i++) oper_name[i]=' ';	oper_name[i]=0x00;	
//	dsp_cursor_oper(cursor_set);
}
/*****************************************************************************/
void dsp_oper_name(void){
	erase_box0(245, 115, 245+80+34, 115+16+2);
	out_line_16(245, 115, oper_name, BLK_LCD);		
//	dsp_cursor_oper(cursor_set);
}
/*****************************************************************************/
void dsp_strip_set(void){
u16 i, j=0;
	LCDbmpxx(bm_list_sample,292,60);
	erase_box0(340, 67, 340+140, 65+24);
	out_line_16(340, 67, "List of Strip", BLK_LCD);		
	
	for(i=cursor;i<MAX_STRIPNO;i++){ 
		get_strip_name(i);
		sprintf(charbuff, "%2d", i+1);
		if(i==STRIP_NO){
			out_line_16(305, 102+(j*27), charbuff, RED_LCD);		
			out_line_16(350, 102+(j*27), buff_strip_name, RED_LCD);
		}else{
			out_line_16(305, 102+(j*27), charbuff, BLK_LCD);		
			out_line_16(350, 102+(j*27), buff_strip_name, BLK_LCD);
		}		
		if((++j)>=12) break;
	}	
}
/*****************************************************************************/
void dsp_change_PR(void){
u16 i, j;
	dsp_base_bar();
	LCDbmpxx(bm_change_pr2,40,58);
	for(i=0;i<3;i++){
		for(j=0;j<5;j++){
			LCDbmpxx(bm_bm69,70+i*100,180+j*35);
		}
	}
	i =  70+((cursor%3)*100)-1;
	j = 180+((cursor/3)*35);
	LCDbmpxx(bm_ellipse_g, i, j);
	type_pr_name();
	LCDbmpxx(bm_bm72,420,132);

	for (i = 1; i < 10; i++)
	{

		for (j = 0; j < 8; j++)
		{
			charbuff[j] = (UNIT_FLAG==0? co_item_ram[cursor + 1][i][j] : si_item_ram[cursor + 1][i][j]);
		}
		charbuff[j] = NULL;
		out_line_24(437, 134 + (27 * (i)), charbuff, BLK_LCD);
	}

	for (i = 1; i < 10; i++) 
	{
		sprintf(charbuff, "%3d", TEMP_PARA_NEW[cursor][i]);
		out_line_24(467 + 150, 134 + (27 * (i)), charbuff, BLK_LCD);
	}

}
/*****************************************************************************/
#define TPR_YOFF	35

void type_pr_name(void){
//	char buff[3];
	int i;
	for (i = 0; i < 3; i++)charbuff[i] = co_item_ram[1][0][i];
	charbuff[i] = NULL;
	out_line_24(92, 186 + (TPR_YOFF * 0), charbuff, BLK_LCD);
	for (i = 0; i < 3; i++)charbuff[i] = co_item_ram[2][0][i];
	charbuff[i] = NULL; 
	out_line_24(192, 186 + (TPR_YOFF * 0), charbuff, BLK_LCD);
	for (i = 0; i < 3; i++)charbuff[i] = co_item_ram[3][0][i];
	charbuff[i] = NULL; 
	out_line_24(292, 186 + (TPR_YOFF * 0), charbuff, BLK_LCD);
	for (i = 0; i < 3; i++)charbuff[i] = co_item_ram[4][0][i];
	charbuff[i] = NULL; 
	out_line_24(92, 186 + (TPR_YOFF * 1), charbuff, BLK_LCD);
	for (i = 0; i < 3; i++)charbuff[i] = co_item_ram[5][0][i];
	charbuff[i] = NULL; 
	out_line_24(192, 186 + (TPR_YOFF * 1), charbuff, BLK_LCD);
	for (i = 0; i < 3; i++)charbuff[i] = co_item_ram[6][0][i];
	charbuff[i] = NULL; 
	out_line_24(292, 186 + (TPR_YOFF * 1), charbuff, BLK_LCD);
	for (i = 0; i < 3; i++)charbuff[i] = co_item_ram[7][0][i];
	charbuff[i] = NULL; 
	out_line_24(92, 186 + (TPR_YOFF * 2), charbuff, BLK_LCD);
	for (i = 0; i < 3; i++)charbuff[i] = co_item_ram[8][0][i];
	charbuff[i] = NULL; 
	out_line_24(192, 186 + (TPR_YOFF * 2), charbuff, BLK_LCD);
	for (i = 0; i < 3; i++)charbuff[i] = co_item_ram[9][0][i];
	charbuff[i] = NULL; 
	out_line_24(292, 186 + (TPR_YOFF * 2), charbuff, BLK_LCD);
	for (i = 0; i < 3; i++)charbuff[i] = co_item_ram[10][0][i];
	charbuff[i] = NULL; 
	out_line_24(92, 186 + (TPR_YOFF * 3), charbuff, BLK_LCD);
	for (i = 0; i < 3; i++)charbuff[i] = co_item_ram[11][0][i];
	charbuff[i] = NULL; 
	out_line_24(192, 186 + (TPR_YOFF * 3), charbuff, BLK_LCD);
	for (i = 0; i < 3; i++)charbuff[i] = co_item_ram[12][0][i];
	charbuff[i] = NULL; 
	out_line_24(292, 186 + (TPR_YOFF * 3), charbuff, BLK_LCD);
	for (i = 0; i < 3; i++)charbuff[i] = co_item_ram[13][0][i];
	charbuff[i] = NULL; 
	out_line_24(92, 186 + (TPR_YOFF * 4), charbuff, BLK_LCD);
	for (i = 0; i < 3; i++)charbuff[i] = co_item_ram[14][0][i];
	charbuff[i] = NULL; 
	out_line_24(192, 186 + (TPR_YOFF * 4), charbuff, BLK_LCD);
	for (i = 0; i < 3; i++)charbuff[i] = co_item_ram[15][0][i];
	charbuff[i] = NULL; 
	out_line_24(298, 186 + (TPR_YOFF * 4), " - ", BLK_LCD);
}
/*****************************************************************************/
void dsp_test(void){
	clr_screen();
	dsp_los_table();
	LCDbmpxx(bm_seq, 246, 66);
	LCDbmpxx(bm_12025_w, 246+120, 66);
	LCDbmpxx(bm_strip,246,106);
	LCDbmpxx(bm_12025_w, 246+120, 106);
	
	LCDbmpxx(bm_samid, 246, 146);
	LCDbmpxx(bm_360_w, 246+120, 146);

	dsp_item_tb();
	get_strip_name(temp_stripno[measCnt]);
	dsp_color_no(temp_colorno[measCnt], 0);
	dsp_trubi_no(temp_turbino[measCnt], 0);

	if(SEQ_NO < END_SEQ_NO){
		sprintf(buff_seqno, "%4d", temp_seqno[measCnt]);
	}else{
		sprintf(buff_seqno, "%4d", END_SEQ_NO);
	}
	fill_zero(buff_seqno);
	if(!fullFlag){
		out_line_16(371, 70, buff_seqno, BLK_LCD);
	}else{
		out_line_16(371, 70, buff_seqno, RED_LCD);
	}
	if(store_ErrCode != NO_STRIP || measCnt == 0){
		out_line_16(371, 110, buff_strip_name, BLK_LCD);
	}else{
		out_line_16(371, 110, s_NoStrip[LANGUAGE_FLAG], BLK_LCD);
	}
	out_line_16(371, 150, tempSampleID[measCnt], BLK_LCD);
	tempMeasCnt = measCnt;

//	if (SEQ_NO != END_SEQ_NO && trayFullFlag)AddError(Err_WasteFull);
}
/*****************************************************************************/
void dsp_test_set(void){
	printf0("\r dsp test set");
	LCDbmpxx(bm_keyboard,80,58);
	cursor_set = 0;
	
	dsp_test_set_seqno();

	LCDbmpxx(bm_arrow_left, 368, 65);
	LCDbmpxx(bm_arrow_right,402, 65);
	LCDbmpxx(bm_strip,118,106);
	LCDbmpxx(bm_12025_g,118+120,106);
	
	dsp_color_no(temp_colorno2[tempMeasCnt], 94);
	dsp_trubi_no(temp_turbino2[tempMeasCnt], 94);
	dsp_test_set_sid();
	get_strip_name(temp_stripno2[tempMeasCnt]);
	out_line_16(118+120+4, 111, buff_strip_name, BLK_LCD);
}
/*****************************************************************************/
void dsp_test_set_seqno(void){
	if(modex==MODE_TEST_seq) {
		LCDbmpxx(bm_seq, 118, 66);
		LCDbmpxx(bm_12025_w, 118+120, 66);
	}
	else{
		LCDbmpxx(bm_seq, 118, 66);
		LCDbmpxx(bm_12025_g, 118+120, 66);
	}
	sprintf(buff_seqno, "%4d", temp_seqno2[tempMeasCnt]);
	fill_zero(buff_seqno);
	out_line_16(250, 70, buff_seqno, BLK_LCD);
}
/*****************************************************************************/
void dsp_test_set_sid(void){
	if(modex==MODE_TEST_sid){
		LCDbmpxx(bm_samid, 118,146);
		LCDbmpxx(bm_360_w, 118+120, 146);
	}else{
		LCDbmpxx(bm_samid, 118,146);
		LCDbmpxx(bm_360_g, 118+120, 146);
	}
	out_line_16(250, 150, tempSampleID2[tempMeasCnt], BLK_LCD);
}
/*****************************************************************************/
void dsp_test_set_strip(void){
u16 i, j;
u32 sp;
	LCDbmpxx(bm_bm20,118+120,106);
	prt_msg("\r MAX_STRIPNO:%d,%d,%d", MAX_STRIPNO, cursor_set, temp_stripno2[tempMeasCnt]);	
	for(i=0;i<9;i++){
		if((i+cursor_set)>=MAX_STRIPNO) break;
		sp = EXT_BUFF+STRIP_INFO_RAM+((i+cursor_set)*STRIP_OFFSET)+2;
		for(j=0;j<10;j++){
			charbuff[j] = *(u8*)(sp++);
			if(/*(charbuff[j]==' ')||*/(charbuff[j]==NULL)) break;
		}
		charbuff[j] = NULL;
		out_line_16(118+120+4, 111+(i*25), charbuff, BLK_LCD);		
	}	
	flag_dsp_strip = 1;
}
/*****************************************************************************/
void dsp_test_set_color(void){
u16 i;
	LCDbmpxx(bm_bm20,530-94+120,66);
	prt_msg("\r color no:%d", temp_colorno2[tempMeasCnt]);	
	for(i=0;i<9;i++){
		switch(i){
			case 0:	 strcpy(charbuff, s_None[LANGUAGE_FLAG]);		break; //1	
			case 1:	 strcpy(charbuff, s_Yellow[LANGUAGE_FLAG]);		break; //2	
			case 2:	 strcpy(charbuff, s_DKYellow[LANGUAGE_FLAG]);	break; //3	
			case 3:	 strcpy(charbuff, s_Straw[LANGUAGE_FLAG]);		break; //4	
			case 4:	 strcpy(charbuff, s_Amber[LANGUAGE_FLAG]);		break; //5 
			case 5:	 strcpy(charbuff, s_Red[LANGUAGE_FLAG]);		break; //6 
			case 6:	 strcpy(charbuff, s_Orange[LANGUAGE_FLAG]);		break; //7 
			case 7:	 strcpy(charbuff, s_Green[LANGUAGE_FLAG]);		break; //8 
			default: strcpy(charbuff, s_Other[LANGUAGE_FLAG]);		break; //9 
		}
		out_line_16(530-94+120+8, 71+(i*25), charbuff, BLK_LCD);		
	}	
	flag_dsp_color = 1;
}
/*****************************************************************************/
void dsp_test_set_turbi(void){
u16 i;
	LCDbmpxx(bm_bm20,530-94+120,106);
	prt_msg("\r turbi no:%d", temp_turbino2[tempMeasCnt]);	
	for(i=0;i<6;i++){
		switch(i){
			case 0:	 strcpy(charbuff, s_None[LANGUAGE_FLAG]);		break; //1	
			case 1:	 strcpy(charbuff, s_Clear[LANGUAGE_FLAG]);		break; //2	
			case 2:	 strcpy(charbuff, s_SL_Cloudy[LANGUAGE_FLAG]);	break; //3	
			case 3:	 strcpy(charbuff, s_Cloudy[LANGUAGE_FLAG]);		break; //4	
			case 4:	 strcpy(charbuff, s_Turbid[LANGUAGE_FLAG]);		break; //5 
			default: strcpy(charbuff, s_Other[LANGUAGE_FLAG]);		break; //6
		}
		out_line_16(530-94+120+8, 111+(i*25), charbuff, BLK_LCD);		
	}	
	flag_dsp_turbi = 1;
}
/*****************************************************************************/
void init_test_set(void){
u16 i,j;	
	maxTempCnt = 0;
	tempMeasCnt = measCnt = 0;
	los_count = 0;
	los_cursor = 0;
    barcodeFlag = 0;
	errorCnt = 0;
	test_type = GENERAL;
	for(i=0 ; i<1000; i++){
        if(SEQ_NO+i>END_SEQ_NO){
			temp_seqno[i] = temp_seqno2[i] = END_SEQ_NO;
		}else{
			temp_seqno[i] = temp_seqno2[i] = SEQ_NO+i;
		}
        temp_stripno[i] = temp_stripno2[i]=STRIP_NO;
        for(j=0;j<MAX_USERID;j++) tempSampleID[i][j] = tempSampleID2[i][j] = USER_ID[j] = ' ';
		tempSampleID[i][j] = tempSampleID2[i][j] = USER_ID[j] = NULL;
        temp_colorno[i] = temp_colorno2[i] = COLOR_NO = 0;
        temp_turbino[i] = temp_turbino2[i] = TURBI_NO = 0;
    }
	for(i=0;i<1023;i++)buffRESULT[i] = ' ';
}
void cancel_test_set(void){
	int i, j;
	for(i=measCnt;i<1000;i++){
		for(j=0;j<MAX_USERID+1;j++)tempSampleID2[i][j] = tempSampleID[i][j] ;
		temp_seqno2[i] = temp_seqno[i];
		temp_colorno2[i] = temp_colorno[i];
		temp_turbino2[i] = temp_turbino[i];
		temp_stripno2[i] = temp_stripno[i];
	}
	//printf3("c:%d m:%d", tempMeasCnt, maxTempCnt);
	tempMeasCnt = maxTempCnt;
	for(i=0;i<1024;i++)buffRESULT[i] = ' ';
}
void enter_test_set(void){//setting saving function
u16 i,j;	
//char tempString[30];
	//barcodeFlag = 0;
	for(i=measCnt;i<1000;i++){
		for(j=0;j<MAX_USERID+1;j++){
			if(tempSampleID[i][j] != tempSampleID2[i][j]) barcodeFlag = 0;
			tempSampleID[i][j] = tempSampleID2[i][j];
		}
		temp_seqno[i] = temp_seqno2[i];
		temp_colorno[i] = temp_colorno2[i];
		temp_turbino[i] = temp_turbino2[i];
		temp_stripno[i] = temp_stripno2[i];
	}
	if(tempMeasCnt+1>maxTempCnt)maxTempCnt = tempMeasCnt+1;
	for(j=0;j<MAX_USERID;j++)USER_ID[j] = tempSampleID[measCnt][j];
	store_seqno();
	COLOR_NO = temp_colorno[measCnt];
	write24w(COLOR_ADR, COLOR_NO);
	TURBI_NO = temp_turbino[measCnt];
	write24w(TURBI_ADR, TURBI_NO);
	STRIP_NO = temp_stripno[measCnt];
	move_strip_info();
	store_vars(STRIPNO_ADR, STRIP_NO);
	if(maxTempCnt == 0)maxTempCnt++;
	for(i=0;i<1024;i++)buffRESULT[i] = ' ';
	returnFlag = maxTempCnt-1;
	dsp_test();
	LCDbmpxx(bm_strip,246,106);
	LCDbmpxx(bm_12025_w, 246+120, 106);
	out_line_16(371, 110, strip_name, BLK_LCD);
	//clr_screen_box(160, 16, 320, 16);
	//sprintf(tempString, "temp:%2d max:%2d meas:%2d", tempMeasCnt, maxTempCnt, measCnt);
	//out_line_16(160, 16, tempString, BLK_LCD);
}
/*****************************************************************************/
void dsp_qc(void){
	char lotno[9]; 
	u8 i;
	clr_screen();
	dsp_los_table();

	LCDbmpxx(bm_seq, 246, 66);
	LCDbmpxx(bm_12025_w, 246+120, 66);

	if(QC_NO < END_QC_NO){
		sprintf(buff_seqno, "%4d", temp_seqno[measCnt]);
	}else{
		sprintf(buff_seqno, "%4d", END_QC_NO);
	}

	fill_zero(charbuff);
	if(!fullFlag){
		out_line_16(371, 70, buff_seqno, BLK_LCD);
	}else{
		out_line_16(371, 70, buff_seqno, RED_LCD);
	}
	LCDbmpxx(bm_strip, 246, 106);
	LCDbmpxx(bm_12025_w, 246+120, 106);
	get_strip_name(temp_stripno[measCnt]);
	out_line_16(371, 110, buff_strip_name, BLK_LCD);
	if (!NEGPOS) {
		LCDbmpxx(bm_qc, 246, 146);
		LCDbmpxx(bm_neg_w, 366, 146);
		LCDbmpxx(bm_pos_g, 498, 146);
	}
	else {
		LCDbmpxx(bm_qc, 246, 146);
		LCDbmpxx(bm_neg_g, 366, 146);
		LCDbmpxx(bm_pos_w, 498, 146);
	}
	LCDbmpxx(bm_lotno, 530, 66);
	LCDbmpxx(bm_12025_w, 530+120, 66);
	
	for(i=0;i<MAX_LOTNO;i++)lotno[i] = temp_lotno[measCnt][i];
	lotno[i] = NULL;
	out_line_16(656, 70, lotno, BLK_LCD);

	dsp_item_tb();
	tempMeasCnt = measCnt;
//	if (QC_NO != END_QC_NO && trayFullFlag)AddError(Err_WasteFull);
}

/*****************************************************************************/
void dsp_qc_set(void){
	printf0("\r dsp qc set");
	LCDbmpxx(bm_keyboard,80,58);
	cursor_set = 0;
	
	dsp_qc_set_seqno();
	LCDbmpxx(bm_arrow_left, 368, 65);
	LCDbmpxx(bm_arrow_right,402, 65);

	LCDbmpxx(bm_strip,118,106);
	LCDbmpxx(bm_12025_g,118+120,106);

	get_strip_name(temp_stripno2[tempMeasCnt]);
	out_line_16(118+120+4, 111, buff_strip_name, BLK_LCD);
	
	dsp_qc_set_negpos();

	dsp_qc_set_lot();
}
/*****************************************************************************/
void dsp_qc_set_seqno(void){
	if(modex==MODE_QC_seq) {
		LCDbmpxx(bm_seq, 118, 66);
		LCDbmpxx(bm_12025_w, 118+120, 66);
	}
	else {
		LCDbmpxx(bm_seq, 118, 66);
		LCDbmpxx(bm_12025_w, 118+120, 66);
	}
	sprintf(buff_seqno, "%4d", temp_seqno2[tempMeasCnt]);
	fill_zero(buff_seqno);
	out_line_16(250, 70, buff_seqno, BLK_LCD);
}
/*****************************************************************************/
void dsp_qc_set_negpos(void){
	if (!temp_negpos2[tempMeasCnt]) {
		LCDbmpxx(bm_qc, 118, 146);
		LCDbmpxx(bm_neg_w, 238, 146);
		LCDbmpxx(bm_pos_g, 358, 146);
	}
	else {
		LCDbmpxx(bm_qc, 118, 146);
		LCDbmpxx(bm_neg_g, 238, 146);
		LCDbmpxx(bm_pos_w, 358, 146);
	}
}
/*****************************************************************************/
void dsp_qc_set_lot(void){
	if(modex==MODE_QC_lot) {
		LCDbmpxx(bm_lotno, 530-94, 66);
		LCDbmpxx(bm_12025_w, 530-94+120, 66);

	}
	else{
		LCDbmpxx(bm_lotno, 530-94, 66);
		LCDbmpxx(bm_12025_g, 530-94+120, 66);

	}
	out_line_16(660-98, 70,temp_lotno2[tempMeasCnt], 	BLK_LCD);
	if(modex==MODE_QC_lot)dsp_cursor_qc_lot(cursor_set);
}
/*****************************************************************************/
void init_qc_set(void){
u16 i, j;	
	maxTempCnt = los_count=los_cursor=0;	//List of samples �ʱ�ȭ
    tempMeasCnt = measCnt=0;
	errorCnt = 0;
	test_type = QC_MEMORY;
    for(i=0;i<999;i++){
		if(QC_NO+i>END_QC_NO){
        temp_seqno[i] = temp_seqno2[i] = END_QC_NO;
		}else{
			temp_seqno[i] = temp_seqno2[i] = QC_NO+i;
		}
        temp_stripno[i] = temp_stripno2[i] = STRIP_NO;
        temp_colorno[i] = temp_colorno2[i] = COLOR_NO = 0;
        temp_turbino[i] = temp_turbino2[i] = TURBI_NO = 0;
        temp_negpos[i] = temp_negpos2[i] = NEGPOS=0;
		for(j=0;j<MAX_LOTNO;j++)tempSampleID[i][j] = tempSampleID2[i][j] = temp_lotno[i][j] = temp_lotno2[i][j] = LOT_NO[j] = ' ';
		tempSampleID[i][j] = tempSampleID2[i][j] = temp_lotno[i][j] = temp_lotno2[i][j] = LOT_NO[j] = 0;
		tempSampleID[i][8] = tempSampleID2[i][8] = 'N';
		tempSampleID[i][9] = tempSampleID2[i][9] = 'E';
		tempSampleID[i][10] = tempSampleID2[i][10] = 'G';
		tempSampleID[i][11] = tempSampleID2[i][11] = 'A';
		tempSampleID[i][12] = tempSampleID2[i][12] = 'T';
		tempSampleID[i][13] = tempSampleID2[i][13] = 'I';
		tempSampleID[i][14] = tempSampleID2[i][14] = 'V';
		tempSampleID[i][15] = tempSampleID2[i][15] = 'E';
    }
}
void cancel_qc_set(void){
	int i, j;
	for(i=measCnt;i<999;i++){
        temp_stripno2[i] = temp_stripno[i];
        for(j=0;j<MAX_LOTNO+1;j++)temp_lotno2[i][j] = temp_lotno[i][j]; 
		for(j=0;j<MAX_USERID;j++)tempSampleID2[i][j] = tempSampleID[i][j];
        temp_negpos2[i] = temp_negpos[i];
	}
		tempMeasCnt = maxTempCnt;
		for(i=0;i<1024;i++)buffRESULT[i] = ' ';
}
void enter_qc_set(void){
	int i, j;
	for(i=measCnt;i<999;i++){
		for (j = 0; j < MAX_LOTNO; j++)tempSampleID[i][j]=temp_lotno[i][j] = temp_lotno2[i][j];
		for(j=MAX_LOTNO;j<MAX_USERID;j++)tempSampleID[i][j] = tempSampleID2[i][j];
		tempSampleID[i][j] = NULL;
		temp_negpos[i] = temp_negpos2[i];
		temp_stripno[i] = temp_stripno2[i];
	}
	if(tempMeasCnt+1>maxTempCnt)maxTempCnt = tempMeasCnt+1;
	NEGPOS = temp_negpos[measCnt];
	printf0("\nLot No saved\n");
	write24w(QC_NO_ADR, QC_NO);
	printf0("\nSEQ saved\n");	
	STRIP_NO = temp_stripno[measCnt];
	move_strip_info();
	store_vars(STRIPNO_ADR, STRIP_NO);
	returnFlag = maxTempCnt-1;
	for(i=0;i<1024;i++)buffRESULT[i] = ' ';
	dsp_qc();
}
/*****************************************************************************/
/*****************************************************************************/
void dsp_los_table(void) {
	u16 a, j;
	u8 losCnt;
	int idLength;
	char viewID[12];
	LCDbmpxx(bm_list_sample, 2, 66);
	if(measCnt>0 || maxTempCnt>0){
		if(maxTempCnt<=LOSCOUNT){
			losCnt = maxTempCnt;
		}
		else{
			losCnt = LOSCOUNT;
		}
		for (a = 0; a < losCnt; a++) {
			//SEQ 표시
			sprintf(charbuff, "%4d", temp_seqno[los_cursor+a]);
			fill_zero(charbuff);
			out_line_16(3, 107 + (a * 27), charbuff, BLK_LCD);
			
			//ID 표시
			if(modex == MODE_TEST || modex == MODE_QC || modex == MODE_TEST_Error || modex == MODE_QC_Error){
				for(j=0;j<8;j++)viewID[j] = tempSampleID[los_cursor+a][j];
				viewID[j]=NULL;
				out_line_16(52, 107 + (a * 27), viewID, BLK_LCD);
			}else{
				for(idLength=17;idLength>=0;idLength--){
					if(tempSampleID[los_cursor+a][idLength] != 0 && tempSampleID[los_cursor+a][idLength] != ' ')break;
				}idLength++;
				if(idLength>12){
					for(j=0;j<4;j++){
						viewID[j] = tempSampleID[los_cursor+a][j];
					}
					viewID[4] = '.';
					viewID[5] = '.';
					viewID[6] = '.';
					for(j=idLength;j>idLength-4;j--){
						viewID[10 + (j - idLength)] = tempSampleID[los_cursor+a][j-1];
					}
					viewID[11] = NULL;				
					out_line_16(52, 107 + (a * 27), viewID, BLK_LCD);
				}else
				{
					out_line_16(52, 107 + (a * 27), tempSampleID[los_cursor+a], BLK_LCD);
				}
			}
		}
	}
}
void dsp_los_table_store(void) {
	u32 i, b, c;
	u8 j, k;
	char viewID[12];
	LCDbmpxx(bm_list_sample, 2, 66);

	for (i = 0; i < LOSCOUNT; i++) {
		if (i >= los_count - mem_point) {
			sprintf(charbuff, "                  ");
			out_line_16(3, 107 + (i * 27), charbuff, BLK_LCD);					//Sequence
			out_line_16(52, 107 + (i * 27), charbuff, BLK_LCD);					//Sample ID
		}
		else {
			b = LOS_BUFF + ((mem_point + i) * LOS_SIZE);
			c = *(u8*)(b++);
			c = (c << 8) + *(u8*)(b++);
			temp_seqno[i] = c;
			sprintf(charbuff, "%4d", c);
			fill_zero(charbuff);
			out_line_16(3, 107 + (i * 27), charbuff, BLK_LCD);					//Sequence

			for (c = 0; c < MAX_USERID; c++) charbuff[c] = *(u8*)(b++);
			charbuff[c] = NULL;
			k=0;
				for(j=0;j<MAX_USERID;j++){
					if(charbuff[j] != ' ' && charbuff[j] != NULL)k++;
				}
				if(temp_seqno[i]>END_SEQ_NO){
					for(j=0;j<MAX_LOTNO;j++){
						viewID[j] = charbuff[j+8];
					}
					out_line_16(52, 107 + (i * 27), viewID, BLK_LCD);					
				}else if(k>12){
					for(j=0;j<4;j++){
						viewID[j] = charbuff[j];
					}
					viewID[4] = '.';
					viewID[5] = '.';
					viewID[6] = '.';
					for(j=k;j>k-4;j--){
						viewID[10 + (j - k)] = charbuff[j-1];
					}
					viewID[11] = NULL;				
					out_line_16(52, 107 + (i * 27), viewID, BLK_LCD);
				}
				else{
					out_line_16(52, 107 + (i * 27), charbuff, BLK_LCD);					//Sample ID
				}

		}
	}
}
/*****************************************************************************/
void add_los_test(u16 sno){
u16 c;	
u32 b;
	b = LOS_BUFF + (los_count*LOS_SIZE);
	*(u8*)(b++) = sno>>8;
	*(u8*)(b++) = sno>>0;
	for(c=0;c<MAX_USERID+1;c++) *(u8*)(b++) = MEASuserid[c];
	if(los_count>=999) los_count=0;
	else los_count++;
}
/*****************************************************************************/
void add_los_qc(u16 sno){
u16 c;	
u32 b;
	b = LOS_BUFF + (los_count*LOS_SIZE);
	*(u8*)(b++) = sno>>8;
	*(u8*)(b++) = sno>>0;
	for (c = 0; c < 6; c++) *(u8*)(b++) = MEASuserid[c];
	if(los_count>=999) los_count=0;
	else los_count++;
}
/*****************************************************************************/
void add_los_memory(u16 sno) {
	u16 c;
	u32 b;

	b = LOS_BUFF + (los_count * LOS_SIZE);
	*(u8*)(b++) = sno >> 8;
	*(u8*)(b++) = sno >> 0;
	for (c = 0; c < MAX_USERID; c++) *(u8*)(b++) = MEASuserid[c];
	if (los_count >= 4999) los_count = 0;
	else los_count++;
}
/*****************************************************************************/
void clr_screen(void){
	uint32_t index0;
	uint32_t index;
	u16 xx,yy;
	for(yy=50;yy<430;yy++){
		index0 = LCD_FRAME_BUFFER+ (yy*800*3);
		index = LCD_FRAME_BUFFER+ BUFFER_OFFSET+(yy*800*3);
		for(xx=0;xx<(800*3);xx+=3){
	    	*(__IO uint8_t*)(index + xx + 0) = 0xFF;
	    	*(__IO uint8_t*)(index + xx + 1) = 0xFF;
	    	*(__IO uint8_t*)(index + xx + 2) = 0xFF;			
	    	*(__IO uint8_t*)(index0 + xx + 0) = 0xFF;
	    	*(__IO uint8_t*)(index0 + xx + 1) = 0xFF;
	    	*(__IO uint8_t*)(index0 + xx + 2) = 0xFF;			
		}
	}
}
/*****************************************************************************/
void dsp_strip(void){
	u16 i, j=0x0001;
	for(int a=0;a<15;a++){
		LCDbmpxx(bm_strip_no, 317+32*a, 431);
	}
	for(i=0;i<15;i++){
		if(read_strip_move&j){
//			draw_box(100+(38*i),20,120+(38*i),60, GRN_LCD);
			LCDbmpxx(bm_strip_yes,317+(i*32),431);
		}else{
//			draw_box(100+(38*i),20,120+(38*i),60, BLU_LCD);
			LCDbmpxx(bm_strip_no,317+(i*32),431);
		}
		j <<= 1;
	}
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
void dsp_novalid(void){		
}
/************************************************************************/
//#define IS_STRIP	350
//#define IS_STRIP	500	//210423
//#define IS_STRIP	250	//210510
//#define IS_STRIP	100	//210511


void dsp_result(u16 sno){
//u32 ww;
//char str[16];
	out_time();
	LCDbmpxx(bm_seq, 246, 66);
	LCDbmpxx(bm_12025_w, 246+120, 66);
	make_seqno2dgtstr(sno);
	if(!fullFlag){
		out_line_16(371, 70, charbuff, BLK_LCD);
	}else{
		out_line_16(371, 70, charbuff, RED_LCD);
	}
	LCDbmpxx(bm_strip,246,106);
	LCDbmpxx(bm_12025_w,246+120,106);
	out_line_16(390, 110, s_NoStrip[LANGUAGE_FLAG], BLK_LCD);
}
/************************************************************************/
void dsp_result_qc(u16 sno){
	out_time();
	LCDbmpxx(bm_seq, 246, 66);
	LCDbmpxx(bm_12025_w, 246+120, 66);
	make_seqno2dgtstr(sno);
	if(!fullFlag){
		out_line_16(371, 70, charbuff, BLK_LCD);
	}else{
		out_line_16(371, 70, charbuff, RED_LCD);
	}
	LCDbmpxx(bm_strip,246,106);							//Display Pop Up
	LCDbmpxx(bm_12025_w,246+120,106);							//Display Pop Up
	out_line_16(390, 110, s_NoStrip[LANGUAGE_FLAG], BLK_LCD);
	dsp_item_title();									//dsp_item_tb();
	LCDbmpxx(bm_popup6_top, 250, 185);
	LCDbmpxx(bm_popup6_mid, 250, 185+50);
	LCDbmpxx(bm_popup2_bottom, 250, 185+50+70);
}
/************************************************************************/
void dsp_result_store(void){
//u32 ww;
char str[MAX_USERID+1];
u16 i;
	clr_screen();
	if (modex <= MODE_QC_lot && modex >= MODE_TEST){
		dsp_los_table();
	}
	else{
		dsp_los_table_store();
	}
	if(cur_dsp_record_no>=SRT_QC_NO)test_type = QC_MEMORY;
	else test_type = GENERAL;
	out_time();
	LCDbmpxx(bm_seq, 246, 66);
	LCDbmpxx(bm_12025_w, 246+120, 66);
	make_seqno2dgtstr(cur_dsp_record_no);
	out_line_16(371, 70, charbuff, BLK_LCD);

	LCDbmpxx(bm_strip,246,106);							//strip name box white
	LCDbmpxx(bm_12025_w,246+120,106);							//strip name box white
	
	if (test_type == QC_MEMORY) {
		if (store_NEGPOS == 0){
			LCDbmpxx(bm_qc, 246, 146);
			LCDbmpxx(bm_neg_w, 366, 146);
			LCDbmpxx(bm_pos_g, 486, 146);
		}
		else if(store_NEGPOS == 1){
			LCDbmpxx(bm_qc, 246, 146);
			LCDbmpxx(bm_neg_g, 366, 146);
			LCDbmpxx(bm_pos_w, 486, 146);
		}
		LCDbmpxx(bm_lotno, 530, 66);
		LCDbmpxx(bm_12025_w, 530+120, 66);

		for(i=0;i<8;i++) str[i] = cmdbuff[i+30];  
		str[i] = NULL;
		out_line_16(656, 70, str, BLK_LCD);
	}
	else{
		LCDbmpxx(bm_samid, 246, 146);					//sample name box white
		LCDbmpxx(bm_360_w, 246+120, 146);
		dsp_color_no(store_COLOR_NO, 0);
		dsp_trubi_no(store_TURBI_NO, 0);
		for(i=0;i<MAX_USERID;i++) str[i] = cmdbuff[i+30];  
		str[i] = NULL;
		if (sampleIDFlag) {
			out_line_16(371, 150, str, BLK_LCD);				//sample id
		}
	}

	if (store_ErrCode == NO_STRIP) {
		LCDbmpxx(bm_item_tb, 246, 182);
		out_line_16(371, 110, s_NoStrip[LANGUAGE_FLAG], BLK_LCD);
		dsp_item_title();
	}
	else {
		out_line_16(371, 110, MEASstripname, BLK_LCD);
		dsp_item_tb();
	}
}
/************************************************************************/
void dsp_item_tb(void){
char str[64];
u8 acrFlag = 0;
u16 i, j, k, m, rno=0;
	dsp_item_title();
	if(store_ErrCode != NO_STRIP){
		for(i=0;i<64;i++) str[i] = ' ';
		for(i=0;i<16;i++){ 
			if (buffRESULT[(i * 42) - 2] == 0x0d) {
				rno++;
			}
		}
	//	for(i=0;i<STRIP_INFO[0];i++){
		for(i=0;i<rno;i++){ 
			if(i<8){ 
				k = i; 
				m = 0; 
			}
			else{
				k = i-8; 
				m = 272; 
			}
			
			for(j= 5;j< 8;j++)str[j- 5] = buffRESULT[(i*42)+j];
			str[j- 5] = 0x00;
			if (temp_result[i] == 0 || !MARK_FLAG) {
				if(MEASstripinfo[i+1]>0)LCDbmpxx(tb_item[0][MEASstripinfo[i+1]-1],246+m,209+(27*k));
				else if(str[0] != ' '){
					acrFlag++;
					LCDbmpxx(bm_result_acr_b,246+m,209+(27*k));
				}
			}
			else{
				if(MEASstripinfo[i+1]>0)LCDbmpxx(tb_item[1][MEASstripinfo[i+1]-1],246+m,209+(27*k));
				else if(str[0] != ' '){
					acrFlag++;
					LCDbmpxx(bm_result_acr_r,246+m,209+(27*k));
				}
			}
			for(j=13;j<21;j++) str[j-13] = buffRESULT[(i*42)+j]; 
			str[j-13] = 0x00;
			if (temp_result[i] == 0 || !MARK_FLAG) {
				if (PLUS_FLAG)out_line_16(m + 260 + 64, 188 + 27 + (k * 27), str, BLK_LCD);
			}
			else {
				if (PLUS_FLAG)out_line_16(m + 260 + 64, 188 + 27 + (k * 27), str, RED_LCD);
			}
			if(MEASstripinfo[i]==0 && str[0] != ' '){
				acrFlag++;
				LCDbmpxx(bm_result_normal+tb_acr-1, 246+68+m,209+(27*k));
			}
			for(j=21;j<29;j++) str[j-21] = buffRESULT[(i*42)+j]; 
			str[j-21] = 0x00;
			if (temp_result[i] == 0 || !MARK_FLAG) {
				out_line_16(m + 260 + 129, 188 + 27 + (k * 27), str, BLK_LCD);
			}
			else {
				out_line_16(m + 260 + 129, 188 + 27 + (k * 27), str, RED_LCD);
			}
			if(MEASstripinfo[i]==0 && str[0] != ' '){
				acrFlag++;
				LCDbmpxx(bm_result_normal+tb_acr-1, 246+68+m,209+(27*k));
			}
			for(j=29;j<37;j++) str[j-29] = buffRESULT[(i*42)+j]; 
			str[j-29] = 0x00;
			if (temp_result[i] == 0 || !MARK_FLAG) {
				if(acrFlag<=1 || acrFlag == 0){
					LCDbmpxx(tb_unit[0][MEASstripinfo[i+1]-1][UNIT_FLAG],246+204+m,209+(27*k));
					if(MEASstripinfo[i+1]==0)acrFlag++;
				}else{
					LCDbmpxx(bm_result_empty,246+204+m,209+(27*k));
				}
			}
			else {
				if(acrFlag<=1){
					LCDbmpxx(tb_unit[1][MEASstripinfo[i+1]-1][UNIT_FLAG],246+204+m,209+(27*k));
					if(MEASstripinfo[i+1]==0)acrFlag++;
				}
				else{
					LCDbmpxx(bm_result_empty,246+204+m,209+(27*k));
				}
			}
		}
	}
}
/************************************************************************/
void dsp_item_title(void){
	LCDbmpxx(bm_item_tb,246,182);
	if (UNIT_FLAG == 1) {
		LCDbmpxx(bm_result_item,246,182);
		LCDbmpxx(bm_result_arb,246+68,182);
		LCDbmpxx(bm_result_conv,246+68+68,182);
		LCDbmpxx(bm_result_unit_g,246+68+68+68,182);
		LCDbmpxx(bm_result_item,246+68+68+68+68,182);
		LCDbmpxx(bm_result_arb,246+68+68+68+68+68,182);
		LCDbmpxx(bm_result_conv,246+68+68+68+68+68+68,182);
		LCDbmpxx(bm_result_unit_g,246+68+68+68+68+68+68+68,182);
	}
	else if(UNIT_FLAG == 2){
		LCDbmpxx(bm_result_item,246,182);
		LCDbmpxx(bm_result_arb,246+68,182);
		LCDbmpxx(bm_result_si,246+68+68,182);
		LCDbmpxx(bm_result_unit_g,246+68+68+68,182);
		LCDbmpxx(bm_result_item,246+68+68+68+68,182);
		LCDbmpxx(bm_result_arb,246+68+68+68+68+68,182);
		LCDbmpxx(bm_result_si,246+68+68+68+68+68+68,182);
		LCDbmpxx(bm_result_unit_g,246+68+68+68+68+68+68+68,182);
	}
}
/************************************************************************/
void dsp_color_no(u16 cno, u16 xp){
	LCDbmpxx(bm_color,530-xp, 66);
	LCDbmpxx(bm_12025_w,530-xp+120, 66);
printf0("\r color no:%d, xp:%d", cno, xp);
	switch(cno){
		case 0:	 out_line_16(660-xp, 70,s_None[LANGUAGE_FLAG], 	BLK_LCD);		break; //1	
		case 1:	 out_line_16(660-xp, 70,s_Yellow[LANGUAGE_FLAG], 	BLK_LCD);		break; //2	
		case 2:	 out_line_16(660-xp, 70,s_DKYellow[LANGUAGE_FLAG],BLK_LCD);		break; //3	
		case 3:	 out_line_16(660-xp, 70,s_Straw[LANGUAGE_FLAG], 	BLK_LCD);		break; //4	
		case 4:	 out_line_16(660-xp, 70,s_Amber[LANGUAGE_FLAG], 	BLK_LCD);		break; //5 
		case 5:	 out_line_16(660-xp, 70,s_Red[LANGUAGE_FLAG], 		BLK_LCD);		break; //6 
		case 6:	 out_line_16(660-xp, 70,s_Orange[LANGUAGE_FLAG], 	BLK_LCD);		break; //7 
		case 7:	 out_line_16(660-xp, 70,s_Green[LANGUAGE_FLAG], 	BLK_LCD);		break; //8 
		default: out_line_16(660-xp, 70,s_Other[LANGUAGE_FLAG], 	BLK_LCD);		break; //9 
	}
}
/************************************************************************/
void dsp_trubi_no(u16 tno, u16 xp){
	LCDbmpxx(bm_trub,530-xp,106);
	LCDbmpxx(bm_12025_w,530-xp+120, 106);
printf0("\r turbi no:%d, xp:%d", tno, xp);
	switch(tno){
		case 0:	 out_line_16(660-xp, 110,s_None[LANGUAGE_FLAG], 	BLK_LCD);		break; //1	
		case 1:	 out_line_16(660-xp, 110,s_Clear[LANGUAGE_FLAG], 	BLK_LCD);		break; //2	
		case 2:	 out_line_16(660-xp, 110,s_SL_Cloudy[LANGUAGE_FLAG],BLK_LCD);		break; //3	
		case 3:	 out_line_16(660-xp, 110,s_Cloudy[LANGUAGE_FLAG], 	BLK_LCD);		break; //4	
		case 4:	 out_line_16(660-xp, 110,s_Turbid[LANGUAGE_FLAG], 	BLK_LCD);		break; //5 
		default: out_line_16(660-xp, 110,s_Other[LANGUAGE_FLAG], 	BLK_LCD);		break; //9 
	}
}
/************************************************************************/
/************************************************************************/

/************************************************************************/
void fill_zero(char *str){
u8 a=0;
	while(1){
		if(str[a]==' ') str[a]='0';
		else if(str[a]==NULL) break;
		if((++a)>250) break;
	}	
}
/************************************************************************/
void make_seqno2dgtstr(u16 seqno){
	sprintf(charbuff, "%4d", seqno);
	fill_zero(charbuff);
	//printf0("\n seq.no:%s", charbuff);
}
/************************************************************************/
void dsp_sysset(void) {
	clr_screen();
	LCDbmpxx(bm_system_set2, 40, 58);

	LCDbmpxx(bm_print, 70, 130);
	if (PRINT_FLAG == 1)
	{
		LCDbmpxx(bm_1_w, 110, 130);
		LCDbmpxx(bm_2_g, 220, 130);
		LCDbmpxx(bm_0_g, 330, 130);
	}
	else if (PRINT_FLAG == 2)
	{
		LCDbmpxx(bm_1_g, 110, 130);
		LCDbmpxx(bm_2_w, 220, 130);
		LCDbmpxx(bm_0_g, 330, 130);
	}
	else /*if (PRINT_FLAG == 0)*/{
		LCDbmpxx(bm_1_g, 110, 130);
		LCDbmpxx(bm_2_g, 220, 130);
		LCDbmpxx(bm_0_w, 330, 130);
	}

	LCDbmpxx(bm_usb, 70, 180);
	if (SBAUD_FLAG == 0){
		LCDbmpxx(bm_9600_w, 110, 180);
		LCDbmpxx(bm_38400_g, 220, 180);
		LCDbmpxx(bm_115200_g, 330, 180);
	}
	else if (SBAUD_FLAG == 1){
		LCDbmpxx(bm_9600_g, 110, 180);
		LCDbmpxx(bm_38400_w, 220, 180);
		LCDbmpxx(bm_115200_g, 330, 180);
	}
	else{
		LCDbmpxx(bm_9600_g, 110, 180);
		LCDbmpxx(bm_38400_g, 220, 180);
		LCDbmpxx(bm_115200_w, 330, 180);
	}

	LCDbmpxx(bm_10101, 70, 230);
	if (BAUD_FLAG == 0){
		LCDbmpxx(bm_9600_w, 110, 230);
		LCDbmpxx(bm_38400_g, 220, 230);
		LCDbmpxx(bm_115200_g, 330, 230);
	}
	else if (BAUD_FLAG == 1){
		LCDbmpxx(bm_9600_g, 110, 230);
		LCDbmpxx(bm_38400_w, 220, 230);
		LCDbmpxx(bm_115200_g, 330, 230);
	}
	else{
		LCDbmpxx(bm_9600_g, 110, 230);
		LCDbmpxx(bm_38400_g, 220, 230);
		LCDbmpxx(bm_115200_w, 330, 230);
	}

	LCDbmpxx(bm_speker, 70, 280);
	if (SOUND_FLAG == 1){
		LCDbmpxx(bm_on_w, 110, 280);
		LCDbmpxx(bm_off_g, 220, 280);
	}
	else if (SOUND_FLAG == 0){
		LCDbmpxx(bm_on_g, 110, 280);
		LCDbmpxx(bm_off_w, 220, 280);
	}

	LCDbmpxx(bm_globe, 70, 330);
	if (LANGUAGE_FLAG == 0){
		LCDbmpxx(bm_lang1_w, 110, 330);
		LCDbmpxx(bm_lang2_g, 220, 330);
	}
	else if (LANGUAGE_FLAG == 1){
		LCDbmpxx(bm_lang1_g, 110, 330);
		LCDbmpxx(bm_lang2_w, 220, 330);
	}

	//LCDbmpxx(bm_target, 70, 380);//reset : always off
	//LCDbmpxx(bm_on_g, 110, 380);
	//LCDbmpxx(bm_off_w, 220, 380);


	LCDbmpxx(bm_auto, 70, 380);
	if (AUTO_FLAG == 1){
		LCDbmpxx(bm_on_w, 110, 380);
		LCDbmpxx(bm_off_g, 220, 380);
	}
	else if (AUTO_FLAG == 0){
		LCDbmpxx(bm_on_g, 110, 380);
		LCDbmpxx(bm_off_w, 220, 380);
	}

	LCDbmpxx(bm_mainticon, 470, 380);
	LCDbmpxx(bm_maintenance, 510, 380);

	LCDbmpxx(bm_save, 690, 438);
}

/*****************************************************************************/
void dsp_maintanence_password(void){
	LCDbmpxx(bm_digit_box,400-126,58);
	LCDbmpxx(bm_12040_w,468-126,66);
	password[0] = '0';
	password[1] = '0';
	password[2] = '0';
	password[3] = '0';
	password[4] = NULL;
	out_line_32(471-116, 70, password, BLK_LCD);
	dsp_cursor_password(0);
}
void dsp_maintanence(void){
	clr_screen();
	dsp_base_bar();
	LCDbmpxx(bm_maintwide, 40, 58);

	LCDbmpxx(bm_calicon, 70, 130);		//Calibration
	LCDbmpxx(bm_calibration, 110, 130);

	LCDbmpxx(bm_firmup, 70, 180);			//Firmware Update
	LCDbmpxx(bm_fw_update, 110, 180);

	LCDbmpxx(bm_imageup, 70, 230);		//Image Update
	LCDbmpxx(bm_image_update, 110, 230);

	LCDbmpxx(bm_fontup, 70, 280);		//Font Update
	LCDbmpxx(bm_font_update, 110, 280);

	LCDbmpxx(bm_serialdata, 70, 330);		//Serial Data
	LCDbmpxx(bm_serial_data , 110, 330);

	LCDbmpxx(bm_target, 70, 380);//reset : always off
	//LCDbmpxx(bm_on_g, 110, 380);
	//LCDbmpxx(bm_off_w, 220, 380);
}
/************************************************************************/
void dsp_unit(void) {
	clr_screen();
	LCDbmpxx(bm_result_unit2, 40, 58);

	LCDbmpxx(bm_triangle, 120, 150);
	if (temp_UNIT_FLAG == UNIT_SI) {
		LCDbmpxx(bm_conv_g, 160, 150);
		LCDbmpxx(bm_si_w, 270, 150);
	}
	else {
		LCDbmpxx(bm_conv_w, 160, 150);
		LCDbmpxx(bm_si_g, 270, 150);
	}
	LCDbmpxx(bm_plusminis, 120, 250);
	if (temp_PLUS_FLAG == 0) {
		LCDbmpxx(bm_on_g, 160, 250);
		LCDbmpxx(bm_off_w, 270, 250);
	}
	else {
		LCDbmpxx(bm_on_w, 160, 250);
		LCDbmpxx(bm_off_g, 270, 250);
	}
	LCDbmpxx(bm_pencel, 120, 350);
	if (temp_MARK_FLAG == 0) {
		LCDbmpxx(bm_on_g, 160, 350);
		LCDbmpxx(bm_off_w, 270, 350);
	}
	else {
		LCDbmpxx(bm_on_w, 160, 350);
		LCDbmpxx(bm_off_g, 270, 350);
	}
	LCDbmpxx(bm_save, 690, 438);
}
/************************************************************************/
void dsp_mem_printupload(void){
	char count[5];
	if(modex == MODE_MEM_print)LCDbmpxx(bm_print_confirm, 250, 185);
	else if(modex == MODE_MEM_upload) LCDbmpxx(bm_upload_confirm, 250, 185);
	LCDbmpxx(bm_popup3_bottom, 250, 185+50+70);
	sprintf(count, "%d", memCount+1);
	out_line_16(325, 223, count, BLK_LCD);
}
/************************************************************************/
void dsp_mem_date_set(void){
	LCDbmpxx(bm_testdate, 90, 66);
	if(modex == MODE_MEM_date_set){
		LCDbmpxx(bm_200_w, 210, 66);
	}else{
		LCDbmpxx(bm_200_g, 210, 66);	
	}
	if(dateMemFlag){
		if (!datePresentation) {
			sprintf(charbuff, "20%2d%c%2d%c%2d", setyear, tempDateDelimiter, setmonth, tempDateDelimiter, setday);
			fill_zero(charbuff);
			out_line_16(220, 70, charbuff, BLK_LCD);
		}
		else {
			sprintf(charbuff, "%2d%c%2d%c20%2d", setday, tempDateDelimiter, setmonth, tempDateDelimiter, setyear);
			fill_zero(charbuff);
			out_line_16(220, 70, charbuff, BLK_LCD);
		}
	}
}
/************************************************************************/
void dsp_input_password(void){
	LCDbmpxx(bm_digit_box,400-126,58);
	LCDbmpxx(bm_12040_w,468-126,66);
	password[0] = '0';
	password[1] = '0';
	password[2] = '0';
	password[3] = '0';
	password[4] = NULL;
	out_line_32(471-116, 70, password, BLK_LCD);
	dsp_cursor_password(0);
}
/************************************************************************/
void dsp_seq(void){
	LCDbmpxx(bm_seq, 246, 66);
	LCDbmpxx(bm_12025_w, 246+120, 66);
//	if(measCnt == 0){
		if(!errorSwitch || measCnt == 0)sprintf(charbuff, "%4d", temp_seqno[measCnt]);
		else sprintf(charbuff, "%4d", temp_seqno[measCnt]-1);

//	}else{
//		sprintf(charbuff, "%4d", temp_seqno[measCnt]-1);
//	}
	fill_zero(charbuff);
	if(!fullFlag){
		out_line_16(371, 70, charbuff, BLK_LCD);
	}else{
		out_line_16(371, 70, charbuff, RED_LCD);
	}
}
/************************************************************************/
/************************************************************************/
void dsp_Error(void){
	key_buzzerF();
	tch_error();
	if(modex == MODE_TEST || modex == MODE_QC)dsp_seq();
	switch(errorStack[0]){
		case Err_PlateOut:
			if(temp_modex2[0]<MODE_TEST){
				LCDbmpxx(bm_popup1_top, 250, 185);
				LCDbmpxx(bm_popup1_mid, 250, 185+50);
				LCDbmpxx(bm_popup2_bottom, 250, 185+50+70);
			}else{
				LCDbmpxx(bm_popup4_top, 250, 185);
				LCDbmpxx(bm_popup4_mid, 250, 185+50);
				LCDbmpxx(bm_popup2_bottom, 250, 185+50+70);
			}
			noStripFlag = 0;
			if(modex < MODE_TEST)		modex = MODE_MEAS_Error;
			else if(modex < MODE_QC)	modex = MODE_TEST_Error;
			else 						modex = MODE_QC_Error;
		break;
		case Err_WasteOut:
			LCDbmpxx(bm_popup2_top, 250, 185);
 			LCDbmpxx(bm_popup2_mid, 250, 185+50);
 			LCDbmpxx(bm_popup2_bottom, 250, 185+50+70);
			if(modex < MODE_TEST)		modex = MODE_MEAS_Error;
			else if(modex < MODE_QC)	modex = MODE_TEST_Error;
			else 						modex = MODE_QC_Error;
		break;
		case Err_WasteFull:
			LCDbmpxx(bm_popup3_top, 250, 185);
 			LCDbmpxx(bm_popup3_mid, 250, 185+50);
 			LCDbmpxx(bm_popup2_bottom, 250, 185+50+70);
			if(modex < MODE_TEST)		modex = MODE_MEAS_Error;
			else if(modex < MODE_QC)	modex = MODE_TEST_Error;
			else 						modex = MODE_QC_Error;
		break;
		case Err_MemoryFull:
			LCDbmpxx(bm_popup5_top, 250, 185);
 			LCDbmpxx(bm_popup5_mid, 250, 185+50);
 			LCDbmpxx(bm_popup3_bottom, 250, 185+50+70);
			if(modex < MODE_TEST)		modex = MODE_MEAS_Error2;
			else if(modex < MODE_QC)	modex = MODE_TEST_Error2;
			else 						modex = MODE_QC_Error2;
		break;
		case Err_NoStrip:
			LCDbmpxx(bm_popup6_top, 250, 185);
 			LCDbmpxx(bm_popup6_mid, 250, 185+50);
 			LCDbmpxx(bm_popup2_bottom, 250, 185+50+70);
			if(modex < MODE_TEST)		modex = MODE_MEAS_Error;
			else if(modex < MODE_QC)	modex = MODE_TEST_Error;
			else 						modex = MODE_QC_Error;
		break;
	}
	errorSwitch = 0;
}
/************************************************************************/

