#define _TM_ADC_C_

#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_adc.h"
#include "common\32f429_port.h"
#include "util\tm_adc.h"

void init_ADC(void){
	TM_ADC_Init(ADC1, TM_ADC_Channel_4);
	TM_ADC_Init(ADC1, TM_ADC_Channel_5);
	TM_ADC_Init(ADC1, TM_ADC_Channel_6);
	TM_ADC_Init(ADC3, TM_ADC_Channel_7);
}

/* Private functions */
void TM_ADC_INT_Channel_4_Init(ADC_TypeDef* ADCx);
void TM_ADC_INT_Channel_5_Init(ADC_TypeDef* ADCx);
void TM_ADC_INT_Channel_6_Init(ADC_TypeDef* ADCx);
void TM_ADC_INT_Channel_7_Init(ADC_TypeDef* ADCx);
void TM_ADC_INT_InitPin(GPIO_TypeDef* GPIOx, uint16_t PinX);

void TM_ADC_Init(ADC_TypeDef* ADCx, uint8_t channel) {
	TM_ADC_Channel_t ch = (TM_ADC_Channel_t) channel;
	if (ch == TM_ADC_Channel_4) {
		TM_ADC_INT_Channel_4_Init(ADCx);
	} else if (ch == TM_ADC_Channel_5) {
		TM_ADC_INT_Channel_5_Init(ADCx);
	} else if (ch == TM_ADC_Channel_6) {
		TM_ADC_INT_Channel_6_Init(ADCx);
	} else if (ch == TM_ADC_Channel_7) {
		TM_ADC_INT_Channel_7_Init(ADCx);
	}	
	/* Init ADC */
	TM_ADC_InitADC(ADCx);
}

void TM_ADC_InitADC(ADC_TypeDef* ADCx) {
	ADC_InitTypeDef ADC_InitStruct;
	ADC_CommonInitTypeDef ADC_CommonInitStruct;

	/* Init ADC settings */
	ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStruct.ADC_ExternalTrigConv = DISABLE;
	ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStruct.ADC_NbrOfConversion = 1;
	ADC_InitStruct.ADC_ScanConvMode = DISABLE;
	
	/* Enable clock and fill resolution settings */
	if (ADCx == ADC1) {
		/* Enable ADC clock */
		RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
		
		/* Set resolution */
		ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;
	} else if (ADCx == ADC2) {
		/* Enable ADC clock */
		RCC->APB2ENR |= RCC_APB2ENR_ADC2EN;
		
		/* Set resolution */
		ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;
	} else if (ADCx == ADC3) {
		/* Enable ADC clock */
		RCC->APB2ENR |= RCC_APB2ENR_ADC3EN;
		
		/* Set resolution */
		ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;
	}

	/* Set common ADC settings */
	ADC_CommonInitStruct.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStruct.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStruct.ADC_Prescaler = ADC_Prescaler_Div4;
	ADC_CommonInitStruct.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_8Cycles;
	ADC_CommonInit(&ADC_CommonInitStruct);
	
	/* Initialize ADC */
	ADC_Init(ADCx, &ADC_InitStruct);
	
	/* Enable ADC */
	ADCx->CR2 |= ADC_CR2_ADON;
}

uint16_t TM_ADC_Read(ADC_TypeDef* ADCx, uint8_t channel) {
	uint32_t timeout = 0xFFFF;
	
	ADC_RegularChannelConfig(ADCx, channel, 1, ADC_SampleTime_112Cycles);

	/* Start software conversion */
	ADCx->CR2 |= (uint32_t)ADC_CR2_SWSTART;
	
	/* Wait till done */
	while (!(ADCx->SR & ADC_SR_EOC)) {
		if (timeout-- == 0x00) {
			return 0;
		}
	}
	
	/* Return result */
	return ADCx->DR;
}

void TM_ADC_EnableVbat(void) {
	/* Enable VBAT */
	ADC->CCR |= ADC_CCR_VBATE;
}

void TM_ADC_DisableVbat(void) {
	/* Disable VBAT */
	ADC->CCR &= ~ADC_CCR_VBATE;
}

uint16_t TM_ADC_ReadVbat(ADC_TypeDef* ADCx) {
	uint32_t result;
	/* Read battery voltage data */
	/* Start conversion */
	ADC_RegularChannelConfig(ADCx, ADC_Channel_Vbat, 1, ADC_SampleTime_112Cycles);
	
	/* Start software conversion */
	ADCx->CR2 |= (uint32_t)ADC_CR2_SWSTART;
	
	/* Wait till done */
	while (!(ADCx->SR & ADC_SR_EOC));
	
	/* Get result */
	result = ADCx->DR;
	
	/* Convert to voltage */
	result = result * ADC_VBAT_MULTI * ADC_SUPPLY_VOLTAGE / 0xFFF;
	
	/* Return value in mV */
	return (uint16_t) result;
}

/* Private functions */
void TM_ADC_INT_Channel_4_Init(ADC_TypeDef* ADCx) {
	if (ADCx == ADC1 || ADCx == ADC2) {
		TM_ADC_INT_InitPin(GPIOA, GPIO_Pin_4);
	} 
}
void TM_ADC_INT_Channel_5_Init(ADC_TypeDef* ADCx) {
	if (ADCx == ADC1 || ADCx == ADC2) {
		TM_ADC_INT_InitPin(GPIOA, GPIO_Pin_5);
	} 
}
void TM_ADC_INT_Channel_6_Init(ADC_TypeDef* ADCx) {
	if (ADCx == ADC1 || ADCx == ADC2) {
		TM_ADC_INT_InitPin(GPIOA, GPIO_Pin_6);
	} 
}
void TM_ADC_INT_Channel_7_Init(ADC_TypeDef* ADCx) {
	if (ADCx == ADC1 || ADCx == ADC2) {
		TM_ADC_INT_InitPin(GPIOA, GPIO_Pin_7);
	}else if (ADCx == ADC3) {
		TM_ADC_INT_InitPin(GPIOF, GPIO_Pin_9);
	}
}


void TM_ADC_INT_InitPin(GPIO_TypeDef* GPIOx, uint16_t PinX) {
	/* Enable GPIO */
//	TM_GPIO_Init(GPIOx, PinX, TM_GPIO_Mode_AN, TM_GPIO_OType_PP, TM_GPIO_PuPd_DOWN, TM_GPIO_Speed_Medium);
	
	GPIO_InitTypeDef      GPIO_InitStructure;
	/* Configure ADC1 Channel6 pin as analog input ******************************/
	GPIO_InitStructure.GPIO_Pin = PinX;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOx, &GPIO_InitStructure);

}

/********************************************************************************
ADC2.4	PA4		NTC
ADC1.5	PA5		TOUCH X
ADC1.6	PA6		TOUCH Y
ADC3.7	PF9		STRIP CHECK
********************************************************************************/

void adc_init(void){
	scan_adc = cnt_adc = 0;
	xp_adc = yp_adc = ntc_adc = strip_adc = 0;
	xp_sum = yp_sum = ntc_sum = strip_sum = 0;
}
/*******************************************************************************/
#define X_TOP	3700
#define Y_TOP	2100
#define XP_GAP	30
#define YP_GAP	30

u16	xp_pre, xp_dat, yp_pre, yp_dat, cnt_tch=0;
//#define NEW_TCH
#define CNT_TAP		16*1

void adc_loop(void){						//600[us]
	switch(scan_adc){
		case 0:
			strip_sum += ADC3->DR;
			if(cnt_adc>=CNT_TAP){ strip_adc = strip_sum/CNT_TAP; strip_sum=0; }
			ADC_RegularChannelConfig(ADC1, TM_ADC_XP, 1, ADC_SampleTime_112Cycles);
			ADC1->CR2 |= (uint32_t)ADC_CR2_SWSTART;	/* Start software conversion */
			scan_adc = 1;
		break;
		case 1:
#ifdef	NEW_TCH
			xp_dat  = ADC1->DR;
#else
			xp_sum += ADC1->DR;
			if(cnt_adc>=CNT_TAP){ xp_adc = xp_sum/(CNT_TAP*4); xp_sum=0; }
#endif
			ADC_RegularChannelConfig(ADC1, TM_ADC_NTC, 1, ADC_SampleTime_112Cycles);
			ADC1->CR2 |= (uint32_t)ADC_CR2_SWSTART;	/* Start software conversion */
			scan_adc = 2;
			PORT_LEDlo(TSXP);	PORT_LEDhi(TSXN);	PORT_LEDhi(TSYP);	PORT_LEDlo(TSYN);		// Read Y
		break;
		case 2:
			ntc_sum += ADC1->DR;
			if(cnt_adc>=CNT_TAP){ ntc_adc = ntc_sum/CNT_TAP; ntc_sum=0; /*cnt_adc=0;*/ }
			ADC_RegularChannelConfig(ADC1, TM_ADC_YP, 1, ADC_SampleTime_112Cycles);
			ADC1->CR2 |= (uint32_t)ADC_CR2_SWSTART;	/* Start software conversion */
			scan_adc = 3;
		break;
		default:
#ifdef	NEW_TCH
			yp_dat  = ADC1->DR;
			if(((xp_pre+XP_GAP)>xp_dat)&&(xp_pre<(xp_dat+XP_GAP))
			 &&((yp_pre+YP_GAP)>yp_dat)&&(yp_pre<(yp_dat+YP_GAP))){
				xp_sum += xp_dat;
				yp_sum += yp_dat;
				if((++cnt_tch)>=CNT_TAP){
					xp_adc = xp_sum/(CNT_TAP*4); xp_sum=0; 
					yp_adc = yp_sum/(CNT_TAP*4); yp_sum=0;
					cnt_tch = 0;
				}
			}else{ xp_sum = yp_sum = cnt_tch = 0; }
			yp_pre = yp_dat;
			xp_pre = xp_dat;
#else
			yp_sum += ADC1->DR;
			if(cnt_adc>=CNT_TAP){ yp_adc = yp_sum/(CNT_TAP*4); yp_sum=0; cnt_adc=0; };			//38.4[ms] = 0.6*4*16
			cnt_adc++;
#endif
			ADC_RegularChannelConfig(ADC3, TM_ADC_STRIP, 1, ADC_SampleTime_112Cycles);
			ADC3->CR2 |= (uint32_t)ADC_CR2_SWSTART;	/* Start software conversion */
			scan_adc = 0;
			PORT_LEDhi(TSXP);	PORT_LEDlo(TSXN);	PORT_LEDlo(TSYP);	PORT_LEDhi(TSYN);		// Read X
		break;
	}		
}

/*******************************************************************************/
//	x-top	3700
//	y-top	2100
/*******************************************************************************/
/*******************************************************************************/
/*******************************************************************************/
/*******************************************************************************/
#define TM4_10K		7214
#define TM4_3K3		7214*3

/*******************************************************************************/
void TM4_TIMER_Init(void) {
    TIM_TimeBaseInitTypeDef TIM_BaseStruct;
    
    /* Enable clock for TIM4 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    TIM_BaseStruct.TIM_Prescaler = 0;
    TIM_BaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_BaseStruct.TIM_Period = TM4_3K3; /* 10kHz PWM */
    TIM_BaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_BaseStruct.TIM_RepetitionCounter = 0;
    /* Initialize TIM4 */
    TIM_TimeBaseInit(TIM4, &TIM_BaseStruct);
    /* Start count on TIM4 */
    TIM_Cmd(TIM4, ENABLE);
}
/*******************************************************************************/
void TM4_PWM_Init(void) {
    TIM_OCInitTypeDef TIM_OCStruct;
    
    /* Common settings */
    /* PWM mode 2 = Clear on compare match */
    /* PWM mode 1 = Set on compare match */
    TIM_OCStruct.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCStruct.TIM_OCPolarity = TIM_OCPolarity_Low;
    
/*
    To get proper duty cycle, you have simple equation
    pulse_length = ((TIM_Period + 1) * DutyCycle) / 100 - 1
    where DutyCycle is in percent, between 0 and 100%
    25% duty cycle:     pulse_length = ((8399 + 1) * 25) / 100 - 1 = 2099
    50% duty cycle:     pulse_length = ((8399 + 1) * 50) / 100 - 1 = 4199
    75% duty cycle:     pulse_length = ((8399 + 1) * 75) / 100 - 1 = 6299
    100% duty cycle:    pulse_length = ((8399 + 1) * 100) / 100 - 1 = 8399
    
    Remember: if pulse_length is larger than TIM_Period, you will have output HIGH all the time
*/
//    TIM_OCStruct.TIM_Pulse = 2099; /* 25% duty cycle */
//    TIM_OC1Init(TIM4, &TIM_OCStruct);
//    TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
    
    TIM_OCStruct.TIM_Pulse = TM4_3K3/2; /* 50% duty cycle */
    TIM_OC2Init(TIM4, &TIM_OCStruct);
    TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
    
//    TIM_OCStruct.TIM_Pulse = 6299; /* 75% duty cycle */
//    TIM_OC3Init(TIM4, &TIM_OCStruct);
//    TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
    
//    TIM_OCStruct.TIM_Pulse = 8399; /* 100% duty cycle */
//    TIM_OC4Init(TIM4, &TIM_OCStruct);
//    TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
}
/*******************************************************************************/
void TM4_LEDS_port(void) {
    GPIO_InitTypeDef GPIO_InitStruct;
    
    /* Clock for GPIOD */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
 
    /* Alternating functions for pins */
//    GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4);
//    GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_TIM4);
//    GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_TIM4);
    
    /* Set pins */
//    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStruct);
}
/*******************************************************************************/
void TM4_LEDS_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct;
    
    /* Clock for GPIOD */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
 
    /* Alternating functions for pins */
//    GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4);
//    GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_TIM4);
//    GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_TIM4);
    
    /* Set pins */
//    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStruct);
}
/*******************************************************************************/
extern void delay_ms(u32 dly);
extern u8 buzz_delay;
void init_BUZZ(void){
	PORT_LEDlo(ENVE );
	buzz_delay=0;
//	PORT_LEDhi(ENVE );
	TM4_LEDS_Init();
	TM4_TIMER_Init();
	TM4_PWM_Init();
//	delay_ms(50);
//	PORT_LEDlo(ENVE );
}
/*******************************************************************************/
/*******************************************************************************/
/*******************************************************************************/
/*******************************************************************************/
/*******************************************************************************/
