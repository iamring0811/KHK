/********************  Step Motor  ********************/
#define TICK_STEPA		2120	//Shifter
#define TICK_STEPB		2420	//PushBar
//#define MOTOR_DLY		250
#define MOTOR_DLY		350
#define MT_A			0
#define MT_B			1

void push_strip(void);
void move_strip(void);
void chk_table(void);
void delay_ms(u32 dly);
void delay_us(u16 dly);

/***********************  RTC  ************************/
/*	MAP	OF RAM IN DS1302  */
#define	RTC_SECOND		0x00
#define	RTC_MINUTE		0x01
#define	RTC_HOUR		0x02
#define	RTC_DAY			0x03
#define	RTC_MONTH		0x04
#define	RTC_WEEK		0x05
#define	RTC_YEAR		0x06
#define	RTC_DSWP		0x07
#define	RTC_TCRV		0x08

#define	RTCend			0x3F

#define	rtc_read()		PORT_LEDInit(DSIO, IN_DIR)
#define	rtc_write()		PORT_LEDInit(DSIO, OUT_DIR)

#define off_photo()	 	PORT_LEDlo(PHON);
#define on_photo()	 	PORT_LEDhi(PHON);

void writeRTC(u8 add, u8 sd);
u8   readRTC(u8 add);
void sendRTC(u8 add);
void sendAll_RTC(void);
u8   dataRTC(void);
void initRTC(void);
u8   bcd2bin2(u8 ch);
void readAllRTC(void);
void out_time(void);
void readTime(void);
void test_rtc_init(void);
void test_rtc(void);

/********************  Serial(Ext) Flash  **********************/
#define FREAD	0x03		//Read Data
#define FBTPR	0x02		//Byte Write
//#define FWRPR	0xAD		//Word Write
#define FRDSR	0x05		//Read Status
#define FEWSR	0x50		//Enable Write Status Reg.
#define FWRSR	0x01		//Write Status Reg.
#define FWREN	0x06		//Write Enable
//#define FWRDI	0x04		//Write Disable
#define FRDID	0x90		//Read I.D.
#define FERASE	0x20		//4K Bytes Erase
#define FJDRD	0x9F		//Jedec Read I.D.

#define	en1302()	 		PORT_LEDlo(DSEN)	//DS_ENABLE()	//DS1302 enable	(RST_PIN)
#define	dn1302()	 		PORT_LEDhi(DSEN)	//DS_DISABLE()	//DS1302 disable(RST_PIN)
#define	clr_dat02()	 		PORT_LEDlo(DSIO)	//DSIO_LO()		//DS1302 data low out
#define	set_dat02()	 		PORT_LEDhi(DSIO)	//DSIO_HI()		//DS1302 data high out
#define	read_dat02() 		(DSIO_PORT->IDR & 0x0080)	//DSIO_READ		//DS1302 data read
#define	clr_clk02()	 		PORT_LEDlo(DSCK)	//DSCK_LO()		//CLOCK	LOW
#define	set_clk02()	 		PORT_LEDhi(DSCK)	//DSCK_HI()		//CLOCK	HIGH

#define WP24_HI()			PORT_LEDhi(WP24)
#define WP24_LO()			PORT_LEDlo(WP24)
#define DI24_HI()			PORT_LEDhi(DI24)
#define DI24_LO()			PORT_LEDlo(DI24)
#define CK24_HI()			set_clk02()
#define CK24_LO()			clr_clk02()

#define DO24	   			(DO24_PORT->IDR & 0x2000)

void test_flash(u8 tno);
void writeFR4k(u32 adr, u16 size);
void readFR4k(u32 add, u16 size);
short read_jedec(void);
short cmpFR4k(u32 adr, u16 size);


#ifdef	_RESM_C_
/*************************	Step  *********************/
u16	run_stepa, run_stepb;
u16	motor_step, motor_wait;
u8	read_strip_have, read_strip_none, read_strip_flag, read_strip_step, read_strip_wait;
u16	read_strip_move;
u8	flag_mtab;
u8	flag_table, flag_box;
u16	table_delay, box_delay;

/*************************	RTC  **********************/
u8 setyear,setmonth,setday;			// 날짜 설정 데이터
u8 sethour,setmin,setsec;			// 날짜 설정 데이터
u8 year,month,day,hour,min,sec;		// 날짜 설정 데이터
u8 tempyear, tempmonth, tempday, temphour, tempmin, tempsec;		// 날짜 설정 데이터
u8 time_flag;						// 시간이 바뀌었다.
u8 testrtc;

/***********************  Sflash  **********************/
u8	flash_buff[0x1000];
u8	flash_read[0x1000];

#else
/*************************	Step  *********************/
extern u16	run_stepa, run_stepb;
extern u16	motor_step, motor_wait;
extern u8	read_strip_have, read_strip_none, read_strip_flag, read_strip_step, read_strip_wait;
extern u16	read_strip_move;
extern u8	flag_mtab;
extern u8	flag_table, flag_box;
extern u16	table_delay, box_delay;

/*************************	RTC  **********************/
extern u8 setyear,setmonth,setday;			// 날짜 설정 데이터
extern u8 sethour,setmin,setsec;				// 날짜 설정 데이터
extern u8 year,month,day,hour,min,sec;		// 날짜 설정 데이터
extern u8 time_flag;
extern u8 tempyear, tempmonth, tempday, temphour, tempmin, tempsec;		// 날짜 설정 데이터
/***********************  Sflash  **********************/
extern u8	flash_buff[0x1000];
extern u8	flash_read[0x1000];

#endif

