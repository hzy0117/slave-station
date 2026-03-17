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
  * @file GPIO/IOToggle/stm32f10x_conf.h 
  * @author  MCD Application Team
  * @version  V3.0.0
  * @date  04/06/2009
  * @brief  Library configuration file.
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
#ifndef __STM32F10x_CONF_H
#define __STM32F10x_CONF_H

/* Includes ------------------------------------------------------------------*/
/* Uncomment the line below to enable peripheral header file inclusion */
/* #include "stm32f10x_adc.h" */
/* #include "stm32f10x_bkp.h" */
/* #include "stm32f10x_can.h" */
/* #include "stm32f10x_crc.h" */
/* #include "stm32f10x_dac.h" */
/* #include "stm32f10x_dbgmcu.h" */
/* #include "stm32f10x_dma.h" */
/* #include "stm32f10x_exti.h" */
// #include "stm32f10x_flash.h" 
/* #include "stm32f10x_fsmc.h" */
/* #include "stm32f10x_i2c.h" */
// #include "stm32f10x_iwdg.h" 
/* #include "stm32f10x_pwr.h" */
 #include "stm32f10x_rcc.h"
/* #include "stm32f10x_rtc.h" */
/* #include "stm32f10x_sdio.h" */
 #include "stm32f10x_spi.h" 
/* #include "stm32f10x_tim.h" */
 #include "stm32f10x_usart.h" 
 #include "stm32f10x_gpio.h" 
/* #include "stm32f10x_wwdg.h" */
 #include "misc.h"   /* High level functions for NVIC and SysTick (add-on to CMSIS functions) */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Uncomment the line below to expanse the "assert_param" macro in the 
   Standard Peripheral Library drivers code */
/* #define USE_FULL_ASSERT    1 */

/* Exported macro ------------------------------------------------------------*/
#ifdef  USE_FULL_ASSERT

/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param expr: If expr is false, it calls assert_failed function
  *   which reports the name of the source file and the source
  *   line number of the call that failed. 
  *   If expr is true, it returns no value.
  * @retval : None
  */
  #define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
  void assert_failed(uint8_t* file, uint32_t line);
#else
  #define assert_param(expr) ((void)0)
#endif /* USE_FULL_ASSERT */

#endif /* __STM32F10x_CONF_H */

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
