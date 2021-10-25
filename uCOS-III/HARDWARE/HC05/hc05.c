#include "sys.h"
#include "usart.h"
#include <stdio.h>
#include "hc05.h"
#include "led.h"
#include "oled.h"

void uart3_init(void) {
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); //使能GPIOA时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); //使能USART1时钟
//串口2对应引脚复用映射
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3); //GPIOA9复用为USART1
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3); //GPIOA10复用为USART1
//USART2端口配置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; //GPIOA9与GPIOA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //速度50MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
    GPIO_Init(GPIOB, &GPIO_InitStructure); //初始化PA9，PA10
//USART2 初始化设置
    USART_InitStructure.USART_BaudRate = 9600;//波特率设置
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //收发模式
    USART_Init(USART3, &USART_InitStructure); //初始化串口1
    USART_Cmd(USART3, ENABLE);  //使能串口1
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启相关中断
    //Usart2 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;//串口1中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;   //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure); //根据指定的参数初始化VIC寄存器、
		
}

//void USART3_IRQHandler(void)                 //串口2中断服务程序
//{
//    u8 com_data;
//	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断
//    {
//			USART_ClearITPendingBit(USART3,USART_IT_RXNE);
//			LED0 = !LED0;		
//			com_data = USART_ReceiveData(USART3);
//		  Openmv_Receive_Data(com_data);//openmv数据处理函数
//    }
//		 
//}

//void Openmv_Receive_Data(int16_t data)//接收Openmv传过来的数据
//{
//	u8 i;
//	if(state==0&&data==0x2C)
//	{
//		state=1;
//		openmv[bit_number++]=data;
//	}
//	else if(state==1&&data==18)
//	{
//		state=2;
//		openmv[bit_number++]=data;
//	}
//	else if(state==2)
//	{
//		openmv[bit_number++]=data;
//		if(bit_number>=17)
//		{
//			state=3;
//		}
//	}
//	else if(state==3)		//检测是否接受到结束标志
//	{
//        	if(data == 0x5B)
//        	{
//            		state = 0;
//            		openmv[bit_number++]=data;
//        	}
//        	else if(data != 0x5B)
//        	{
//           		 state = 0;
//            		for(i=0;i<18;i++)
//            		{
//               			 openmv[i]=0x00;
//            		}           
//        	}
//	}    
//	else
//	{
//		state = 0;
//		bit_number=0;
//            	for(i=0;i<18;i++)
//            	{
//               		 openmv[i]=0x00;
//           	 }
//	}
//}

