/*//////////////////v19040223///////////////////////////////////////
1)DI/DO/AI/AOԴ����ͨ�������Webҳ��ɼ����ЩDI/DO/AI/AO��
2)CAN�߼�ͨ�ų��򣺼�CANͨ�ű�̣�����CANOpen\DeviceNet\iCAN,
   ͨ���������Webҳ��Ϳ��Լ�ص�·��Ĳ����������·����ԣ�
3)��·�����ò���(���·��ĵ�ַ��ͨ���ٶȵ�)���ܵ��籣�棬
   ͨ��Webҳ��  �����á���ȡ��Щ������
4)֧��ʵʱ�����ĵ��籣�湦��,
   ��·��ʾ����DO��AO���ϵ�ָ�����ǰ״̬�ĳ���
5)��·��Ԥ��һ��CANͨ��ʧ�ܿ��Ź���ʱ�����ڽ��ղ�����ЧCAN��ʱ��
   ���Ź���ʱ�����ۻ���ʱ�������ڵ�·��CANͨ�ŶϿ��жϴ�����
�пռ���QQȺ204408503,�ڿ������ļ�����������Դ������ϸ����˵������*///
#include "main_CLS_DIDOAIAO.h"  //�û����Ը�
extern "C"
{
	#include "bsp_usart.h"
	#include "app_usart.h"
//		#include "jiexi.h"
}
#include "app.h"
C_DIDOAIAO cIO;                 //�û����Ը�
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
//STM32��2��3����ȵĽ��ջ�������FIFO0��FIFO1 
//���������FIFO0�Ľ����жϣ�����CAN1_RX0_IRQn�ж��������� 
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
	
	zcTimer_Init();//C_Timer ���ʼ�� ��1��
	
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
	
//�ҸĵĲ���-------begin
	USART_Config();//����1��ʼ��	
	paraCheck();//�������
	APP_USART_Config();

//�ҸĵĲ���-------end
	for(;;)
	{

//		PVD_Scanfor();
		cCW.Scan_for_SndCANMsg();			
//    NTPCnttry++;//	���ܲ���////////////	
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

		
		
	if(	ztimIOScan.PastX10uS()>=10)//100uS��ʱ��  0.1msɨ��һ��/////////////////
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
				{///////////////////////////��RESETָ��///����һ����ʱ���ҵ���
					//if(CmAI_Bf[129]==12929)// //IOģ��Զ��ǿ������_���粻����
					if(zCCfgSW_DIOnOffBefore!=zCCfgSW_DIOnOff)
					{	
				  #if(Def_BOD_Y1W0) //sect0 PZ; sect1 BOD
						IAP_buf_WriteFun(1,&BOD_Ax_Bf[0],Def_BODAx_Bf_WSz*2);//STMF10x ��0����PZ��1���ڵ��籣��
						IAP_buf_FLASH_Lock();
          #endif							
						NVIC_SystemReset();							
					}						
					if(PZAI[1]==12929)	//����12929
					{		
            IAP_buf_EraseFun(0);						
						IAP_buf_WriteFun(0,&PZAI[2],(Def_CmAIAO_Bf_WSz-(4*Def_IxAutoSndNuMAX+4))*2);//LPC11C14 ��2����PZ��3���ڵ��籣��
						//PZAI=(INT16*)&(CmAI_Bf[4*Def_IxAutoSndNuMAX+2]);	PZAI[0],PZAI[1]���ñ��棬����Ϊ���濪ʼλΪ4*Def_IxAutoSndNuMAX+4
          #if(Def_BOD_Y1W0) //sect0 PZ; sect1 BOD
						IAP_buf_WriteFun(1,&BOD_Ax_Bf[0],Def_BODAx_Bf_WSz*2);//STMF10x ��0����PZ��1���ڵ��籣��
          #endif
						IAP_buf_FLASH_Lock();
						NVIC_SystemReset();
					}
				}		
			cIO.Scan100uS();
	}///End of 	100uS  0.1msɨ��һ��//
	if(SystemInitialOK128==128)
		if(	WDT_Tim.PastX10uS()>=100000)//1S���Ź�ɨ��/////////////
			{	
			 cIO.Scan1S();		
			  WDT_Tim.Reset();
			  WDT_Feed();
			}		


	if(SystemInitialOK128!=128)
				{	
							if(	ztim.PastX10uS()>=300000)	//3S 1��//��ΪzCSysCfg.DI���˲�Ϊ1S
							{	ztim.Reset();	
								SystemInitialOK128=128;
								IAP_buf_ReadFun(0,&PZAI[2],(Def_CmAIAO_Bf_WSz-(4*Def_IxAutoSndNuMAX+4))*2);	
								
							//PZAI=(INT16*)&(CmAI_Bf[4*Def_IxAutoSndNuMAX+2]);	PZW[0],PZW[1]���ñ��棬����Ϊ���濪ʼλΪ4*Def_IxAutoSndNuMAX+4
		
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

