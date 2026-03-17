//#include "jiexi.h"

//extern uint8_t rec_datas[3];
//uint8_t ij;
////uint8_t zushu;
////uint8_t dongzuo;
//extern uint8_t rec_flag;//接收标志
//extern C_CANWeb cCW;

//void benzhan(uint8_t dongzuo)
//{
//	switch(dongzuo)
//	{
//		case 0x00:
//			GPIO_ResetBits(GPIOD, GPIO_Pin_All);
//			GPIO_ResetBits(GPIOE, GPIO_Pin_All);
//		break;
//		case 0x01:
//			GPIO_SetBits(GPIOD,GPIO_Pin_0);
//		break;
//		case 0x02:
//			GPIO_SetBits(GPIOD,GPIO_Pin_1);
//		break;
//		case 0x03:
//			GPIO_SetBits(GPIOD,GPIO_Pin_2);
//		break;
//		case 0x04:
//			GPIO_SetBits(GPIOD,GPIO_Pin_3);
//		break;
//		case 0x05:
//			GPIO_SetBits(GPIOD,GPIO_Pin_4);
//		break;
//		case 0x06:
//			GPIO_SetBits(GPIOD,GPIO_Pin_5);
//		break;
//		case 0x07:
//			GPIO_SetBits(GPIOD,GPIO_Pin_6);
//		break;
//		case 0x08:
//			GPIO_SetBits(GPIOD,GPIO_Pin_7);
//		break;
//		case 0x09:
//			GPIO_SetBits(GPIOE,GPIO_Pin_8);
//		break;
//		case 0x0a:
//			GPIO_SetBits(GPIOE,GPIO_Pin_9);
//		break;
//		case 0x0b:
//			GPIO_SetBits(GPIOE,GPIO_Pin_10);
//		break;
//		case 0x0c:
//			GPIO_SetBits(GPIOE,GPIO_Pin_11);
//		break;
//		case 0x0d:
//			GPIO_SetBits(GPIOE,GPIO_Pin_12);
//		break;
//		case 0x0e:
//			GPIO_SetBits(GPIOE,GPIO_Pin_13);
//		break;
//		case 0x0f:
//			GPIO_SetBits(GPIOE,GPIO_Pin_14);
//		break;
//		case 0x10:
//			GPIO_SetBits(GPIOE,GPIO_Pin_15);
//		break;
//	}
//}

//void linjia(uint8_t mode,uint8_t dongzuo)
//{
//	uint8_t mode_send=mode;
//	uint8_t zushu=0x00;
//	uint8_t dongzuo_send=dongzuo;
//	uint8_t flag_send=0x01;
//	cCW.SIO_AISet(4,mode);
////cCW.SIO_AISet(5,ADC_ConvertedValue[5]);
//	cCW.SIO_AISet(5,zushu);
//	//cCW.SIO_AISet(6,ADC_ConvertedValue[6]);
//	cCW.SIO_AISet(6,dongzuo_send);
//	//cCW.SIO_AISet(7,ADC_ConvertedValue[7]);
//	cCW.SIO_AISet(7,flag_send);
//	
//	
//	
//}

//void chengzhu(uint8_t mode,uint8_t zushu,uint8_t dongzuo)
//{
//	uint8_t mode_send=mode;
//	uint8_t zushu_send=zushu;
//	uint8_t dongzuo_send=dongzuo;
//	uint8_t flag_send=0x01;
//}

//void data_jiexi()
//{
//	if(rec_datas[0]==0x00)
//	{
//		benzhan(rec_datas[2]);
//	}
//	else if(rec_datas[0]==0x01||rec_datas[0]==0x02)
//	{
//		linjia(rec_datas[0],rec_datas[2]);
//	}
//	else if(rec_datas[0]==0x03||rec_datas[0]==0x04)
//	{
//		chengzhu(rec_datas[0],rec_datas[1],rec_datas[2]);
//	}

//}
