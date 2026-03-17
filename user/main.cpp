/*//////////////////v19040223///////////////////////////////////////
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
#include "main_CLS_DIDOAIAO.h"  //用户可以改
extern "C"
{
	#include "bsp_usart.h"
//		#include "jiexi.h"
}
#include "app.h"
C_DIDOAIAO cIO;                 //用户可以改
extern u16  CmAI_Bf[Def_CmAIAO_Bf_WSz]; 
extern INT16*  PZAI;
extern UINT16  BOD_Ax_Bf[Def_BODAx_Bf_WSz];

int i=0;
extern uint8_t rec_datas[3];
extern u8 zCCfgSW_DIOnOff; ////=1_ON =0_Off		
extern u8 zCCfgSW_DIOnOffBefore;
#if(Def_CfgSWDI_Y1W0==1) 
C_CfgSWDI cCfgSWDI;
#endif

extern C_CANWeb cCW;
u8  SystemInitialOK128;

extern "C" void CAN1_RX0_IRQHandler(void)//CAN1 FIFO0
{
//STM32有2个3级深度的接收缓冲区：FIFO0和FIFO1 
//如果是来自FIFO0的接收中断，则用CAN1_RX0_IRQn中断来处理。 
//YXCAN1_EXT_ReceiveMsg(&YXRcvCAN_Msg,0); 
	cCW.RcvCANMsg(1,0);
}
#if(Def_CANY1Y2==2)
extern "C" void CAN2_RX1_IRQHandler(void)//CAN2 FIFO1
{
	//YXCAN2_EXT_ReceiveMsg(&YXRcvCAN_Msg,0);	
	
					cCW.RcvCANMsg(2,0);			
     			
};
#endif	


int main(void)
{
	PZAI=(INT16*)&(CmAI_Bf[4*Def_IxAutoSndNuMAX+2]); 	
	SystemInit();
	
	zcTimer_Init();//C_Timer 类初始化 仅1次
	
	C_Timer ztimIOScan;  
	ztimIOScan.Initial();	
	
	C_Timer ztim;	
	ztim.Initial();
	
	C_Timer  WDT_Tim;   
	WDT_Tim.Initial();
	
#if(Def_CfgSWDI_Y1W0==1) 	
	cCfgSWDI.Initial();
#else
	zCCfgSW_DIOnOff=1;
#endif
	
	SystemInitialOK128=0;///
	
//我改的部分-------begin
	USART_Config();//串口1初始化	
	paraCheck();//参数检测
//我改的部分-------end
	for(;;)
	{

		
//		PVD_Scanfor();
		cCW.Scan_for_SndCANMsg();			
//    NTPCnttry++;//	裸跑测试////////////	
		cIO.Scanfor();	
//		if(rec_flag==1)
//		{
//			data_jiexi();
//////			rec_flag=0;
//		}
	
//		for(i=0;i<2;i++)
//		{
//			printf("%x",rec_datas[i]);
//		}

		
		
	if(	ztimIOScan.PastX10uS()>=10)//100uS定时器  0.1ms扫描一次/////////////////
	{		ztimIOScan.Reset();
		
		  zCCfgSW_DIOnOffBefore=zCCfgSW_DIOnOff;
		#if(Def_CfgSWDI_Y1W0==1) 
		  cCfgSWDI.Scan_for100uS();
		#endif
//		if(rec_flag==1)
//		{
//		
//			rec_flag=0;
//		}
		data_jiexi(); 
			if(SystemInitialOK128==128)
				{///////////////////////////软RESET指令///化了一整天时间找到的
					//if(CmAI_Bf[129]==12929)// //IO模块远程强制重启_掉电不保存
					if(zCCfgSW_DIOnOffBefore!=zCCfgSW_DIOnOff)
					{	
				  #if(Def_BOD_Y1W0) //sect0 PZ; sect1 BOD
						IAP_buf_WriteFun(1,&BOD_Ax_Bf[0],Def_BODAx_Bf_WSz*2);//STMF10x 的0用于PZ，1用于掉电保存
						IAP_buf_FLASH_Lock();
          #endif							
						NVIC_SystemReset();							
					}						
					if(PZAI[1]==12929)	//重启12929
					{		
            IAP_buf_EraseFun(0);						
						IAP_buf_WriteFun(0,&PZAI[2],(Def_CmAIAO_Bf_WSz-(4*Def_IxAutoSndNuMAX+4))*2);//LPC11C14 的2用于PZ，3用于掉电保存
						//PZAI=(INT16*)&(CmAI_Bf[4*Def_IxAutoSndNuMAX+2]);	PZAI[0],PZAI[1]不用保存，所以为保存开始位为4*Def_IxAutoSndNuMAX+4
          #if(Def_BOD_Y1W0) //sect0 PZ; sect1 BOD
						IAP_buf_WriteFun(1,&BOD_Ax_Bf[0],Def_BODAx_Bf_WSz*2);//STMF10x 的0用于PZ，1用于掉电保存
          #endif
						IAP_buf_FLASH_Lock();
						NVIC_SystemReset();
					}
				}		
			cIO.Scan100uS();
	}///End of 	100uS  0.1ms扫描一次//
	if(SystemInitialOK128==128)
		if(	WDT_Tim.PastX10uS()>=100000)//1S看门狗扫描/////////////
			{	
			 cIO.Scan1S();		
			  WDT_Tim.Reset();
			  WDT_Feed();
			}		


	if(SystemInitialOK128!=128)
				{	
							if(	ztim.PastX10uS()>=300000)	//3S 1次//因为zCSysCfg.DI的滤波为1S
							{	ztim.Reset();	
								SystemInitialOK128=128;
								IAP_buf_ReadFun(0,&PZAI[2],(Def_CmAIAO_Bf_WSz-(4*Def_IxAutoSndNuMAX+4))*2);	
								
							//PZAI=(INT16*)&(CmAI_Bf[4*Def_IxAutoSndNuMAX+2]);	PZW[0],PZW[1]不用保存，所以为保存开始位为4*Def_IxAutoSndNuMAX+4
		
				#if(Def_BOD_Y1W0) //sect0 PZ; sect1 BOD
							IAP_buf_ReadFun(1,&BOD_Ax_Bf[0],Def_BODAx_Bf_WSz*2);	
				#endif		
								cIO.Initial();
								WDT_Init(3000);//ms	
				#if(Def_BOD_Y1W0)	
								BOD_InitAfter3S();	
				#endif													
								//  BOD_InitAfter3S();		
										
							 }//End of if(	ztim.PastXuS()>=2000000)
							 continue;
				}
			
			
  }//end of for				
};

