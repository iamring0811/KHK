/***********************************************************
1. test입력창에서 아이템별로 입력
2. 
***********************************************************/
#define MODE_HOME			0x0000
#define MODE_MEMORY			0x0100
#define MODE_MEM_type		0x0110
#define MODE_MEM_seq		0x0120
#define MODE_MEM_sid		0x0130
#define MODE_MEM_oid		0x0140
#define MODE_MEM_view		0x0150
#define MODE_MEM_delete		0x0160
#define MODE_MEM_print		0x0170
#define MODE_MEM_upload		0x0180
#define MODE_MEM_date_set	0x0190

#define MODE_MEAS			0x0200
#define MODE_MEAS_Error		0x0201
#define MODE_MEAS_Error2	0x0202
#define MODE_oper_set		0x0210
#define MODE_oper_new		0x0220
#define MODE_strip_set		0x0230
#define MODE_TEST			0x0300
#define MODE_TEST_Error		0x0301
#define MODE_TEST_Error2	0x0302
#define MODE_TEST_Return	0x0303

#define MODE_TEST_seq		0x0310
#define MODE_TEST_strip		0x0320
#define MODE_TEST_sid		0x0330		// user id
#define MODE_TEST_color		0x0340
#define MODE_TEST_trubi		0x0350

#define MODE_QC				0x0400
#define MODE_QC_Error		0x0401
#define MODE_QC_Error2		0x0402
#define MODE_QC_Return		0x0403

#define MODE_QC_seq			0x0410
#define MODE_QC_strip		0x0420
#define MODE_QC_negpos		0x0430		// no use
#define MODE_QC_lot			0x0440
#define MODE_SET			0x0500
#define MODE_datetime		0x0510
#define MODE_date_set		0x0511
#define MODE_time_set		0x0512	
#define MODE_result_unit	0x0520
#define MODE_RESULT_NOSTRIP 0x0521
#define MODE_system_set		0x0530
#define MODE_system_set_Return	0x0531
#define MODE_system_set_Restart	0x0532

#define MODE_oper_info		0x0540
#define MODE_system_chk		0x0550
#define MODE_system_chk_confirm 0x0551
#define MODE_input_password	0x0560
#define MODE_change_PR		0x0561
#define MODE_digit_PR		0x0562

#define MODE_MAINTANENCE_password	0x0580
#define MODE_MAINTANENCE			0x0581
#define MODE_MAINTANENCE_cali		0x0582
#define MODE_MAINTANENCE_serial		0x0583
#define MODE_MAINTANENCE_fw			0x0584
#define MODE_MAINTANENCE_image		0x0585
#define MODE_MAINTANENCE_font		0x0586
#define MODE_MAINTANENCE_Reset		0x0587
#define MODE_MAINTANENCE_Confirm	0x0588
#define NOW_QC_DATA			0x0001
#define memoryLine			8

void key_decode(void);
void key_clear(void);
void tch_read(void);
void dbg_wait(char *str);

void tch_home(void);
void tch_setting(void);
void tch_system_chk(void);
void tch_maintanence(void);
void tch_maintanence_cali(void);
void tch_maintanence_serial(void);
void tch_maintanence_fw(void);
void tch_maintanence_image(void);
void tch_maintanence_font(void);
void tch_datetime(void);
void tch_date_set(void);
void tch_time_set(void);

void tch_oper_set(void);
void tch_oper_new(void);
void tch_strip_set(void);
void tch_change_pr_set(void);
void tch_digit_pr(void);

void tch_memory(void);
void tch_mem_set(void);
void alpa_mem_seq(u8 cod);
void alpa_mem_sid(u8 cod);
void alpa_mem_oid(u8 cod);
void alpa_mem_date_set(u8 cod);

void tch_test(void);
void tch_test_set(void);
void digit_test_seq(u16 cod);
void alpa_test_sid(u16 cod);
void tch_test_wait(void);
void tch_test_set_strip(void);
void tch_test_set_color(void);
void tch_test_set_turbi(void);

void tch_QC(void);
void tch_qc_set(void);
void tch_qc_set_strip(void);
void digit_qc_seq(u16 cod);
void alpa_qc_lot(u16 cod);
void tch_qc_wait(void);

void tch_error(void);
void tch_mem_view(void);
void tch_measure(void);
void dayInMonth(u16 year, u8 month);
void tch_unit(void);
void tch_sysset(void);
void tch_sysset_Restart(void);
void update_bmp(void);
void tch_printupload(void);
void tch_input_password(void);
void tch_maintanence_password(void);
void tch_maintanence_Reset(void);
void tch_maintanence_Confirm(void);
void tch_QC_return(void);
void tch_test_return(void);
void tch_sysset_return(void);
void tch_result_nostrip(void);
void tch_error_confirm(void);
void tch_syschk_confirm(void);
#ifdef _TOUCH_C_
u16	modex, temp_modex, flag_motor_run;
u16	tchx, tchy;
u8	flag_self;
u8	flag_dsp_strip;						// 1:display strip list, 0:close strip list
u16	cur_dsp_record_no;
u16	first_USB_HS = 0;
u16 tmp_line, tmp_digit;
char oper_name[MAX_OPERID+3];
u8 tempDatePresentation;
char tempDateDelimiter;
u8 tempTimePresentation;
char tempTimeDelimiter;
u8 dayCount;
u8 timeFlag, tempTimeFlag, dateMemFlag;
char buff_date[16];
char buff_time[16];
u8 languageFlag, fullFlag, setChangeFlag, returnFlag;
#else
extern u16	modex, temp_modex, flag_motor_run;
extern u16	tchx, tchy;
extern u8	flag_self;
extern u8	flag_dsp_strip;
extern u16	cur_dsp_record_no;
extern char oper_name[MAX_OPERID+3];
extern u16 tmp_line, tmp_digit;
extern u8 tempDatePresentation;
extern char tempDateDelimiter;
extern u8 tempTimePresentation;
extern char tempTimeDelimiter;
extern u8 dayCount;
extern u8 timeFlag, tempTimeFlag, dateMemFlag;
extern char buff_date[16];
extern char buff_time[16];
extern u8 fullFlag, setChangeFlag, returnFlag;
#endif 

#define flag_dsp_color	flag_dsp_strip
#define flag_dsp_turbi	flag_dsp_strip

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
#ifdef _TOUCH_C_
/*   xs, xe, ys, ye */
#define tch_home_no	3
const u16 tch_home_tb[4*tch_home_no]={
	110,289,250,620,		//memory 
	340,510,250,620,		//measure
	570,740,250,620 		//setting
};

#define tch_setting_no	8
const u16 tch_setting_tb[4*tch_setting_no]={
	 42,122,788,846, 		//home
	775,841,788,846, 		//back
	110,289,480,740,		//clock
	340,510,480,740,		//unit
	570,740,480,740, 		//system
	110,289,130,410,		//operater 
	340,510,130,410,		//sys_check
	570,740,130,410 		//change%R
};

#define tch_datetime_no	15
const u16 tch_datetime_tb[4 * tch_datetime_no] = {
	 42,122,788,846, 		//00	home
	775,841,788,846, 		//01	back
	 76,375,570,650,		//02	date
	 76,375,292,372,		//03	time
	750,860, 45,110, 		//04	save
	120,250,500,565,		//05	YYYYMMDD
	250,375,500,570,		//06	DDMMYYYY
	125,255,415,480,		//07	Date /
	255,380,415,485,		//08	Date -
	380,500,420,495,		//09	Date ,
	120,250,205,275,		//10	12hr
	250,375,210,275,		//11	24hr
	110,250,125,190,		//12	Time :
	250,370,130,200,		//13	Time -
	370,500,135,200			//14	Time ,
};

#define tch_date_set_no	16
const u16 tch_date_set_tb[4*tch_date_set_no]={
	 42,122,788,846, 		//home	0
	775,841,788,846, 		//back	1
	467,542,566,665, 		//'1'	2
	548,628,566,665, 		//'2'	3
	630,707,566,665,		//'3'	4
	467,542,465,560, 		//'4'	5
	548,628,465,560, 		//'5'	6
	630,707,465,560,		//'6'	7
	467,542,330,460, 		//'7'	8 
	548,628,330,460, 		//'8'	9
	630,707,330,460,		//'9'	10
	548,628,218,325, 		//'0'	11
	467,542,218,325, 		//AM/PM	12
	630,707,218,325,		//back	13
	459,568,122,200, 		//save	14
	603,716,122,200, 		//CLOSE	15
};
#define tch_time_set_no		tch_date_set_no
#define tch_time_set_tb		tch_date_set_tb

#define tch_memory_no	22
const u16 tch_memory_tb[4 * tch_memory_no] = {
	 42,122,788,846, 		//00	home
	775,841,788,846, 		//01	back
	770,860,635,680, 		//02	test type
	 60,420,610,670, 		//03	test date
	430,770,635,685, 		//04	seq
	435,570,580,630, 		//05	sample id 1 = ALL
	570,720,580,630, 		//06	sample id 2 = NEG
	720,865,585,630, 		//07	sample id 3 = POS
	 60,420,570,620, 		//08	oper id
	840,880,360,520,		//09	up
	840,880,135,295,		//10	dn
	420,560, 20, 90,		//11	print
	590,720, 20, 90,		//12	upload
	750,880, 20, 90, 		//13	delete
	 10,850,450,500, 		//14	Line1
	 10,850,400,450, 		//15	Line2
	 10,850,350,400, 		//16	Line3
	 10,850,300,350, 		//17	Line4
	 10,850,250,300, 		//18	Line5
	 10,850,200,250, 		//19	Line6
	 10,850,150,200, 		//20	Line7
	 10,850,100,150 		//21	Line8
};


#define tch_memview_no	19
const u16 tch_memview_tb[4*tch_memory_no]={
	 42,122,788,846, 		//00	home
	775,841,788,846, 		//01	back
	420,560, 20, 90,		//02	print
	590,720, 20, 90,		//03	upload
	750,880, 20, 90, 		//04	delete
	10, 220, 640, 690,		//05	1
	10, 220, 590, 640,		//06	2
	10, 220, 540, 590,		//07	3
	10, 220, 500, 540,		//08	4
	10, 220, 450, 500,		//09	5
	10, 220, 400, 450,		//10	6
	10, 220, 350, 400,		//11	7
	10, 220, 300, 350,		//12	8
	10, 220, 250, 300,		//13	9
	10, 220, 200, 250,		//14	10
	10, 220, 150, 200,		//15	11
	10, 220, 100, 150,		//16	12
	220,262,612,662,		//17	Up
	220,262,118,180,		//18	Down
};
/*순서
Test Type			90,70	/
SEQ				90,100	/220, 104
Operator ID			90,130	/220, 134
Sample ID / Solution		90,160	/220, 164
*/
#define tch_memset_no	51
const u16 tch_memset_tb[4*tch_memset_no]={
	 42,122,788,846, 		//	00	home
	775,841,788,846, 		//	01	back
	105,465,715,750, 		//	02	test date
	105,465,650,700, 		//	03	seq
	105,460,600,650, 		//	04	oper id
	105,270,550,600, 		//	05	sample id1
	270,405,550,600, 		//	06	sample id2
	405,535,550,600, 		//	07	sample id3
	600,730,660,720, 		//	08	General
	600,730,600,660, 		//	09	QC
	246,364,120,185, 		//	10	SAVE		
	499,614,120,185,  		//	11	CLOSE
	685,745,289,365, 		//	12	'<'		 7번째줄
	559,620,209,285, 		//	13	CLEAR
	621,745,209,285, 		//	14	SPACEBAR
	685,745,459,545, 		//	15	'0'		10번째줄
	120,182,459,545, 		//	16	'1'
	183,245,459,545, 		//	17	'2'
	246,307,459,545, 		//	18	'3'
	308,370,459,545, 		//	19	'4'
	371,433,459,545, 		//	20	'5'
	434,496,459,545, 		//	21	'6'
	497,558,459,545, 		//	22	'7'
	559,620,459,545, 		//	23	'8'
	621,684,459,545, 		//	24	'9'
	120,182,289,365, 		//	25	'A'   	20번째줄
	371,433,209,285, 		//	26	'B'
	246,307,209,285, 		//	27	'C'
	246,307,289,365, 		//	28	'D'
	246,307,369,455, 		//	29	'E'
	308,370,289,365, 		//	30	'F'
	371,433,289,365, 		//	31	'G'
	434,496,289,365, 		//	32	'H' 
	559,620,369,455, 		//	33	'I'
	497,558,289,365, 		//	34	'J'
	559,620,289,365, 		//	35	'K'
	621,684,289,365, 		//	36	'L'
	497,558,209,285, 		//	37	'M'   
	434,496,209,285, 		//	38	'N'
	621,684,369,455, 		//	39	'O'
	685,745,369,455, 		//	40	'P'
	120,182,369,455, 		//	41	'Q'		
	308,370,369,455, 		//	42	'R'
	183,245,289,365, 		//	43	'S'
	371,433,369,455, 		//	44	'T'
	497,558,369,455, 		//	45	'U'
	308,370,209,285, 		//	46	'V'
	183,245,369,455, 		//	47	'W'  
	183,245,209,285, 		//	48	'X'   
	434,496,369,455, 		//	49	'Y' 
	120,182,209,285, 		//	50	'Z'
};                              
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#define tch_testset_no	51
const u16 tch_testset_tb[4*tch_testset_no]={
	 42,122,788,846, 		//	00	home
	775,841,788,846, 		//	01	back
	130,400,700,760, 		//	02	seq no
	443,485,700,760, 		//	03	seq no up
	400,442,700,760, 		//	04	seq no dn
	130,400,620,690, 		//	05	strip
	130,520,545,620, 		//	06	sample id
	480,750,700,760, 		//	07	color
	480,750,620,690, 		//	08	turbi
	246,364,120,185, 		//	09	SAVE		
	499,614,120,185,  		//	10	CLOSE
	685,745,289,365, 		//	11	'<'		14번째줄
	559,620,209,285, 		//	12	CLEAR
	621,745,209,285, 		//	13	SPACEBAR
	685,745,459,545, 		//	14	'0'		17번째줄
	120,182,459,545, 		//	15	'1'
	183,245,459,545, 		//	16	'2'
	246,307,459,545, 		//	17	'3'
	308,370,459,545, 		//	18	'4'
	371,433,459,545, 		//	19	'5'
	434,496,459,545, 		//	20	'6'
	497,558,459,545, 		//	21	'7'
	559,620,459,545, 		//	22	'8'
	621,684,459,545, 		//	23	'9'
	120,182,289,365, 		//	24	'A'   	27번째줄
	371,433,209,285, 		//	25	'B'
	246,307,209,285, 		//	26	'C'
	246,307,289,365, 		//	27	'D'
	246,307,369,455, 		//	28	'E'
	308,370,289,365, 		//	29	'F'
	371,433,289,365, 		//	30	'G'
	434,496,289,365, 		//	31	'H' 
	559,620,369,455, 		//	32	'I'
	497,558,289,365, 		//	33	'J'
	559,620,289,365, 		//	34	'K'
	621,684,289,365, 		//	35	'L'
	497,558,209,285, 		//	36	'M'   
	434,496,209,285, 		//	37	'N'
	621,684,369,455, 		//	38	'O'
	685,745,369,455, 		//	39	'P'
	120,182,369,455, 		//	40	'Q'		
	308,370,369,455, 		//	41	'R'
	183,245,289,365, 		//	42	'S'
	371,433,369,455, 		//	43	'T'
	497,558,369,455, 		//	44	'U'
	308,370,209,285, 		//	45	'V'
	183,245,369,455, 		//	47	'W'  
	183,245,209,285, 		//	48	'X'   
	434,496,369,455, 		//	49	'Y' 
	120,182,209,285, 		//	50	'Z'
};                              
#define tch_qcset_strip_no	tch_testset_strip_no
#define tch_qcset_strip_tb	tch_testset_strip_tb

#define tch_testset_strip_no	22
const u16 tch_testset_strip_tb[4 * tch_testset_strip_no] = {
	 42,122,788,846, 		//	00	home
	775,841,788,846, 		//	01	back
	268,398,638,682, 		//	02			
	268,398,593,637, 		//	03			
	268,398,548,592, 		//	04			
	268,398,503,547, 		//	05 		
	268,398,458,502, 		//	06			
	268,398,413,457, 		//	07			
	268,398,368,412, 		//	08			
	268,398,323,367, 		//	09
	268,398,277,322, 		//	10			
	399,448,592,682, 		//	11  up
	399,448,277,367, 		//	12	dn		
	246,364,120,185, 		//	13	SAVE
	499,614,120,185,  		//	14	CLOSE
	130,400,700,760,  		//	15	SEQ
	443,485,700,760, 		//	16	seq no up
	400,442,700,760, 		//	17	seq no dn
	480,750,700,760, 		//	18	color
	480,750,620,690,  		//	19	Turbidity
	130,270,570,600,  		//	20	Sample ID1
	440,540,570,620  		//	21	Sample ID2
};
#define tch_testset_color_no	19
const u16 tch_testset_color_tb[4 * tch_testset_color_no] = {
	 42,122,788,846, 		//	00	home
	775,841,788,846, 		//	01	back
	610,750,718,762, 		//	02			
	610,750,673,717, 		//	03			
	610,750,628,672, 		//	04			
	610,750,583,627, 		//	05 		
	610,750,538,582, 		//	06			
	610,750,493,537, 		//	07			
	610,750,446,492, 		//	08			
	610,750,403,447, 		//	09
	610,750,357,402, 		//	10			
	246,364,120,185, 		//	11	SAVE		
	499,614,120,185,  		//	12	CLOSE
	130,400,700,760,  		//	13	SEQ
	443,485,700,760, 		//	14	seq no up
	400,442,700,760, 		//	15	seq no dn
	130,400,620,690, 		//	16	strip
	480,620,620,690,  		//	17	Turbidity
	130,520,545,620, 		//	18	sample id
};
#define tch_testset_turbi_no	19
const u16 tch_testset_turbi_tb[4 * tch_testset_turbi_no] = {
	 42,122,788,846, 		//	00	home
	775,841,788,846, 		//	01	back
	610,750,643,687, 		//	02			
	610,750,598,642, 		//	03			
	610,750,553,597, 		//	04			
	610,750,508,552, 		//	05 		
	610,750,463,507, 		//	06			
	610,750,418,462, 		//	07			
	610,750,373,417, 		//	08			
	610,750,328,372, 		//	09
	610,750,282,327, 		//	10			
	246,364,120,185, 		//	11	SAVE		
	499,614,120,185,  		//	12	CLOSE
	130,400,700,760,  		//	13	SEQ
	443,485,700,760, 		//	14	seq no up
	400,442,700,760, 		//	15	seq no dn
	130,400,620,690, 		//	16	strip
	480,750,700,760,  		//	17	Color
	130,520,545,620, 		//	18	sample id
};
#define tch_testwait_no	11
const u16 tch_testwait_tb[4*tch_testwait_no]={
	 42,122,788,846, 		//	00	home
	775,841,788,846, 		//	01	back
	130,400,700,760, 		//	02	seq no
	130,400,620,690, 		//	03	strip
	130,520,545,620, 		//	04	sample id
	480,750,700,760, 		//	05	color
	480,750,620,690, 		//	06	turbi
	246,364,120,185, 		//	07	SAVE		
	499,614,120,185,  		//	08	CLOSE
	443,485,700,760, 		//	09	seq no up
	400,442,700,760, 		//	10	seq no dn
};

//QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ
#define tch_qcset_no	52
const u16 tch_qcset_tb[4 * tch_qcset_no] = {
	 42,122,788,846, 		//	00	home
	775,841,788,846, 		//	01	back
	130,400,700,760, 		//	02	seq no
	443,485,700,760, 		//	03	seq no up
	400,442,700,760, 		//	04	seq no dn
	130,400,620,690, 		//	05	strip
	130,275,565,605, 		//	06	neg/pos Toggle
	275,405,565,605, 		//	07	negative
	405,535,565,605, 		//	08	Positive
	480,750,700,760, 		//	09	lot no
	480,750,620,690, 		//	10	turbi => no use
	246,364,120,185, 		//	11	SAVE		
	499,614,120,185,  		//	12	CLOSE
	685,745,289,365, 		//	13	'<'		14��°��
	559,620,209,285, 		//	14	CLEAR
	621,745,209,285, 		//	15	SPACEBAR
	685,745,459,545, 		//	16	'0'		17��°��
	120,182,459,545, 		//	17	'1'
	183,245,459,545, 		//	18	'2'
	246,307,459,545, 		//	19	'3'
	308,370,459,545, 		//	20	'4'
	371,433,459,545, 		//	21	'5'
	434,496,459,545, 		//	22	'6'
	497,558,459,545, 		//	23	'7'
	559,620,459,545, 		//	24	'8'
	621,684,459,545, 		//	25	'9'
	120,182,289,365, 		//	26	'A'   	27��°��
	371,433,209,285, 		//	27	'B'
	246,307,209,285, 		//	28	'C'
	246,307,289,365, 		//	29	'D'
	246,307,369,455, 		//	30	'E'
	308,370,289,365, 		//	31	'F'
	371,433,289,365, 		//	32	'G'
	434,496,289,365, 		//	33	'H' 
	559,620,369,455, 		//	34	'I'
	497,558,289,365, 		//	35	'J'
	559,620,289,365, 		//	36	'K'
	621,684,289,365, 		//	37	'L'
	497,558,209,285, 		//	38	'M'   
	434,496,209,285, 		//	39	'N'
	621,684,369,455, 		//	40	'O'
	685,745,369,455, 		//	41	'P'
	120,182,369,455, 		//	42	'Q'		
	308,370,369,455, 		//	43	'R'
	183,245,289,365, 		//	44	'S'
	371,433,369,455, 		//	45	'T'
	497,558,369,455, 		//	46	'U'
	308,370,209,285, 		//	47	'V'
	183,245,369,455, 		//	48	'W'  
	183,245,209,285, 		//	49	'X'   
	434,496,369,455, 		//	50	'Y' 
	120,182,209,285, 		//	51	'Z'
};

#define tch_qcwait_no	12
const u16 tch_qcwait_tb[4 * tch_qcwait_no] = {
	 42,122,788,846, 		//	00	home
	775,841,788,846, 		//	01	back
	130,400,700,760, 		//	02	seq no
	130,400,620,690, 		//	03	strip
	135,270,565,605, 		//	04	neg/pos Toggle
	270,405,565,605, 		//	05	Negative
	405,540,565,605, 		//	06	Positive
	480,750,700,760, 		//	07	lot no
	246,364,120,185, 		//	08	SAVE		
	499,614,120,185,  		//	09	CLOSE
	443,485,700,760, 		//	10	seq no up
	400,442,700,760, 		//	11	seq no dn
};
//			    	    012345678901234567890123456789012345
const char alpa_tb[] = "0123456789QWERTYUIOPASDFGHJKLZXCVBNM";
                                
#define tch_measure_no	9       
const u16 tch_measure_tb[4*tch_measure_no]={
	  2,122,788,846, 		//home
	775,841,788,846, 		//back
	  8,350,570,660,		//operator id
	  8,350,480,570,		//strip
	  8,350,390,480,		//Unit
	  8,350,300,390,		//Mark Plus
	  8,350,210,300,		//Mark positive
	390,640,250,650,		//test
	640,840,250,650 		//QC
};                              


#define tch_QC_no	23
const u16 tch_QC_tb[4 * tch_QC_no] = {
	 42,122,788,846, 		//00	home
	775,841,788,846, 		//01	back
	 25,260,689,752, 		//02	ListOfSample
	220,262,612,662, 		//03	up
	220,262,118,180, 		//04	down
	280,560,690,760, 		//05	SEQ
	280,560,620,690, 		//06	strip
	270,405,565,630, 		//07	neg/pos Toggle
	405,555,565,630, 		//08	negative
	555,700,565,630, 		//09	Positive
	580,850,700,762, 		//10	lot no
	10, 240, 640, 690,		//11	1
	10, 240, 590, 640,		//12	2
	10, 240, 540, 590,		//13	3
	10, 240, 500, 540,		//14	4
	10, 240, 450, 500,		//15	5
	10, 240, 400, 450,		//16	6
	10, 240, 350, 400,		//17	7
	10, 240, 300, 350,		//18	8
	10, 240, 250, 300,		//19	9
	10, 240, 200, 250,		//20	10
	10, 240, 150, 200,		//21	11
	10, 240, 100, 150		//22	12
};

#define tch_test_no	22
const u16 tch_test_tb[4*tch_test_no]={
	 42,122,788,846, 		//00	home
	775,841,788,846, 		//01	back
	 25,260,689,752, 		//02	ListOfSample
	220,262,612,662, 		//03	up
	220,262,118,180, 		//04	down
	280,560,690,760, 		//05	SEQ
	280,560,620,690, 		//06	strip
	280,680,550,620, 		//07	sampleID
	580,860,690,760, 		//08	color
	580,860,620,690, 		//09	trub
	10, 240, 640, 690,		//10	1
	10, 240, 590, 640,		//11	2
	10, 240, 540, 590,		//12	3
	10, 240, 500, 540,		//13	4
	10, 240, 450, 500,		//14	5
	10, 240, 400, 450,		//15	6
	10, 240, 350, 400,		//16	7
	10, 240, 300, 350,		//17	8
	10, 240, 250, 300,		//18	9
	10, 240, 200, 250,		//19	10
	10, 240, 150, 200,		//20	11
	10, 240, 100, 150		//21	12
};

#define tch_sysset_no	23
u16 tch_sysset_tb[4 * tch_sysset_no] = {
	42,122,788,846, 		//00	home
   775,850,788,846, 		//01	back
   750,860, 45,110, 		//02	save
   142,250,585,650, 		//03	Print 1
   251,386,585,650, 		//04	Print 2
   387,484,585,650, 		//05	Print 0
   142,250,500,560, 		//06	USB 9600
   251,386,500,560, 		//07	USB 38400
   387,484,500,560, 		//08	USB 115200
   142,250,405,460, 		//09	Serial 9600
   251,386,405,460, 		//10	Serial 38400
   387,484,405,460, 		//11	Serial 115200
   142,250,295,365, 		//12	Sound On
   251,386,295,365, 		//13	Sound Off
   142,250,195,265, 		//14	Language 1
   251,386,195,265, 		//15	Language 2
   142,250,110,170, 		//16	Reset On?
   251,386,110,170, 		//17	Reset Off?
   565,685,325,380, 		//18	Sample ID On
   685,805,325,380, 		//19	Sample ID Off
   570,690,240,320, 		//20	Auto ID On
   690,820,240,320, 		//21	Auto ID Off
   570,820,150,220			//22	Maintenance
};

#define tch_syschk_no	11
const u16 tch_syschk_tb[4*tch_syschk_no]={
	 42,122,788,846, 		//home
	775,841,788,846, 		//back
	750,860, 45,110, 		//start
	450,660,535,580, 		//Plate
	450,660,490,535, 		//Waste Box
	450,660,445,490, 		//Moving Table
	450,660,400,445, 		//Push Bar
	450,660,355,400, 		//Sensor Move
	450,660,310,355, 		//Red
	450,660,275,310, 		//Green
	450,660,230,275 		//Blue
};

#define tch_syschk_serial_no	12
const u16 tch_syschk_serial_tb[4*tch_syschk_serial_no]={
	 42,122,788,846, 		//home
	775,841,788,846, 		//back
	150,340,520,630, 		//Type 1
	360,540,520,630, 		//Type 2
	570,750,540,630, 		//Type 3
	160,340,400,520, 		//Type 4
	360,550,410,520, 		//Type 5
	570,750,420,520, 		//Type 6
	150,340,270,400, 		//Type 7
	350,540,270,400, 		//Type 8
	560,750,270,400, 		//Type 9
	750,875,50 ,120 		//Save
};

#define tch_syschk_fw_no		4
const u16 tch_syschk_fw_tb[4*tch_syschk_fw_no]={
	 42,122,788,846, 		//home
	775,841,788,846, 		//back
	115,390,290,480, 		//Yes
	490,770,290,480 		//Exit
};

#define tch_oper_no		15
const u16 tch_oper_tb[4*tch_oper_no]={
	 42,122,788,846, 		//home
	775,841,788,846, 		//back
	610,650,450,515, 		//up
	610,650,150,215, 		//dn
	420,550, 40,100, 		//new id
	590,710, 40,100, 		//log in
	750,870, 40,100, 		//delete
	 65,625,470,525, 		//Line 1
	 65,625,425,470,		//Line 2
	 65,625,380,425,		//Line 3
	 65,625,330,380,		//Line 4
	 65,625,280,330,		//Line 5
	 65,625,230,280,		//Line 6
	 65,625,180,230,		//Line 7
	 65,625,130,180,		//Line 8
};

#define tch_strip_no		16
const u16 tch_strip_tb[4*tch_strip_no]={
	 42,122,788,846, 		//00	home
	775,841,788,846, 		//01	back
	537,572,634,680, 		//02	up
	537,572,144,210, 		//03	dn
	310,530,660,710, 		//04	01
	310,530,610,660, 		//05	02
	310,530,560,610, 		//06	03
	310,530,510,560, 		//07	04
	310,530,460,510, 		//08	05
	310,530,410,460, 		//09	06
	310,530,360,410, 		//10	07
	310,530,310,360, 		//11	08
	310,530,260,310, 		//12	09
	310,530,210,260, 		//13	10
	310,530,160,210, 		//14	11
	310,530,110,160 		//15	12
};

#define tch_change_no		25
const u16 tch_change_tb[4*tch_change_no]={
	 42,122,788,846, 		//home
	775,841,788,846, 		//back
	 80,190,477,540, 		//uro
	190,300,477,540, 		//glu
	300,410,477,540, 		//bil
	 80,190,414,477, 		//ket
	190,300,414,477, 		//sg
	300,410,414,477, 		//bld
	 80,190,351,414, 		//ph
	190,300,351,414, 		//pro
	300,410,351,414, 		//nit
	 80,190,288,351, 		//leu
	190,300,288,351, 		//asa
	300,410,288,351, 		//na
	 80,190,228,288, 		//cre
	190,300,228,288, 		//cal
	460,800,540,590, 		//dgt1
	460,800,490,540, 		//dgt2
	460,800,440,490, 		//dgt3
	460,800,390,440, 		//dgt4
	460,800,340,390, 		//dgt5
	460,800,290,340, 		//dgt6
	460,800,240,290, 		//dgt7
	460,800,190,240, 		//dgt8
	460,800,140,190 		//dgt9
};

#define tch_dgtpr_no		15
const u16 tch_dgtpr_tb[4*tch_dgtpr_no]={
	 42,122,788,846, 		//00	home
	775,841,788,846, 		//01	back
	110,198,555,670, 		//02	1
	198,286,555,670, 		//03	2
	286,374,555,670, 		//04	3
	110,198,440,555, 		//05	4
	198,286,440,555, 		//06	5
	286,374,440,555, 		//07	6
	110,198,325,440, 		//08	7
	198,286,325,440, 		//09	8
	286,374,325,440, 		//10	9
	198,286,210,325, 		//11	0
	286,374,210,325, 		//12	<
	110,225,120,190, 		//13	save
	255,365,120,190 		//14	close
};

#define tch_maintanence_no	9
const u16 tch_maintanence_tb[4 * tch_maintanence_no] = {
	 42,122,788,846, 		//00	home
	775,841,788,846, 		//01	back
	142,385,585,650, 		//02	Calibration
	142,385,500,560, 		//03	Firmware Update
	142,385,405,460, 		//04	Image Update
	142,385,295,365, 		//05	Font Update
	142,385,195,265, 		//06	Serial Data
	 70,385,110,190, 		//07	Reset
//	251,385,110,170,		//08	Result
//	386,520,110,170,		//09	Print
	750,860, 45,110, 		//08	save
};
#define tch_unit_no	9
u16 tch_unit_tb[4 * tch_unit_no] = {
	42,122,788,846, 		//00	home
   775,821,788,846, 		//01	back
   750,860, 45,110, 		//02	save
   185,300,535,600, 		//03	Unit:Conv.
   300,430,535,605, 		//04	Unit:S.I.
   180,300,350,415, 		//05	plus On
   300,425,355,425, 		//06	plus Off
   175,300,170,235, 		//07	mark On
   300,420,175,240 		//08	mark Off
};

#define tch_mem_printsend_no		4
const u16 tch_mem_printsend_tb[4*tch_mem_printsend_no]={
	 42,122,788,846, 		//home
	775,841,788,846, 		//back
	300,400,260,380, 		//confirm
	400,550,260,380 		//cancel
};

#define tch_input_password_no	16
const u16 tch_input_password_tb[4*tch_date_set_no]={
	 42,122,788,846, 				//home	0
	775,841,788,846, 				//back	1
	467-126,542-126,566,665, 		//'1'	2
	548-126,628-126,566,665, 		//'2'	3
	630-126,707-126,566,665,		//'3'	4
	467-126,542-126,465,560, 		//'4'	5
	548-126,628-126,465,560, 		//'5'	6 
	630-126,707-126,465,560,		//'6'	7
	467-126,542-126,330,460, 		//'7'	8
	548-126,628-126,330,460, 		//'8'	9
	630-126,707-126,330,460,		//'9'	10
	548-126,628-126,218,325, 		//'0'	11
	467-126,542-126,218,325, 		//AM/PM	12
	630-126,707-126,218,325,		//back	13
	459-126,568-126,122,200, 		//save	14
	603-126,716-126,122,200 		//CLOSE	15
};

#define tch_confirm_no		3
const u16 tch_confirm_tb[4*tch_confirm_no]={
	 42,122,788,846, 		//home
	775,841,788,846, 		//back
	400,500,260,380, 		//confirm
};


#endif 
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
// xs, xe, ys, ye

