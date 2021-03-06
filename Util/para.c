#define _PARA_H_
#include <stdio.h>
#include "stm32f4xx.h"
#include "Common\stm324x9i_eval.h"
#include "common\32f429_port.h"
#include "common\32f429_lcd.h"
#include "util\usart.h"			// include port.h
#include "util\Flash.h"
#include "util\lcd.h"
#include "util\eep.h"
#include "util\para.h"
#include "resm.h"
#include "touch.h"
#include "dsp.h"

/*****************************************************************************/
void init_RAM(void){
	MS10 = 0;
	usec_delay = 0;
	flag_flash = 0; 
	rcv_wait = rcv_step = 0;
	run_stepa = run_stepb = 0;
	flag_motor_run = 0;
//	sprintf(USER_ID, "123456789");
	read_strip_have = read_strip_none = read_strip_flag = read_strip_step = read_strip_wait=read_strip_move=0;
	motor_step = motor_wait = 0;
	flag_table = table_delay = 0;
	flag_box = box_delay = 0;
	//oper_no = 0;
}
/***************************************************************************************/
const u8 *ar_printform[20][10]= {	
//	 01234567890   12345678   90123456   78901234   56789012   34567890   12345678   90123456   78901234   56789012
//        0          1          2          3          4          5          6          7          8          9 
	"URO        ","norm    ","norm    ","+1      ","+2      ","+3      ","        ","        ","        ","        ",	//00
	"GLU        ","neg.    ","+/-     ","+1      ","+2      ","+3      ","        ","        ","        ","        ",	//01
	"BIL        ","neg.    ","+1      ","+2      ","+3      ","        ","        ","        ","        ","        ",	//02
	"KET        ","neg.    ","+/-     ","+1      ","+2      ","+3      ","        ","        ","        ","        ",	//03
	"S.G        ","1.000   ","1.005   ","1.010   ","1.015   ","1.020   ","1.025   ","1.030   ","        ","        ",	//04
	"BLD        ","neg.    ","+/-     ","+1      ","+2      ","+3      ","        ","        ","        ","        ",	//05
	"pH         ","5       ","5.5     ","6       ","6.5     ","7       ","7.5     ","8       ","8.5     ","9       ",	//06
	"PRO        ","neg.    ","+/-     ","+1      ","+2      ","+3      ","+4      ","        ","        ","        ",	//07
	"NIT        ","neg.    ","pos.    ","        ","        ","        ","        ","        ","        ","        ",	//08
	"LEU        ","neg.    ","+1      ","+2      ","+3      ","        ","        ","        ","        ","        ",	//09
	"ASA        ","neg.    ","+1      ","+2      ","        ","        ","        ","        ","        ","        ",	//10
	"ALBmg/L    ","10      ","30      ","80      ","150     ","        ","        ","        ","        ","        ",	//11
	"CREmg/dL   ","10      ","50      ","100     ","200     ","300     ","        ","        ","        ","        ",	//12
	"MDA        ","        ","        ","        ","        ","        ","        ","        ","        ","        ",	//13
	"BHB        ","Neg.    ","+/-     ","+1      ","+2      ","+3      ","        ","        ","        ","        ",	//14
	"CALmg/dL   ","1       ","5       ","10      ","20      ","40      ","        ","        ","        ","        ",	//15
	"NIC        ","Neg.    ","Low     ","Medium  ","High    ","        ","        ","        ","        ","        ",	//16
	"           ","        ","        ","        ","        ","        ","        ","        ","        ","        ",	//17
	"           ","        ","        ","        ","        ","        ","        ","        ","        ","        ",	//18
	"           ","        ","        ","        ","        ","        ","        ","        ","        ","        ",	//19
};                                                                     //             
/***************************************************************************************/
const u8 *si_printform[20][10]= {			                     
	"UROumol/L  ","norm    ","16      ","33      ","66      ","131     ","        ","        ","        ","        ",	//00
	"GLUmmol/L  ","neg.    ","5.5     ","14      ","28      ","55      ","        ","        ","        ","        ",	//01
	"BIL        ","neg.    ","Small   ","Moderate","Large   ","        ","        ","        ","        ","        ",	//02
	"KETmmol/L  ","neg.    ","0.5     ","1.5     ","4       ","10      ","        ","        ","        ","        ",	//03
	"S.G        ","1.000   ","1.005   ","1.010   ","1.015   ","1.020   ","1.025   ","1.030   ","        ","        ",	//04
	"BLDRBC/uL  ","neg.    ","trace   ","10      ","50      ","250     ","        ","        ","        ","        ",	//05
	"pH         ","5       ","5.5     ","6       ","6.5     ","7       ","7.5     ","8       ","8.5     ","9       ",	//06
	"PROg/L     ","neg.    ","trace   ","0.3     ","1       ","3       ","10      ","        ","        ","        ",	//07
	"NIT        ","neg.    ","pos.    ","        ","        ","        ","        ","        ","        ","        ",	//08
	"LEUWBC/uL  ","neg.    ","25      ","75      ","500     ","        ","        ","        ","        ","        ",	//09
	"ASAmmol/L  ","neg.    ","1.2     ","2.4     ","        ","        ","        ","        ","        ","        ",	//10
	"ALBmg/L    ","10      ","30      ","80      ","150     ","        ","        ","        ","        ","        ",	//11
	"CREmmol/L  ","0.9     ","4.4     ","8.8     ","17.7    ","26.5    ","        ","        ","        ","        ",	//12
	"MDA        ","        ","        ","        ","        ","        ","        ","        ","        ","        ",	//13
	"BHBumol/L  ","Neg.    ","100     ","200     ","500     ","1000    ","        ","        ","        ","        ",	//14
	"CALmmol/L  ","1.0     ","2.5     ","5.0     ","7.5     ","10      ","        ","        ","        ","        ",	//15
	"NICug/mL   ","None    ","2       ","5       ","12      ","        ","        ","        ","        ","        ",	//16
	"           ","        ","        ","        ","        ","        ","        ","        ","        ","        ",	//17
	"           ","        ","        ","        ","        ","        ","        ","        ","        ","        ",	//18
	"           ","        ","        ","        ","        ","        ","        ","        ","        ","        ",	//19
};                                                                     //             
/***************************************************************************************/
const u8 *co_printform[20][10]= {	                                                  
	"UROmg/dL   ","norm    ","1       ","2       ","4       ","8       ","        ","        ","        ","        ",	//00
	"GLUmg/dL   ","neg.    ","100     ","250     ","500     ","1000    ","        ","        ","        ","        ",	//01
	"BIL        ","neg.    ","Small   ","Moderate","Large   ","        ","        ","        ","        ","        ",	//02
	"KETmg/dL   ","neg.    ","5       ","15      ","40      ","100     ","        ","        ","        ","        ",	//03
	"S.G        ","1.000   ","1.005   ","1.010   ","1.015   ","1.020   ","1.025   ","1.030   ","        ","        ",	//04
	"BLDRBC/uL  ","neg.    ","trace   ","10      ","50      ","250     ","        ","        ","        ","        ",	//05
	"pH         ","5       ","5.5     ","6       ","6.5     ","7       ","7.5     ","8       ","8.5     ","9       ",	//06
	"PROmg/dL   ","neg.    ","trace   ","30      ","100     ","300     ","1000    ","        ","        ","        ",	//07
	"NIT        ","neg.    ","pos.    ","        ","        ","        ","        ","        ","        ","        ",	//08
	"LEUWBC/uL  ","neg.    ","25      ","75      ","500     ","        ","        ","        ","        ","        ",	//09
	"ASAmg/dL   ","neg.    ","20      ","40      ","        ","        ","        ","        ","        ","        ",	//10
	"ALBmg/L    ","10      ","30      ","80      ","150     ","        ","        ","        ","        ","        ",	//11
	"CREmg/dL   ","10      ","50      ","100     ","200     ","300     ","        ","        ","        ","        ",	//12
	"MDA        ","        ","        ","        ","        ","        ","        ","        ","        ","        ",	//13
	"BHBmg/dL   ","Neg.    ","1       ","2       ","5       ","10      ","        ","        ","        ","        ",	//14
	"CALmg/dL   ","1       ","5       ","10      ","20      ","40      ","        ","        ","        ","        ",	//15
	"NICug/mL   ","None    ","2       ","5       ","12      ","        ","        ","        ","        ","        ",	//16
	"           ","        ","        ","        ","        ","        ","        ","        ","        ","        ",	//17
	"           ","        ","        ","        ","        ","        ","        ","        ","        ","        ",	//18
	"           ","        ","        ","        ","        ","        ","        ","        ","        ","        ",	//19
};                                                                     //             
/***************************************************************************************/

const u8 ini_strip[]={ 			//1026=27*38
//	  012345678901234567890123456789012	
     "01URINE-G         120000000000000"		//00
     "02URINE-P         180000000000000"		//01
     "03URINE-GK        224000000000000"		//02
     "04URINE-1         110000000000000"		//03
     "05URINE-2         212000000000000"		//04
     "06URINE-3         312300000000000"		//05 
     "07URINE-4         412340000000000"		//06
     "08URINE-5         512345000000000"		//07
     "09URINE-6         612345600000000"		//08
     "10URINE-7         712345670000000"		//09
     "11URINE-8         812345678000000"		//10
     "12URINE-9         912345678900000"		//11
     "13URINE-10        A123456789A0000"		//12
     "14URINE-11        B123456789AB000"		//13
     "15URINE-12        C123456789ABC00"		//14
     "16URINE-13        D123456789ABCD0"		//15
     "17URINE-14        E123456789ABCDE"		//16
     "18EndOfStrip                     "		//17
     "19EndOfStrip                     "		//18
     "20EndOfStrip                     "		//19
};                                
                                 
/***************************************************************************************/
const u8 ini_position[]={			//64Bytes, 16???????????? ????????????
//	   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16	
//	   10, 50,100,130,160,190,220,250,280,310,340,370,400,430,460,490	
	"0010005001000130016001900220025002800310034003700400043004600490"
};

/***************************************************************************************/
const u8 ini_weight[]={ 
//	'2','3','2','2','1','1','1','1','2','2','2','1','2','0','0','0','0','0','0','0' 	//OLD
//	 1   2   1   1   0   0   0   0   1   1   1   0   1   1   1   0   0   0   0   0
//   12345678901234567890
	"12110000111011100000"		//NEW
};

/***************************************************************************************/
const u8 ini_para[20*41]={
//	 01234567890123456789012345678901234567890
	"11300 781 780 650 540   0   0   0   0   0"
	"11300 740 480 270 220   0   0   0   0   0"
	"11300 750 650 550   0   0   0   0   0   0"
	"11300 750 690 540 340   0   0   0   0   0"
	"0   0 240 300 350 430 680 7801300   0   0"
	"11300 900 730 500 300   0   0   0   0   0"
	"113001000 900 800 700 600 500 400 300   0"
	"11300 820 770 700 540 410   0   0   0   0"
	"11300 930   0   0   0   0   0   0   0   0"
	"11300 900 750 530   0   0   0   0   0   0"
	"0   0 500 6501300   0   0   0   0   0   0"
	"11300 780 680 600   0   0   0   0   0   0"
	"11300 550 350 260 220   0   0   0   0   0"
	"11300 890 850 810 780 750 720   0   0   0"
	"11300 730 630 520 420   0   0   0   0   0"
	"11300 900 780 680   0   0   0   0   0   0"
	"11300 500 350 290 250   0   0   0   0   0"
	"0   0   0   0   0   0   0   0   0   0   0"
	"0   0   0   0   0   0   0   0   0   0   0"
	"0   0   0   0   0   0   0   0   0   0   0"
};






