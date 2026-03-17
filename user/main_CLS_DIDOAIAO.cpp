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
#include "main_CLS_DIDOAIAO.h"
extern UINT16  CmAI_Bf[Def_CmAIAO_Bf_WSz]; 
extern UINT16  CmAO_Bf[Def_CmAIAO_Bf_WSz];
extern INT16*  PZAI;  //PZAI=(INT16*)&(CmAI_Bf[4*Def_IxAutoSndNuMAX+2]);
extern C_CANWeb cCW;
extern C_DIDOAIAO cIO;
//我改的部分-------begin
extern uint8_t rec_flag;//接收标志
extern uint8_t rec_datas[3];	
extern "C"
{
	#include "bsp_usart.h"
	#include "mapping.h"	
//#include "jiexi.h"
}
#include "app.h"
extern UINT8 ActionDown;		//自动模式下单个动作的完成标志
extern UINT8 NoParaAct;		//自动模式下不需要参数的动作标志
extern UINT8 DownloadFlag;  //参数下载完成标志
extern UINT8 ADcolumnnumber=0;      //AD转换第几个数
//extern uint8_t rec_flag;//接收标志
//int i=0;
//extern uint8_t rec_datas[3];
//我改的部分-------end
//////////////above is 共用///////////////////////////////////

#include "stm32f10x_adc.h"//for ADC
#include "stm32f10x_dma.h"//for ADC
#include "stm32f10x_dac.h"//for DAC

__IO u16 ADC_ConvertedValue[12]; //AI处理使用
__IO u16 ADC_SumValue[12]={0,0,0,0,0,0,0,0,0,0,0,0};//存储AI每一路数据的和
uint16_t ADC_usart_SumValue[12]={0,0,0,0,0,0,0,0,0,0,0,0};//串口发送的传感器数据
UINT8 C_DIDOAIAO::Initial(void)
{
		  //硬件类型编号
      cCW.HTp=Def_IOMTypeV;//Web 页面中 HTp
		  // PZ[0]=12891:128从站模块为强制模式，模块的DI、AI可强制置值； CmAI_Bf[128]
			// PZ[1]=12929:129从站模块远程强制重启；CmAI_Bf[129]
			// PZ[2]=10:   130从站模块的CAN口速率；///1_1000K,2__500K,3_250K,4_125K,5_100K,6_50K,7_20K_通信速度 8_10K,9_5K
			// PZ[3]=9:    131从站模块的CAN口的编号Id；//1~98 ,99不能用,99是配置开关ON的预置
			// PZ[4]=2500: SIO最长通信周期MXPD 单位mS
			// PZ[5]=250:  SIO最短通信周期MNPD 单位mS
			// PZ[6]=10:  SIOAI信号通信上传AD死区；   AI变量触发发送变化死区
	    //PZ[7]从站模块的采集量滤波值；//DI AI 有效		需用户自己编程处理!!!				
	   // if(PZAI[7]>250) PZAI[7]=50; if(PZAI[7]<2) PZAI[7]=2; 
    	//PZ[8]=2:  //从站硬件序列号 Web页面中 HNo=PZ[8]
	    //cCW.HNo=PZAI[8];///LPC11C14
///////////！！！上面PZ[0]~PZ[8]为共用,用户不要修改！！！/////////////////////////////////////

	
//Begin of 用户程序部分_用户初始化部分//////////////////////////////////
				 //配置参数数量>=9 并 <=72个WORD  PZ[0]~PZ[cCW.PZWL-1]在编程中不能赋值！！
				 //在编程中只能被引用,PZ[]是通过MST的Web 页面来设置的。
				 //具体参见：04冗余双CAN_STM32F105Keil开发板及源程序说明80128.doc  页16配置参数介绍	
				 cCW.PZWL=61;	                            //!!用户可改!!
				 
				 //监控参数(cCW.DOWL+cCW.AOWL+cCW.DIWL+cCW.AIWL)<=48个WORD
				 cCW.DOWL=1;//DO的数量为 cCW.DOWL*16  //!!用户可改!!
				 cCW.AOWL=13;//AO的数量为 cCW.AOWL     //!!用户可改!!
				 cCW.DIWL=2;//DI的数量为 cCW.DIWL*16	  //!!用户可改!!
				 cCW.AIWL=8;//AI的数量为 cCW.AIWL     //!!用户可改!!
				 /// 要求(DOWL+AOWL+DIWL+AIWL<=48个WORD		
				 
			  //!!DI 初始化 //cCW.SIO_DISet(u16 ChNo); cCW.SIO_DIRst(u16 ChNo); //ChNo=0~(cCW.DIWL*16-1)
				//PD8~PD15,PC6,PC7,PC8,PC9,PC10,PA8,PA9,PA10
				 GPIO_InitTypeDef GPIO_InitStructure;
				 RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOA, ENABLE); // 使能PD,PC,PA端口时钟  
				//PD
					GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8  |GPIO_Pin_9  |GPIO_Pin_10 |GPIO_Pin_11 |GPIO_Pin_12 |
																				GPIO_Pin_13 |GPIO_Pin_14 |GPIO_Pin_15 ;	
					GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
								////GPIO_InitStructure.GPIO_Mode 的取值 //////////////////
															//      GPIO_Mode_AIN = 0x0,           // 模拟输入 
															//      GPIO_Mode_IN_FLOATING = 0x04,  // 浮空输入 
															//      GPIO_Mode_IPD = 0x28,          // 下拉输入 
															//      GPIO_Mode_IPU = 0x48,          // 上拉输入 
															//      GPIO_Mode_Out_OD = 0x14,       // 开漏输出 
															//      GPIO_Mode_Out_PP = 0x10,       // 推挽输出 
															//      GPIO_Mode_AF_OD = 0x1C,        // 复用开漏输出 
															//     GPIO_Mode_AF_PP = 0x18         // 复用推挽输出 		 			
					GPIO_Init(GPIOD, &GPIO_InitStructure);  //初始化端口
				//PC
					GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6  |GPIO_Pin_7  |GPIO_Pin_8  |GPIO_Pin_9  |GPIO_Pin_10;	
					GPIO_Init(GPIOC, &GPIO_InitStructure);  //初始化端口
					
				//PA
					GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8  |GPIO_Pin_9  |GPIO_Pin_10;	
					GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化端口
					GPIOA->ODR &= (!GPIO_Pin_10);
					
				//!!DO初始化	  //u8 bbb=cCW.SIO_DOVal(u16 ChNo); //ChNo=0~(cCW.DOWL*16-1)
					//PD0~7,PE8~15
					RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE); // 使能端口时钟  
					//PD
					GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  |GPIO_Pin_1  |GPIO_Pin_2  |GPIO_Pin_3  |GPIO_Pin_4  |
																				GPIO_Pin_5  |GPIO_Pin_6  |GPIO_Pin_7 ;	
					GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
					GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
					GPIO_Init(GPIOD, &GPIO_InitStructure);  //初始化端口
				  GPIO_ResetBits(GPIOD, GPIO_Pin_All);//继电器控制初始化为0
					//PE
					GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8  |GPIO_Pin_9  |GPIO_Pin_10 |GPIO_Pin_11 |GPIO_Pin_12 |
																				GPIO_Pin_13 |GPIO_Pin_14 |GPIO_Pin_15 ;
					GPIO_Init(GPIOE, &GPIO_InitStructure);  //初始化端口
				  GPIO_ResetBits(GPIOE, GPIO_Pin_All);//继电器控制初始化为0
					
					
					
				//!!AI 初始化	 //cCW.SIO_AISet(u16 ChNo,s16 Val);//ChNo=0~(cCW.AIWL-1)
					 //PA0~7，PB0，PB1，PC0，PC1
					 /*配置采样通道端口 使能GPIO时钟	  设置ADC采样PA0端口信号*/  
						RCC_APB2PeriphClockCmd ( RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE );	
						//PA
						GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 |GPIO_Pin_1 |GPIO_Pin_2 |GPIO_Pin_3 |
																					GPIO_Pin_4 |GPIO_Pin_5 |GPIO_Pin_6 |GPIO_Pin_7; // 配置 ADC IO 引脚模式
						GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;	// 初始化 ADC IO
						GPIO_Init(GPIOA, &GPIO_InitStructure);		
						//PB
						GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 |GPIO_Pin_1; // 配置 ADC IO 引脚模式
						GPIO_Init(GPIOB, &GPIO_InitStructure);		
						//PC
						GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 |GPIO_Pin_1 ; // 配置 ADC IO 引脚模式
						GPIO_Init(GPIOC, &GPIO_InitStructure);		
						/*配置ADC1的工作模式为MDA模式  */		
						DMA_InitTypeDef DMA_InitStructure;
						ADC_InitTypeDef ADC_InitStructure;				
						// 打开DMA时钟
						RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
						// 打开ADC时钟
						RCC_APB2PeriphClockCmd ( RCC_APB2Periph_ADC1, ENABLE );				
						// 复位DMA控制器
						DMA_DeInit(DMA1_Channel1);				
						// 配置 DMA 初始化结构体
						// 外设基址为：ADC 数据寄存器地址
						DMA_InitStructure.DMA_PeripheralBaseAddr = ( uint32_t ) ( & ( ADC1->DR ) );				
						// 存储器地址，实际上就是一个内部SRAM的变量
						DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_ConvertedValue;				
						// 数据源来自外设
						DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;				
						// 缓冲区大小为1，缓冲区的大小应该等于存储器的大小
						DMA_InitStructure.DMA_BufferSize = 12;				
						// 外设寄存器只有一个，地址不用递增
						DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
						// 存储器地址递增
						DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 				
						// 外设数据大小为半字，即两个字节
						DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;				
						// 存储器数据大小也为半字，跟外设数据大小相同
						DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;				
						// 循环传输模式
						DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	
						// DMA 传输通道优先级为高，当使用一个DMA通道时，优先级设置不影响
						DMA_InitStructure.DMA_Priority = DMA_Priority_High;				
						// 禁止存储器到存储器模式，因为是从外设到存储器
						DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;				
						// 初始化DMA
						DMA_Init(DMA1_Channel1, &DMA_InitStructure);				
						// 使能 DMA 通道
						DMA_Cmd(DMA1_Channel1 , ENABLE);				
						// ADC 模式配置
						// 只使用一个ADC，属于单模式
						ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;				
						// 禁止扫描模式，多通道才要，单通道不需要
						ADC_InitStructure.ADC_ScanConvMode = ENABLE ; 
						// 连续转换模式
						ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
						// 不用外部触发转换，软件开启即可
						ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
						// 转换结果右对齐
						ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;				
						// 转换通道1个
						ADC_InitStructure.ADC_NbrOfChannel = 12;						
						// 初始化ADC
						ADC_Init(ADC1, &ADC_InitStructure);				
						// 配置ADC时钟为PCLK2的8分频，即9MHz
						RCC_ADCCLKConfig(RCC_PCLK2_Div8); 				
						// 配置 ADC 通道转换顺序为1，第一个转换，采样时间为55.5个时钟周期
						ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
						ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_55Cycles5);	
						ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_55Cycles5);
						ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 4, ADC_SampleTime_55Cycles5);
						ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 5, ADC_SampleTime_55Cycles5);
						ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 6, ADC_SampleTime_55Cycles5);
						ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 7, ADC_SampleTime_55Cycles5);
						ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 8, ADC_SampleTime_55Cycles5);
						ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 9, ADC_SampleTime_55Cycles5);
						ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 10, ADC_SampleTime_55Cycles5);
						ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 11, ADC_SampleTime_55Cycles5);
						ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 12, ADC_SampleTime_55Cycles5);
						// 使能ADC DMA 请求
						ADC_DMACmd(ADC1, ENABLE);				
						// 开启ADC ，并开始转换
						ADC_Cmd(ADC1, ENABLE);				
						// 初始化ADC 校准寄存器  
						ADC_ResetCalibration(ADC1);
						// 等待校准寄存器初始化完成
						while(ADC_GetResetCalibrationStatus(ADC1));				
						// ADC开始校准
						ADC_StartCalibration(ADC1);
						// 等待校准完成
						while(ADC_GetCalibrationStatus(ADC1));				
						// 由于没有采用外部触发，所以使用软件触发ADC转换 
						ADC_SoftwareStartConvCmd(ADC1, ENABLE);
						//!!AO初始化	 //s16 kkk=cCW.SIO_AOVal(u16 ChNo); //ChNo=0~(cCW.AOWL-1)
				//PA4/AI4/AO1/SPI1_NSS
					//GPIO_InitTypeDef GPIO_InitStructure;
//						DAC_InitTypeDef DAC_InitType;
//						RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );      //使能PORTA通道时钟
//						RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE );      //使能DAC通道时钟 

//						GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;                 // 端口配置
//						GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;          //模拟输入
//						GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//						GPIO_Init(GPIOA, &GPIO_InitStructure);
//						GPIO_SetBits(GPIOA,GPIO_Pin_4)    ;//PA.4 输出高
//														
//						DAC_InitType.DAC_Trigger=DAC_Trigger_None;    //不使用触发功能 TEN1=0
//						DAC_InitType.DAC_WaveGeneration=DAC_WaveGeneration_None;//不使用波形发生
//						DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude=DAC_LFSRUnmask_Bit0;//屏蔽、幅值设置
//						DAC_InitType.DAC_OutputBuffer=DAC_OutputBuffer_Disable ;    //DAC1输出缓存关闭 BOFF1=1
//						DAC_Init(DAC_Channel_1,&DAC_InitType);     //初始化DAC通道1
//						DAC_Cmd(DAC_Channel_1, ENABLE);  //使能DAC1	
//						
//						DAC_SetChannel1Data(DAC_Align_12b_R, 0);  //12位右对齐数据格式设置DAC值

//End of 用户程序部分_用户初始化部分////////////////////////////////////


	 cCW.Initial();//必须对zCNWbIni的DOW、AOWL、DIWL、DOWL、PZWL初始化后，才能调用zCNWb.Initial()!!!

//Begin of 用户程序部分///////////////////////////////	
	///客户定义变量的初始化///////////////////
	 ztimledtryok.Initial();	//一个测试定时器初始化	
   DIFlt=PZAI[7]; //必须把 DIFlt=PZAI[7];放在 cCW.Initial();后面!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	 #if(Def_BOD_Y1W0) //掉电保存数据处理 掉电保存 DO AO 
	 //BOD_ReadFun( UINT16 Dest_WBg,void *Src_Buf, UINT16 Buf_WLn);
		 BOD_ReadFun(0,&CmAO_Bf[cCW.DOWBg],cCW.DOWL);
		 BOD_ReadFun(0,&CmAI_Bf[cCW.DOWBg],cCW.DOWL);
		 
		 BOD_ReadFun(cCW.DOWL,&CmAO_Bf[cCW.AOWBg],cCW.AOWL);
		 BOD_ReadFun(cCW.DOWL,&CmAI_Bf[cCW.AOWBg],cCW.AOWL);	 	 
	 #endif 
//End of 用户程序部分///////////////////////////////

Initial127OK=127;	
	  return 1;                                 
};

u8 C_DIDOAIAO::Scan100uS()//在main.cpp的100us定时器里已被调用, 0.1ms扫描一次
{if(Initial127OK!=127) return 0;if(PZAI[0]==12891) return 0;//强制12891
	
//Begin of 用户程序部分///////////////////////////////	
	#if(Def_BOD_Y1W0)	//掉电保存数据处理 掉电保存 DO AO 
	//BOD_WriteFun(UINT16 Dest_WBg,void *Src_Buf, UINT16 Buf_WLn);
	  BOD_WriteFun(0,&CmAO_Bf[cCW.DOWBg], cCW.DOWL);
	  BOD_WriteFun(cCW.DOWL,&CmAO_Bf[cCW.AOWBg], cCW.AOWL);
  #endif		

		    //!!!从站温度 此温度可显示在 从站列表中,此温度
						s16 SIOTmp;//从站温度
						SIOTmp=-321;//321/10=32.1度
						cCW.SIO_Temperature(SIOTmp);//从站温度 此温度可显示在 从站列表中	
					
		    //!!DI 处理//cCW.SIO_DISet(u16 ChNo); cCW.SIO_DIRst(u16 ChNo); //ChNo=0~(cCW.DIWL*16-1)
			  //PD8~PD15，PC6，PC7，PC8，PC9，PA8，PA9，PA10, PC10    ///GPIO_ReadInputDataBit
						if(!(GPIOD->IDR&GPIO_Pin_8)) //PD8
								cCW.SIO_DIRst(0);	else cCW.SIO_DISet(0);
						if(!(GPIOD->IDR&GPIO_Pin_9)) //PD9
								cCW.SIO_DIRst(1);	else cCW.SIO_DISet(1);
						if(!(GPIOD->IDR&GPIO_Pin_10)) //PD10
								cCW.SIO_DIRst(2);	else cCW.SIO_DISet(2);
						if(!(GPIOD->IDR&GPIO_Pin_11)) //PD11
								cCW.SIO_DIRst(3);	else cCW.SIO_DISet(3);
						if(!(GPIOD->IDR&GPIO_Pin_12)) //PD12
								cCW.SIO_DIRst(4);	else cCW.SIO_DISet(4);
						if(!(GPIOD->IDR&GPIO_Pin_13)) //PD13
								cCW.SIO_DIRst(5);	else cCW.SIO_DISet(5);
						if(!(GPIOD->IDR&GPIO_Pin_14)) //PD14
								cCW.SIO_DIRst(6);	else cCW.SIO_DISet(6);
						if(!(GPIOD->IDR&GPIO_Pin_15)) //PD15
								cCW.SIO_DIRst(7);	else cCW.SIO_DISet(7);
						
						if(!(GPIOC->IDR&GPIO_Pin_6)) ////PC6
								cCW.SIO_DIRst(8);	else cCW.SIO_DISet(8);
						if(!(GPIOC->IDR&GPIO_Pin_7)) ////PC7
								cCW.SIO_DIRst(9);	else cCW.SIO_DISet(9);
						if(!(GPIOC->IDR&GPIO_Pin_8)) ////PC8
								cCW.SIO_DIRst(10);	else cCW.SIO_DISet(10);
						if(!(GPIOC->IDR&GPIO_Pin_9)) ////PC9
								cCW.SIO_DIRst(11);	else cCW.SIO_DISet(11);

						if(!(GPIOA->IDR&GPIO_Pin_8)) //PA8
								cCW.SIO_DIRst(12);	else cCW.SIO_DISet(12);
						if(!(GPIOA->IDR&GPIO_Pin_9)) //PA9
								cCW.SIO_DIRst(13);	else cCW.SIO_DISet(13);
						if(!(GPIOA->IDR&GPIO_Pin_10)) //PA10
								cCW.SIO_DIRst(14);	else cCW.SIO_DISet(14);
						
						//if(!(GPIOC->IDR&GPIO_Pin_10)) //PC10
							//	cCW.SIO_DIRst(15);	else cCW.SIO_DISet(15);

		    //!!DO处理//u8 bbb=cCW.SIO_DOVal(u16 ChNo); //ChNo=0~(cCW.DOWL*16-1)
				//PD0~7,PE8~15 DO0~D015
		
	//我改的部分-------begin
				//判断是否是手动模式
			
	//我改的部分-------end							
				    if(ADcolumnnumber<50)
				     {					   
					      ADC_SumValue[0]=ADC_SumValue[0]+ADC_ConvertedValue[0];
									       
					      ADC_SumValue[1]= ADC_SumValue[1]+ADC_ConvertedValue[1];
					    
						    ADC_SumValue[2]= ADC_SumValue[2]+ADC_ConvertedValue[2];
				
						    ADC_SumValue[3]=ADC_SumValue[3]+ADC_ConvertedValue[3];
				
				        ADC_SumValue[4]=ADC_SumValue[4]+ADC_ConvertedValue[4];
				
				        ADC_SumValue[5]=ADC_SumValue[5]+ADC_ConvertedValue[5];
				
				        ADC_SumValue[6]=ADC_SumValue[6]+ADC_ConvertedValue[6];
						
						    ADC_SumValue[7]=ADC_SumValue[7]+ADC_ConvertedValue[7];					   
						    
							  ADcolumnnumber++;
							 
							 
					   } 	
													
					  
					   else						   
             {
						 //!!AI 处理  PA0~7，PB0，PB1，PC0，PC1 //cCW.SIO_AISet(u16 ChNo,s16 Val);//ChNo=0~(cCW.AIWL-1)
//						//cCW.SIO_AISet(0,ADC_ConvertedValue[0]);
//							 cCW.SIO_AISet(0,(ADC_SumValue[0]*3300/4096)/50);
//						//cCW.SIO_AISet(1,ADC_ConvertedValue[1]);
//						   cCW.SIO_AISet(1,(ADC_SumValue[1]*3300/4096)/50);
//						//cCW.SIO_AISet(2,ADC_ConvertedValue[2]);
//						   cCW.SIO_AISet(2,(ADC_SumValue[2]*3300/4096)/50);
//						//cCW.SIO_AISet(3,ADC_ConvertedValue[3]);
//						   cCW.SIO_AISet(3,(ADC_SumValue[3]*3300/4096)/50);
						//cCW.SIO_AISet(4,ADC_ConvertedValue[4]);
						   cCW.SIO_AISet(4,(ADC_SumValue[4]*3300/4096)/50);
 						//cCW.SIO_AISet(5,ADC_ConvertedValue[5]);
						   cCW.SIO_AISet(5,(ADC_SumValue[5]*3300/4096)/50);
						//cCW.SIO_AISet(6,ADC_ConvertedValue[6]);
						   cCW.SIO_AISet(6,(ADC_SumValue[6]*3300/4096)/50);
						//cCW.SIO_AISet(7,ADC_ConvertedValue[7]);
						   cCW.SIO_AISet(7,(ADC_SumValue[7]*3300/4096)/50);
//						cCW.SIO_AISet(8,ADC_ConvertedValue[8]);
//						cCW.SIO_AISet(9,ADC_ConvertedValue[9]*3300/4096);
//						cCW.SIO_AISet(10,ADC_ConvertedValue[10]*3300/4096);
//						cCW.SIO_AISet(11,ADC_ConvertedValue[11]*3300/4096);

							ADC_usart_SumValue[0]=0xff;
							ADC_usart_SumValue[1]=(ADC_SumValue[0]*3300/4096)/50;
							ADC_usart_SumValue[2]=(ADC_SumValue[1]*3300/4096)/50;
							ADC_usart_SumValue[3]=(ADC_SumValue[2]*3300/4096)/50;
							ADC_usart_SumValue[4]=(ADC_SumValue[3]*3300/4096)/50;
							ADC_usart_SumValue[5]=0xee;
//							Usart_SendHalfWord(USART3,ADC_usart_SumValue[3]);
//							printf("%d",ADC_usart_SumValue[1]);
              Usart_SendArray(USART3,ADC_usart_SumValue,6);
		    //!!AO处理  //s16 kkk=cCW.SIO_AOVal(u16 ChNo); //ChNo=0~(cCW.AOWL-1)
					if(0 ==(cCW.SIO_AOVal(0)))//判断最低AO(0)最低位是否为0,为0则手动模式2020/05/15修改 
					{
					manalMode();
					}else//自动模式
					if(1 ==(cCW.SIO_AOVal(0)))//判断最低AO(0)最低位是否为1,为1则自动模式2020/05/15修改 
					{
					//自动模式函数
					autoAction();
					}
						
					if(2 ==(cCW.SIO_AOVal(0))&& DownloadFlag == 0)//2020/05/15修改 
					
					{//参数下载
					paraDownLoad();
					}
					if(3 ==(cCW.SIO_AOVal(0)))//判断最低AO(0)最低位是否为3,则为复位参数下载标志
				  {
				  cCW.SIO_DIRst(15);
				  }			
							
				ADcolumnnumber=0;
				for(int i=0;i<8;i++)
				ADC_SumValue[i]=0;						
				}
//
				data_jiexi();
	
			//目前的设计是开机后只有一次下载参数的机会
				
                    

				//!!CANWeb总线共用AO输出测试：
					 //CANWeb总线网络预留8个共用AO，MST可向多个从站同时发送共用AO值；
//					for(i=0;i<8;i++)	
//					cCW.SIO_AISet(8+i,cCW.SIO_GYAOVal(i));	


				//!!从站通信失败看门狗计时器
				//从站预留一个CANWeb通信失败看门狗计时器，在接收不到有效CAN包时，看门狗计时器会累积计时，可用于从站CAN通信断开判断处理；
				//测试 SIO接收到有效CAN包后计时 <32000mS	
          /////cCW.SIO_AfterRcvCANBagTmS()可用于从站CAN通信断开判断处理				
					//cCW.SIO_AISet(4,cCW.SIO_AfterRcvCANBagTmS());//
					
//End of 用户程序部分////////////////////////////////
	return 1;
};
u8 C_DIDOAIAO::Scan1S()////1秒扫描一次
{if(Initial127OK!=127) return 0;if(PZAI[0]==12891) return 0;//强制12891

//Begin of 用户程序部分///
//	printf("0x10");

//			if(rec_flag==1)
//	{
//		data_jiexi();
//		rec_flag=0;
//	}
		if(NoParaAct != 0)//从1开始
		{
			NoParaAct++;
			if(NoParaAct ==8)
			{
				NoParaAct = 0;//1秒后不用参数的动作结束
				ActionDown = 1;//表示动作完成
			}
		}
//End of 用户程序部分/////////////	
	
return 1;
};

u8 C_DIDOAIAO::Scanfor()//在main.cpp for(;;)中调用		
{if(Initial127OK!=127) return 0;if(PZAI[0]==12891) return 0;//强制12891
	
//Begin of 用户程序部分/////////////////////////////////	
				if(ztimledtryok.PastX10uS()>50000)//50000X10=500,000us=0.5秒  扫描一次，50000客户可改
				{ztimledtryok.Reset();

					
				}
	
//End of 用户程序部分///////////////////////////////	
	return 1;
};

void benzhan(uint8_t dongzuo)
{
	switch(dongzuo)
	{
		case 0x00:
			GPIO_ResetBits(GPIOD, GPIO_Pin_All);
			GPIO_ResetBits(GPIOE, GPIO_Pin_All);
		break;
		case 0x01:
			GPIO_SetBits(GPIOD,GPIO_Pin_0);
		break;
		case 0x02:
			GPIO_SetBits(GPIOD,GPIO_Pin_1);
		break;
		case 0x03:
			GPIO_SetBits(GPIOD,GPIO_Pin_2);
		break;
		case 0x04:
			GPIO_SetBits(GPIOD,GPIO_Pin_3);
		break;
		case 0x05:
			GPIO_SetBits(GPIOD,GPIO_Pin_4);
		break;
		case 0x06:
			GPIO_SetBits(GPIOD,GPIO_Pin_5);
		break;
		case 0x07:
			GPIO_SetBits(GPIOD,GPIO_Pin_6);
		break;
		case 0x08:
			GPIO_SetBits(GPIOD,GPIO_Pin_7);
		break;
		case 0x09:
			GPIO_SetBits(GPIOE,GPIO_Pin_8);
		break;
		case 0x0a:
			GPIO_SetBits(GPIOE,GPIO_Pin_9);
		break;
		case 0x0b:
			GPIO_SetBits(GPIOE,GPIO_Pin_10);
		break;
		case 0x0c:
			GPIO_SetBits(GPIOE,GPIO_Pin_11);
		break;
		case 0x0d:
			GPIO_SetBits(GPIOE,GPIO_Pin_12);
		break;
		case 0x0e:
			GPIO_SetBits(GPIOE,GPIO_Pin_13);
		break;
		case 0x0f:
			GPIO_SetBits(GPIOE,GPIO_Pin_14);
		break;
		case 0x10:
			GPIO_SetBits(GPIOE,GPIO_Pin_15);
		break;
	}
}

void linjia(uint8_t mode,uint8_t dongzuo)
{
	uint8_t mode_send=mode;
	uint8_t zushu=0x00;
	uint8_t dongzuo_send=dongzuo;
	uint8_t flag_send=0x01;
	cCW.SIO_AISet(4,mode_send);
//cCW.SIO_AISet(5,ADC_ConvertedValue[5]);
	cCW.SIO_AISet(5,zushu);
	//cCW.SIO_AISet(6,ADC_ConvertedValue[6]);
	cCW.SIO_AISet(6,dongzuo_send);
	//cCW.SIO_AISet(7,ADC_ConvertedValue[7]);
	cCW.SIO_AISet(7,flag_send);
	
	
	
}

void chengzhu(uint8_t mode,uint8_t zushu,uint8_t dongzuo)
{
	uint8_t mode_send=mode;
	uint8_t zushu_send=zushu;
	uint8_t dongzuo_send=dongzuo;
	uint8_t flag_send=0x01;
	
		cCW.SIO_AISet(4,mode_send);
//cCW.SIO_AISet(5,ADC_ConvertedValue[5]);
	cCW.SIO_AISet(5,zushu_send);
	//cCW.SIO_AISet(6,ADC_ConvertedValue[6]);
	cCW.SIO_AISet(6,dongzuo_send);
	//cCW.SIO_AISet(7,ADC_ConvertedValue[7]);
	cCW.SIO_AISet(7,flag_send);
}

void data_jiexi()
{
	if(rec_datas[0]==0x00)
	{
		benzhan(rec_datas[2]);
	}
	else if(rec_datas[0]==0x01||rec_datas[0]==0x02)
	{
		linjia(rec_datas[0],rec_datas[2]);
	}
	else if(rec_datas[0]==0x03||rec_datas[0]==0x04)
	{
		chengzhu(rec_datas[0],rec_datas[1],rec_datas[2]);
	}

}

//void sensior_set()
//{
//	_
//}
