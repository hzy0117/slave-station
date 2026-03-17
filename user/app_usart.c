#include "app_usart.h"

/**
 * @brief  USART1 初始化配置（仅发送功能，修复重映射+GPIO）
 * @param  无
 * @retval 无
 */
void APP_USART_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;

  // 步骤1：打开AFIO时钟（重映射必须）
  RCC_APB2PeriphClockCmd(APP_USART_REMAP_CLK, ENABLE);
  
  // 步骤2：开启USART1引脚重映射（B6=TX，B7=RX）
  APP_USART_REMAP_CMD(APP_USART_REMAP_MODE, ENABLE);

  // 步骤3：打开GPIO时钟（GPIOB）
  APP_USART_GPIO_APBxClkCmd(APP_USART_GPIO_CLK, ENABLE);
  
  // 步骤4：打开USART1外设时钟（APB2总线）
  APP_USART_APBxClkCmd(APP_USART_CLK, ENABLE);

  // 步骤5：配置TX引脚（GPIOB6）- 推挽复用输出（发送必须）
  GPIO_InitStructure.GPIO_Pin = APP_USART_TX_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    // 复用推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(APP_USART_TX_GPIO_PORT, &GPIO_InitStructure);

  // （可选）RX引脚：若不需要接收，直接注释以下3行
  GPIO_InitStructure.GPIO_Pin = APP_USART_RX_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(APP_USART_RX_GPIO_PORT, &GPIO_InitStructure);
  
  // 步骤6：配置USART1核心参数（仅发送模式）
  USART_InitStructure.USART_BaudRate = APP_USART_BAUDRATE; // 115200
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Tx; // 仅发送
  USART_Init(APP_USARTx, &USART_InitStructure);
  
  // 步骤7：使能USART1
  USART_Cmd(APP_USARTx, ENABLE);	    
}

/*****************  USART1 发送函数（逻辑不变） **********************/
void APP_Usart_SendByte(USART_TypeDef* pUSARTx, uint8_t ch)
{
  USART_SendData(pUSARTx, ch);
  while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
}

void APP_Usart_SendHalfWord(USART_TypeDef* pUSARTx, uint16_t ch)
{
  uint8_t half1, half2;
  half1 = (ch & 0xff00) >> 8;
  half2 = (ch & 0xff);
  APP_Usart_SendByte(pUSARTx, half1);
  APP_Usart_SendByte(pUSARTx, half2);
}

void APP_Usart_SendArray(USART_TypeDef* pUSARTx, uint8_t *array, uint8_t num)
{
  uint8_t i=0;
  for(;i<num;i++)
  {
    APP_Usart_SendByte(pUSARTx, array[i]);
  }
}

void APP_Usart_SendStr(USART_TypeDef* pUSARTx, char *str)
{
  uint8_t i=0;
  do
  {
    APP_Usart_SendByte(pUSARTx, str[i]);
    i++;
  }while(str[i]!='\0');
}

// printf重定向（可选）
int APP_fputc(int ch, FILE *f)
{
  USART_SendData(APP_USARTx, (uint8_t)ch);
  while(USART_GetFlagStatus(APP_USARTx, USART_FLAG_TXE) == RESET);
  return ch;
}