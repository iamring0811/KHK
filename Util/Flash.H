
void prog_decode(void);
void flash_decode(u8 dat);

/*************************	FMC  **********************/
#define MAX_PAGE_NO		NAND_BLOCK_SIZE
//#define MAX_BLOCK_NO	1024
void FMC_NAND_Check(void);

u8 run_format(void);
void store_bmp_usb(void);
void store_bmp(u8 bar, u8 sel);
void store_font(u8 bar);
void NAND_Test(void);
void move_BMP(void);
void move_BMP_second(void);									//2020-07-19 12:44오후
void move_FONT(void);
#define FONT_HEAD_SIZE		0x1000
#define BMP_HEAD_SIZE		0x1000

void test_flash_write(void);

#ifdef	_FLASH_C_
/************************  flash  **********************/
u8	flag_flash, flag_upload, flag_second;										//2020-07-19 12:44오후
u16	BMP_NUMBER, BMP_VERSION;
u16	FONT_NUMBER, FONT_VERSION;

u8 BMP_TB[BMP_HEAD_SIZE];
u16 NAND_TB[0x400];
u8	FONT_HEAD[FONT_HEAD_SIZE];
u32	ENG40ADR, ENG48ADR, ENG56ADR, ENG64ADR;

#else
/************************  flash  **********************/
extern u8	flag_flash, flag_upload, flag_second;									//2020-07-19 12:44오후
extern u16	BMP_NUMBER, BMP_VERSION;
extern u16	FONT_NUMBER, FONT_VERSION;

extern u8 BMP_TB[BMP_HEAD_SIZE];
extern u16 NAND_TB[0x400];
extern u8	FONT_HEAD[FONT_HEAD_SIZE];
extern u32	ENG40ADR, ENG48ADR, ENG56ADR, ENG64ADR;

#endif

