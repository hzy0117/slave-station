#include "mapping.h"
#include "stm32f10x_gpio.h"
//这是每个数字量输出和ARM管脚的映射关系
MAPPING DOS[] = 
{
	{GPIOD, GPIO_Pin_0},	//DO0 LED1
	{GPIOD, GPIO_Pin_1},	//DO1 LED2
	{GPIOD, GPIO_Pin_2},	//DO2 LED3
	{GPIOD, GPIO_Pin_3},	//DO3 LED4
	{GPIOD, GPIO_Pin_7},	//DO4 LED5
	{GPIOD, GPIO_Pin_6},	//DO5 LED6
	{GPIOD, GPIO_Pin_5},	//DO6 LED7
	{GPIOD, GPIO_Pin_4},	//DO7 LED8
	{GPIOE, GPIO_Pin_8},	//DO8 LED9
	{GPIOE, GPIO_Pin_9},	//DO9 LED10
	{GPIOE, GPIO_Pin_10},	//DO10 LED11
	{GPIOE, GPIO_Pin_11},	//DO11 LED12
	{GPIOE, GPIO_Pin_15},	//DO12 LED13
	{GPIOE, GPIO_Pin_14},	//DO13 LED14
	{GPIOE, GPIO_Pin_13},	//DO14 LED15
	{GPIOE, GPIO_Pin_12},	//DO15 LED16
};
//8个控制部分
CONPART parts[] = 
{
	{"底调升降",14,15,0,0},
	{"抬底伸缩",12,13,-1,-1},//矿大用于前梁
	{"二级护帮伸缩",10,11,-1,-1},//{"二级护帮伸缩",10,11,4,2}
	//{"一级护帮伸缩",8,9,6,1},//{"一级护帮伸缩",8,9,6,1}
	{"一级护帮伸缩",8,9,2,1},//{"一级护帮伸缩",8,9,6,1}
	{"侧护伸缩",4,5,-1,-1},
	{"平衡伸缩",4,5,6,3},
	{"推溜拉架",3,2,4,2},//推溜拉架顺序更换
	{"立柱升降",0,1,0,0},
};
//自动模式功能执行顺序
//[控制部分编号][1升2减]
uint8_t fun[][2] = 
{
	{3,2},//收互帮
    {4,2},//收侧护
	{7,2},//降柱
	{6,1},//
	{7,1},//升柱
	{4,1},//伸侧护
	{3,1},//伸互帮
 };










