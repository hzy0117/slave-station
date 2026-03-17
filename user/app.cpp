#include "app.h"
#include "main_CLS_DIDOAIAO.h"
#include "mapping.h"
extern "C"
{
	#include "bsp_usart.h"
}
extern __IO u16 ADC_ConvertedValue[12];
extern __IO u16 ADC_SumValue[12];//存储AI每一路数据的和
extern C_CANWeb cCW;
extern C_DIDOAIAO cIO;

UINT8 DO_StepFlags[16] = {0};//测试用来进行一次移动段的标志位
INT8 AutoComplete = -1; //自动模式执行情况标志
extern MAPPING DOS[16];	//每个数字量输出和ARM管脚的映射关系
extern CONPART parts[8];//8个控制部分的描述
UINT8 DownloadFlag = 0;

#define	fun_num  7//fun的真实行数
extern uint8_t fun[fun_num][2];//自动模式功能执行顺序
UINT8 ActionDown = 0;		//自动模式下单个动作的完成标志
UINT8 NoParaAct = 0;		//自动模式下不需要参数的动作标志

#define WRITED	0x14//参数检测的标志的值，每次更改参数都要改变

//参数检测写入函数
void paraCheck(void)
{
	//读取flash中的数据
	UINT8 writeflag = 0;
	//读参数写入标志位
	// 从0x02读1个字节，写入writeflag
	IAP_buf_ReadFun(0X02,&writeflag,1);
	printf("writeflag = 0x%x\n",writeflag);//打印一下writeflag
	
	if(writeflag == WRITED)
	{//flash中有数据，直接读入数组中
		//读取最大值
		IAP_buf_ReadFun(0x03,(uint8_t *)cIO.Max,sizeof(cIO.Max));
		//读取最小值
		IAP_buf_ReadFun(0x04,(uint8_t *)cIO.Min,sizeof(cIO.Min));
		//读取段值
		IAP_buf_ReadFun(0x05,(uint8_t *)cIO.Step,sizeof(cIO.Step)); 
		
		for(int i = 0;i<AO_ItemNum;i++)
		{
			printf("`cIO.Max[%d] = %d\n",i,cIO.Max[i]);
			printf("`cIO.Min[%d] = %d\n",i,cIO.Min[i]);
			printf("`cIO.Step[%d] = %d\n\n",i,cIO.Step[i]);
		}

	}else
	{//flash中没有数据，现写
		UINT16 tmpMax0[AO_ItemNum] = {180,180,180,180,180,180,180,180};
		UINT16 tmpMin0[AO_ItemNum] = {80,80,80,80,80,80,80,80};
		UINT16 tmpStep0[AO_ItemNum] = {10,10,10,10,10,10,10,10};
		
		IAP_buf_EraseFun(0X03);
		IAP_buf_WriteFun(0x03,(uint8_t *)tmpMax0,sizeof(tmpMax0));
		IAP_buf_FLASH_Lock();
		
		IAP_buf_EraseFun(0X04);
		IAP_buf_WriteFun(0x04,(uint8_t *)tmpMin0,sizeof(tmpMin0));
		IAP_buf_FLASH_Lock();
		
		IAP_buf_EraseFun(0X05);
		IAP_buf_WriteFun(0x05,(uint8_t *)tmpStep0,sizeof(tmpStep0));
		IAP_buf_FLASH_Lock();
		
		writeflag = WRITED;

		IAP_buf_EraseFun(0X02);
		IAP_buf_WriteFun(0x02,&writeflag,1);
		IAP_buf_FLASH_Lock();
		IAP_buf_ReadFun(0X02,&writeflag,1);
		
		IAP_buf_ReadFun(0X03,cIO.Max,sizeof(cIO.Max));
		IAP_buf_ReadFun(0X04,cIO.Min,sizeof(cIO.Min));
		IAP_buf_ReadFun(0X05,cIO.Step,sizeof(cIO.Step));
		
		if(writeflag == WRITED)
		{
			printf("数据写进了flash\n");
			
		}
	}
}
//自动模式执行函数
void autoAction(void)
{
	if(1 == cCW.SIO_AOVal(0))
	{
		if(AutoComplete == -1)
		{
			AutoComplete = 0;
			printf("进入自动模式\n");
		}
							
		if(AutoComplete>=0 && AutoComplete < fun_num)
		{
			if(ActionDown == 0)
			{
				//不需要参数的动作
				if(parts[fun[AutoComplete][0]].para_num == -1 || parts[fun[AutoComplete][0]].AI_num == -1)
				{
					if(NoParaAct == 0)
					{
						printf("---无参动作：%s %d---\n",parts[fun[AutoComplete][0]].name,fun[AutoComplete][1]);
						if(fun[AutoComplete][1] == 1)//判断现在是升高
						{
							GPIO_SetBits(DOS[parts[fun[AutoComplete][0]].add_DO].GPIO, DOS[parts[fun[AutoComplete][0]].add_DO].GPIO_Pin);
							cCW.SIO_DISet(16+parts[fun[AutoComplete][0]].add_DO);//显示继电器动作
						}else
						if(fun[AutoComplete][1] == 2)//判断现在是降低
						{
							GPIO_SetBits(DOS[parts[fun[AutoComplete][0]].minus_DO].GPIO, DOS[parts[fun[AutoComplete][0]].minus_DO].GPIO_Pin);
							cCW.SIO_DISet(16+parts[fun[AutoComplete][0]].minus_DO);//显示继电器动作
						}
						NoParaAct = 1;
					}
				}else
				if(fun[AutoComplete][1] == 1)//判断现在是升高
				{//判断现在的行程是否合适
					if(cIO.Max[parts[fun[AutoComplete][0]].para_num] >= ADC_SumValue[parts[fun[AutoComplete][0]].AI_num]*3300/4096/50)
					{
						if ((fun[AutoComplete][0]==7)&&(fun[AutoComplete][1]==1))//判断如果是立柱操作则前后立柱同时动作
						{
						GPIO_SetBits(DOS[0].GPIO, DOS[0].GPIO_Pin);
						GPIO_SetBits(DOS[14].GPIO, DOS[14].GPIO_Pin);	
						cCW.SIO_DISet(16+parts[fun[AutoComplete][0]].add_DO);//显示继电器动作
						}
						else	
						{
						GPIO_SetBits(DOS[parts[fun[AutoComplete][0]].add_DO].GPIO, DOS[parts[fun[AutoComplete][0]].add_DO].GPIO_Pin);	
						cCW.SIO_DISet(16+parts[fun[AutoComplete][0]].add_DO);//显示继电器动作
						}
					}else
					{
						printf("MAX:%d  MIN:%d  val:%d  \n",cIO.Max[parts[fun[AutoComplete][0]].para_num+fun[AutoComplete][1]-1],cIO.Min[parts[fun[AutoComplete][0]].para_num+fun[AutoComplete][1]-1],ADC_SumValue[parts[fun[AutoComplete][0]].AI_num]*3300/4096/50);
						ActionDown = 1;
					}	
				}
				else if(fun[AutoComplete][1] == 2)//判断现在是降低
				{//判断现在的行程是否合适
					if(cIO.Min[parts[fun[AutoComplete][0]].para_num+1] <= ADC_SumValue[parts[fun[AutoComplete][0]].AI_num]*3300/4096/50)
					{
						if ((fun[AutoComplete][0]==7)&&(fun[AutoComplete][1]==2))
						{
						GPIO_SetBits(DOS[1].GPIO, DOS[1].GPIO_Pin);
						GPIO_SetBits(DOS[15].GPIO, DOS[15].GPIO_Pin);
						cCW.SIO_DISet(16+parts[fun[AutoComplete][0]].add_DO);//显示继电器动作
						}
						else	
						{
						GPIO_SetBits(DOS[parts[fun[AutoComplete][0]].minus_DO].GPIO, DOS[parts[fun[AutoComplete][0]].minus_DO].GPIO_Pin);
						cCW.SIO_DISet(16+parts[fun[AutoComplete][0]].minus_DO);//显示继电器动作
						}
					}else
					{
						printf("MAX:%d  MIN:%d  val:%d  \n",cIO.Max[parts[fun[AutoComplete][0]].para_num+fun[AutoComplete][1]-1],cIO.Min[parts[fun[AutoComplete][0]].para_num+fun[AutoComplete][1]-1],ADC_SumValue[parts[fun[AutoComplete][0]].AI_num]*3300/4096/50);
						ActionDown = 1;
					}	
				}
			}
										
			//达到了指定位置
			if(ActionDown == 1)
			{
				if(fun[AutoComplete][1] == 1)//停止升高
				{
					if ((fun[AutoComplete][0]==7)&&(fun[AutoComplete][1]==1))
						{
						GPIO_ResetBits(DOS[0].GPIO, DOS[0].GPIO_Pin);
						GPIO_ResetBits(DOS[14].GPIO, DOS[14].GPIO_Pin);	
						cCW.SIO_DISet(16+parts[fun[AutoComplete][0]].add_DO);//显示继电器动作
						}
						else	
						{
						GPIO_ResetBits(DOS[parts[fun[AutoComplete][0]].add_DO].GPIO, DOS[parts[fun[AutoComplete][0]].add_DO].GPIO_Pin);	
						cCW.SIO_DISet(16+parts[fun[AutoComplete][0]].add_DO);//显示继电器动作
						}
				}	
				else if(fun[AutoComplete][1] == 2)//停止降低
				{
					if ((fun[AutoComplete][0]==7)&&(fun[AutoComplete][1]==2))
						{
						GPIO_ResetBits(DOS[1].GPIO, DOS[1].GPIO_Pin);
						GPIO_ResetBits(DOS[15].GPIO, DOS[15].GPIO_Pin);
						cCW.SIO_DISet(16+parts[fun[AutoComplete][0]].add_DO);//显示继电器动作
						}
						else	
						{
						GPIO_ResetBits(DOS[parts[fun[AutoComplete][0]].minus_DO].GPIO, DOS[parts[fun[AutoComplete][0]].minus_DO].GPIO_Pin);
						cCW.SIO_DISet(16+parts[fun[AutoComplete][0]].minus_DO);//显示继电器动作
						}
				}
				ActionDown = 0;
				printf("完成动作:%s %d\n",parts[fun[AutoComplete][0]].name,fun[AutoComplete][1]);
				AutoComplete++;
													
				//cCW.SIO_DISet(16+AutoComplete);//显示自动执行到了那一步
				if(AutoComplete == fun_num)
				{
					//cCW.SIO_DISet(31);//自动模式完成标志位置一
					printf("自动模式结束\n");
				}
			}
		}
	}
}
//参数下载执行函数
void paraDownLoad(void)
{
	for(int i = 0;i<4;i++)
	{
		cIO.Max[i*2] = cCW.SIO_AOVal(1+i);
		cIO.Max[i*2+1] = cCW.SIO_AOVal(1+i);
		cIO.Min[i*2] = cCW.SIO_AOVal(1+i+4);
		cIO.Min[i*2+1] = cCW.SIO_AOVal(1+i+4);
		cIO.Step[i*2] = cCW.SIO_AOVal(1+i+8);
		cIO.Step[i*2+1] = cCW.SIO_AOVal(1+i+8);
	}
			
	for(int i = 0;i<AO_ItemNum;i++)
	{
		printf("--cIO.Max[%d] = %d\n",i,cIO.Max[i]);
		printf("--cIO.Min[%d] = %d\n",i,cIO.Min[i]);
		printf("--cIO.Step[%d] = %d\n\n",i,cIO.Step[i]);
	}
	IAP_buf_EraseFun(0X03);
	IAP_buf_WriteFun(0x03,(uint8_t *)cIO.Max,sizeof(cIO.Max)*2);
	IAP_buf_FLASH_Lock();
					
	IAP_buf_EraseFun(0X04);
	IAP_buf_WriteFun(0x04,(uint8_t *)cIO.Min,sizeof(cIO.Min)*2);
	IAP_buf_FLASH_Lock();
					
	IAP_buf_EraseFun(0X05);
	IAP_buf_WriteFun(0x05,(uint8_t *)cIO.Step,sizeof(cIO.Step)*2);
	IAP_buf_FLASH_Lock();
					
	IAP_buf_ReadFun(0X03,(uint8_t *)cIO.Max,sizeof(cIO.Max)*2);
	IAP_buf_ReadFun(0X04,(uint8_t *)cIO.Min,sizeof(cIO.Min)*2);
	IAP_buf_ReadFun(0X05,(uint8_t *)cIO.Step,sizeof(cIO.Step)*2);
	for(int i = 0;i<AO_ItemNum;i++)
	{
		printf("Max[%d] = %d\n",i,cIO.Max[i]);
		printf("Min[%d] = %d\n",i,cIO.Min[i]);
		printf("Step[%d] = %d\n\n",i,cIO.Step[i]);
	}
	printf("参数下载完了\n");
	DownloadFlag = 1;
	cCW.SIO_DISet(15);	
}
//手动模式执行函数
void manalMode(void)
{
	for(int i = 0;i<16;i++)
	{
		if(cCW.SIO_DOVal(i) != 0)
			{
				GPIO_SetBits(DOS[i].GPIO, DOS[i].GPIO_Pin);//DO_x
				cCW.SIO_DISet(16+i);//显示继电器动作
				//printf("动作置一的DO：15+%d",i);
			}else
			{
				GPIO_ResetBits(DOS[i].GPIO, DOS[i].GPIO_Pin);//DO_x
				cCW.SIO_DIRst(16+i);//显示继电器动作
				//printf("动作置零的DO：15+%d",i);
			}
	}
//	for(int i = 0;i<5;i++)
//	{
//		//有动作要求，判断动作是否超过界限
//		if(cIO.Max[i] >= ADC_ConvertedValue[i]*3300/4096 && cIO.Min[i] <= ADC_ConvertedValue[i]*3300/4096 && 0 != cCW.SIO_DOVal(i))
//		{
//			//符合要求可以动作
//			if(DO_StepFlags[i] != 0)
//			{
//				GPIO_SetBits(DOS[i].GPIO, DOS[i].GPIO_Pin);//DO_x
//			}else
//			{//计算下一个状态的理想的AI值
//				DO_StepFlags[i] = (ADC_ConvertedValue[i]*3300/4096)+cIO.Step[i];
//				if(DO_StepFlags[i] > cIO.Max[i])
//				{
//					DO_StepFlags[i] = cIO.Max[i];
//				}
//			}
//		}else
//		{//对应DO为0是清空对应的DO_StepFlags[]
//			GPIO_ResetBits(DOS[i].GPIO, DOS[i].GPIO_Pin);
//			DO_StepFlags[i] = 0;
//		}
//	}
	AutoComplete = -1;//这不是自动模式
}







