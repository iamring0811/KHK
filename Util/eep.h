#define BASE32F429		0x08000000	//Internal Flash
#define STORE_ROM		BASE32F429+0x00010000
#define STORE_SIZE		0x00006000

#define SYSTEM_RAM		0x00000000	//External Serial Flash
#define SYSTEM_SIZE		0x00000300	
#define POSITION_RAM	0x00001000	//216	0x0D8
#define POSITION_SIZE	64			//
#define STRIP_INFO_RAM	0x00002000	//
#define STRIP_INFO_SIZE	33*20		//
#define PARA_RAM		0x00003000	//20+(17*7*3*4)=20+1428, ���� 20Bytes�� Weight Data�̴�.
#define PARA_SIZE		20+820		//(1+(4*10))*20, including weight data
#define WEIGHT_SIZE		20
#define PFORM_RAM		0x00004000	//21*8*8*3
#define PFORM_SIZE		4980		//83*20*3
//		end PFORM_RAM	0x00005373	//21*8*8*3

#define MAX_RECORD		5000		//
#define MAX_RECORD		5000		//
#define ALL_RECORD		MAX_RECORD + 100	

#define SRT_SEQ_NO      1
#define END_SEQ_NO      MAX_RECORD

#define SRT_QC_NO		MAX_RECORD + 1
#define END_QC_NO		MAX_RECORD + 100

#define LOSCOUNT        12

#define RESULT_PAGE		0x100
#define SECTOR_SIZE		0x1000
#define RESULT_ADR		0x010000	//size=ALL_RECORD*0x100=0x13EC00+0x010000=0x14EC00
#define RESULT_END		0x14EBFF
//#define RESULT_ADR		0x010000	//size=MAX_RECORD*0x100=138800+010000=148800
//#define RESULT_END		0x1487FF
//#define QCDATA_ADR		0x150000	//size=MAX_QCDATA*0x100=  6400+140000=146400
//#define QCDATA_END		0x1563FF
#define INDEX_PAGE		0x40		// 64

#define SPAN_RED_ADR	0x1F0000	//0x05f8=1528=764*2
#define SPAN_GRN_ADR	0x1F1000	//0x05f8=1528=764*2
#define SPAN_BLU_ADR	0x1F2000	//0x05f8=1528=764*2		//0x11e8=4584=764*2*3
#define SEQNO_ADR 		0x1F8000	// 2B + 2B
#define SYS_TEST_ADR	0x1FF000

#define ID55AA	 		0x55A8
#define ID55AA_ADR 		0x00
#define STRIPNO_ADR		0x02		// 2B, 0~99
#define UNIT_ADR 		0x04		// 2B
#define COLOR_ADR		0x06		// 2B, 1B
#define TURBI_ADR		0x08		// 2B, 1B
#define SERIAL_ADR		0x0A		// 2B, 0,1,2
#define BAUD_ADR		0x0C		// 2B, 2bit
#define SOUND_ADR		0x0E		// 2B, 1bit
#define AUTO_ADR 		0x10		// 2B, 1bit
#define QC_NO_ADR 		0x12		// 2B, 1bit
#define DAC_ADR			0x14		// 2B(16bit) rrrr,gggg,bbbb,sum
#define PRINT_ADR 		0x16		// 2B, 1bit
#define PLUS_ADR		0x18		// 2B, 1bit
#define MARK_ADR		0x1A		// 2B, 1bit
#define ID_CHKSUM 		0x1C		// 2B

#define DATEDELIMITERADR    0x1E    //2B, 2bit
#define TIMEDELIMITERADR    0x20    //2B, 2bit

#define DATEPRESENTATIONADR 0x22    //2B, 1bit
#define TIMEPRESENTATIONADR 0x24    //2B, 1bit

#define LANGUAGE_ADR    0x26        //2b, 1bit
#define SAMPLE_ADR      0x28        //2b, 1bit
#define SBAUD_ADR      0x2A         //2b, 1bit
#define CHK_REF_ADR		0x2C		// 2B, 1bit
#define SYSID_ADR 		0x30		//11B

#define OPERATOR_ADR 	0x100		//(10+18)*8 + 1 = 224 + 1 = 225
#define OPERATOR_SIZE	0xE1		//(MAX_OPERID+18)*8+1		
#define CHANGEPR_ADR 	0x200		//
#define CHANGEPR_SIZE	0x96		//15*5*2 = 150		


#define PRINT_ENABLE	1
#define PRINT_DISABLE	0
#define DATA_MENUAL		1
#define DATA_AUTO		0

#define UNIT_ARBI		0
#define UNIT_CONV		1
#define UNIT_SI			2

#define ADD10B					10
#define STRIP_NAME_SIZE			16		//was 10

//============================================================
void initEEP(void);
void read_eep_all(void);
void store_seqno(void);
void default_id(void);
void write_id_chksum(void);
void clr_user_id(void); 
void store_vars(u16 adr, u16 dat);

void chk_ini_position(void);
void chk_ini_para(void);
void chk_ini_strip(void);
void get_max_stripno(void);
void write_vars(u8 cmd);
void chk_ini_printform(void);

u16 read_ram4(u16 pnt);
void move_position(void);
void move_strip_info(void);
void get_strip_name(u16 sno);
void move_parameter(void);
void move_printform(void);
u16 back_parameter(void);

void backup_intflash(u16 adr, u16 size);		//EXT.=> INT
void restore_sflash(u16 adr, u16 size);			//INT.=> EXT
void store_sflash(u16 adr, u16 size);
void write_sflash(u32 adr, u16 size);			

void store_strip_no(void);
void store_oper_buff(void);
void store_pr_buff(void);

void store_result(u16 tmp_seqno);
u16 read_result(u16 sno, u16 rrd);	// extROM ==> RAM
void restore_result(void);
u16 check_result(u16 sno);
void make_index(void); 
void add_index(void);
void del_index(u16 dno);
void delete_record(u16 rno);
u8 chk_index(u16 dno);

u8 read24(u16 adr);
u16 read24w(u16 adr);
void write24(u16 adr, u8 dat);
void write24w(u16 adr, u16 dat);
void key_buzzer2(void);
void key_buzzer3(void);
void store_reference_rgb(void);
u16 get_white_high(u16 *str);
void read_ref_data(void);
u8 asc2dec(u8 asc);

void ReadMemory(void);
void ReadLos12(void);

#define Version "V1.09-211220"
#define MAX_SYSID		11
#define MAX_OPERID		10
#define MAX_USERID		18
#define MAX_LOTNO		8
#define NO_ERROR		0
#define NO_STRIP        1
#define TIME_OVER       2
//============================================================
#define MAX_REF			5000
#define MIN_REF			1
#define CHK_REF_INIT	1000

#ifdef	_EEP_C_
char SYS_ID[MAX_SYSID+3];					//
char OPER_ID[MAX_OPERID+3];					//
char oper_buff[OPERATOR_SIZE];			//
char strip_name[STRIP_NAME_SIZE + 3];			//
char USER_ID[MAX_USERID+3];					//

u16 SEQ_NO;									//1~5000				�����ҹ�ȣ
u16 QC_NO;									//1~100					�����ҹ�ȣ
u8  PRINT_FLAG;								//0:On,     1:Off     	�����Ϸ��� ������ ���� ����
u8  UNIT_FLAG, temp_UNIT_FLAG;								//0:Conv.1:SI, 2:Arbitrary

u8  STRIP_NO;								//0-19
u8  MAX_STRIPNO;							//0-20
u8  AUTO_FLAG;								//0:On(Auto), 1:Off
u8	PLUS_FLAG, temp_PLUS_FLAG;				//1:enable
u8	MARK_FLAG, temp_MARK_FLAG;				//1:yes
u8	oper_no, oper_cursor;				//0~7
u8  cmdbuff[PFORM_SIZE + 100];
u8	tmp_strip;
u8  SOUND_FLAG;								//0:off,1:on
u8  BAUD_FLAG;								//0:9600,1:38400,2:115200
u8  SBAUD_FLAG;								//0:9600,1:38400,2:115200
u8	COLOR_NO;
u8	TURBI_NO;
u8	SERIAL_FLAG, temp_SERIAL_FLAG;							//������ ������ ����
u8	ErrCode;						//0:No Error, 1:No Strip. 2:Time Over
u8	NEGPOS;									//0:Negative, 1:Positive
u8  LANGUAGE_FLAG;								//0:English, 1:Korean
int memCount;

u16 CHK_REF;
#else
extern char SYS_ID[MAX_SYSID + 3];					//
extern char OPER_ID[MAX_OPERID + 3];					//
extern char oper_buff[OPERATOR_SIZE];			//
extern char strip_name[STRIP_NAME_SIZE + 3];			//
extern char USER_ID[MAX_USERID + 3];					//
extern u16 SEQ_NO;									//1~5000				�����ҹ�ȣ
extern u16 QC_NO;									//1~100					�����ҹ�ȣ
extern u8  PRINT_FLAG;								//0:On,     1:Off     	�����Ϸ��� ������ ���� ����
extern u8  UNIT_FLAG, temp_UNIT_FLAG;								//0:Conv.1:SI, 2:Arbitrary
extern u8  STRIP_NO;								//0-19
extern u8  MAX_STRIPNO;								//0-20
extern u8  AUTO_FLAG;								//0:On(Auto), 1:Off
extern u8	PLUS_FLAG, temp_PLUS_FLAG;								//1:enable
extern u8	MARK_FLAG, temp_MARK_FLAG;								//1:yes
extern u8	oper_no, oper_cursor;				//0~7
extern u8  cmdbuff[PFORM_SIZE + 100];
extern u8  tmp_strip;
extern u8  SOUND_FLAG;								//0:off,1:on
extern u8  BAUD_FLAG;								//0:38400,1:19200,7:9600
extern u8  SBAUD_FLAG;								//0:38400,1:19200,7:9600
extern u8	COLOR_NO;
extern u8	TURBI_NO;
extern u8	SERIAL_FLAG, temp_SERIAL_FLAG;
extern u8	ErrCode;
extern u8	NEGPOS;									//0:Negative, 1:Positive
extern u8  LANGUAGE_FLAG;								//0:English, 1:Korean
extern int memCount;
extern s16 CHK_REF;
#endif

