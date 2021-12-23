  
/* Includes ------------------------------------------------------------------*/

#include "stm324x9i_eval.h"
#include "stm32f4xx_sdio.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_i2c.h"

/** @defgroup STM324x9I_EVAL_LOW_LEVEL 
  * @brief This file provides firmware functions to manage Leds, push-buttons, 
  *        COM ports, SD card on SDIO and serial EEPROM (sEE) available on 
  *        STM324x9I-EVAL evaluation board from STMicroelectronics.
  * @{
  */ 

USART_TypeDef* COM_USART[COMn] = {			EVAL_COM1,				EVAL_COM2,     			EVAL_COM3,     			EVAL_COM4,				EVAL_COM5,     			EVAL_COM6,     			EVAL_COM7				}; 
GPIO_TypeDef* COM_TX_PORT[COMn] = {			EVAL_COM1_TX_GPIO_PORT,	EVAL_COM2_TX_GPIO_PORT,	EVAL_COM3_TX_GPIO_PORT,	EVAL_COM4_TX_GPIO_PORT,	EVAL_COM5_TX_GPIO_PORT,	EVAL_COM6_TX_GPIO_PORT,	EVAL_COM7_TX_GPIO_PORT	};
GPIO_TypeDef* COM_RX_PORT[COMn] = {			EVAL_COM1_RX_GPIO_PORT,	EVAL_COM2_RX_GPIO_PORT,	EVAL_COM3_RX_GPIO_PORT,	EVAL_COM4_RX_GPIO_PORT,	EVAL_COM5_RX_GPIO_PORT,	EVAL_COM6_RX_GPIO_PORT,	EVAL_COM7_RX_GPIO_PORT	};
const uint32_t COM_USART_CLK[COMn] = {		EVAL_COM1_CLK,			EVAL_COM2_CLK,			EVAL_COM3_CLK,			EVAL_COM4_CLK,			EVAL_COM5_CLK,			EVAL_COM6_CLK,			EVAL_COM7_CLK			};
const uint32_t COM_TX_PORT_CLK[COMn] = {	EVAL_COM1_TX_GPIO_CLK,	EVAL_COM2_TX_GPIO_CLK,	EVAL_COM3_TX_GPIO_CLK,	EVAL_COM4_TX_GPIO_CLK,	EVAL_COM5_TX_GPIO_CLK,	EVAL_COM6_TX_GPIO_CLK,	EVAL_COM7_TX_GPIO_CLK	};
const uint32_t COM_RX_PORT_CLK[COMn] = {	EVAL_COM1_RX_GPIO_CLK,	EVAL_COM2_RX_GPIO_CLK,	EVAL_COM3_RX_GPIO_CLK,	EVAL_COM4_RX_GPIO_CLK,	EVAL_COM5_RX_GPIO_CLK,	EVAL_COM6_RX_GPIO_CLK,	EVAL_COM7_RX_GPIO_CLK	};
const uint16_t COM_TX_PIN[COMn] = {			EVAL_COM1_TX_PIN,		EVAL_COM2_TX_PIN,		EVAL_COM3_TX_PIN,		EVAL_COM4_TX_PIN,		EVAL_COM5_TX_PIN,		EVAL_COM6_TX_PIN,		EVAL_COM7_TX_PIN		};
const uint16_t COM_RX_PIN[COMn] = {			EVAL_COM1_RX_PIN,		EVAL_COM2_RX_PIN,		EVAL_COM3_RX_PIN,		EVAL_COM4_RX_PIN,		EVAL_COM5_RX_PIN,		EVAL_COM6_RX_PIN,		EVAL_COM7_RX_PIN		};
const uint16_t COM_TX_PIN_SOURCE[COMn] = {	EVAL_COM1_TX_SOURCE,	EVAL_COM2_TX_SOURCE,	EVAL_COM3_TX_SOURCE,	EVAL_COM4_TX_SOURCE,	EVAL_COM5_TX_SOURCE,	EVAL_COM6_TX_SOURCE,	EVAL_COM7_TX_SOURCE		};
const uint16_t COM_RX_PIN_SOURCE[COMn] = {	EVAL_COM1_RX_SOURCE,	EVAL_COM2_RX_SOURCE,	EVAL_COM3_RX_SOURCE,	EVAL_COM4_RX_SOURCE,	EVAL_COM5_RX_SOURCE,	EVAL_COM6_RX_SOURCE,	EVAL_COM7_RX_SOURCE		};
const uint16_t COM_TX_AF[COMn] = {			EVAL_COM1_TX_AF,		EVAL_COM2_TX_AF,		EVAL_COM3_TX_AF,		EVAL_COM4_TX_AF,		EVAL_COM5_TX_AF,		EVAL_COM6_TX_AF,		EVAL_COM7_TX_AF			};
const uint16_t COM_RX_AF[COMn] = {			EVAL_COM1_RX_AF,		EVAL_COM2_RX_AF,		EVAL_COM3_RX_AF,		EVAL_COM4_RX_AF,		EVAL_COM5_RX_AF,		EVAL_COM6_RX_AF,		EVAL_COM7_RX_AF			};
                                                     
/**
  * @brief  Configures COM port.
  * @param  COM: Specifies the COM port to be configured.
  *   This parameter can be one of following parameters:    
  *     @arg COM1
  *     @arg COM2  
  * @param  USART_InitStruct: pointer to a USART_InitTypeDef structure that
  *   contains the configuration information for the specified USART peripheral.
  * @retval None
  */
void STM_EVAL_COMInit(COM_TypeDef COM, USART_InitTypeDef* USART_InitStruct)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable GPIO clock */
  RCC_AHB1PeriphClockCmd(COM_TX_PORT_CLK[COM] | COM_RX_PORT_CLK[COM], ENABLE);

   /* Enable UART clock */
  if ((COM == COM1)||(COM == COM6))
  {
    RCC_APB2PeriphClockCmd(COM_USART_CLK[COM], ENABLE);
  }
  else
  {  
  	RCC_APB1PeriphClockCmd(COM_USART_CLK[COM], ENABLE);
  }

    RCC_APB2PeriphClockCmd(COM_USART_CLK[COM], ENABLE);
    RCC_APB1PeriphClockCmd(COM_USART_CLK[COM], ENABLE);

  /* Connect PXx to USARTx_Tx*/
  GPIO_PinAFConfig(COM_TX_PORT[COM], COM_TX_PIN_SOURCE[COM], COM_TX_AF[COM]);

  /* Connect PXx to USARTx_Rx*/
  GPIO_PinAFConfig(COM_RX_PORT[COM], COM_RX_PIN_SOURCE[COM], COM_RX_AF[COM]);

  /* Configure USART Tx as alternate function  */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

  GPIO_InitStructure.GPIO_Pin = COM_TX_PIN[COM];
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(COM_TX_PORT[COM], &GPIO_InitStructure);

  /* Configure USART Rx as alternate function  */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = COM_RX_PIN[COM];
  GPIO_Init(COM_RX_PORT[COM], &GPIO_InitStructure);

  /* USART configuration */
  USART_Init(COM_USART[COM], USART_InitStruct);
    
  /* Enable USART */
  USART_Cmd(COM_USART[COM], ENABLE);
}

    
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
