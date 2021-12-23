
#define COM_SYS_INFO			0x51		//Q
#define COM_SYS_ID_SET			0x52		//R
#define COM_MEAS_RESULT			0x54		//T
#define COM_USERID_SET			0x55		//U
#define COM_OPERID_SET			0x56		//V
#define COM_TIME_WRITE			0x58		//X
#define COM_VERSION_READ		0x59		//Y
#define COM_TIME_READ			0x5A		//Z
#define COM_POSITION_READ		0x61		//a
#define COM_POSITION_WRITE		0x62		//b
#define COM_PARA_READ			0x63		//c
#define COM_PARA_WRITE			0x64		//d
#define COM_STRIP_INFO_READ		0x65		//e
#define COM_STRIP_INFO_WRITE	0x66		//f
#define COM_STRIP_NO_RD			0x67		//g
#define COM_STRIP_NO_WR			0x68		//h
#define COM_FULL_RAW_DATA		0x69		//i
//#define COM_SYS_CHECK			0x6D		//m
#define COM_PFORM_READ			0x71		//q
#define COM_PFORM_WRITE			0x72		//r
#define COM_CAL_SEND			0x73		//s


//	#define COM_MEAS_START		0x53		//S
//	#define COM_SPD_READ		0x56		//V		//0:ǥ��, 1:����
//	#define COM_SPD_WRITE		0x57		//W
//	#define COM_CLEAN_LOAD		0x5B		//[
//	#define COM_DATA_COUNT		0x6A		//j
//	#define COM_CALIBRATION		0x6C		//l
//	#define COM_DATA_NOSTORE	0x6E		//n		������������Ÿ�� �������.
//	#define COM_DATA_ERASE		0x6F		//o		����Ÿ�� �����.
//	#define COM_ALL_RESULT		0x70		//p		����� ����Ÿ��ü�� ������.

#define URLF 			0x0A

#define RX0_WAIT  		10
#define RX0_1st			0
#define RX0_2nd			1
#define RX0_RCV			4
#define RX3_WAIT	  	3			//20~30[ms]

void store_sysid(u8 wflag);

void system_decode(u8 dat);
void send_meas_step(u8 step);
void chk_com_dly(void);
void store_para(void);
void fill_space(char *str, short cnt);
void fill_space2(char* str, short cnt);

#ifdef _COM_C_
u8	rx3_step;
u8  COMCMD;
u8  com3dat;
u16 rx3_wait;						//��ſ��� ���ÿ�.
u16 rx3_cnt;

void com_time_write(void);
void com_version_read(void);
void com_time_read(void);
void com_sys_info(void);
void com_sys_id_set(void);
void com_meas_result(void);	
void com_userid_set(void);		
void com_operid_set(void);
void com_position_read(void);	
void com_position_write(void);	
void com_para_read(void);	
void com_para_write(void);	
void com_strip_info_read(void);	
void com_strip_info_write(void);	
void com_strip_no_read(void);	
void com_strip_no_write(void);	
void com_full_raw_data(void);
void com_pform_read(void);
void com_pform_write(void);
void com_cal_send(void);
void send_position(void);
void send_para(void);
void send_strip(void);

#endif

