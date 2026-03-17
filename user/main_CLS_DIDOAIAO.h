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
#include "YXF105GY_CLS_CANWeb2.H"
//////////////////above is 共用///////////////////////////////////

#define Def_IOMTypeV 162 //IO类型编号0~255 //用户可修改 Web 页面中 HTp

//我改的部分-------begin

#define AO_ItemNum	8
//我改的部分-------end
class C_DIDOAIAO////在一个程序中仅能调用一次//
{
public:
////////初始化////////////////
		u8 Initial();// 			
///////////正常使用/////////////////		
  	u8 Scan100uS();//100us=0.1ms, 0.1ms扫描一次
  	u8 Scanfor();  //在main() for(;;)中调用	
    u8 Scan1S();//1秒扫描一次
		UINT8 Initial127OK;//放在 Initial()的最后面 别的程序 最前面

//Begin of 用户程序部分////用户可在 这个 空间里添加 变量
		UINT16 i;
    UINT16 DIFlt;	
    C_Timer ztimledtryok;
		
//我改的部分-------begin
		UINT16 Max[AO_ItemNum];		//阈值的最大值
		UINT16 Min[AO_ItemNum];		//阈值的最小值
		INT16 Step[AO_ItemNum];		//一次活动的步长,可正可负
//我改的部分-------end		

//End of 用户程序部分////			
};
void data_jiexi(void);	
		

