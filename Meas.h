
#define STRIP_OFFSET		33				//was 27
#define MAX_PAD_NO			14				//was 11

#define CNT_MEAS			4584			// ((((1146*8)/12)*3)*2)
#define MaxRawData			764				// CNT_MEAS/6		// 764
#define MULTI				1
#define COMBO				2
#define RESULT_SIZE_ORG		262				// 261+NEGPOS
#define RESULT_SIZE			178				// 

#define ADC_BLU				0xA0			// LSB 5bit
#define ADC_RED				0xC0			// LSB 5bit
#define ADC_GRN				0xE0			// LSB 5bit
#define ADC_LOW				0x00			// LSB 7bit
#define RUN_CMD				'R'
#define BIAS_CMD			'B'
#define TEST_CMD			'T'

#define ADC_END1			0x85			// 8bit
#define ADC_END2			0x2A			// 8bit
#define BIAS_RED			0x81			// 8bit
#define BIAS_GRN			0x82			// 8bit
#define BIAS_BLU			0x83			// 8bit
#define BIAS_LOW			0x50			// LSB 4bit			

/******************************************************************************/
#define ACpnt				18
#define PCpnt				19
#define CCpnt				20

#define MEAS_NAME_SIZE		10
#define errorStackSize		250
#define Err_PlateOut			1
#define Err_WasteOut			2
#define Err_WasteFull			3
#define Err_MemoryFull			5
#define Err_NoStrip				6
#define Err_MovingTableOut		7
#define Err_PushBarMove			8
#define Err_SensorMove			9
#define Err_SensorValue			10
#define Err_USBOut				11
#define Err_USBFail				12

//===================================================================
void span_curve(void);
void get_pad_data(void);
void run_measure(void);
void send_result(void);
void conv_result(void);
void calc_ratio2(void);
void print_result(void);
void print_sound(void);
void Send720(void);
void SendYD(u8 flag);
void SendBayer(void);
int  send_result_store(u16 ssno, u16 eeno);
short view_result_store(u16 view_seqno);
void delete_result_store(u16 ssno, u16 eeno);
void make_result(void);
void conv_result_store(void);

void send_rawdata(void);
void send_printdata(void);
void correction_curve(void);

void IncreseSampleID(void);

void AddError(u16 ErrorCode);

#define MAX_buffRESULT	1024

#define GENERAL			0			//test type
#define QC_MEMORY		1			//test type
#define NUMBER_SEARCH	0			//memory search
#define SAMPLE_SEARCH	1			//memory search

#define YOFF_SYS_CHK			80
#define RSCX_OFFSET	480
#define FAIL_OFFSET 8
#define StripCount	100
//===================================================================
#ifdef	_MEAS_C_
u16 StripPosition[16];
u8 	pad;
u16 buffRED1[MaxRawData+5], buffGRN1[MaxRawData+5], buffBLU1[MaxRawData+5];
u16 spanRED[MaxRawData],  	spanGRN[MaxRawData],  	spanBLU[MaxRawData];
u16	ref_red_buf[MaxRawData],ref_grn_buf[MaxRawData],ref_blu_buf[MaxRawData];
u16 dataRED1[MAX_PAD_NO+1], dataGRN1[MAX_PAD_NO+1], dataBLU1[MAX_PAD_NO+1];
u16	WHITE_RED, WHITE_GRN, WHITE_BLU;
u8  STRIP_INFO[STRIP_OFFSET];
u8	store_ErrCode, store_NEGPOS;
u16	store_COLOR_NO, store_TURBI_NO;
u8 temp_result[20];
u8 temp_solution[100];
u16	test_loscnt, curcnt;
u16	PARA_NEW[20][11], TEMP_PARA_NEW[20][11];
u8  m_final_results[MAX_PAD_NO+1];   //���� ��� (0- ff)
u8 inputCnt;//210713
u8  weight[22];
u8  buffRESULT[MAX_buffRESULT];	
u16	m_para_min,	m_para_max;
short datPRO, datALB, datCRE, datCAL;
u16 END_RESULT;
u8 errorStack[errorStackSize], errorCnt, errorSwitch;
u8  MEASyear,MEASmonth,MEASday,MEAShour,MEASmin,MEASsec;
char MEASoperid[MAX_OPERID+1];
char MEASuserid[MAX_USERID+1];
char MEASstripname[STRIP_NAME_SIZE+1];
char MEASstripinfo[MAX_PAD_NO+1+1];
char MEAS_results[MAX_PAD_NO];
u8	MEASstripno;
u16	MEASseqno;
u16	srt_seqno, end_seqno, srt_qcno, end_qcno;
u16	test_type;		//0:general, 1:OC
u16	search_theme;	//0:NUMBER_SEARCH, 1:sample(user) ID, 2:operator ID
u8 stripCnt, trayFlag, trayFullFlag, plateFlag, measFlag;
u8 plateCheck, trayCheck, tableCheck, pushbarCheck, sensorCheck, rCheck, gCheck, bCheck, checkFlag, noStripFlag;
u16 temp_modex2[errorStackSize];
                 	//  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
const char pt_size[] = {0,0,1,0,0,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

#else 
extern u16 	StripPosition[16];
extern u8	pad;
extern u16  buffRED1[MaxRawData+5], buffGRN1[MaxRawData+5], buffBLU1[MaxRawData+5];
extern u16  spanRED[MaxRawData], 	spanGRN[MaxRawData], 	spanBLU[MaxRawData];
extern u16	ref_red_buf[MaxRawData],ref_grn_buf[MaxRawData],ref_blu_buf[MaxRawData];
extern u16 	dataRED1[MAX_PAD_NO+1], dataGRN1[MAX_PAD_NO+1], dataBLU1[MAX_PAD_NO+1];
extern u16	WHITE_RED, WHITE_GRN, WHITE_BLU;
extern u8 	STRIP_INFO[STRIP_OFFSET];
extern u8	store_ErrCode, store_NEGPOS;
extern u16	store_COLOR_NO, store_TURBI_NO;
extern u8 temp_result[20];
extern u8 temp_solution[100];
extern u16	test_loscnt, curcnt;
extern u16	PARA_NEW[20][11], TEMP_PARA_NEW[20][11];
extern u8  	m_final_results[MAX_PAD_NO+1];   //���� ��� (0- ff)
extern u8 inputCnt;//210713
extern u8  	weight[22];
extern u8  	buffRESULT[MAX_buffRESULT];	
extern u8 errorStack[errorStackSize], errorCnt, errorSwitch;
extern u8  	MEASyear,MEASmonth,MEASday,MEAShour,MEASmin,MEASsec;
extern char MEASoperid[MAX_OPERID+1];
extern char MEASuserid[MAX_USERID+1];
extern char MEASstripname[MEAS_NAME_SIZE+1];
extern char MEASstripinfo[MAX_PAD_NO+1+1];
extern char MEAS_results[MAX_PAD_NO];
extern u8	MEASstripno;
extern u16	MEASseqno;
extern u16	srt_seqno, end_seqno, srt_qcno, end_qcno;
extern u16	test_type;								//0:general, 1:OC
extern u16	search_theme;	//0:NUMBER_SEARCH, 1:sample(user) ID, 2:operator ID
extern u8 stripCnt, trayFlag, trayFullFlag,plateFlag, measFlag;
extern u8 plateCheck, trayCheck, tableCheck, pushbarCheck, sensorCheck, rCheck, gCheck, bCheck, checkFlag, noStripFlag;
extern u16 temp_modex2[errorStackSize];
#endif

#ifdef	_MEAS_C_
/*                           01234567890123456789012345678901 */
//const char *PrintLine1  = "= = = = SEQ# : 0000  = = = = = =";
//const char *PrintLine2  = " 2000/01/01          AM 00:00:00";
//const char *PrintLine3  = "                ";
//const char *PrintLine4  = " ID:            ";
//const char *PrintLine6  = "= = = = = = = = = = = = = = = = ";

//=================================================================================================
const char alb_item[5][5]= {	
//	{0,3,3,3,0},	//0
	{4,3,3,3,0},	//0
	{1,2,2,3,0},	//1
	{1,2,2,2,0},	//2
	{1,1,2,2,0},	//3
	{1,1,1,2,0}		//4
};
/***************************************************************************************/
const char pro_item[6][5]= {	
	{1,1,1,1,1},	//0
	{2,2,2,1,1},	//1
	{2,2,2,2,1},	//2
	{2,2,2,2,2},	//3
	{2,2,2,2,2},	//4
	{2,2,2,2,2}		//5
};
/***************************************************************************************/
const char albumin_tb[5][32]= {	
	{' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},	//0
	{' ',' ',' ',' ',' ','N','o','r','m','a','l',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},	//1
	{' ',' ',' ',' ',' ','A','b','n','o','r','m','a','l',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},	//2
	{' ',' ',' ',' ',' ','H','i','g','h',' ',' ',' ',' ','a','b','n','o','r','m','a','l',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},	//3
	{' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '}	//4
};
/***************************************************************************************/
//const char *text_AC = " A:C ";
const char text_AC[] = {' ','A',':','C',' '};
//const char *text_PC = " P:C ";
const char text_PC[] = {' ','P',':','C',' '};
//const char *text_CC = " C:C ";
const char text_CC[] = {' ','C',':','C',' '};
/***************************************************************************************/
const u8 normal_range[14][8]={	//nega reason
// min, max
	{ 0,0,0,0,0,0,0,1 },	//{ 1,7 },//color
	{ 0,0,1,1,1,1,1,1 },	//{ 1,2 },//uro
	{ 0,1,1,1,1,1,1,1 },	//{ 1,1 },//glu
	{ 0,1,1,1,1,1,1,1 },	//{ 1,1 },//bil
	{ 0,1,1,1,1,1,1,1 },	//{ 1,1 },//ket
	{ 0,0,0,0,0,0,0,1 },	//{ 1,7 },//sg
	{ 0,1,1,1,1,1,1,1 },	//{ 1,1 },//bld
	{ 0,0,0,0,0,1,1,1 },	//{ 1,5 },//ph
	{ 0,1,1,1,1,1,1,1 },	//{ 1,1 },//pro
	{ 0,1,1,1,1,1,1,1 },	//{ 1,1 },//nit
	{ 0,1,1,1,1,1,1,1 },	//{ 1,1 },//leu
	{ 0,1,1,1,1,1,1,1 },	//{ 1,1 } //asa
	{ 0,0,0,0,0,0,0,0 },	//{ 1,1 } //ALB
	{ 0,0,0,0,0,0,0,0 }		//{ 1,1 } //CRE
};
/***************************************************************************************/
const u8 item_string[21][8] = {
	{' ',' ',' ',' ','C','O','L',' '},	//0
	{' ',' ',' ',' ','U','R','O',' '},	//1
	{' ',' ',' ',' ','G','L','U',' '},	//2
	{' ',' ',' ',' ','B','I','L',' '},	//3
	{' ',' ',' ',' ','K','E','T',' '},	//4
	{' ',' ',' ',' ','S','.','G',' '},	//5
	{' ',' ',' ',' ','B','L','D',' '},	//6
	{' ',' ',' ',' ','p','H',' ',' '},	//7
	{' ',' ',' ',' ','P','R','O',' '},	//8
	{' ',' ',' ',' ','N','I','T',' '},	//9
	{' ',' ',' ',' ','L','E','U',' '},	//A	10
	{' ',' ',' ',' ','A','S','A',' '},	//B	11
	{' ',' ',' ',' ','A','L','B',' '},	//C	12
	{' ',' ',' ',' ','C','R','E',' '},	//D	13
	{' ',' ',' ',' ','M','D','A',' '},	//E	14
	{' ',' ',' ',' ','B','H','B',' '},	//F	15
	{' ',' ',' ',' ','C','A','L',' '},	//G	16
	{' ',' ',' ',' ','N','I','C',' '},	//H	17
	{' ',' ',' ',' ','A',':','C',' '},	//I 18
	{' ',' ',' ',' ','P',':','C',' '},	//J 19
	{' ',' ',' ',' ','C',':','C',' '}	//K 20
};      

const u8 item_string1[21][8] = {	//positive
	{'=','>',' ',' ','C','O','L',' '},
	{'=','>',' ',' ','U','R','O',' '},
	{'=','>',' ',' ','G','L','U',' '},
	{'=','>',' ',' ','B','I','L',' '},
	{'=','>',' ',' ','K','E','T',' '},
	{'=','>',' ',' ','S','.','G',' '},
	{'=','>',' ',' ','B','L','D',' '},
	{'=','>',' ',' ','p','H',' ',' '},
	{'=','>',' ',' ','P','R','O',' '},
	{'=','>',' ',' ','N','I','T',' '},
	{'=','>',' ',' ','L','E','U',' '},
	{'=','>',' ',' ','A','S','A',' '},
	{' ',' ',' ',' ','A','L','B',' '},	//C
	{' ',' ',' ',' ','C','R','E',' '},	//D
	{' ',' ',' ',' ','M','D','A',' '},	//E
	{' ',' ',' ',' ','B','H','B',' '},	//F
	{' ',' ',' ',' ','C','A','L',' '},	//G
	{' ',' ',' ',' ','N','I','C',' '},	//H
	{' ',' ',' ',' ','A',':','C',' '},	//I
	{' ',' ',' ',' ','P',':','C',' '},	//J
	{' ',' ',' ',' ','C',':','C',' '} 	//K
};      

const char alb_tb_text[5][5][8] = {
	//    0123456789012345678901234567890	
		{{"mg/g    "},	{"Retest  "},	{"300     "},	{"800     "},	{"1500    "}},//0
		{{"mg/g    "} ,	{"20      "},	{"60      "},	{"160     "},	{"300     "}},//1
		{{"mg/g    "},	{"10      "},	{"30      "},	{"80      "},	{"150     "}},//2
		{{"mg/g    "},	{"5       "},	{"15      "},	{"40      "},	{"75      "}},//3
		{{"mg/g    "},	{"3.3     "},	{"10      "},	{"26.7    "},	{"50      "}},//4
};

const char alb_tb_text_si[5][5][8] = {
	//    0123456789012345678901234567890	
		{{"mg/mmol "},	{"Retest  "},	{"33.3    "},	{"88.9    "},	{"166.7   "}},//0
		{{"mg/mmol "},	{"2.3     "},	{"6.8     "},	{"18.2    "},	{"34.1    "}},//1
		{{"mg/mmol "},	{"1.1     "},	{"3.4     "},	{"9.1     "},	{"17.0    "}},//2
		{{"mg/mmol "},	{"0.6     "},	{"1.7     "},	{"4.5     "},	{"8.5     "}},//3
		{{"mg/mmol "},	{"0.4     "},	{"1.1     "},	{"3.0     "},	{"5.7     "}},//4
};

const char yd_table_cybow[17][9][9] = {
	{{"mg/dl   "}, {"+-  norm"}, {"+-     1"}, {"+      2"}, {"++     4"}, {"+++    8"}, {"        "},{"        "}, {"        "}},//URO
	{{"mg/dl   "}, {"-    neg"}, {"+-   100"}, {"+    250"}, {"++   500"}, {"+++ 1000"}, {"        "},{"        "}, {"        "}},//GLU
	{{"mg/dl   "}, {"-    neg"}, {"+      1"}, {"++     2"}, {"+++    3"}, {"        "}, {"        "},{"        "}, {"        "}},//BIL
	{{"mg/dl   "}, {"-    neg"}, {"+-     5"}, {"+     15"}, {"++    40"}, {"+++  100"}, {"        "},{"        "}, {"        "}},//KET
	{{"        "}, {"   1.000"}, {"   1.005"}, {"   1.010"}, {"   1.015"}, {"   1.020"}, {"   1.025"},{"   1.030"}, {"        "}},//SG
	{{"RBC/ul  "}, {"-    neg"}, {"+-     5"}, {"+     10"}, {"++    50"}, {"+++  250"}, {"        "},{"        "}, {"        "}},//BLD
	{{"        "}, {"     5.0"}, {"     6.0"}, {"     6.5"}, {"     7.0"}, {"     8.0"}, {"     9.0"},{"        "}, {"        "}},//pH
	{{"mg/dl   "}, {"-    neg"}, {"+-    15"}, {"+     30"}, {"++   100"}, {"+++  300"}, {"++++1000"},{"        "}, {"        "}},//PRO
	{{"        "}, {"-    neg"}, {"+    pos"}, {"        "}, {"        "}, {"        "}, {"        "},{"        "}, {"        "}},//NIT
	{{"WBC/ul  "}, {"-    neg"}, {"+     25"}, {"++    75"}, {"+++  500"}, {"        "}, {"        "},{"        "}, {"        "}},//LEU
	{{"mg/dl   "}, {"-    neg"}, {"+     20"}, {"++    40"}, {"        "}, {"        "}, {"        "},{"        "}, {"        "}},//ASA
	{{"mg/l    "}, {"      10"}, {"      30"}, {"      80"}, {"     150"}, {"        "}, {"        "},{"        "}, {"        "}},//ALB
	{{"mg/dl   "}, {"      10"}, {"      50"}, {"     100"}, {"     200"}, {"     300"}, {"        "},{"        "}, {"        "}},//CRE
	{{"        "}, {"        "}, {"        "}, {"        "}, {"        "}, {"        "}, {"        "},{"        "}, {"        "}},//Empty
	{{"        "}, {"        "}, {"        "}, {"        "}, {"        "}, {"        "}, {"        "},{"        "}, {"        "}},//Empty
	{{"        "}, {"        "}, {"        "}, {"        "}, {"        "}, {"        "}, {"        "},{"        "}, {"        "}},//Empty
	{{"        "}, {"        "}, {"        "}, {"        "}, {"        "}, {"        "}, {"        "},{"        "}, {"        "}},//Empty
};

const char yd_table_combostik[17][9][9] = {
	{{"mg/dl   "}, {"+-  norm"}, {"+-     1"}, {"+      2"}, {"++     4"}, {"+++    8"}, {"        "},{"        "}, {"        "}},//URO
	{{"mg/dl   "}, {"-    neg"}, {"+     50"}, {"++   100"}, {"+++  300"}, {"++++1000"}, {"        "},{"        "}, {"        "}},//GLU
	{{"mg/dl   "}, {"-    neg"}, {"+      1"}, {"++     2"}, {"+++    3"}, {"        "}, {"        "},{"        "}, {"        "}},//BIL
	{{"mg/dl   "}, {"-    neg"}, {"+-     5"}, {"+     15"}, {"++    40"}, {"+++  100"}, {"        "},{"        "}, {"        "}},//KET
	{{"        "}, {"   1.000"}, {"   1.005"}, {"   1.010"}, {"   1.015"}, {"   1.020"}, {"   1.025"},{"   1.030"}, {"        "}},//SG
	{{"RBC/ul  "}, {"-    neg"}, {"+-     5"}, {"+     10"}, {"++    50"}, {"+++  250"}, {"        "},{"        "}, {"        "}},//BLD
	{{"        "}, {"     5.0"}, {"     6.0"}, {"     6.5"}, {"     7.0"}, {"     8.0"}, {"     9.0"},{"        "}, {"        "}},//pH
	{{"mg/dl   "}, {"-    neg"}, {"+-    15"}, {"+     30"}, {"++   100"}, {"+++  300"}, {"++++1000"},{"        "}, {"        "}},//PRO
	{{"        "}, {"-    neg"}, {"+    pos"}, {"        "}, {"        "}, {"        "}, {"        "},{"        "}, {"        "}},//NIT
	{{"WBC/ul  "}, {"-    neg"}, {"+     25"}, {"++    75"}, {"+++  500"}, {"        "}, {"        "},{"        "}, {"        "}},//LEU
	{{"mg/dl   "}, {"-    neg"}, {"+     20"}, {"++    40"}, {"        "}, {"        "}, {"        "},{"        "}, {"        "}},//ASA
	{{"mg/l    "}, {"      10"}, {"      30"}, {"      80"}, {"     150"}, {"        "}, {"        "},{"        "}, {"        "}},//ALB
	{{"mg/dl   "}, {"      10"}, {"      50"}, {"     100"}, {"     200"}, {"     300"}, {"        "},{"        "}, {"        "}},//CRE
	{{"        "}, {"        "}, {"        "}, {"        "}, {"        "}, {"        "}, {"        "},{"        "}, {"        "}},//Empty
	{{"        "}, {"        "}, {"        "}, {"        "}, {"        "}, {"        "}, {"        "},{"        "}, {"        "}},//Empty
	{{"        "}, {"        "}, {"        "}, {"        "}, {"        "}, {"        "}, {"        "},{"        "}, {"        "}},//Empty
	{{"        "}, {"        "}, {"        "}, {"        "}, {"        "}, {"        "}, {"        "},{"        "}, {"        "}},//Empty
};

const char yd_table_bayer[17][9][9] = {
	{{"mg/dl   "}, {"+-  norm"}, {"+-     1"}, {"+      2"}, {"++     4"}, {"+++    8"}, {"        "},{"        "}, {"        "}},//URO
	{{"mg/dl   "}, {"-    neg"}, {"+-   100"}, {"+    250"}, {"++   500"}, {"+++ 1000"}, {"++++2000"},{"        "}, {"        "}},//GLU
	{{"mg/dl   "}, {"-    neg"}, {"+      1"}, {"++     2"}, {"+++    3"}, {"        "}, {"        "},{"        "}, {"        "}},//BIL
	{{"mg/dl   "}, {"-    neg"}, {"+-     5"}, {"+     15"}, {"++    40"}, {"+++   80"}, {"++++ 160"},{"        "}, {"        "}},//KET
	{{"        "}, {"   1.000"}, {"   1.005"}, {"   1.010"}, {"   1.015"}, {"   1.020"}, {"   1.025"},{"   1.030"}, {"        "}},//SG
	{{"RBC/ul  "}, {"-    neg"}, {"+-     5"}, {"+     25"}, {"++    80"}, {"+++  200"}, {"        "},{"        "}, {"        "}},//BLD
	{{"        "}, {"     5.0"}, {"     6.0"}, {"     6.5"}, {"     7.0"}, {"     7.5"}, {"     8.0"},{"     8.5"}, {"        "}},//pH
	{{"mg/dl   "}, {"-    neg"}, {"+-    15"}, {"+     30"}, {"++   100"}, {"+++  300"}, {"++++1000"},{"        "}, {"        "}},//PRO
	{{"        "}, {"-    neg"}, {"+    pos"}, {"        "}, {"        "}, {"        "}, {"        "},{"        "}, {"        "}},//NIT
	{{"WBC/ul  "}, {"-    neg"}, {"+-    25"}, {"+     70"}, {"++   125"}, {"+++  500"}, {"        "},{"        "}, {"        "}},//LEU
	{{"mg/dl   "}, {"-    neg"}, {"+     20"}, {"++    40"}, {"        "}, {"        "}, {"        "},{"        "}, {"        "}},//ASA
	{{"mg/l    "}, {"      10"}, {"      30"}, {"      80"}, {"     150"}, {"        "}, {"        "},{"        "}, {"        "}},//ALB
	{{"mg/dl   "}, {"      10"}, {"      50"}, {"     100"}, {"     200"}, {"     300"}, {"        "},{"        "}, {"        "}},//CRE
	{{"        "}, {"        "}, {"        "}, {"        "}, {"        "}, {"        "}, {"        "},{"        "}, {"        "}},//Empty
	{{"        "}, {"        "}, {"        "}, {"        "}, {"        "}, {"        "}, {"        "},{"        "}, {"        "}},//Empty
	{{"        "}, {"        "}, {"        "}, {"        "}, {"        "}, {"        "}, {"        "},{"        "}, {"        "}},//Empty
	{{"        "}, {"        "}, {"        "}, {"        "}, {"        "}, {"        "}, {"        "},{"        "}, {"        "}},//Empty
};


const char as720_table_cybow_conv[17][8][17] = {
	{{"norm    0.1mg/dl"}, {"  +-      1mg/dl"}, {"   +      2mg/dl"}, {"  ++      4mg/dl"}, {" +++      8mg/dl"}, {"                "},{"                "}, {"                "}},//URO
	{{"   -        neg "}, {"  +-    100mg/dl"}, {"   +    250mg/dl"}, {"  ++    500mg/dl"}, {" +++   1000mg/dl"}, {"                "},{"                "}, {"                "}},//GLU
	{{"   -        neg "}, {"   +      1mg/dl"}, {"  ++      2mg/dl"}, {" +++      3mg/dl"}, {"                "}, {"                "},{"                "}, {"                "}},//BIL
	{{"   -        neg "}, {"  +-      5mg/dl"}, {"   +     15mg/dl"}, {"  ++     40mg/dl"}, {" +++    100mg/dl"}, {"                "},{"                "}, {"                "}},//KET
	{{"          1.000 "}, {"          1.005 "}, {"          1.010 "}, {"          1.015 "}, {"          1.020 "}, {"          1.025 "},{"          1.030 "}, {"                "}},//SG
	{{"   -        neg "}, {"  +-     5RBC/ul"}, {"   +    10RBC/ul"}, {"  ++    50RBC/ul"}, {" +++   250RBC/ul"}, {"                "},{"                "}, {"                "}},//BLD
	{{"            5.0 "}, {"            6.0 "}, {"            6.5 "}, {"            7.0 "}, {"            8.0 "}, {"            9.0 "},{"                "}, {"                "}},//pH
	{{"   -        neg "}, {"  +-     15mg/dl"}, {"   +     30mg/dl"}, {"  ++    100mg/dl"}, {" +++    300mg/dl"}, {"++++   1000mg/dl"},{"                "}, {"                "}},//PRO
	{{"   -        neg "}, {"   +        pos "}, {"                "}, {"                "}, {"                "}, {"                "},{"                "}, {"                "}},//NIT
	{{"   -        neg "}, {"   +    25WBC/ul"}, {"  ++    75WBC/ul"}, {" +++   500WBC/ul"}, {"                "}, {"                "},{"                "}, {"                "}},//LEU
	{{"   -        neg "}, {"   +     20mg/dl"}, {"  ++     40mg/dl"}, {"                "}, {"                "}, {"                "},{"                "}, {"                "}},//ASA
	{{"         10mg/l "}, {"         30mg/l "}, {"         80mg/l "}, {"        150mg/l "}, {"                "}, {"                "},{"                "}, {"                "}},//ALB
	{{"        10mg/dl "}, {"        50mg/dl "}, {"       100mg/dl "}, {"       200mg/dl "}, {"       300mg/dl "}, {"                "},{"                "}, {"                "}},//CRE
	{{"                "}, {"                "}, {"                "}, {"                "}, {"                "}, {"                "},{"                "}, {"                "}},//Empty
	{{"                "}, {"                "}, {"                "}, {"                "}, {"                "}, {"                "},{"                "}, {"                "}},//Empty
	{{"                "}, {"                "}, {"                "}, {"                "}, {"                "}, {"                "},{"                "}, {"                "}},//Empty
	{{"                "}, {"                "}, {"                "}, {"                "}, {"                "}, {"                "},{"                "}, {"                "}},//Empty
};

const char as720_table_combostik_conv[17][8][17] = {
	{{"norm    0.1mg/dl"}, {"  +-      1mg/dl"}, {"   +      2mg/dl"}, {"  ++      4mg/dl"}, {" +++      8mg/dl"}, {"                "},{"                "}, {"                "}},//URO
	{{"   -        neg "}, {"   +     50mg/dl"}, {"  ++    100mg/dl"}, {" +++    300mg/dl"}, {"++++   1000mg/dl"}, {"                "},{"                "}, {"                "}},//GLU
	{{"   -        neg "}, {"   +      1mg/dl"}, {"  ++      2mg/dl"}, {" +++      3mg/dl"}, {"                "}, {"                "},{"                "}, {"                "}},//BIL
	{{"   -        neg "}, {"  +-      5mg/dl"}, {"   +     15mg/dl"}, {"  ++     40mg/dl"}, {" +++    100mg/dl"}, {"                "},{"                "}, {"                "}},//KET
	{{"          1.000 "}, {"          1.005 "}, {"          1.010 "}, {"          1.015 "}, {"          1.020 "}, {"          1.025 "},{"          1.030 "}, {"                "}},//SG
	{{"   -        neg "}, {"  +-     5RBC/ul"}, {"   +    10RBC/ul"}, {"  ++    50RBC/ul"}, {" +++   250RBC/ul"}, {"                "},{"                "}, {"                "}},//BLD
	{{"            5.0 "}, {"            6.0 "}, {"            6.5 "}, {"            7.0 "}, {"            8.0 "}, {"            9.0 "},{"                "}, {"                "}},//pH
	{{"   -        neg "}, {"  +-     15mg/dl"}, {"   +     30mg/dl"}, {"  ++    100mg/dl"}, {" +++    300mg/dl"}, {"++++   1000mg/dl"},{"                "}, {"                "}},//PRO
	{{"   -        neg "}, {"   +        pos "}, {"                "}, {"                "}, {"                "}, {"                "},{"                "}, {"                "}},//NIT
	{{"   -        neg "}, {"   +    25WBC/ul"}, {"  ++    75WBC/ul"}, {" +++   500WBC/ul"}, {"                "}, {"                "},{"                "}, {"                "}},//LEU
	{{"   -        neg "}, {"   +     20mg/dl"}, {"  ++     40mg/dl"}, {"                "}, {"                "}, {"                "},{"                "}, {"                "}},//ASA
	{{"         10mg/l "}, {"         30mg/l "}, {"         80mg/l "}, {"        150mg/l "}, {"                "}, {"                "},{"                "}, {"                "}},//ALB
	{{"        10mg/dl "}, {"        50mg/dl "}, {"       100mg/dl "}, {"       200mg/dl "}, {"       300mg/dl "}, {"                "},{"                "}, {"                "}},//CRE
	{{"                "}, {"                "}, {"                "}, {"                "}, {"                "}, {"                "},{"                "}, {"                "}},//Empty
	{{"                "}, {"                "}, {"                "}, {"                "}, {"                "}, {"                "},{"                "}, {"                "}},//Empty
	{{"                "}, {"                "}, {"                "}, {"                "}, {"                "}, {"                "},{"                "}, {"                "}},//Empty
	{{"                "}, {"                "}, {"                "}, {"                "}, {"                "}, {"                "},{"                "}, {"                "}},//Empty
};

const char as720_table_bayer_conv[17][8][17] = {
	{{"norm    0.1mg/dl"}, {"  +-      1mg/dl"}, {"   +      2mg/dl"}, {"  ++      4mg/dl"}, {" +++      8mg/dl"}, {"                "},{"                "}, {"                "}},//URO
	{{"   -        neg "}, {"  +-    100mg/dl"}, {"   +    250mg/dl"}, {"  ++    500mg/dl"}, {" +++   1000mg/dl"}, {"++++   2000mg/dl"},{"                "}, {"                "}},//GLU
	{{"   -        neg "}, {"   +      1mg/dl"}, {"  ++      2mg/dl"}, {" +++      3mg/dl"}, {"                "}, {"                "},{"                "}, {"                "}},//BIL
	{{"   -        neg "}, {"  +-      5mg/dl"}, {"   +     15mg/dl"}, {"  ++     40mg/dl"}, {" +++     80mg/dl"}, {"++++    160mg/dl"},{"                "}, {"                "}},//KET
	{{"          1.000 "}, {"          1.005 "}, {"          1.010 "}, {"          1.015 "}, {"          1.020 "}, {"          1.025 "},{"          1.030 "}, {"                "}},//SG
	{{"   -        neg "}, {"  +-     5RBC/ul"}, {"   +    25RBC/ul"}, {"  ++    80RBC/ul"}, {" +++   200RBC/ul"}, {"                "},{"                "}, {"                "}},//BLD
	{{"            5.0 "}, {"            6.0 "}, {"            6.5 "}, {"            7.0 "}, {"            7.5 "}, {"            8.0 "},{"            8.5 "}, {"                "}},//pH
	{{"   -        neg "}, {"  +-     15mg/dl"}, {"   +     30mg/dl"}, {"  ++    100mg/dl"}, {" +++    300mg/dl"}, {"++++   1000mg/dl"},{"                "}, {"                "}},//PRO
	{{"   -        neg "}, {"   +        pos "}, {"                "}, {"                "}, {"                "}, {"                "},{"                "}, {"                "}},//NIT
	{{"   -        neg "}, {"  +-    25WBC/ul"}, {"   +    70WBC/ul"}, {"  ++   125WBC/ul"}, {" +++   500WBC/ul"}, {"                "},{"                "}, {"                "}},//LEU
	{{"   -        neg "}, {"   +     20mg/dl"}, {"  ++     40mg/dl"}, {"                "}, {"                "}, {"                "},{"                "}, {"                "}},//ASA
	{{"         10mg/l "}, {"         30mg/l "}, {"         80mg/l "}, {"        150mg/l "}, {"                "}, {"                "},{"                "}, {"                "}},//ALB
	{{"        10mg/dl "}, {"        50mg/dl "}, {"       100mg/dl "}, {"       200mg/dl "}, {"       300mg/dl "}, {"                "},{"                "}, {"                "}},//CRE
	{{"                "}, {"                "}, {"                "}, {"                "}, {"                "}, {"                "},{"                "}, {"                "}},//Empty
	{{"                "}, {"                "}, {"                "}, {"                "}, {"                "}, {"                "},{"                "}, {"                "}},//Empty
	{{"                "}, {"                "}, {"                "}, {"                "}, {"                "}, {"                "},{"                "}, {"                "}},//Empty
	{{"                "}, {"                "}, {"                "}, {"                "}, {"                "}, {"                "},{"                "}, {"                "}},//Empty
};

const char as720_table_cybow_si[17][8][17] = {
	{{"norm   0.1umol/l"}, {"  +-    16umol/l"}, {"   +    33umol/l"}, {"  ++    66umol/l"}, {" +++   131umol/l"}, {"                "},{"                "}, {"                "}},//URO
	{{"   -        neg "}, {"  +-   5.5mmol/l"}, {"   +    14mmol/l"}, {"  ++    28mmol/l"}, {" +++    55mmol/l"}, {"                "},{"                "}, {"                "}},//GLU
	{{"   -        neg "}, {"   +      1mg/dl"}, {"  ++      2mg/dl"}, {" +++      3mg/dl"}, {"                "}, {"                "},{"                "}, {"                "}},//BIL
	{{"   -        neg "}, {"  +-   0.5mmol/l"}, {"   +   1.5mmol/l"}, {"  ++   3.9mmol/l"}, {" +++    10mmol/l"}, {"                "},{"                "}, {"                "}},//KET
	{{"          1.000 "}, {"          1.005 "}, {"          1.010 "}, {"          1.015 "}, {"          1.020 "}, {"          1.025 "},{"          1.030 "}, {"                "}},//SG
	{{"   -        neg "}, {"  +-     5RBC/ul"}, {"   +    10RBC/ul"}, {"  ++    50RBC/ul"}, {" +++   250RBC/ul"}, {"                "},{"                "}, {"                "}},//BLD
	{{"            5.0 "}, {"            6.0 "}, {"            6.5 "}, {"            7.0 "}, {"            8.0 "}, {"            9.0 "},{"                "}, {"                "}},//pH
	{{"   -        neg "}, {"  +-     0.15g/l"}, {"   +      0.3g/l"}, {"  ++        1g/l"}, {" +++        3g/l"}, {"++++       10g/l"},{"                "}, {"                "}},//PRO
	{{"   -        neg "}, {"   +        pos "}, {"                "}, {"                "}, {"                "}, {"                "},{"                "}, {"                "}},//NIT
	{{"   -        neg "}, {"   +    25WBC/ul"}, {"  ++    75WBC/ul"}, {" +++   500WBC/ul"}, {"                "}, {"                "},{"                "}, {"                "}},//LEU
	{{"   -        neg "}, {"   +   1.2mmol/l"}, {"  ++   2.4mmol/l"}, {"                "}, {"                "}, {"                "},{"                "}, {"                "}},//ASA
	{{"         10mg/l "}, {"         30mg/l "}, {"         80mg/l "}, {"        150mg/l "}, {"                "}, {"                "},{"                "}, {"                "}},//ALB
	{{"       0.9mmol/l"}, {"       4.4mmol/l"}, {"       8.8mmol/l"}, {"      17.7mmol/l"}, {"      26.5mmol/l"}, {"                "},{"                "}, {"                "}},//CRE
	{{"                "}, {"                "}, {"                "}, {"                "}, {"                "}, {"                "},{"                "}, {"                "}},//Empty
	{{"                "}, {"                "}, {"                "}, {"                "}, {"                "}, {"                "},{"                "}, {"                "}},//Empty
	{{"                "}, {"                "}, {"                "}, {"                "}, {"                "}, {"                "},{"                "}, {"                "}},//Empty
	{{"                "}, {"                "}, {"                "}, {"                "}, {"                "}, {"                "},{"                "}, {"                "}},//Empty
};

const char as720_table_combostik_si[17][8][17] = {
	{{"norm   0.1umol/l"}, {"  +-    16umol/l"}, {"   +    33umol/l"}, {"  ++    66umol/l"}, {" +++   131umol/l"}, {"                "},{"                "}, {"                "}},//URO
	{{"   -        neg "}, {"  +-   2.8mmol/l"}, {"   +   5.5mmol/l"}, {"  ++    17mmol/l"}, {" +++    55mmol/l"}, {"                "},{"                "}, {"                "}},//GLU
	{{"   -        neg "}, {"   +      1mg/dl"}, {"  ++      2mg/dl"}, {" +++      3mg/dl"}, {"                "}, {"                "},{"                "}, {"                "}},//BIL
	{{"   -        neg "}, {"  +-   0.5mmol/l"}, {"   +   1.5mmol/l"}, {"  ++   3.9mmol/l"}, {" +++    10mmol/l"}, {"                "},{"                "}, {"                "}},//KET
	{{"          1.000 "}, {"          1.005 "}, {"          1.010 "}, {"          1.015 "}, {"          1.020 "}, {"          1.025 "},{"          1.030 "}, {"                "}},//SG
	{{"   -        neg "}, {"  +-     5RBC/ul"}, {"   +    10RBC/ul"}, {"  ++    50RBC/ul"}, {" +++   250RBC/ul"}, {"                "},{"                "}, {"                "}},//BLD
	{{"            5.0 "}, {"            6.0 "}, {"            6.5 "}, {"            7.0 "}, {"            8.0 "}, {"            9.0 "},{"                "}, {"                "}},//pH
	{{"   -        neg "}, {"  +-     0.15g/l"}, {"   +      0.3g/l"}, {"  ++        1g/l"}, {" +++        3g/l"}, {"++++       10g/l"},{"                "}, {"                "}},//PRO
	{{"   -        neg "}, {"   +        pos "}, {"                "}, {"                "}, {"                "}, {"                "},{"                "}, {"                "}},//NIT
	{{"   -        neg "}, {"   +    25WBC/ul"}, {"  ++    75WBC/ul"}, {" +++   500WBC/ul"}, {"                "}, {"                "},{"                "}, {"                "}},//LEU
	{{"   -        neg "}, {"   +   1.2mmol/l"}, {"  ++   2.4mmol/l"}, {"                "}, {"                "}, {"                "},{"                "}, {"                "}},//ASA
	{{"         10mg/l "}, {"         30mg/l "}, {"         80mg/l "}, {"        150mg/l "}, {"                "}, {"                "},{"                "}, {"                "}},//ALB
	{{"       0.9mmol/l"}, {"       4.4mmol/l"}, {"       8.8mmol/l"}, {"      17.7mmol/l"}, {"      26.5mmol/l"}, {"                "},{"                "}, {"                "}},//CRE
	{{"                "}, {"                "}, {"                "}, {"                "}, {"                "}, {"                "},{"                "}, {"                "}},//Empty
	{{"                "}, {"                "}, {"                "}, {"                "}, {"                "}, {"                "},{"                "}, {"                "}},//Empty
	{{"                "}, {"                "}, {"                "}, {"                "}, {"                "}, {"                "},{"                "}, {"                "}},//Empty
	{{"                "}, {"                "}, {"                "}, {"                "}, {"                "}, {"                "},{"                "}, {"                "}},//Empty
};

const char as720_table_bayer_si[17][8][17] = {
	{{"norm   0.1umol/l"}, {"  +-    16umol/l"}, {"   +    33umol/l"}, {"  ++    66umol/l"}, {" +++   131umol/l"}, {"                "},{"                "}, {"                "}},//URO
	{{"   -        neg "}, {"  +-   5.5mmol/l"}, {"   +    14mmol/l"}, {"  ++    28mmol/l"}, {" +++    55mmol/l"}, {"++++   111mmol/l"},{"                "}, {"                "}},//GLU
	{{"   -        neg "}, {"   +      1mg/dl"}, {"  ++      2mg/dl"}, {" +++      3mg/dl"}, {"                "}, {"                "},{"                "}, {"                "}},//BIL
	{{"   -        neg "}, {"  +-   0.5mmol/l"}, {"   +   1.5mmol/l"}, {"  ++   3.9mmol/l"}, {" +++     8mmol/l"}, {"++++    16mmol/l"},{"                "}, {"                "}},//KET
	{{"          1.000 "}, {"          1.005 "}, {"          1.010 "}, {"          1.015 "}, {"          1.020 "}, {"          1.025 "},{"          1.030 "}, {"                "}},//SG
	{{"   -        neg "}, {"  +-     5RBC/ul"}, {"   +    25RBC/ul"}, {"  ++    80RBC/ul"}, {" +++   200RBC/ul"}, {"                "},{"                "}, {"                "}},//BLD
	{{"            5.0 "}, {"            6.0 "}, {"            6.5 "}, {"            7.0 "}, {"            7.5 "}, {"            8.0 "},{"            8.5 "}, {"                "}},//pH
	{{"   -        neg "}, {"  +-     0.15g/l"}, {"   +      0.3g/l"}, {"  ++        1g/l"}, {" +++        3g/l"}, {"++++       10g/l"},{"                "}, {"                "}},//PRO
	{{"   -        neg "}, {"   +        pos "}, {"                "}, {"                "}, {"                "}, {"                "},{"                "}, {"                "}},//NIT
	{{"   -        neg "}, {"  +-    25WBC/ul"}, {"   +    70WBC/ul"}, {"  ++   125WBC/ul"}, {" +++   500WBC/ul"}, {"                "},{"                "}, {"                "}},//LEU
	{{"   -        neg "}, {"   +   1.2mmol/l"}, {"  ++   2.4mmol/l"}, {"                "}, {"                "}, {"                "},{"                "}, {"                "}},//ASA
	{{"         10mg/l "}, {"         30mg/l "}, {"         80mg/l "}, {"        150mg/l "}, {"                "}, {"                "},{"                "}, {"                "}},//ALB
	{{"       0.9mmol/l"}, {"       4.4mmol/l"}, {"       8.8mmol/l"}, {"      17.7mmol/l"}, {"      26.5mmol/l"}, {"                "},{"                "}, {"                "}},//CRE
	{{"                "}, {"                "}, {"                "}, {"                "}, {"                "}, {"                "},{"                "}, {"                "}},//Empty
	{{"                "}, {"                "}, {"                "}, {"                "}, {"                "}, {"                "},{"                "}, {"                "}},//Empty
	{{"                "}, {"                "}, {"                "}, {"                "}, {"                "}, {"                "},{"                "}, {"                "}},//Empty
	{{"                "}, {"                "}, {"                "}, {"                "}, {"                "}, {"                "},{"                "}, {"                "}},//Empty
};

const unsigned int acr_item[5][4]= {	
	{0,3,3,3},	//0
	{1,2,2,3},	//1
	{1,2,2,2},	//2
	{1,1,2,2},	//3
	{1,1,1,2}		//4
};
const char acr_result_conv[4][18]={
	{"     <30mg/g     "},
	{"     <30mg/g     "},
	{"  30~300mg/g     "},
	{"    >300mg/g     "}
};
const char acr_result_si[4][18]={
	{"    <3.4mg/mmol  "},
	{"    <3.4mg/mmol  "},
	{"3.4~33.9mg/mmol  "},
	{"   >33.9mg/mmol  "}
};
const char acr_tb[4][14]= {	
	{"Dilute Normal"},	//0
	{"Normal       "},	//1
	{"Abnormal     "},	//2
	{"High Abnormal"}	//3
};

#endif
