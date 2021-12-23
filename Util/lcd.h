
#define LcdFrameSize	BUFFER_OFFSET					//0x119400	//0x5DC00*3
#define BMP_SIZE		LcdFrameSize					//

#define XS_OFFSET	12

//TEXT COLOR
#define WHT_LCD		0xffffff
#define RED_LCD		0x0000FF
#define GRN_LCD		0x00FF00
#define BLU_LCD		0xFF0000
#define BLK_LCD		0x000000
#define CYN_LCD		0xFFFF00
#define YEL_LCD		0x00FFFF
#define MGT_LCD		0xFF00FF
#define GRY_LCD		0x303030

void set_lcd_buff(u32 addr);

void init_lcd(void);
void copy_lcdbase(void);
void test_lcd(void);
void msg24(u16 x, u16 y, u32 color, char *fmt,...);
void msg32(u16 x, u16 y, u32 color, char *fmt,...);
void msg16(u16 x, u16 y, u32 color, char *fmt,...);
void clr_cursor_mem_set(void);
void dsp_cursor_mem_seq(u16 cursor);
void dsp_cursor_mem_sid(u16 cursor);
void dsp_cursor_mem_oid(u16 cursor);
void dsp_cursor_mem_date(u16 cursor);
void dsp_cursor_mem_date2(u16 cursor);
void dsp_cursor_mem_time(u16 cursor);
void dsp_cursor_oper(u16 cursor);
void dsp_cursor_password(u16 cursor);
void clr_cursor_test_set(void);
void dsp_cursor_test_seq(u16 cursor);
void dsp_cursor_test_sid(u16 cursor);
void clr_cursor_qc_set(void);
void dsp_cursor_qc_seq(u16 cursor);
void dsp_cursor_qc_lot(u16 cursor);
void clr_screen_box(u16 xp, u16 yp, u16 xsize, u16 ysize);

void cursor_24(u16 xs, u16 ys, u16 cr, char *str);
void cursor_20(u16 xs, u16 ys, u16 cr, char *str);
void cursor_16(u16 xs, u16 ys, u16 cr, char *str);
void LCDbmpxx(u32 bmpnt, u16 xp, u16 yp);
void init_fonts(void);
void init_bmps(void);
void set_size24(void);
void set_size32(void);
void draw_box(u16 xs, u16 ys, u16 xe, u16 ye, u32 color);
void erase_box(u16 xs, u16 ys, u16 xe, u16 ye);
void erase_box0(u16 xs, u16 ys, u16 xe, u16 ye);
void line_x(u16 xs, u16 ys, u16 length, u16 width, u32 color);
void line_y(u16 xs, u16 ys, u16 length, u16 width, u32 color);
void line_box(u16 xs, u16 ys, u16 xe, u16 ye, u16 width, u32 color);
u32 bin2bcdasc(u32 bin);
u32 bin2bcd(u32 bin);
u16 hexasc2bin(u32 asc);
u16 bcd2bin(u32 bcdasc);
u32 bin2hexasc4(u32 bin);
u16 asc2bin4(u32 asc4);
u32 bin2bcdasc4(u32 bin);
u8 hexasc2bin_1(u8 hexasc);
u8 bin2hexasc(u8 bin);
void time_copy(u8 year, u8 month, u8 day, u8 hour, u8 min, u8 sec);
u16  get_xpos24(char *str, u16 xn);
u16  get_xpos32(char *str, u16 xn);

int  out_text_64e(int x_pos, int y_pos, int cod, u32 color);
void out_line_64(int x_pos, int y_pos, char *str, u32 color);
int  out_text_56e(int x_pos, int y_pos, int cod, u32 color);
void out_line_56(int x_pos, int y_pos, char *str, u32 color);
int  out_text_48e(int x_pos, int y_pos, int cod, u32 color);
void out_line_48(int x_pos, int y_pos, char *str, u32 color);
int  out_text_40e(int x_pos, int y_pos, int cod, u32 color);
void out_line_40(int x_pos, int y_pos, char *str, u32 color);
int  out_text_32e(int x_pos, int y_pos, int cod, u32 color);
void out_line_32(int x_pos, int y_pos, char *str, u32 color);
int  out_text_24e(int x_pos, int y_pos, int cod, u32 color);
void out_line_24(int x_pos, int y_pos, char *str, u32 color);
int  out_text_16e(int x_pos, int y_pos, int cod, u32 color);
void out_line_16(int x_pos, int y_pos, char *str, u32 color);

#ifdef _LCD_C_

const char *text_SEQ_NO="SEQ : ";
const char *text_ID_NO ="ID No.:  ";
int  LCDx_res, LCDy_res;        
int  LCDx_bpp_res;     
u8 	RASTNO;
int  ENG32ADR, HAN32ADR, ENG24ADR, HAN24ADR, ENG16ADR, HAN16ADR, FIG32ADR, ENG20ADR;
u8 	 size16[128], size24[128], size32[128], size40[128], size48[128], size56[128], size64[128];
char top_str[64];
u32	LcdBuffer;

u8	bmp_head[0x600];

#else

extern u8   RASTNO;
extern int  ENG32ADR, HAN32ADR, ENG24ADR, HAN24ADR, ENG16ADR, HAN16ADR, FIG32ADR, ENG20ADR;
extern u8 	size16[128], size24[128], size32[128], size40[128], size48[128], size56[128], size64[128];
extern char top_str[64];
extern u32	LcdBuffer;



#endif 

/******************************************************************************/
/******************************************************************************/

