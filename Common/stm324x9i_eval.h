/**
  ******************************************************************************
  * @file    stm324x9i_eval.h
  * @author  MCD Application Team
  * @version V1.0.3
  * @date    13-November-2013
  * @brief   This file contains definitions for STM324x9I_EVAL's Leds, push-buttons
  *          and COM ports hardware resources.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM324X9I_EVAL_H
#define __STM324X9I_EVAL_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32f4xx_usart.h"
#include "stm32_eval_legacy.h"
   
typedef enum 
{
  COM1 = 0,
  COM2 = 1,
  COM3 = 2,
  COM4 = 3,
  COM5 = 4,
  COM6 = 5,
  COM7 = 6
} COM_TypeDef;

#define COMn                             7
/**
 * @brief Definition for COM port1, connected to USART1
 */ 
#define EVAL_COM1                        USART1
#define EVAL_COM1_CLK                    RCC_APB2Periph_USART1
#define EVAL_COM1_TX_PIN                 GPIO_Pin_6
#define EVAL_COM1_TX_GPIO_PORT           GPIOB
#define EVAL_COM1_TX_GPIO_CLK            RCC_AHB1Periph_GPIOB
#define EVAL_COM1_TX_SOURCE              GPIO_PinSource6
#define EVAL_COM1_TX_AF                  GPIO_AF_USART1
#define EVAL_COM1_RX_PIN                 GPIO_Pin_7
#define EVAL_COM1_RX_GPIO_PORT           GPIOB
#define EVAL_COM1_RX_GPIO_CLK            RCC_AHB1Periph_GPIOB
#define EVAL_COM1_RX_SOURCE              GPIO_PinSource7
#define EVAL_COM1_RX_AF                  GPIO_AF_USART1
#define EVAL_COM1_IRQn                   USART1_IRQn

#define EVAL_COM2                        USART2
#define EVAL_COM2_CLK                    RCC_APB1Periph_USART2
#define EVAL_COM2_TX_PIN                 GPIO_Pin_2
#define EVAL_COM2_TX_GPIO_PORT           GPIOA
#define EVAL_COM2_TX_GPIO_CLK            RCC_AHB1Periph_GPIOA
#define EVAL_COM2_TX_SOURCE              GPIO_PinSource2
#define EVAL_COM2_TX_AF                  GPIO_AF_USART2
#define EVAL_COM2_RX_PIN                 GPIO_Pin_3
#define EVAL_COM2_RX_GPIO_PORT           GPIOA
#define EVAL_COM2_RX_GPIO_CLK            RCC_AHB1Periph_GPIOA
#define EVAL_COM2_RX_SOURCE              GPIO_PinSource3
#define EVAL_COM2_RX_AF                  GPIO_AF_USART2
#define EVAL_COM2_IRQn                   USART2_IRQn

#define EVAL_COM3                        USART3
#define EVAL_COM3_CLK                    RCC_APB1Periph_USART3
#define EVAL_COM3_TX_PIN                 GPIO_Pin_10
#define EVAL_COM3_TX_GPIO_PORT           GPIOB
#define EVAL_COM3_TX_GPIO_CLK            RCC_AHB1Periph_GPIOB
#define EVAL_COM3_TX_SOURCE              GPIO_PinSource10
#define EVAL_COM3_TX_AF                  GPIO_AF_USART3
#define EVAL_COM3_RX_PIN                 GPIO_Pin_11
#define EVAL_COM3_RX_GPIO_PORT           GPIOB
#define EVAL_COM3_RX_GPIO_CLK            RCC_AHB1Periph_GPIOB
#define EVAL_COM3_RX_SOURCE              GPIO_PinSource11
#define EVAL_COM3_RX_AF                  GPIO_AF_USART3
#define EVAL_COM3_IRQn                   USART3_IRQn

#define EVAL_COM4                        UART4
#define EVAL_COM4_CLK                    RCC_APB1Periph_UART4
#define EVAL_COM4_TX_PIN                 GPIO_Pin_10
#define EVAL_COM4_TX_GPIO_PORT           GPIOC
#define EVAL_COM4_TX_GPIO_CLK            RCC_AHB1Periph_GPIOC
#define EVAL_COM4_TX_SOURCE              GPIO_PinSource10
#define EVAL_COM4_TX_AF                  GPIO_AF_UART4
#define EVAL_COM4_RX_PIN                 GPIO_Pin_11
#define EVAL_COM4_RX_GPIO_PORT           GPIOC
#define EVAL_COM4_RX_GPIO_CLK            RCC_AHB1Periph_GPIOC
#define EVAL_COM4_RX_SOURCE              GPIO_PinSource11
#define EVAL_COM4_RX_AF                  GPIO_AF_UART4
#define EVAL_COM4_IRQn                   UART4_IRQn

#define EVAL_COM5                        UART5
#define EVAL_COM5_CLK                    RCC_APB1Periph_UART5
#define EVAL_COM5_TX_PIN                 GPIO_Pin_12
#define EVAL_COM5_TX_GPIO_PORT           GPIOC
#define EVAL_COM5_TX_GPIO_CLK            RCC_AHB1Periph_GPIOC
#define EVAL_COM5_TX_SOURCE              GPIO_PinSource12
#define EVAL_COM5_TX_AF                  GPIO_AF_UART5
#define EVAL_COM5_RX_PIN                 GPIO_Pin_2 
#define EVAL_COM5_RX_GPIO_PORT           GPIOD
#define EVAL_COM5_RX_GPIO_CLK            RCC_AHB1Periph_GPIOD
#define EVAL_COM5_RX_SOURCE              GPIO_PinSource2 
#define EVAL_COM5_RX_AF                  GPIO_AF_UART5
#define EVAL_COM5_IRQn                   UART5_IRQn

#define EVAL_COM6                        USART6
#define EVAL_COM6_CLK                    RCC_APB2Periph_USART6
#define EVAL_COM6_TX_PIN                 GPIO_Pin_6 
#define EVAL_COM6_TX_GPIO_PORT           GPIOC
#define EVAL_COM6_TX_GPIO_CLK            RCC_AHB1Periph_GPIOC
#define EVAL_COM6_TX_SOURCE              GPIO_PinSource6 
#define EVAL_COM6_TX_AF                  GPIO_AF_USART6
#define EVAL_COM6_RX_PIN                 GPIO_Pin_7  
#define EVAL_COM6_RX_GPIO_PORT           GPIOC
#define EVAL_COM6_RX_GPIO_CLK            RCC_AHB1Periph_GPIOC
#define EVAL_COM6_RX_SOURCE              GPIO_PinSource7 
#define EVAL_COM6_RX_AF                  GPIO_AF_USART6
#define EVAL_COM6_IRQn                   USART6_IRQn

#define EVAL_COM7                        UART7
#define EVAL_COM7_CLK                    RCC_APB1Periph_UART7
#define EVAL_COM7_TX_PIN                 GPIO_Pin_7 
#define EVAL_COM7_TX_GPIO_PORT           GPIOF
#define EVAL_COM7_TX_GPIO_CLK            RCC_AHB1Periph_GPIOF
#define EVAL_COM7_TX_SOURCE              GPIO_PinSource7
#define EVAL_COM7_TX_AF                  GPIO_AF_UART7
#define EVAL_COM7_RX_PIN                 GPIO_Pin_6 
#define EVAL_COM7_RX_GPIO_PORT           GPIOF
#define EVAL_COM7_RX_GPIO_CLK            RCC_AHB1Periph_GPIOF
#define EVAL_COM7_RX_SOURCE              GPIO_PinSource6 
#define EVAL_COM7_RX_AF                  GPIO_AF_UART7
#define EVAL_COM7_IRQn                   UART7_IRQn

/**
  * @}
  */ 
#endif


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
