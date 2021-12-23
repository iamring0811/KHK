#define LCD18BIT
//#define SELF_TEST
//#define NO_MT_SENSOR

#include "stm32f4xx.h"

#define PORT_LEDlo	PORT_LEDOn
#define PORT_LEDhi	PORT_LEDOff

#define LCD_ON()	PORT_LEDhi(PWM0)
#define LCD_OFF()	PORT_LEDlo(PWM0)

#define SENS_ON()	PORT_LEDhi(PHON)
#define SENS_OFF()	PORT_LEDlo(PHON)

/******************************************************************************/
typedef enum 
{
	DSCK ,		//00
	MOTA1,		//01
	MOTA2,		//02
	MOTA3,		//03
	MOTA4,		//04
	PWREN,		//05
	TP09 ,		//06
	TP01 ,		//07
	MTAB ,		//08	Hi:MT-A,  Lo:MT-B
	STPR ,		//09
	PHON ,		//10
	DSIO ,		//11
	TSXP ,		//12
	TSXN ,		//13
	TSYP ,		//14
	TSYN ,		//15
	FAN  ,		//16
	ENVE ,		//17
	BUZZ ,		//18
	PWM0 ,		//19
	RSTBL,		//20
	LEDR ,		//21
	LEDG ,		//22
	DI24 ,		//23
	FEED ,		//24
	CE24 ,		//25
	WP24 ,		//26
	RSTET,		//27
	DSEN  		//28
} Led_TypeDef;

#define LEDn                        29

#define DSCK_PIN                    GPIO_Pin_6					//06 CK24와 같이 사용
#define DSCK_PORT                   GPIOE
#define DSCK_CLK               		RCC_AHB1Periph_GPIOE
#define MOTA1_PIN                   GPIO_Pin_8					//Lo:MT RUN
#define MOTA1_PORT                  GPIOI
#define MOTA1_CLK               	RCC_AHB1Periph_GPIOI
#define MOTA2_PIN                   GPIO_Pin_13					//Lo:MT RUN
#define MOTA2_PORT                  GPIOC
#define MOTA2_CLK               	RCC_AHB1Periph_GPIOC
#define MOTA3_PIN                   GPIO_Pin_14					//Lo:MT RUN
#define MOTA3_PORT                  GPIOC
#define MOTA3_CLK               	RCC_AHB1Periph_GPIOC
#define MOTA4_PIN                   GPIO_Pin_15					//Lo:MT RUN
#define MOTA4_PORT                  GPIOC
#define MOTA4_CLK               	RCC_AHB1Periph_GPIOC

#define PWREN_PIN                   GPIO_Pin_11					//27 LCD MODE => No Use , 화면 위치 이동
#define PWREN_PORT                  GPIOI
#define PWREN_CLK              		RCC_AHB1Periph_GPIOI

#define TP09_PIN                   	GPIO_Pin_8					//
#define TP09_PORT                  	GPIOF
#define TP09_CLK              		RCC_AHB1Periph_GPIOF
#define TP01_PIN                   	GPIO_Pin_1					//OSC OUT
#define TP01_PORT                  	GPIOH
#define TP01_CLK              		RCC_AHB1Periph_GPIOH

#define MTAB_PIN                    GPIO_Pin_0					//Hi:MT-A,  Lo:MT-B
#define MTAB_PORT                   GPIOC
#define MTAB_CLK               		RCC_AHB1Periph_GPIOC
#define STPR_PIN                    GPIO_Pin_1					//Hi:STCHK, Lo:Print
#define STPR_PORT                   GPIOC
#define STPR_CLK               		RCC_AHB1Periph_GPIOC

#define PHON_PIN                   	GPIO_Pin_4					//Hi:Photo On
#define PHON_PORT                  	GPIOH
#define PHON_CLK              		RCC_AHB1Periph_GPIOH

#define DSIO_PIN                   	GPIO_Pin_7 					//RTC I/O
#define DSIO_PORT                  	GPIOA						//DA24
#define DSIO_CLK              		RCC_AHB1Periph_GPIOA

#define TSXP_PIN                   	GPIO_Pin_4					//08
#define TSXP_PORT                  	GPIOC
#define TSXP_CLK             		RCC_AHB1Periph_GPIOC
#define TSXN_PIN                   	GPIO_Pin_5					//09
#define TSXN_PORT                  	GPIOC
#define TSXN_CLK             		RCC_AHB1Periph_GPIOC
#define TSYP_PIN                   	GPIO_Pin_0					//10
#define TSYP_PORT                  	GPIOB
#define TSYP_CLK             		RCC_AHB1Periph_GPIOB
#define TSYN_PIN                   	GPIO_Pin_1					//11
#define TSYN_PORT                  	GPIOB
#define TSYN_CLK             		RCC_AHB1Periph_GPIOB

#define FAN_PIN                   	GPIO_Pin_2					//Hi:FAN ON
#define FAN_PORT                  	GPIOB
#define FAN_CLK             		RCC_AHB1Periph_GPIOB

#define ENVE_PIN                   	GPIO_Pin_12					//Hi:BUZZ ON
#define ENVE_PORT                  	GPIOB
#define ENVE_CLK             		RCC_AHB1Periph_GPIOB
#define BUZZ_PIN                   	GPIO_Pin_13					//Hi:BUZZ ON
#define BUZZ_PORT                  	GPIOD
#define BUZZ_CLK             		RCC_AHB1Periph_GPIOD

#define PWM0_PIN                    GPIO_Pin_3					//03 LCD_ON_pin
#define PWM0_PORT                   GPIOG
#define PWM0_CLK               		RCC_AHB1Periph_GPIOG

#define RSTBL_PIN                   GPIO_Pin_8					//26
#define RSTBL_PORT                  GPIOC
#define RSTBL_CLK              		RCC_AHB1Periph_GPIOC

#define LEDG_PIN                    GPIO_Pin_9					//
#define LEDG_PORT                   GPIOC
#define LEDG_CLK               		RCC_AHB1Periph_GPIOC
#define LEDR_PIN                    GPIO_Pin_8					//
#define LEDR_PORT       	        GPIOA
#define LEDR_CLK               		RCC_AHB1Periph_GPIOA

#define DI24_PIN                    GPIO_Pin_3					//
#define DI24_PORT       	        GPIOI
#define DI24_CLK               		RCC_AHB1Periph_GPIOI

#define FEED_PIN                   	GPIO_Pin_3					//
#define FEED_PORT                  	GPIOD
#define FEED_CLK              		RCC_AHB1Periph_GPIOD

#define CE24_PIN                    GPIO_Pin_12					//
#define CE24_PORT                   GPIOG
#define CE24_CLK               		RCC_AHB1Periph_GPIOG
#define WP24_PIN                    GPIO_Pin_14					//
#define WP24_PORT                   GPIOG
#define WP24_CLK               		RCC_AHB1Periph_GPIOG

#define RSTET_PIN                   GPIO_Pin_6 					//24
#define RSTET_PORT                  GPIOI
#define RSTET_CLK              		RCC_AHB1Periph_GPIOI
#define DSEN_PIN                    GPIO_Pin_7					//04
#define DSEN_PORT                   GPIOI
#define DSEN_CLK               		RCC_AHB1Periph_GPIOI

/******************************************************************************/

#define DSW1_PIN                    GPIO_Pin_2
#define DSW1_PORT                   GPIOE
#define DSW1_CLK                    RCC_AHB1Periph_GPIOE
#define DSW2_PIN                    GPIO_Pin_3
#define DSW2_PORT                   GPIOE
#define DSW2_CLK                    RCC_AHB1Periph_GPIOE
#define DSW3_PIN                    GPIO_Pin_4
#define DSW3_PORT                   GPIOE
#define DSW3_CLK                    RCC_AHB1Periph_GPIOE
#define DSW4_PIN                    GPIO_Pin_5
#define DSW4_PORT                   GPIOE
#define DSW4_CLK                    RCC_AHB1Periph_GPIOE

#define DO24_PIN                    GPIO_Pin_13					//
#define DO24_PORT                   GPIOG
#define DO24_CLK               		RCC_AHB1Periph_GPIOG

/******************************************************************************/
#define PHIN1_PIN                   GPIO_Pin_0
#define PHIN1_PORT                  GPIOA
#define PHIN1_CLK                   RCC_AHB1Periph_GPIOA
#define PHIN2_PIN                   GPIO_Pin_1
#define PHIN2_PORT                  GPIOA
#define PHIN2_CLK                   RCC_AHB1Periph_GPIOA
#define PHIN3_PIN                   GPIO_Pin_2
#define PHIN3_PORT                  GPIOH
#define PHIN3_CLK                   RCC_AHB1Periph_GPIOH
#define PHIN4_PIN                   GPIO_Pin_3
#define PHIN4_PORT                  GPIOH
#define PHIN4_CLK                   RCC_AHB1Periph_GPIOH

#define PHSW4_ON		(PHIN4_PORT->IDR & 0x0008)			// Strip Box
#define PH_BOX		PHSW4_ON
#define PHSW3_ON		(PHIN3_PORT->IDR & 0x0004)			// Strip Move(MotorB)
#define PH_MTA		PHSW3_ON
#define PHSW2_ON		(PHIN2_PORT->IDR & 0x0002)			// Strip Table
#define PH_TBL		PHSW2_ON
#define PHSW1_ON		(PHIN1_PORT->IDR & 0x0001)			// Strip Push(MotorA)
#define PH_MTB		PHSW1_ON

#define DIPSW_BMP		(dsw_data & 0x02)					// sw2
#define OUT_MSG			(dsw_data & 0x04)					// sw3
#define StripIN			!(DSW4_PORT->IDR & 0x0004)			//(dsw_data & 0x08)					// sw4

/******************************************************************************/

#define OUT_DIR		0
#define IN_DIR		1

void PORT_Config(void);
void PORT_LEDInit(Led_TypeDef Led, u8 dir);
void PORT_LEDOn(Led_TypeDef Led);
void PORT_LEDOff(Led_TypeDef Led);
void PORT_LEDTog(Led_TypeDef Led);

void DSW_Init(void);
void DSW_Read(void);

/******************************************************************************/
#define step1hi()	PORT_LEDlo(MOTA1) 	//
#define step1lo()	PORT_LEDhi(MOTA1)	//
#define step3hi()	PORT_LEDlo(MOTA2) 	//
#define step3lo()	PORT_LEDhi(MOTA2)	//
#define step2hi()	PORT_LEDlo(MOTA3) 	//
#define step2lo()	PORT_LEDhi(MOTA3)	//
#define step4hi()	PORT_LEDlo(MOTA4) 	//
#define step4lo()	PORT_LEDhi(MOTA4)	//

#ifdef _PORT_C_
u8	dsw_data;	

#else
extern u8	dsw_data;	

#endif /* __PORT_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

