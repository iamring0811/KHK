#define _PORT_C_

#include "common\32f429_port.h"
#include "util\usart.h"


GPIO_TypeDef* GPIO_PORT[LEDn] = { 
//	0		   1		   2		   3		   4		   5		   6		  7			   8		   9	
	DSCK_PORT, MOTA1_PORT, MOTA2_PORT, MOTA3_PORT, MOTA4_PORT, PWREN_PORT, TP09_PORT , TP01_PORT , MTAB_PORT , STPR_PORT ,
	PHON_PORT, DSIO_PORT , TSXP_PORT , TSXN_PORT , TSYP_PORT , TSYN_PORT , FAN_PORT  , ENVE_PORT , BUZZ_PORT , PWM0_PORT , 
	RSTBL_PORT,LEDR_PORT , LEDG_PORT , DI24_PORT , FEED_PORT , CE24_PORT , WP24_PORT , RSTET_PORT, DSEN_PORT };
								  
const uint16_t GPIO_PIN[LEDn] = { 
	DSCK_PIN , MOTA1_PIN , MOTA2_PIN , MOTA3_PIN , MOTA4_PIN , PWREN_PIN , TP09_PIN  , TP01_PIN  , MTAB_PIN  , STPR_PIN  ,
	PHON_PIN , DSIO_PIN  , TSXP_PIN  , TSXN_PIN  , TSYP_PIN  , TSYN_PIN  , FAN_PIN   , ENVE_PIN  , BUZZ_PIN  , PWM0_PIN  ,
	RSTBL_PIN, LEDR_PIN  , LEDG_PIN  , DI24_PIN  , FEED_PIN  , CE24_PIN  , WP24_PIN  , RSTET_PIN , DSEN_PIN  };

const uint32_t GPIO_CLK[LEDn] = { 
	DSCK_CLK  , MOTA1_CLK , MOTA2_CLK , MOTA3_CLK , MOTA4_CLK , PWREN_CLK , TP09_CLK , TP01_CLK  , MTAB_CLK  , STPR_CLK  ,
	PHON_CLK  , DSIO_CLK  , TSXP_CLK  , TSXN_CLK  , TSYP_CLK  , TSYN_CLK  , FAN_CLK  , ENVE_CLK  , BUZZ_CLK  , PWM0_CLK  ,
	RSTBL_CLK , LEDR_CLK  , LEDG_CLK  , DI24_CLK  , FEED_CLK  , CE24_CLK  , WP24_CLK , RSTET_CLK , DSEN_CLK  };

void PORT_Config(void){
	PORT_LEDInit(DSCK , OUT_DIR);
	PORT_LEDhi(DSCK );
	PORT_LEDInit(MOTA1, OUT_DIR);
	PORT_LEDhi(MOTA1);
	PORT_LEDInit(MOTA2, OUT_DIR);
	PORT_LEDhi(MOTA2);
	PORT_LEDInit(MOTA3, OUT_DIR);
	PORT_LEDhi(MOTA3);
	PORT_LEDInit(MOTA4, OUT_DIR);
	PORT_LEDhi(MOTA4);
	PORT_LEDInit(PWREN, OUT_DIR);
	PORT_LEDhi(PWREN);
	PORT_LEDInit(TP09 , OUT_DIR);
	PORT_LEDhi(TP09 );
	PORT_LEDInit(TP01 , OUT_DIR);
	PORT_LEDhi(TP01 );
	PORT_LEDInit(MTAB , OUT_DIR);
	PORT_LEDhi(MTAB );				//Hi:MT-A,  Lo:MT-B
	PORT_LEDInit(STPR , OUT_DIR);
	PORT_LEDlo(STPR );				//Hi:STCHK, Lo:Print
	PORT_LEDInit(PHON , OUT_DIR);
	PORT_LEDhi(PHON );
	PORT_LEDInit(DSIO , OUT_DIR);
	PORT_LEDhi(DSIO );
	PORT_LEDInit(TSXP , OUT_DIR);
	PORT_LEDhi(TSXP );
	PORT_LEDInit(TSXN , OUT_DIR);
	PORT_LEDhi(TSXN );
	PORT_LEDInit(TSYP ,	OUT_DIR);
	PORT_LEDhi(TSYP );
	PORT_LEDInit(TSYN ,	OUT_DIR);
	PORT_LEDhi(TSYN );
	PORT_LEDInit(FAN  ,	OUT_DIR);
	PORT_LEDhi(FAN  );
	PORT_LEDInit(ENVE ,	OUT_DIR);
	PORT_LEDlo(ENVE );
	PORT_LEDInit(BUZZ ,	OUT_DIR);
	PORT_LEDhi(BUZZ );
	PORT_LEDInit(PWM0 ,	OUT_DIR);
	PORT_LEDhi(PWM0 );
	PORT_LEDInit(RSTBL,	OUT_DIR);
	PORT_LEDhi(RSTBL);
	PORT_LEDInit(LEDR ,	OUT_DIR);
	PORT_LEDhi(LEDR );
	PORT_LEDInit(LEDG , OUT_DIR);
	PORT_LEDhi(LEDG );
	PORT_LEDInit(DI24 ,	OUT_DIR);
	PORT_LEDhi(DI24 );
	PORT_LEDInit(FEED , OUT_DIR);
	PORT_LEDhi(FEED );				//Lo:paper out
	PORT_LEDInit(CE24 , OUT_DIR);
	PORT_LEDhi(CE24 );
	PORT_LEDInit(WP24 , OUT_DIR);
	PORT_LEDhi(WP24 );
	PORT_LEDInit(RSTET,	OUT_DIR);
	PORT_LEDhi(RSTET);
	PORT_LEDInit(DSEN,	OUT_DIR);
	PORT_LEDhi(DSEN );
}

/*******************************************************************************/

void PORT_LEDInit(Led_TypeDef Led, u8 dir)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  /* Enable the GPIO_LED Clock */
  RCC_AHB1PeriphClockCmd(GPIO_CLK[Led], ENABLE);

  /* Configure the GPIO_LED pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN[Led];
  if(!dir)  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		//OUT_DIR
  else      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;		// IN_DIR
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIO_PORT[Led], &GPIO_InitStructure);
}

void PORT_LEDOn(Led_TypeDef Led)
{
  GPIO_PORT[Led]->BSRRH = GPIO_PIN[Led];  		// Hi ==> Lo
}

void PORT_LEDOff(Led_TypeDef Led)
{
  GPIO_PORT[Led]->BSRRL = GPIO_PIN[Led];		// Lo ==> Hi
}

void PORT_LEDTog(Led_TypeDef Led)
{
  GPIO_PORT[Led]->ODR ^= GPIO_PIN[Led];
}

/*************************************************************************************/

void DSW_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	/* Enable the GPIO_LED Clock */
	RCC_AHB1PeriphClockCmd(DSW1_CLK|DSW2_CLK|DO24_CLK|PHIN1_CLK|PHIN3_CLK, ENABLE);

	/* Configure the GPIO_DSW pin */
	GPIO_InitStructure.GPIO_Pin = DSW1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DSW1_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = DSW2_PIN;
	GPIO_Init(DSW2_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = DSW3_PIN;
	GPIO_Init(DSW3_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = DSW4_PIN;
	GPIO_Init(DSW4_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = DO24_PIN;
	GPIO_Init(DO24_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = PHIN1_PIN;
	GPIO_Init(PHIN1_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = PHIN2_PIN;
	GPIO_Init(PHIN2_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = PHIN3_PIN;
	GPIO_Init(PHIN3_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = PHIN4_PIN;
	GPIO_Init(PHIN4_PORT, &GPIO_InitStructure);
}

void DSW_Read(void)
{
	u8 a, b, c=0;
	dsw_data = 0x5A;
	for(a=0;a<20;a++){
		b = 0;
		if(!(DSW1_PORT->IDR & 0x0020)) b |= 0x01; 
		if(!(DSW2_PORT->IDR & 0x0010)) b |= 0x02; 
		if(!(DSW3_PORT->IDR & 0x0008)) b |= 0x04; 	
		if(!(DSW4_PORT->IDR & 0x0004)) b |= 0x08;
		if(b==dsw_data){ 
			if((++c)>=2) break; 
		}else{
			dsw_data = b;
			c = 0;
		}
	}
	printf0("\r DIP SW = %2x", dsw_data);
	if(a>=20) printf0("\t *** DIP SW read Error!!! ***");
}

      
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
