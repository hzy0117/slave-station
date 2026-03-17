/*您好
您还在用RS485或232通信吗？那就落伍了！！！
CANweb是结合CAN和以太网优点的CAN现场总线高层协议，比CANOpen\DeviceNet\iCAN简易、好用；
CANWeb通信可靠(可选冗余)、节点多(最多98个CAN节点)、速度快(1Mbps)、通信距离远(10公里)；
通过CANWeb，可Web网页浏览监控、调试、配置您的每个CAN节点内部变量和硬件接口DIDOAIAO；
通过CANWeb，可用以太网、RS485、PROFIBUS、物联网、I2C、SPI与每个CAN节点实时快速通信；
CANWeb接口成本低，和增加一个RS485接口成本一样，双CAN的STM32F105RBT6芯片仅8元左右！
CANWeb技术全面开源，无授权费，免费提供CANWeb高级协议C++Keil源程序(可为用户芯片定制)；
用户可在此冗余双CANWeb开发板的PCB文件及Keil源程序基础上，轻松、快捷开发自己的产品，如
多通道DI、DO、AI、AO模块、温度、压力现场智能仪表、电动调节阀、电磁阀、电量表、RS485网
关（可与RS485的设备通信） 等设备。   

CANWeb总线是微群电子依托交通大学，总结多年大型电厂控制系统IO模块的设计、生产经验，
利用先进的电子技术，结合 CAN总线 、以太网的优点，扬长避短，精心设计出高性价比的现场总线
网络集成技术CANWeb标准（类似CANOpen、Devicenet、iCAN标准)。

CANWebSTMF105开发板从站采用 STM32F105RBT6芯片，板上支持冗余双CANWeb，4个DI，4个DO，1个AI
，1个AO；开发板提供Keil源程序及PCB文件。
*通过MST模块的Web页面，用户可实时监视、控制开发板从站上的DI\DO\AI\AO硬件接口；
*通过MST模块的Web页面，用户可实时监视、控制最多48个开发板从站内部WORD变量；
*通过MST模块的Web页面，用户可配置最多72个开发板从站内部WORD配置参数，配置参数可掉电保存；
*通过MST模块的以太网或RS485 Modbus协议，用户可与最多98个开发板从站实时交换数据。

QQ515192147索取_CANWebSTMF105开发板套件快速入门.doc 
QQ515192147索取_CANWeb总线技术详细介绍
http://bauto.taobao.com
*/////////////////////////////////////////////////////////////////
/**
  ******************************************************************************
  * @file GPIO/IOToggle/stm32f10x_it.h 
  * @author  MCD Application Team
  * @version  V3.0.0
  * @date  04/06/2009
  * @brief  This file contains the headers of the interrupt handlers.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F10x_IT_H
#define __STM32F10x_IT_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
//#include "stm32f10x_nvic.h"
//#include "stm32f10x_lib.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

//extern "C" void NMIException(void);
//extern "C" void HardFaultException(void);
//extern "C" void MemManageException(void);
//extern "C" void BusFaultException(void);
//extern "C" void UsageFaultException(void);
//extern "C" void DebugMonitor(void);
//extern "C" void SVCHandler(void);
//extern "C" void PendSVC(void);
//extern "C" void SysTickHandler(void);
//extern "C" void WWDG_IRQHandler(void);
//extern "C" void PVD_IRQHandler(void);
//extern "C" void TAMPER_IRQHandler(void);
//extern "C" void RTC_IRQHandler(void);
//extern "C" void FLASH_IRQHandler(void);
//extern "C" void RCC_IRQHandler(void);
//extern "C" void EXTI0_IRQHandler(void);
//extern "C" void EXTI1_IRQHandler(void);
//extern "C" void EXTI2_IRQHandler(void);
//extern "C" void EXTI3_IRQHandler(void);
//extern "C" void EXTI4_IRQHandler(void);
//extern "C" void DMA1_Channel1_IRQHandler(void);
//extern "C" void DMA1_Channel2_IRQHandler(void);
//extern "C" void DMA1_Channel3_IRQHandler(void);
//extern "C" void DMA1_Channel4_IRQHandler(void);
//extern "C" void DMA1_Channel5_IRQHandler(void);
//extern "C" void DMA1_Channel6_IRQHandler(void);
//extern "C" void DMA1_Channel7_IRQHandler(void);
//extern "C" void ADC1_2_IRQHandler(void);
//extern "C" void USB_HP_CAN_TX_IRQHandler(void);
//extern "C" void USB_LP_CAN1_RX0_IRQHandler(void);
//extern "C" void CAN1_RX0_IRQHandler(void);   //
//extern "C" void CAN_RX1_IRQHandler(void);
//extern "C" void CAN_SCE_IRQHandler(void);
//extern "C" void CAN2_RX0_IRQHandler(void);  //
//extern "C" void CAN2_RX1_IRQHandler(void);   // 
//extern "C" void EXTI9_5_IRQHandler(void);
//extern "C" void TIM1_BRK_IRQHandler(void);
//extern "C" void TIM1_UP_IRQHandler(void);
//extern "C" void TIM1_TRG_COM_IRQHandler(void);
//extern "C" void TIM1_CC_IRQHandler(void);
//extern "C" void TIM2_IRQHandler(void);
//extern "C" void TIM3_IRQHandler(void);
//extern "C" void TIM4_IRQHandler(void);
//extern "C" void I2C1_EV_IRQHandler(void);
//extern "C" void I2C1_ER_IRQHandler(void);
//extern "C" void I2C2_EV_IRQHandler(void);
//extern "C" void I2C2_ER_IRQHandler(void);
//extern "C" void SPI1_IRQHandler(void);
//extern "C" void SPI2_IRQHandler(void);
//extern "C" void USART1_IRQHandler(void);
//extern "C" void USART2_IRQHandler(void);
//extern "C" void USART3_IRQHandler(void);
//extern "C" void EXTI15_10_IRQHandler(void);
//extern "C" void RTCAlarm_IRQHandler(void);
//extern "C" void USBWakeUp_IRQHandler(void);
//extern "C" void TIM8_BRK_IRQHandler(void);
//extern "C" void TIM8_UP_IRQHandler(void);
//extern "C" void TIM8_TRG_COM_IRQHandler(void);
//extern "C" void TIM8_CC_IRQHandler(void);
//extern "C" void ADC3_IRQHandler(void);
//extern "C" void FSMC_IRQHandler(void);
//extern "C" void SDIO_IRQHandler(void);
//extern "C" void TIM5_IRQHandler(void);
//extern "C" void SPI3_IRQHandler(void);
//extern "C" void UART4_IRQHandler(void);
//extern "C" void UART5_IRQHandler(void);
//extern "C" void TIM6_IRQHandler(void);
//extern "C" void TIM7_IRQHandler(void);
//extern "C" void DMA2_Channel1_IRQHandler(void);
//extern "C" void DMA2_Channel2_IRQHandler(void);
//extern "C" void DMA2_Channel3_IRQHandler(void);
//extern "C" void DMA2_Channel4_5_IRQHandler(void);
/** @addtogroup StdPeriph_Examples
  * @{
  */

/** @addtogroup GPIO_IOToggle
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval : None
  */
//extern "C" void NMI_Handler(void)
//{
//}

///*
//  USART1中断服务程序
//*/


//							  
//extern "C" void USART1_IRQHandler(void)
//{

//}

///*******************************************************************************
//* Function Name  : USB_LP_CAN_RX0_IRQHandler
//* Description    : This function handles USB Low Priority or CAN RX0 interrupts 
//*                  requests.
//* Input          : None
//* Output         : None
//* Return         : None
//*******************************************************************************/
//extern "C" void CAN1_RX0_IRQHandler(void)
//{

//}

///***********************
// CAN2 FIFO1
//***********************/
//extern "C" void CAN2_RX1_IRQHandler(void)
//{

//}

///***********************
// CAN2 FIFO    暂不使用
//***********************/								   								  
//extern "C" void CAN2_RX0_IRQHandler(void)
//{

//}								 								



///**
//  * @brief  This function handles Hard Fault exception.
//  * @param  None
//  * @retval : None
//  */
//extern "C" void HardFault_Handler(void)
//{
//  /* Go to infinite loop when Hard Fault exception occurs */
//  while (1)
//  {
//  }
//}

///**
//  * @brief  This function handles Memory Manage exception.
//  * @param  None
//  * @retval : None
//  */
//extern "C" void MemManage_Handler(void)
//{
//  /* Go to infinite loop when Memory Manage exception occurs */
//  while (1)
//  {
//  }
//}

///**
//  * @brief  This function handles Bus Fault exception.
//  * @param  None
//  * @retval : None
//  */
//extern "C" void BusFault_Handler(void)
//{
//  /* Go to infinite loop when Bus Fault exception occurs */
//  while (1)
//  {
//  }
//}

///**
//  * @brief  This function handles Usage Fault exception.
//  * @param  None
//  * @retval : None
//  */
//extern "C" void UsageFault_Handler(void)
//{
//  /* Go to infinite loop when Usage Fault exception occurs */
//  while (1)
//  {
//  }
//}

///**
//  * @brief  This function handles SVCall exception.
//  * @param  None
//  * @retval : None
//  */
//extern "C" void SVC_Handler(void)
//{
//}

///**
//  * @brief  This function handles Debug Monitor exception.
//  * @param  None
//  * @retval : None
//  */
//extern "C" void DebugMon_Handler(void)
//{
//}

///**
//  * @brief  This function handles PendSV_Handler exception.
//  * @param  None
//  * @retval : None
//  */
//extern "C" void PendSV_Handler(void)
//{
//}

///**
//  * @brief  This function handles SysTick Handler.
//  * @param  None
//  * @retval : None
//  */
////extern "C" void SysTick_Handler(void)
////{
////}

///******************************************************************************/
///*                 STM32F10x Peripherals Interrupt Handlers                   */
///*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
///*  available peripheral interrupt handler's name please refer to the startup */
///*  file (startup_stm32f10x_xx.s).                                            */
///******************************************************************************/

///**
//  * @brief  This function handles PPP interrupt request.
//  * @param  None
//  * @retval : None
//  */
///*void PPP_IRQHandler(void)
//{
//}*/

///**
//  * @}
//  */

///**
//  * @}
//  */

///******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/




//#endif /* __STM32F10x_IT_H */

///******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/


