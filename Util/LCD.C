#define  _LCD_C_

#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include "common\32f429_lcd.h"
#include "common\32f429_port.h"
#include "util\usart.h"
#include "util\lcd.h"
#include "util\flash.h"
#include "resm.h"
#include "eep.h"
#include "..\dsp.h"

#define OFF_Y3	800*3			//Lcd Horizontal SiZe	`	
extern const unsigned char _acTST1[ ];

u32 bin2bcd(u32 bin) {
	u32 bcd;
	bin %= 1000000;
	bcd = (bin / 100000);			bin %= 100000;
	bcd = (bcd << 4) + (bin / 10000);	bin %= 10000;
	bcd = (bcd << 4) + (bin / 1000);	bin %= 1000;
	bcd = (bcd << 4) + (bin / 100);	bin %= 100;
	bcd = (bcd << 4) + (bin / 10);	bin %= 10;
	bcd = (bcd << 4) + bin;
	return(bcd);
}
u32 bin2bcdasc(u32 bin) {
	u16 i;
	i = bin2bcd(bin);
	bin = ((i >> 12) & 15) + '0';
	bin = (bin << 8) + ((i >> 8) & 15) + '0';
	bin = (bin << 8) + ((i >> 4) & 15) + '0';
	bin = (bin << 8) + ((i >> 0) & 15) + '0';
	return(bin);
}

u32 bin2bcd6(u32 bin);

/********************************************************************/
void set_lcd_buff(u32 addr){
	LTDC_Layer_InitTypeDef LTDC_Layer_InitStruct; 
//	LTDC_Layer2->CFBAR = LcdBaseAdr0;
	LcdBuffer = addr;
	LTDC_Layer1->CFBAR = addr;
	LTDC_LayerInit(LTDC_Layer2, &LTDC_Layer_InitStruct);
	LTDC_ReloadConfig(LTDC_IMReload);
}

/********************************************************************/
void init_lcd(void){	
	printf0("\r LCD init");	
  /* LCD initialization */
	LCD_Init();
  /* LCD Layer initialization */
	LCD_LayerInit();
//	LcdBuffer = LcdBaseAdr0+ BUFFER_OFFSET;
//	printf0("\r LCD Layer init.(current Layer=0x%x)", LcdBuffer);	
  /* Enable the LTDC */
	LTDC_Cmd(ENABLE);
//	LCD_SetLayer(LCD_FOREGROUND_LAYER);
	LCD_SetLayer(LCD_BACKGROUND_LAYER);
//	printf0("\r LCD Set Layer");
//	LCD_Clear(RED_LCD);
	LCD_Clear(BLU_LCD);
	LCD_ON();
//	LCD_SetFont(&Font16x24);
//	LCD_DisplayStringLine(LINE(6), (uint8_t*)"         WaveShare Open4X9I-C 7inch LCD"); 
//	LCD_DisplayStringLine(LINE(8), (uint8_t*)"          EndiTech Co,Ltd. 0123456789"); 
////	LCD_BK_ON();
}

//---------------------------------------------------------------------------------------------
void copy_lcdbase(void){
//register u16 *pos, *dst;
//register u32 i;
//   pos = (u16*)(LcdBaseAdr0);
//    dst = (u16*)(LcdBaseAdr0);
//    for(i=0;i<(BMP_BLOCK_SIZE/2);i++) *(dst++) = *(pos++);
}
//---------------------------------------------------------------------------------------------
/********************************************************************/
void test_lcd(void){
u8 color=0xff;
	while(1)
	{
//  printf0("**%2x\n",color);
  color=0xff;
//		LCDbmp01();	
		out_line_32(0, 50,"�ѱ۽�����-!@#$%^&*", color);
		out_line_32(0, 100,"ABCDEFGHIJKLMNOPQRSTUVW", 0xee);
		out_line_32(0, 150,"ū����:32X32 1234567890", color);
		out_line_32(0, 200,"abcdefghijklmnopqrstuvwxyz", 0xcc);
  color=0x04;
//		LCDbmp02();	
		out_line_24(0, 50,"[INGBIO] �츰 �������� 1234567890", color);
		out_line_24(0, 100,"�������� : 24X24 �̾", 0x80);
		out_line_24(0, 150,"ABCDEFGHIJKLMNOPQRSTUVWXYZ", 0xd0);
		out_line_24(0, 200,"abcdefghijklmnopqrstuvwxyz", 0x80);
	}
}

/********************************************************************/
void msg24(u16 x, u16 y, u32 color, char *fmt,...)
{
va_list ap;
char string[256];
    va_start(ap,fmt);
    vsprintf(string,fmt,ap);
	out_line_24(x, y, string, color);
	va_end(ap);
}
/********************************************************************/
/********************************************************************/
void msg32(u16 x, u16 y, u32 color, char *fmt,...)
{
va_list ap;
char string[256];
    va_start(ap,fmt);
    vsprintf(string,fmt,ap);
	out_line_32(x, y, string, color);
	va_end(ap);
}

/********************************************************************/
void msg16(u16 x, u16 y, u32 color, char *fmt,...)
{
va_list ap;
char string[256];
    va_start(ap,fmt);
    vsprintf(string,fmt,ap);
	out_line_16(x, y, string, color);
	va_end(ap);
}

/********************************************************************/
void cursor_24(u16 xs, u16 ys, u16 cr, char *str){
}

/********************************************************************/
void cursor_20(u16 xs, u16 ys, u16 cr, char *str){
u16 x, y;
u8 *pos;
	y=0;
	xs -= XS_OFFSET;
	for(x=0;x<cr;x++) y += 11;
	xs=271-(xs+8+y);
	ys += 22;
	for(x=xs;x<(xs+9);x++){
	    pos = (u8*)LcdBaseAdr0 + (x*OFF_Y3) + ys;
		for(y=ys;y<(ys+2);y++)	*(pos++) = BLK_LCD;
	}	
}

/********************************************************************/
void cursor_16(u16 xs, u16 ys, u16 cr, char *str){
u16 x, y;
u8 *pos;
	y=0;
	xs -= XS_OFFSET;
	for(x=0;x<cr;x++) y += 8;
	xs=271-(xs+8+y);
	ys += 22;
	for(x=xs;x<(xs+7);x++){
	    pos = (u8*)LcdBaseAdr0 + (x*OFF_Y3) + ys;
		for(y=ys;y<(ys+2);y++)	*(pos++) = BLK_LCD;
	}	
}

/********************************************************************/
//	LCDbmpxx(�׸���ȣ,Xoffset,Yoffset);					
void LCDbmpxx(u32 pnt, u16 xs, u16 ys){
u32 index, CFB, bmpp;
u16 xx, yy, y1, x1;
u8  pic;
//pnt=0;
	pnt *= 8;
	if(BMP_TB[pnt]!='K'){ prt_msg("\r No BMP File! : %d", pnt/8); return; }
	
	bmpp = BMP_TB[pnt+1];
	bmpp = (bmpp*0x100) + BMP_TB[pnt+2];
	bmpp = (bmpp*0x100) + BMP_TB[pnt+3];
//	printf0("\r %lx,", bmpp);
	
	xx = BMP_TB[pnt+4];
	xx = (xx*0x100) + BMP_TB[pnt+5];
	yy = BMP_TB[pnt+6];
	yy = (yy*0x100) + BMP_TB[pnt+7];
//	printf0("\t x:%d-y:%d", xx, yy);

	for(y1=yy;y1>0;y1--){
		index = (((ys+y1-1)*800) + xs) * 3;
//		CFB = LcdBaseAdr0 + BUFFER_OFFSET + index;
		CFB = LcdBaseAdr1 + index;
		for(x1=0;x1<(xx*3);x1+=3){
			pic = *(u8*)(BMP_BASE+bmpp++);				//printf0(" %2x", pic);
			*(__IO uint8_t*)(CFB + x1 + 0) = pic;		
			pic = *(u8*)(BMP_BASE+bmpp++);				//printf0(" %2x", pic);
			*(__IO uint8_t*)(CFB + x1 + 1) = pic;		
			pic = *(u8*)(BMP_BASE+bmpp++);				//printf0(" %2x", pic);
			*(__IO uint8_t*)(CFB + x1 + 2) = pic;		
		}
	}	
}
/********************************************************************/
#define MEM_CURSOR_SEQ_Y	100+16
#define MEM_CURSOR_SEQ_X	90+130
extern char buff_seqno[16];
void dsp_cursor_mem_seq(u16 cursor){
u32 CFB;
u16 x1, y1, xoff=0;
	for(x1=0;x1<cursor;x1++) xoff += (size16[buff_seqno[x1]]+2);
	for(y1=0;y1<3;y1++){
		CFB = LcdBaseAdr1 + (((MEM_CURSOR_SEQ_Y+y1)*800)+MEM_CURSOR_SEQ_X+xoff)*3;
		for(x1=0;x1<(10*3);x1++) *(__IO uint8_t*)(CFB + x1) = 0;		
	}
}
/********************************************************************/
#define MEM_CURSOR_MEM_DATE_Y	70+16
#define MEM_CURSOR_MEM_DATE_X	220
extern char buff_date[16];
void dsp_cursor_mem_date2(u16 cursor) {
	u32 CFB;
	u16 x1, y1, xoff = 0;
	for (x1 = 0; x1 < cursor; x1++) xoff += (size16[buff_date[x1]] + 2);
	for (y1 = 0; y1 < 3; y1++) {
		CFB = LcdBaseAdr1 + (((MEM_CURSOR_MEM_DATE_Y + y1) * 800) + MEM_CURSOR_MEM_DATE_X + xoff) * 3;
		for (x1 = 0; x1 < (10 * 3); x1++) *(__IO uint8_t*)(CFB + x1) = 0;
	}
}

/********************************************************************/
#define MEM_CURSOR_DATE_Y	 78+16
#define MEM_CURSOR_DATE_X	491//+20

void dsp_cursor_mem_date(u16 cursor) {
	u32 CFB;
	u16 x1, y1, xoff = 0;
	for (x1 = 0; x1 < cursor; x1++) xoff += (size16[buff_date[x1]] + 2);
	for (y1 = 0; y1 < 3; y1++) {
		CFB = LcdBaseAdr1 + (((MEM_CURSOR_DATE_Y + y1) * 800) + MEM_CURSOR_DATE_X + xoff) * 3;
		for (x1 = 0; x1 < (10 * 3); x1++) *(__IO uint8_t*)(CFB + x1) = 0;
	}
}
/********************************************************************/
#define MEM_CURSOR_TIME_Y	 78+16
#define MEM_CURSOR_TIME_X	490//+20
extern char buff_time[16];
void dsp_cursor_mem_time(u16 cursor) {
	u32 CFB;
	u16 x1, y1, xoff = 0;
	for (x1 = 0; x1 < cursor; x1++) xoff += (size16[buff_time[x1]] + 2);
	for (y1 = 0; y1 < 3; y1++) {
		CFB = LcdBaseAdr1 + (((MEM_CURSOR_TIME_Y + y1) * 800) + MEM_CURSOR_TIME_X + xoff) * 3;
		for (x1 = 0; x1 < (10 * 3); x1++) *(__IO uint8_t*)(CFB + x1) = 0;
	}
}
/********************************************************************/
//								LCDbmpxx(bm_sampleid_w, 90,134);
#define MEM_CURSOR_SID_Y	160+ 16
#define MEM_CURSOR_SID_X	 90+130
extern char buff_user_id[18+3];
//extern char buff_user_id[16];
void dsp_cursor_mem_sid(u16 cursor){
u32 CFB;
u16 x1, y1, xoff=0;
	for(x1=0;x1<cursor;x1++) xoff += (size16[buff_user_id[x1]]+2);
	for(y1=0;y1<3;y1++){
		CFB = LcdBaseAdr1 + (((MEM_CURSOR_SID_Y+y1)*800)+MEM_CURSOR_SID_X+xoff)*3;
		for(x1=0;x1<(10*3);x1++) *(__IO uint8_t*)(CFB + x1) = 0;		
	}
}
/********************************************************************/
//								LCDbmpxx(bm_operater_g,462, 134);
#define MEM_CURSOR_OID_Y	130+16
#define MEM_CURSOR_OID_X	90+130
extern char buff_oper_id[16];
void dsp_cursor_mem_oid(u16 cursor){
u32 CFB;
u16 x1, y1, xoff=0;
	for(x1=0;x1<cursor;x1++) xoff += (size16[buff_oper_id[x1]]+2);
	for(y1=0;y1<3;y1++){
		CFB = LcdBaseAdr1 + (((MEM_CURSOR_OID_Y+y1)*800)+MEM_CURSOR_OID_X+xoff)*3;
		for(x1=0;x1<(10*3);x1++) *(__IO uint8_t*)(CFB + x1) = 0;		
	}
}
/********************************************************************/
//								LCDbmpxx(bm_operater_g,462, 134);
#define MEM_CURSOR_OPER_Y	130
#define MEM_CURSOR_OPER_X	246
extern char buff_oper_id[16];
void dsp_cursor_oper(u16 cursor){
u32 CFB;
u16 x1, y1, xoff=0;
	for(x1=0;x1<cursor;x1++) xoff += (size16[buff_oper_id[x1]]+2);
	for(y1=0;y1<3;y1++){
		CFB = LcdBaseAdr1 + (((MEM_CURSOR_OPER_Y+y1)*800)+MEM_CURSOR_OPER_X+xoff)*3;
		for(x1=0;x1<(10*3);x1++) *(__IO uint8_t*)(CFB + x1) = 0;		
	}
}
/********************************************************************/
//								LCDbmpxx(bm_operater_g,462, 134);
#define MEM_PASSWORD_Y	100
#define MEM_PASSWORD_X	355
extern char password[5];
void dsp_cursor_password(u16 cursor){												//폰트 : 32
u32 CFB;
u16 x1, y1, xoff=0;
	for(x1=0;x1<cursor;x1++) xoff += (size32[password[x1]]+2);					//size16 => size32
	for(y1=0;y1<3;y1++){
		CFB = LcdBaseAdr1 + (((MEM_PASSWORD_Y+y1)*800)+MEM_PASSWORD_X+xoff)*3;	
		for(x1=0;x1<(10*3*2);x1++) *(__IO uint8_t*)(CFB + x1) = 0;					//(10*3) => (10*3*2)
	}
}
/********************************************************************/
extern u16	cursor_set;
void clr_cursor_mem_set(void){
u32 CFB;
u16 x1, y1;
u8	color;
	for(y1=0;y1<3;y1++){
		CFB = LcdBaseAdr1 + (((MEM_CURSOR_SEQ_Y+y1)*800)+MEM_CURSOR_SEQ_X)*3;
		color = *(__IO uint8_t*)(CFB - 1);
		for(x1=0;x1<(10*3*10);x1++) *(__IO uint8_t*)(CFB + x1 ) = color;		
	}
	for(y1=0;y1<3;y1++){
		CFB = LcdBaseAdr1 + (((MEM_CURSOR_SID_Y+y1)*800)+MEM_CURSOR_SID_X)*3;
		color = *(__IO uint8_t*)(CFB - 1);
		for(x1=0;x1<(10*3*20);x1++) *(__IO uint8_t*)(CFB + x1 ) = color;	//MAX_USERID=18
	}

	for(y1=0;y1<3;y1++){
		CFB = LcdBaseAdr1 + (((MEM_CURSOR_OID_Y+y1)*800)+MEM_CURSOR_OID_X)*3;
		color = *(__IO uint8_t*)(CFB - 1);
		for(x1=0;x1<(10*3*11);x1++) *(__IO uint8_t*)(CFB + x1 ) = color;	//MAX_OPERID=10		
	}
}
/********************************************************************/
#define TEST_CURSOR_SEQ_Y	 66+ 20
#define TEST_CURSOR_SEQ_X	118+133
extern char buff_seqno[16];
void dsp_cursor_test_seq(u16 cursor){
u32 CFB;
u16 x1, y1, xoff=0;
	for(x1=0;x1<cursor;x1++) xoff += (size16[buff_seqno[x1]]+2);
	for(y1=0;y1<3;y1++){
		CFB = LcdBaseAdr1 + (((TEST_CURSOR_SEQ_Y+y1)*800)+TEST_CURSOR_SEQ_X+xoff)*3;
		for(x1=0;x1<(10*3);x1++) *(__IO uint8_t*)(CFB + x1) = 0;		
	}
}
/********************************************************************/
#define TEST_CURSOR_SID_Y	146+ 20
#define TEST_CURSOR_SID_X	118+133
void dsp_cursor_test_sid(u16 cursor){
u32 CFB;
u16 x1, y1, xoff=0;
	for(x1=0;x1<cursor;x1++) xoff += (size16[tempSampleID2[tempMeasCnt][x1]]+2);
	for(y1=0;y1<3;y1++){
		CFB = LcdBaseAdr1 + (((TEST_CURSOR_SID_Y+y1)*800)+TEST_CURSOR_SID_X+xoff)*3;
		for(x1=0;x1<(10*3);x1++) *(__IO uint8_t*)(CFB + x1) = 0;		
	}
}
/********************************************************************/
void clr_cursor_test_set(void){
u32 CFB;
u16 x1, y1;
u8	color;
	for(y1=0;y1<3;y1++){
		CFB = LcdBaseAdr1 + (((TEST_CURSOR_SEQ_Y+y1)*800)+TEST_CURSOR_SEQ_X)*3;
		color = *(__IO uint8_t*)(CFB - 1);
		for(x1=0;x1<(10*3*10);x1++) *(__IO uint8_t*)(CFB + x1 ) = color;		
	}
	for(y1=0;y1<3;y1++){
		CFB = LcdBaseAdr1 + (((TEST_CURSOR_SID_Y+y1)*800)+TEST_CURSOR_SID_X)*3;
		color = *(__IO uint8_t*)(CFB - 1);
		for(x1=0;x1<(10*3*20);x1++) *(__IO uint8_t*)(CFB + x1 ) = color;	//MAX_USERID=18
	}
}
/********************************************************************/
#define TEST_CURSOR_QC_SEQ_Y	TEST_CURSOR_SEQ_Y
#define TEST_CURSOR_QC_SEQ_X	TEST_CURSOR_SEQ_X
void dsp_cursor_qc_seq(u16 cursor){
u32 CFB;
u16 x1, y1, xoff=0;
	for(x1=0;x1<cursor;x1++) xoff += (size16[buff_seqno[x1]]+2);
	for(y1=0;y1<3;y1++){
		CFB = LcdBaseAdr1 + (((TEST_CURSOR_QC_SEQ_Y+y1)*800)+TEST_CURSOR_QC_SEQ_X+xoff)*3;
		for(x1=0;x1<(10*3);x1++) *(__IO uint8_t*)(CFB + x1) = 0;		
	}
}
/********************************************************************/
#define TEST_CURSOR_QC_LOT_Y	66+20
#define TEST_CURSOR_QC_LOT_X	530-100+133
void dsp_cursor_qc_lot(u16 cursor){
u32 CFB;
u16 x1, y1, xoff=0;
	for(x1=0;x1<cursor;x1++) xoff += (size16[temp_lotno2[tempMeasCnt][x1]]+2);
	for(y1=0;y1<3;y1++){
		CFB = LcdBaseAdr1 + (((TEST_CURSOR_QC_LOT_Y+y1)*800)+TEST_CURSOR_QC_LOT_X+xoff)*3;
		for(x1=0;x1<(10*3);x1++) *(__IO uint8_t*)(CFB + x1) = 0;		
	}
}
/********************************************************************/
void clr_cursor_qc_set(void){
u32 CFB;
u16 x1, y1;
u8	color;
	for(y1=0;y1<3;y1++){
		CFB = LcdBaseAdr1 + (((TEST_CURSOR_SEQ_Y+y1)*800)+TEST_CURSOR_SEQ_X)*3;
		color = *(__IO uint8_t*)(CFB - 1);
		for(x1=0;x1<(10*3*10);x1++) *(__IO uint8_t*)(CFB + x1 ) = color;		
	}
	for(y1=0;y1<3;y1++){
		CFB = LcdBaseAdr1 + (((TEST_CURSOR_QC_LOT_Y+y1)*800)+TEST_CURSOR_QC_LOT_X)*3;
		color = *(__IO uint8_t*)(CFB - 1);
		for(x1=0;x1<(10*3*8);x1++) *(__IO uint8_t*)(CFB + x1 ) = color;	//MAX_USERID=18
	}
}
/********************************************************************/
void clr_screen_box(u16 xp, u16 yp, u16 xsize, u16 ysize){
u32 CFB, index;
u16 x1, y1;
u8	color;
	index = ((yp*800) + xp) * 3;
	color = *(__IO uint8_t*) (LcdBaseAdr1 + index);
	for(y1=0;y1<ysize;y1++){
		index = (((yp+y1)*800) + xp) * 3;
		CFB = LcdBaseAdr1 + index;
		for(x1=0;x1<(xsize*3);x1++)	*(__IO uint8_t*)(CFB + x1) = color;		
	}

}
/********************************************************************/
//u32 CFB0;
//	for(y1=yy;y1>0;y1--){
//		index = (((ys+y1-1)*800) + xs) * 3;
//		CFB0 = LcdBaseAdr0 + index;
//		CFB = LcdBaseAdr0 + BUFFER_OFFSET + index;
//		for(x1=0;x1<(xx*3);x1+=3){
//			pic = *(u8*)(BMP_BASE+bmpp++);				//printf0(" %2x", pic);
//			*(__IO uint8_t*)(CFB + x1 + 0) = pic;		
//			*(__IO uint8_t*)(CFB0 + x1 + 0) = pic;		
//			pic = *(u8*)(BMP_BASE+bmpp++);				//printf0(" %2x", pic);
//			*(__IO uint8_t*)(CFB + x1 + 1) = pic;		
//			*(__IO uint8_t*)(CFB0 + x1 + 1) = pic;		
//			pic = *(u8*)(BMP_BASE+bmpp++);				//printf0(" %2x", pic);
//			*(__IO uint8_t*)(CFB + x1 + 2) = pic;		
//			*(__IO uint8_t*)(CFB0 + x1 + 2) = pic;		
//		}
//	}	

/********************************************************************/

void init_fonts(void){
u8 *fp;
short i;
	fp = (u8*) (FONT_BASE+0x20);
	ENG16ADR = 0;	for(i=0;i<4;i++) ENG16ADR = (ENG16ADR<<8) + *(fp++);
	ENG24ADR = 0;	for(i=0;i<4;i++) ENG24ADR = (ENG24ADR<<8) + *(fp++);
	ENG32ADR = 0;	for(i=0;i<4;i++) ENG32ADR = (ENG32ADR<<8) + *(fp++);
	ENG20ADR = 0;	for(i=0;i<4;i++) ENG20ADR = (ENG20ADR<<8) + *(fp++);
	HAN16ADR = 0;	for(i=0;i<4;i++) HAN16ADR = (HAN16ADR<<8) + *(fp++);
	HAN24ADR = 0;	for(i=0;i<4;i++) HAN24ADR = (HAN24ADR<<8) + *(fp++);
	HAN32ADR = 0;	for(i=0;i<4;i++) HAN32ADR = (HAN32ADR<<8) + *(fp++);
	FIG32ADR = 0;	for(i=0;i<4;i++) FIG32ADR = (FIG32ADR<<8) + *(fp++);
//	printf0("\r E16:%lx,E24:%lx,E32:%lx,E20:%lx",ENG16ADR,ENG24ADR,ENG32ADR,ENG20ADR);
	ENG16ADR += FONT_BASE;
	ENG24ADR += FONT_BASE;
	ENG32ADR += FONT_BASE;
	ENG20ADR += FONT_BASE;
	HAN16ADR += FONT_BASE;
	HAN24ADR += FONT_BASE;
	HAN32ADR += FONT_BASE;
	FIG32ADR += FONT_BASE;
	set_size24();
	set_size32();
}

/********************************************************************/
void init_bmps(void){
u8 *fp;
short i;
	fp = (u8*) (BMP_BASE+0);
	for(i=0;i<0x600;i++) bmp_head[i]= *(fp++);
}

/********************************************************************/
#define MIN_SIZE24	12
#define MIN_SIZE32	14
//24X24 ���� Font�� �о �������� �˾Ƶд�.
void set_size24(void){
unsigned short i, j, a, b;
unsigned int k;
unsigned char *fp;

	for(j=0;j<96;j++){
		fp = (unsigned char*) ENG24ADR+(j*24*3);
		b=0;
		for(i=0;i<24;i++){
			k = *(fp++);
			k = (k<<8) + *(fp++);
			k = (k<<8) + *(fp++);
			for(a=0;a<24;a++){
				if(k&0x00800000){ if(a>b) b=a; }
				k = k<<1;
			}
		}
		b += 2;
		if(b>=MIN_SIZE24) size24[j+0x20]=b;
		else size24[j+0x20]=MIN_SIZE24;
	}
}

/********************************************************************/
//32X32 ���� Font�� �о �������� �˾Ƶд�.
void set_size32(void){
unsigned short i, j, a, b;
unsigned int k;
unsigned char *fp;

	for(j=0;j<96;j++){
		fp = (unsigned char*) ENG32ADR+(j*32*4);
		b=0;
		for(i=0;i<32;i++){
			k = *(fp++);
			k = (k<<8) + *(fp++);
			k = (k<<8) + *(fp++);
			k = (k<<8) + *(fp++);
			for(a=0;a<32;a++){
				if(k&0x80000000){ if(a>b) b=a; }
				k = k<<1;
			}
		}
		b += 2;
		if(b>=MIN_SIZE32) size32[j+0x20]=b;
		else size32[j+0x20]=MIN_SIZE32;
	}
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
void draw_box(u16 xs, u16 ys, u16 xe, u16 ye, u32 color){
u8 	bb, rr, gg;
u16 x, x3;
u32 CFB, y;
	bb = color>>16;
	rr = color>>8;
	gg = color>>0;
	for(y=ys;y<ye;y++){
		CFB = LcdBaseAdr0 + BUFFER_OFFSET + ((y*800) * 3);
		for(x=xs;x<xe;x++){
			x3 = x * 3;
			*(__IO uint8_t*)(CFB + x3 + 0) = bb;		
			*(__IO uint8_t*)(CFB + x3 + 1) = rr;		
			*(__IO uint8_t*)(CFB + x3 + 2) = gg;		
		}
	}	
}

/********************************************************************/
void erase_box(u16 xs, u16 ys, u16 xe, u16 ye){
u16 x, y;
u8 *pos, *pos_b;
	for(y=ys;y<ye;y++){
    	pos = (u8*)LcdBaseAdr1 + (y*OFF_Y3) + (xs*3);
    	pos_b = (u8*)LcdBaseAdr0 + (x*OFF_Y3) + (xs*3);
		x = (xe-xs)*3;
		do{
			*(pos++) = *(pos_b++);
			x--;
		}while(x);
	}	
}
/********************************************************************/
void erase_box0(u16 xs, u16 ys, u16 xe, u16 ye){
u8 	bb, rr, gg;
u16 x, x3;
u32 CFB, y;
	CFB = LcdBaseAdr0 + BUFFER_OFFSET + (ys*800*3) + (xs*3);
	bb = *(__IO uint8_t*)(CFB + 0);
	rr = *(__IO uint8_t*)(CFB + 0);
	gg = *(__IO uint8_t*)(CFB + 0);
//	bb=rr=gg=0;
	for(y=ys;y<ye;y++){
		CFB = LcdBaseAdr0 + BUFFER_OFFSET + (y*800*3);
		for(x=xs;x<xe;x++){
			x3 = x * 3;
			*(__IO uint8_t*)(CFB + x3 + 0) = bb;		
			*(__IO uint8_t*)(CFB + x3 + 1) = rr;		
			*(__IO uint8_t*)(CFB + x3 + 2) = gg;		
		}
	}	
}
/******************************************************************************/
//   			x-pos	y-pos	length	width	color	
void line_x(u16 xs, u16 ys, u16 length, u16 width, u32 color){
u8 	bb, rr, gg;
u16 x, x3;
u32 CFB, y;
	bb = color>>16;
	rr = color>>8;
	gg = color>>0;
	for(y=ys;y<(ys+width);y++){
		CFB = LcdBaseAdr1 + (y*OFF_Y3);
		for(x=xs;x<(length+xs);x++){
			x3 = x * 3;
			*(__IO uint8_t*)(CFB + x3 + 0) = bb;		
			*(__IO uint8_t*)(CFB + x3 + 1) = rr;		
			*(__IO uint8_t*)(CFB + x3 + 2) = gg;		
		}
	}	
}

/******************************************************************************/
void line_y(u16 xs, u16 ys, u16 length, u16 width, u32 color){
u8 	bb, rr, gg;
u16 x, x3;
u32 CFB, y;
	bb = color>>16;
	rr = color>>8;
	gg = color>>0;
	for(y=ys;y<(ys+length);y++){
		CFB = LcdBaseAdr1 + (y*OFF_Y3);
		for(x=xs;x<(width+xs);x++){
			x3 = x * 3;
			*(__IO uint8_t*)(CFB + x3 + 0) = bb;		
			*(__IO uint8_t*)(CFB + x3 + 1) = rr;		
			*(__IO uint8_t*)(CFB + x3 + 2) = gg;		
		}
	}	
}

/******************************************************************************/
void line_box(u16 xs, u16 ys, u16 xe, u16 ye, u16 width, u32 color){
u8 	bb, rr, gg;
u16 x, x3;
u32 CFB, y;
	bb = color>>16;
	rr = color>>8;
	gg = color>>0;
	for(y=ys;y<(ys+width);y++){				//x1
		CFB = LcdBaseAdr1 + (y*OFF_Y3);
		for(x=xs;x<xe;x++){
			x3 = x * 3;
			*(__IO uint8_t*)(CFB + x3 + 0) = bb;		
			*(__IO uint8_t*)(CFB + x3 + 1) = rr;		
			*(__IO uint8_t*)(CFB + x3 + 2) = gg;		
		}
	}	
	for(y=ye;y>=(ye-width);y--){				//x2
		CFB = LcdBaseAdr1 + (y*OFF_Y3);
		for(x=xs;x<xe;x++){
			x3 = x * 3;
			*(__IO uint8_t*)(CFB + x3 + 0) = bb;		
			*(__IO uint8_t*)(CFB + x3 + 1) = rr;		
			*(__IO uint8_t*)(CFB + x3 + 2) = gg;		
		}
	}	
	for(x=xs;x<(xs+width);x++){				//y1
		x3 = x * 3;
		CFB = LcdBaseAdr1 + (ys*OFF_Y3);
		for(y=ys;y<ye;y++){
			*(__IO uint8_t*)(CFB + x3 + 0) = bb;		
			*(__IO uint8_t*)(CFB + x3 + 1) = rr;		
			*(__IO uint8_t*)(CFB + x3 + 2) = gg;		
			CFB += OFF_Y3;
		}
	}	
	for(x=xe;x>=(xe-width);x--){				//y2
		x3 = x * 3;
		CFB = LcdBaseAdr1 + (ys*OFF_Y3);
		for(y=ys;y<ye;y++){
			*(__IO uint8_t*)(CFB + x3 + 0) = bb;		
			*(__IO uint8_t*)(CFB + x3 + 1) = rr;		
			*(__IO uint8_t*)(CFB + x3 + 2) = gg;		
			CFB += OFF_Y3;
		}
	}	
}

/********************************************************************/
/********************************************************************/
u16 hexasc2bin(u32 asc){
u16 i, j;
	j = ((asc>>24)&0xff);	if(j>'9') j-=('A'-10); else j-='0';	i = j;
	j = ((asc>>16)&0xff);	if(j>'9') j-=('A'-10); else j-='0';	i = (i<<4) + j;
	j = ((asc>>8)&0xff);	if(j>'9') j-=('A'-10); else j-='0';	i = (i<<4) + j;
	j = ((asc>>0)&0xff);	if(j>'9') j-=('A'-10); else j-='0';	i = (i<<4) + j;
	return(i);
}

/********************************************************************/
u16 bcd2bin(u32 bcd){
u16 i;
	i = ((bcd>>24)&0xff);
	i = (i*10) + ((bcd>>16)&0xff);
	i = (i*10) + ((bcd>>8)&0xff);
	i = (i*10) + ((bcd>>0)&0xff);
	return(i);
}

/********************************************************************/
u8 bin2hexasc(u8 bin){
	if(bin>9) return((bin-10)+'A');
	else return(bin+'0');
}	

/********************************************************************/
u32 bin2hexasc4(u32 bin){
u16 i;
	i = bin;
	bin = bin2hexasc((i>>12)&15); 
	bin = (bin<<8) + bin2hexasc((i>>8)&15); 
	bin = (bin<<8) + bin2hexasc((i>>4)&15); 
	bin = (bin<<8) + bin2hexasc((i>>0)&15);
	return(bin);
}
/********************************************************************/
u8 asc2bin_1(u8 asc){
	if((asc>='0')&&(asc<='9')) asc -= '0';
//	else if((asc>='A')&&(asc<'F')) asc = (asc-'A')+10;
	else asc=0;
	return(asc);
}

/********************************************************************/
u16 asc2bin4(u32 asc4){
u16 i;
	i =          asc2bin_1(((asc4>>24)&0xff)); 
	i = (i*10) + asc2bin_1(((asc4>>16)&0xff));
	i = (i*10) + asc2bin_1(((asc4>> 8)&0xff));
	i = (i*10) + asc2bin_1(((asc4>> 0)&0xff));
	return(i);
}
/********************************************************************/
u32 bin2bcd6(u32 bin){
u32 bcd;
	bin %= 1000000;
	bcd = (bin/100000);			bin %= 100000;
	bcd = (bcd<<4)+(bin/10000);	bin %= 10000;
	bcd = (bcd<<4)+(bin/1000);	bin %= 1000;
	bcd = (bcd<<4)+(bin/100);	bin %= 100;
	bcd = (bcd<<4)+(bin/10);	bin %= 10;
	bcd = (bcd<<4)+ bin;
	return(bcd);
}
/********************************************************************/
u32 bin2bcdasc4(u32 bin){
u16 i;
	i = bin2bcd6(bin);
	bin = ((i>>12)&15)+'0'; 
	bin = (bin<<8) + ((i>>8)&15)+'0'; 
	bin = (bin<<8) + ((i>>4)&15)+'0'; 
	bin = (bin<<8) + ((i>>0)&15)+'0';
	return(bin); 	
}
/******************************************************************************/
u8 hexasc2bin_1(u8 hexasc){
	if(hexasc<='9') return(hexasc-'0');
	else return((hexasc-'A')+10);
}
/********************************************************************/
void time_copy(u8 year, u8 month, u8 day, u8 hour, u8 min, u8 sec){
int i;//, j;	
//year=88; month=8; day=28; hour=8; min=58; sec=58;
	i = bin2bcdasc4(year);
	top_str[0]='2';
	top_str[1]='0';
	top_str[2]=i>>8;
	top_str[3]=i&0xff;
	i = bin2bcdasc4(month);
	top_str[4]='/';
	top_str[5]=i>>8;
	top_str[6]=i&0xff;
	i = bin2bcdasc4(day);
	top_str[7]='/';
	top_str[8]=i>>8;
	top_str[9]=i&0xff;
	top_str[10]=' ';
	if(hour<12){
//		msg24(20,28,0xff,"20%d/%d/%d AM %d:%d:%d", year, month, day, hour, min, sec);
		top_str[11]='A';
		if(!hour) hour=12;
	}else{
//		msg24(20,28,0xff,"20%d/%d/%d PM %d:%d:%d", year, month, day, hour-11, min, sec);
		top_str[11]='P';
		if(hour>12)	hour -= 12;
	}
	i = bin2bcdasc4(hour);
	top_str[12]='M';
	top_str[13]=' ';
	top_str[14]=i>>8;
	top_str[15]=i&0xff;
	i = bin2bcdasc4(min);
	top_str[16]=':';
	top_str[17]=i>>8;
	top_str[18]=i&0xff;
	i = bin2bcdasc4(sec);
	top_str[19]=':';
	top_str[20]=i>>8;
	top_str[21]=i&0xff;
	top_str[22]=' ';
	top_str[23]=0;
//printf0("\r Aa-%d", get_xpos24(top_str, 22));
//printf0("\r Aa-%d", get_xpos32(top_str, 22));
}
/********************************************************************/

/********************************************************************/
u16 get_xpos24(char *str, u16 xn){
u16 i, j;
	j=0;
	for(i=0;i<xn;i++){
		if(str[i]<0x80){
			j += size24[str[i]];
		}else j += 12;					//�ѱ��ΰ��
	}
	return(j);
}

/********************************************************************/
u16 get_xpos32(char *str, u16 xn){
u16 i, j;
	j=0;
	for(i=0;i<xn;i++){
		if(str[i]<0x80){
			j += size32[str[i]];
		}else j += 16;
	}
	return(j);
}

/********************************************************************/
/********************************************************************/

//############################################################################################################
//############################################################################################################
//############################################################################################################

//============================ 64X64 =================================

void out_line_64(int x_pos, int y_pos, char *str, u32 color){
u16 i=0;
int j;
//printf0("\r 64C%6x X%2d,Y%2d", color, x_pos, y_pos);
	while(1){
		j = str[i++]; if(!j) break;
		if(j<0x80){
			x_pos = out_text_64e(x_pos, y_pos, j, color);
//printf0(",%3d  ",x_pos);
		}		
	}	
}

/********************************************************************/
int out_text_64e(int x_pos, int y_pos, int cod, u32 color){
u16 i, j, HS;
u32 pos_d;
long long save64[64];	
long long w64;	

	u8 bb = color>>16;	
	u8 rr = color>>8;	
	u8 gg = color>>0;
	pos_d = ENG64ADR+((cod-0x20)*8*64);
//printf0("\r Pos:%6x,%2d  ",pos_d,HS/2);
	for(i=0;i<64;i++){
		w64 = *(u8*)(FONT_BASE+pos_d); pos_d++;
		w64 = (w64<<8) + *(u8*)(FONT_BASE+pos_d); pos_d++;
		w64 = (w64<<8) + *(u8*)(FONT_BASE+pos_d); pos_d++;
		w64 = (w64<<8) + *(u8*)(FONT_BASE+pos_d); pos_d++;
		w64 = (w64<<8) + *(u8*)(FONT_BASE+pos_d); pos_d++;
		w64 = (w64<<8) + *(u8*)(FONT_BASE+pos_d); pos_d++;
		w64 = (w64<<8) + *(u8*)(FONT_BASE+pos_d); pos_d++;
		w64 = (w64<<8) + *(u8*)(FONT_BASE+pos_d); pos_d++;
		save64[i] = w64;
	}
	pos_d = LcdBaseAdr0 + BUFFER_OFFSET + (y_pos*OFF_Y3) + (x_pos*3);
	HS = size64[cod];
//printf0(" C:%2x-%2d  ",cod,HS);
	for(i=0;i<64;i++){
		w64 = save64[i];
		for(j=0;j<HS;j++){
//			if(w64&0x8000000000000000) *(__IO uint16_t*)(pos_d+j) = color;
			if(w64&0x8000000000000000){
				*(__IO uint8_t*)(pos_d+(j*3)+0) = bb;
				*(__IO uint8_t*)(pos_d+(j*3)+1) = rr;
				*(__IO uint8_t*)(pos_d+(j*3)+2) = gg;
			}
			w64 = w64<<1;
		}
		pos_d += OFF_Y3;
	}
	return(x_pos+HS+2);
}

//============================ 56X56 =================================

void out_line_56(int x_pos, int y_pos, char *str, u32 color){
u16 i=0;
int j;
//printf0("\r 56C%6x X%2d,Y%2d", color, x_pos, y_pos);
	while(1){
		j = str[i++]; if(!j) break;
		if(j<0x80){
			x_pos = out_text_56e(x_pos, y_pos, j, color);
//printf0(",%3d  ",x_pos);
		}		
	}	
}

/********************************************************************/
int out_text_56e(int x_pos, int y_pos, int cod, u32 color){
u16 i, j, HS;
u32 pos_d;
long long save56[56];	
long long w64;	

	u8 bb = color>>16;	
	u8 rr = color>>8;	
	u8 gg = color>>0;
	pos_d = ENG56ADR+((cod-0x20)*7*56);
//printf0("\r Pos:%6x,%2d  ",pos_d,HS/2);
	for(i=0;i<56;i++){
		w64 = *(u8*)(FONT_BASE+pos_d); pos_d++;
		w64 = (w64<<8) + *(u8*)(FONT_BASE+pos_d); pos_d++;
		w64 = (w64<<8) + *(u8*)(FONT_BASE+pos_d); pos_d++;
		w64 = (w64<<8) + *(u8*)(FONT_BASE+pos_d); pos_d++;
		w64 = (w64<<8) + *(u8*)(FONT_BASE+pos_d); pos_d++;
		w64 = (w64<<8) + *(u8*)(FONT_BASE+pos_d); pos_d++;
		w64 = (w64<<8) + *(u8*)(FONT_BASE+pos_d); pos_d++;
		save56[i] = w64;
	}
	pos_d = LcdBaseAdr0 + BUFFER_OFFSET + (y_pos*OFF_Y3) + (x_pos*3);
	HS = size56[cod];
//printf0(" C:%2x-%2d  ",cod,HS);
	for(i=0;i<56;i++){
		w64 = save56[i];
		for(j=0;j<HS;j++){
			if(w64&0x80000000000000){
				*(__IO uint8_t*)(pos_d+(j*3)+0) = bb;
				*(__IO uint8_t*)(pos_d+(j*3)+1) = rr;
				*(__IO uint8_t*)(pos_d+(j*3)+2) = gg;
			}
			w64 = w64<<1;
		}
		pos_d += OFF_Y3;
	}
	return(x_pos+HS+2);
}

//============================ 48X48 =================================

void out_line_48(int x_pos, int y_pos, char *str, u32 color){
u16 i=0;
int j;
	while(1){
		j = str[i++]; if(!j) break;
		if(j<0x80){
			x_pos = out_text_48e(x_pos, y_pos, j, color);
		}		
	}	
}

/********************************************************************/
int out_text_48e(int x_pos, int y_pos, int cod, u32 color){
u16 i, j, HS;
u32 pos_d;
long long save48[48];	
long long w64;	

	u8 bb = color>>16;	
	u8 rr = color>>8;	
	u8 gg = color>>0;
	pos_d = ENG48ADR+((cod-0x20)*6*48);
//printf0("\r Pos:%6x,%2d  ",pos_d,HS/2);
	for(i=0;i<48;i++){
		w64 = *(u8*)(FONT_BASE+pos_d); pos_d++;
		w64 = (w64<<8) + *(u8*)(FONT_BASE+pos_d); pos_d++;
		w64 = (w64<<8) + *(u8*)(FONT_BASE+pos_d); pos_d++;
		w64 = (w64<<8) + *(u8*)(FONT_BASE+pos_d); pos_d++;
		w64 = (w64<<8) + *(u8*)(FONT_BASE+pos_d); pos_d++;
		w64 = (w64<<8) + *(u8*)(FONT_BASE+pos_d); pos_d++;
		save48[i] = w64;
	}
	pos_d = LcdBaseAdr0 + BUFFER_OFFSET + (y_pos*OFF_Y3) + (x_pos*3);
	HS = size48[cod];
	for(i=0;i<48;i++){
		w64 = save48[i];
		for(j=0;j<HS;j++){
			if(w64&0x800000000000){
				*(__IO uint8_t*)(pos_d+(j*3)+0) = bb;
				*(__IO uint8_t*)(pos_d+(j*3)+1) = rr;
				*(__IO uint8_t*)(pos_d+(j*3)+2) = gg;
			}
			w64 = w64<<1;
		}
		pos_d += OFF_Y3;
	}
	return(x_pos+HS+2);
}

//============================ 40X40 =================================

void out_line_40(int x_pos, int y_pos, char *str, u32 color){
u16 i=0;
int j;
	while(1){
		j = str[i++]; if(!j) break;
		if(j<0x80){
			x_pos = out_text_40e(x_pos, y_pos, j, color);
		}		
	}	
}

/********************************************************************/
int out_text_40e(int x_pos, int y_pos, int cod, u32 color){
u16 i, j, HS;
u32 pos_d;
long long save40[40];	
long long w64;	

	u8 bb = color>>16;	
	u8 rr = color>>8;	
	u8 gg = color>>0;
	pos_d = ENG40ADR+((cod-0x20)*5*40);
//printf0("\r Pos:%6x,%2d  ",pos_d,HS/2);
	for(i=0;i<40;i++){
		w64 = *(u8*)(FONT_BASE+pos_d); pos_d++;
		w64 = (w64<<8) + *(u8*)(FONT_BASE+pos_d); pos_d++;
		w64 = (w64<<8) + *(u8*)(FONT_BASE+pos_d); pos_d++;
		w64 = (w64<<8) + *(u8*)(FONT_BASE+pos_d); pos_d++;
		w64 = (w64<<8) + *(u8*)(FONT_BASE+pos_d); pos_d++;
		save40[i] = w64;
	}
	pos_d = LcdBaseAdr0 + BUFFER_OFFSET + (y_pos*OFF_Y3) + (x_pos*3);
	HS = size40[cod]; 
	for(i=0;i<40;i++){
		w64 = save40[i];
		for(j=0;j<HS;j++){
			if(w64&0x8000000000){
				*(__IO uint8_t*)(pos_d+(j*3)+0) = bb;
				*(__IO uint8_t*)(pos_d+(j*3)+1) = rr;
				*(__IO uint8_t*)(pos_d+(j*3)+2) = gg;
			}
			w64 = w64<<1;
		}
		pos_d += OFF_Y3;
	}
	return(x_pos+HS+2);
}

//============================ 32X32 =================================

void out_line_32(int x_pos, int y_pos, char *str, u32 color){
u16 i=0;
int j;
//	x_pos -= XS_OFFSET;
//	x_pos = 272 - x_pos;
	while(1){
		j = str[i++]; if(!j) break;
		if(j<0x80){
			x_pos = out_text_32e(x_pos, y_pos, j, color);
		}		
	}	
}

/********************************************************************/
int out_text_32e(int x_pos, int y_pos, int cod, u32 color){
u16 i, j, HS;
u32 ww, pos_d;
u32 save32[32];	

	u8 bb = color>>16;	
	u8 rr = color>>8;	
	u8 gg = color>>0;
	pos_d = ENG32ADR+((cod-0x20)*4*32);
//printf0("\r Pos:%6x,%2d  ",pos_d,HS/2);
	for(i=0;i<32;i++){
		ww = *(u8*)(FONT_BASE+pos_d); pos_d++;
		ww = (ww<<8) + *(u8*)(FONT_BASE+pos_d); pos_d++;
		ww = (ww<<8) + *(u8*)(FONT_BASE+pos_d); pos_d++;
		ww = (ww<<8) + *(u8*)(FONT_BASE+pos_d); pos_d++;
		save32[i] = ww;
	}
	pos_d = LcdBaseAdr0 + BUFFER_OFFSET + (y_pos*OFF_Y3) + (x_pos*3);
	HS = size32[cod];
	for(i=0;i<32;i++){
		ww = save32[i];
		for(j=0;j<HS;j++){
			if(ww&0x80000000){
				*(__IO uint8_t*)(pos_d+(j*3)+0) = bb;
				*(__IO uint8_t*)(pos_d+(j*3)+1) = rr;
				*(__IO uint8_t*)(pos_d+(j*3)+2) = gg;
			}
			ww = ww<<1;
		}
		pos_d += OFF_Y3;
	}
	return(x_pos+HS+2);
}

//============================ 24X24 =================================

void out_line_24(int x_pos, int y_pos, char *str, u32 color){
u16 i=0;
int j;
//	x_pos -= XS_OFFSET;
//	x_pos = 272 - x_pos;
	while(1){
		j = str[i++]; if(!j) break;
		if(j<0x80){
			x_pos = out_text_24e(x_pos, y_pos, j, color);
		}		
	}	
}

/********************************************************************/
int out_text_24e(int x_pos, int y_pos, int cod, u32 color){
u16 i, j, HS;
u32 ww, pos_d;
u32 save24[24];	

	u8 bb = color>>16;	
	u8 rr = color>>8;	
	u8 gg = color>>0;
	pos_d = ENG24ADR+((cod-0x20)*3*24);
//printf0("\r Pos:%6x,%2d",pos_d,HS/2);
	for(i=0;i<24;i++){
		ww = *(u8*)(FONT_BASE+pos_d); pos_d++;
		ww = (ww<<8) + *(u8*)(FONT_BASE+pos_d); pos_d++;
		ww = (ww<<8) + *(u8*)(FONT_BASE+pos_d); pos_d++;
		save24[i] = ww;
	}
	pos_d = LcdBaseAdr0 + BUFFER_OFFSET + (y_pos*OFF_Y3) + (x_pos*3);
	HS = size24[cod];
	for(i=0;i<24;i++){
		ww = save24[i];
		for(j=0;j<HS;j++){
			if(ww&0x800000){
				*(__IO uint8_t*)(pos_d+(j*3)+0) = bb;
				*(__IO uint8_t*)(pos_d+(j*3)+1) = rr;
				*(__IO uint8_t*)(pos_d+(j*3)+2) = gg;
			}
			ww = ww<<1;
		}
		pos_d += OFF_Y3;
	}
	return(x_pos+HS+2);
}

/********************************************************************/
/******************************************************************************/


//============================== 16X16 ======================================

void out_line_16(int x_pos, int y_pos, char *str, u32 color){
u16 i=0;
int j;
//	x_pos -= XS_OFFSET;
//	x_pos = 272 - x_pos;
	while(1){
		j = str[i++]; if(!j) break;
		if(j<0x80){
			x_pos = out_text_16e(x_pos, y_pos, j, color);
		}		
	}	
}

/********************************************************************/
int out_text_16e(int x_pos, int y_pos, int cod, u32 color){
u16 i, j, HS;
u32 ww, pos_d;
u32 save16[16];	

	u8 bb = color>>16;	
	u8 rr = color>>8;	
	u8 gg = color>>0;
	pos_d = ENG16ADR+((cod-0x20)*2*16);
//printf0("\r Pos:%6x,%2d",pos_d,HS/2);
	for(i=0;i<16;i++){
		ww = *(u8*)(FONT_BASE+pos_d); pos_d++;
		ww = (ww<<8) + *(u8*)(FONT_BASE+pos_d); pos_d++;
		save16[i] = ww;
	}
	pos_d = LcdBaseAdr0 + BUFFER_OFFSET + (y_pos*OFF_Y3) + (x_pos*3);
	HS = size16[cod];
	for(i=0;i<16;i++){
		ww = save16[i];
		for(j=0;j<HS;j++){
			if(ww&0x8000){
				*(__IO uint8_t*)(pos_d+(j*3)+0) = bb;
				*(__IO uint8_t*)(pos_d+(j*3)+1) = rr;
				*(__IO uint8_t*)(pos_d+(j*3)+2) = gg;
			}
			ww = ww<<1;
		}
		pos_d += OFF_Y3;
	}
	return(x_pos+HS+2);
}

//############################################################################################################
//############################################################################################################
//############################################################################################################
//############################################################################################################

/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
