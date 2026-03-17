/*////////////////////////v19040223////////////////////////////////////////
1)DI/DO/AI/AO源程序，通过计算机Web页面可监控这些DI/DO/AI/AO；
2)CAN高级通信程序：简化CAN通信编程，类似CANOpen\DeviceNet\iCAN,
   通过计算机的Web页面就可以监控电路板的参数，方便电路板调试；
3)电路板配置参数(如电路板的地址、通信速度等)，能掉电保存，
   通过Web页面  可配置、读取这些参数；
4)支持实时参数的掉电保存功能,
   电路板示例了DO、AO的上电恢复掉电前状态的程序；
5)电路板预留一个CAN通信失败看门狗计时器，在接收不到有效CAN包时，
   看门狗计时器会累积计时，可用于电路板CAN通信断开判断处理；
有空加下QQ群204408503,在开发板文件夹自行下载源程序详细介绍说明资料*///
#pragma once
//Def_;_WLn(U16长度);_BLn(U8长度);_WSz;_BSz;C_IO cIO; S_IO sIO;标准化
#define Def_CANY1Y2      1 //=1_只有CAN1口,=2_CAN1及CAN2口都有
#define Def_CfgSWDI_Y1W0 1 //=1:开发板有配置DI,=0:开发板无配置DI，开发板Id一直固定为99，速度固定为1MBps
//#define Def_BOD_Y1W0     1 //掉电记忆功能BOD(Brown-Out Detect Interrupt),DO AO保持,可修改

#define Def_BODAx_Bf_WSz 128 //掉电保存数据(U16)长度
#define   Def_IAP_Bf_BSz 256 //扇区Sect数据(U8)的长度<=Def_FLASH_PAGE_BSz(F10x的page为2048byte)//

#define Def_CANIODmnIdV 0x89AB //CAN网络的域 
#define Def_GYAO_Bf_WSz  8   //共用数据(U16)长度,MST可对多个SIO同时发送数据

#if(Def_CfgSWDI_Y1W0==1) 
	//配置开关SYSCFGSW	IO定义,用户可根据自己的实际来修改, 本开发板 IO为PA15
	#define  SYSCFGSW_RCC_APB2Periph_GPIO  RCC_APB2Periph_GPIOA
	#define  SYSCFGSW_GIO_Pin              GPIO_Pin_15
	#define  SYSCFGSW_GPIO                 GPIOA
#endif

//本开发板 CAN1 RX PB8 ， CAN1 TX PB9 调试ok
	#define  CAN1_RCC_APB2Periph_GPIO  RCC_APB2Periph_GPIOB
	#define  CAN1_RX_GIO_Pin           GPIO_Pin_8
	#define  CAN1_TX_GIO_Pin           GPIO_Pin_9
	#define  CAN1_RXTX_GPIO            GPIOB

//本开发板 改成 CAN1 RX PA11 ， CAN1 TX PA12(北京董工要求) 调试ok
//	#define  CAN1_RCC_APB2Periph_GPIO  RCC_APB2Periph_GPIOA
//	#define  CAN1_RX_GIO_Pin           GPIO_Pin_11
//	#define  CAN1_TX_GIO_Pin           GPIO_Pin_12
//	#define  CAN1_RXTX_GPIO            GPIOA

//本开发板 CAN2 RX PB12,  CAN2 TX PB13 调试ok
	#define  CAN2_RCC_APB2Periph_GPIO  RCC_APB2Periph_GPIOB
	#define  CAN2_RX_GIO_Pin           GPIO_Pin_12
	#define  CAN2_TX_GIO_Pin           GPIO_Pin_13
	#define  CAN2_RXTX_GPIO            GPIOB

#include "misc.h"
#include <string.h>

#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_can.h"
#include "stm32f10x_flash.h" 
#include "stm32f10x_iwdg.h" 
#include "stm32f10x_exti.h" 

typedef float            flt32;      // 单精度浮点数（32位长度）
typedef double           dbl64;      // 双精度浮点数（64位长度）
////below as VC C+++
typedef unsigned char    UINT8;      // 无符号8位整型变量
typedef signed   char    INT8;       // 有符号8位整型变量
typedef unsigned short   UINT16;     // 无符号16位整型变量
typedef signed   short   INT16;      // 有符号16位整型变量
typedef unsigned int     UINT32;     // 无符号32位整型变量
typedef signed   int     INT32;      // 有符号32位整型变量
typedef long long signed int INT64;
//模块硬件配置Ix  交换数据Ix (不含PZ配置数据Ix)0~13 
#define Def_IxAutoSndNuMAX  14  //0~13 PZ的开始CmAx_Bf[4*Def_IxAutoSndNuMAX+2]
//2+IxNmDOW+IxNmAOW+IxNmDIW+IxNmAIW<=Def_IxAutoSndNuMAX
 struct S_CAN_Msg{//16BYTE  8WORD S_CAN_Msg
			UINT8     No;     //CAN 端口编号 =1CAN口1, =2CAN口2
			UINT8     format; // =0_标准帧，=1_扩展帧 //
			UINT8     type;   // =0_数据帧，=1_远程帧 //
			UINT8     len;    // Data Length Counter,最多 为8
			UINT32    id;     // CAN Message ID (11-bit or 29-bit)//
			UINT8     d[8];	 // CAN Message Data Bytes 0-8	
      };
class C_CANWeb////在一个程序中仅能调用一次, 已占用 CAN1_RX0_IRQHandler CAN2_RX1_IRQHandler
{///此类负责对 pAI0t3 pAI4t7 相关的值 进行置值
public:
/////////////Begin of 用户用函数/////////CANWeb标准接口///////
    UINT8  HTp;////模块类型在源程序中赋值,MST_Web能显示
 //监控参数(DOWL+AOWL+DIWL+AIWL)<=48个WORD
    UINT8  DOWL;//DOWL=2;//DO的数量为 DOWL*16  //!!用户可改!!MST_Web能显示
		UINT8  AOWL;//AOWL=4;//AO的数量为 AOWL     //!!用户可改!!MST_Web能显示
		UINT8  DIWL;//DIWL=1;//DI的数量为 DIWL*16	  //!!用户可改!!MST_Web能显示
		UINT8  AIWL;//AIWL=12;//AI的数量为 AIWL     //!!用户可改!!MST_Web能显示
 //配置参数数量>=9 并 <=72个WORD  PZ[0]~PZ[PZWL-1]在编程中不能赋值！！
		UINT8  PZWL;//PZWL=11;	//INT16*  PZAI的长度   MST_Web能显示
		UINT8  Initial();//必须对zCNWbIni的DOWL、AOWL、DIWL、DOWL、PZWL初始化后，才能调用zCNWb.Initial()!!!
		UINT8 DOWBg,DIWBg,AOWBg,AIWBg;//CmAI_Bf[],CmAO_Bf[]中的DIDOAIAO开始位，Initial()调用后被赋值	 		

    u8  SIO_Temperature(s16 SIOTmp);////从站温度此温度可显示在 从站列表中
    u8  SIO_DISet(u16 ChNo);        //ChNo=0~(cCW.DIWL*16-1)
    u8  SIO_DIRst(u16 ChNo);        //ChNo=0~(cCW.DIWL*16-1)
    u8  SIO_DOVal(u16 ChNo);        //ChNo=0~(cCW.DOWL*16-1)
    u8  SIO_AISet(u16 ChNo,s16 Val);//ChNo=0~(cCW.AIWL-1)
    s16 SIO_AOVal(u16 ChNo);        //ChNo=0~(cCW.AOWL-1)
    s16 SIO_GYAOVal(u16 ChNo);      //ChNo=0~7 
    u16 SIO_AfterRcvCANBagTmS(); //单位mS:SIO接收到有效CAN包后计时 <32000mS
   //////////////End of 用户用函数///////////CANWeb标准接口//////////

   ///////////下面用户 不用///////////////////////////////////////////////////////////////////////////
    u8  Scan_for_SndCANMsg();//在int main(void) for 中调用
    UINT32 Flagb28to13;//<<13  bit28~13
			
 	  UINT32 CNExRcvFIdBg;//接收过滤开始Id MST发送给SIO的
	  UINT32 CNExRcvFIdEd;//接收过滤结束Id  

	  UINT32 CNExRcvFIdBgGY;//发送过滤开始Id 
	  UINT32 CNExRcvFIdEdGY;//接收过滤共用Id  	id=100		
		//UINT8  AIGYW;

	  UINT32 CNExSndFIdBg;//发送过滤开始Id SIO发送给 MST的
	  UINT32 CNExSndFIdEd;//发送过滤结束Id  

	  UINT32 MsgSndidjCNExSndFIdBg;			
			
	  UINT32 CANRcvCnt;
	  UINT32 CANSndCnt;	
		UINT32 CANSndFailCnt; UINT32 CANBusOffCnt; 
 
	  UINT32 CANRcvCnt10s;
	  UINT32 CANSndCnt10s;	
		
	  UINT32 CANSndRcvCnt10sJS;
		
	  UINT32 CANRcvCntP10S;//每10秒接收的包 用于模块性能监视
	  UINT32 CANSndCntP10S;//每10秒发送的包	用于模块性能监视	
			
    UINT8 RcvCANMsg(u8 CAN1or2No,UINT8 msg_obj_num);
		UINT8 SndCANMsg_In_FIFO();//调用前需对 IxSnd赋值 IxSnd=0~IxSndMx-1;
	  S_CAN_Msg  MsgSnd[32];
	 UINT16     MsgSndCnt[32];		
	 
	////////Below is AIPZ来确定的//PZAI[2]~[134]///
 		    UINT16 BRtXK; //=PZAI[2];//通信速度:1000K,500K,250K,125K,100K,50K,20K,10K,5K_//11C14:10K_5K不行,1768_5K不行
				UINT16 MXPD;//=1000;//PZAI[4];				
				UINT16 MNPD;//=100;//PZAI[5];							
				UINT16 AIDB;//=10;//PZAI[6];		
			  UINT8 Id ;//PZAI[3];	//
	////////Below is 硬件设计来确定的///////
	      UINT8 HNo;
	/////////////C_CANWeb.Initial()后的输出/////////////////
				UINT16 DOBitBg; 
			  UINT16 DIBitBg;	 
   private:
    UINT16 i,j,k,l,m,n;
	 UINT16 IxSndWLen;//for 
	 UINT16 IxRcvWLen;	 
 
	 //UINT16	IxRcvSndNm;//=IxNmDOW+IxNmAOW+IxNmDIW+IxNmAIW+IxNmPZW;//IxRcvSndNm 所有的Ix
	 UINT16	IxAutoSndNu;//IxAutoSndNu=2+IxNmDOW+IxNmAOW+IxNmDIW+IxNmAIW; //IxAutoSndNu不含 IxNmPZW	 
   UINT16 RcvCANBagIntvTmSCntc2;
	 
	 UINT16 IxSnd;//IxSnd=0~IxSndMx-1
	 UINT16 IxRcv;//IxRcv=2~IxRcvMx-1 
	 ///Ix=0,1 预留
	 UINT8 IxBgDOW, IxNmDOW, IxEdDOWLn;
	 UINT8 IxBgAOW, IxNmAOW, IxEdAOWLn;
	 UINT8 IxBgDIW, IxNmDIW, IxEdDIWLn;
	 UINT8 IxBgAIW, IxNmAIW, IxEdAIWLn;
	 UINT8 IxBgPZW, IxNmPZW, IxEdPZWLn;
	 
 	 UINT16 CNTMXP[Def_IxAutoSndNuMAX];
	 UINT16 CNTMNP[Def_IxAutoSndNuMAX];	
	 
	
	 S_CAN_Msg MsgRcv;// for 接收用

  
		UINT16 i1ms,i1mslen,j1ms;
	  u16  CmAI_BfSQ[2+Def_IxAutoSndNuMAX*4];//0~3不用
		
		UINT16 LPC_CAN1_MOD_RstCnt;
		
	  //Begin of SndCANMsg_In_FIFO
	  INT16  IxSndJ;    UINT16  sBfi;   UINT8  IxSndOK; 
		//End of  SndCANMsg_In_FIFO
					////begin of  RcvCANMsg
					  INT16 IxRcvJ;	  UINT8 IxRcvOK;  UINT16 Bfi;
					////End of  RcvCANMsg 
					UINT8 InitialOKEq174; UINT8 foriscn;
					UINT16 Scan_for_SndCANMsgWCnt;
					///160617新移/////////////////
					UINT8 i1msJ;	
				UINT8 tBfi; 		
        UINT16 AIWDlt;				
	 	 		
};
class C_Timer//可多次调用 已占用  SysTick_Handler(void)
{
			public:
			 C_Timer();
			 u8 Initial();//专门针对 u32 ZOS_CNT3  2uS计数
			 u8 Reset();
			 u8 SetX10uS(u32 X10uS);
			 u32 PastX10uS();
			private:
			 u32 OldTick;
};
#if(Def_CfgSWDI_Y1W0==1) 
class C_CfgSWDI////在一个程序中仅能调用一次//
{//配置开关_PA15
public:
		u8 Initial();//
		u8 Scan_for100uS();//	100us  一次=0.1ms			
		u32 DIFltNu;//48+1Syscfg)ch共用<=250_DI 在0.1ms 扫描周期的计数上限//可通过配置程序设置
		u32	DICnt;
		UINT8 Initial127OK;	
};
#endif

#define Def_CmAIAO_Bf_WSz	130////YXLPC AIAO数据共享区
void  zcTimer_Init(void);//在 class C_Timer 调用前须调用一次。//C_Timer 类初始化 仅1次
UINT8 WDT_Init( uint16_t Toutms );//看门狗/////////////////////////////////
void WDT_Feed( void );// 喂狗

u8 IAP_buf_EraseFun(UINT8 Sect);//在调用IAP_buf_WriteFun()前必须调用一次
u8 IAP_buf_WriteFun(u8 Sect,void* Buf, UINT16 Buf_BLn);//8K/1K=8; Sect=0,1,..3,zml190401try0~3_OK
u8 IAP_buf_ReadFun(u8 Sect,void* Buf, UINT16 Buf_BLn);//8K/1K=8; Sect=0,1,...3,zml190401try0~3_OK
void IAP_buf_FLASH_Lock();//用IAP_buf_WriteFun调用后，需调用一次
#if(Def_BOD_Y1W0)	
UINT8 BOD_ReadFun( UINT16 Dest_WBg,void *Src_Buf, UINT16 Buf_WLn);
UINT8 BOD_WriteFun(UINT16 Dest_WBg,void *Src_Buf, UINT16 Buf_WLn);
UINT8 BOD_InitAfter3S( );
#endif

