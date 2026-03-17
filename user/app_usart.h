#ifndef _APP_USART_H_
#define _APP_USART_H_

#include "stm32f10x.h"
#include <stdio.h>

// USART1 核心配置宏定义（仅发送）
#define  APP_USARTx                   USART1
#define  APP_USART_CLK                RCC_APB2Periph_USART1
#define  APP_USART_APBxClkCmd         RCC_APB2PeriphClockCmd
#define  APP_USART_BAUDRATE           115200

// USART1 GPIO 配置（B6/B7为重映射引脚）
#define  APP_USART_GPIO_CLK           (RCC_APB2Periph_GPIOB)
#define  APP_USART_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd
#define  APP_USART_TX_GPIO_PORT       GPIOB   
#define  APP_USART_TX_GPIO_PIN        GPIO_Pin_6
#define  APP_USART_RX_GPIO_PORT       GPIOB  
#define  APP_USART_RX_GPIO_PIN        GPIO_Pin_7

// 新增：USART1重映射时钟宏定义
#define  APP_USART_REMAP_CLK          RCC_APB2Periph_AFIO  // 复用功能时钟
#define  APP_USART_REMAP_CMD          GPIO_PinRemapConfig
#define  APP_USART_REMAP_MODE         GPIO_Remap_USART1    // USART1重映射模式

// 函数声明（仅发送相关）
void APP_USART_Config(void);                    
void APP_Usart_SendByte(USART_TypeDef* pUSARTx, uint8_t ch);       
void APP_Usart_SendHalfWord(USART_TypeDef* pUSARTx, uint16_t ch);  
void APP_Usart_SendArray(USART_TypeDef* pUSARTx, uint8_t *array, uint8_t num); 
void APP_Usart_SendStr(USART_TypeDef* pUSARTx, char *str);         
int APP_fputc(int ch, FILE *f);                  

#endif // _APP_USART_H_