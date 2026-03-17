#ifndef MAPPING_H_
#define MAPPING_H_
#include "stm32f10x.h"
//DO和GPIO的对应关系
typedef struct Mapping
{
	GPIO_TypeDef* GPIO;
	uint16_t GPIO_Pin;
}MAPPING;
//8个主要的控制部分
typedef struct Contral
{
	char *name;					//控制的功能名称
	uint8_t add_DO;			//负责正向动作的DO号
	uint8_t minus_DO;		//负责反向动作的DO号
	int8_t para_num;		//在参数数组中对应的位置
	int8_t AI_num;			//要读的AI的位置
}CONPART;



#endif
