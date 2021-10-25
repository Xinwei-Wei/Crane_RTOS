#include "tf-luna.h"
#include "stm32f4xx.h"
#include "softiic.h"

u16 distance1;
//void tf_io_init(void)
//{
//	GPIO_InitTypeDef  GPIO_InitStructure;
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//使能GPIOA,GPIOE时钟

//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; //PE5 对应引脚
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN ;//下拉
//	GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化GPIOE4

//}

//u8 Get_Dis(void)
//{
//	if(PE==1)
//	{
//		return IIC_Read_Byte(1);
//	}
//}


void USART2_IRQHandler(void)                	//串口2中断服务程序
{
	u8 flag=0;
	static u8 len;
	static u8 i=0;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		if(USART_ReceiveData(USART2)==0x5A)
		{
			flag=1;	
			i++;
		}
		if(flag)
		{
			flag=0;
			len=USART_ReceiveData(USART2);
			i++;
		}
		if(i==len-1)
		{
			i=0;
			len=0;
			distance1=USART_ReceiveData(USART2);
		}
		else
			i++;
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	} 
} 




