

#define ADC_SUPPLY_VOLTAGE		3300
#define ADC_VBAT_MULTI			4

#define TM_ADC_NTC		TM_ADC_Channel_4
#define TM_ADC_XP		TM_ADC_Channel_5
#define TM_ADC_YP		TM_ADC_Channel_6
#define TM_ADC_STRIP	TM_ADC_Channel_7

typedef enum {
	TM_ADC_Channel_0,  /*!< Operate with ADC channel 0 */
	TM_ADC_Channel_1,  /*!< Operate with ADC channel 1 */
	TM_ADC_Channel_2,  /*!< Operate with ADC channel 2 */
	TM_ADC_Channel_3,  /*!< Operate with ADC channel 3 */
	TM_ADC_Channel_4,  /*!< Operate with ADC channel 4 */
	TM_ADC_Channel_5,  /*!< Operate with ADC channel 5 */
	TM_ADC_Channel_6,  /*!< Operate with ADC channel 6 */
	TM_ADC_Channel_7,  /*!< Operate with ADC channel 7 */
	TM_ADC_Channel_8,  /*!< Operate with ADC channel 8 */
	TM_ADC_Channel_9,  /*!< Operate with ADC channel 9 */
	TM_ADC_Channel_10, /*!< Operate with ADC channel 10 */
	TM_ADC_Channel_11, /*!< Operate with ADC channel 11 */
	TM_ADC_Channel_12, /*!< Operate with ADC channel 12 */
	TM_ADC_Channel_13, /*!< Operate with ADC channel 13 */
	TM_ADC_Channel_14, /*!< Operate with ADC channel 14 */
	TM_ADC_Channel_15, /*!< Operate with ADC channel 15 */
	TM_ADC_Channel_16, /*!< Operate with ADC channel 16 */
	TM_ADC_Channel_17, /*!< Operate with ADC channel 17 */
	TM_ADC_Channel_18  /*!< Operate with ADC channel 18 */
} TM_ADC_Channel_t;

void init_ADC(void);
void TM_ADC_InitADC(ADC_TypeDef* ADCx);
void TM_ADC_Init(ADC_TypeDef* ADCx, uint8_t channel);
uint16_t TM_ADC_Read(ADC_TypeDef* ADCx, uint8_t channel);
void TM_ADC_EnableVbat(void);
void TM_ADC_DisableVbat(void);
uint16_t TM_ADC_ReadVbat(ADC_TypeDef* ADCx);

void adc_init(void);
void adc_loop(void);

void TM4_LEDS_port(void);
void TM4_TIMER_Init(void);
void TM4_PWM_Init(void);
void TM4_LEDS_Init(void);
void init_BUZZ(void);

#ifdef _TM_ADC_C_
u8	scan_adc, cnt_adc;
u16	ntc_adc, xp_adc, yp_adc, strip_adc;
u16	ntc_sum, xp_sum, yp_sum, strip_sum;

#else
extern u16	ntc_adc, xp_adc, yp_adc, strip_adc;

#endif

