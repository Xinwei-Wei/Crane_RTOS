#include "stm32f4xx.h"
#include "stdio.h"
#include "delay.h"
#include "usart.h"
#include "adc.h"
#include "freecars.h"
#include "pwm.h"
#include "oled.h"
#include "led.h"
#include "key.h"
#include "exti.h"
#include "beep.h"
#include "menu.h"
#include "math.h"
#include "hc05.h"
#include "hc-sr04.h"
#include "ccd.h"
#include "tf-luna.h"


///**********************************************************************
//********************   WARNING! ONLY FOR TEST    **********************
//******************** USE app.c IN uC/OS-III APP  **********************
//**********************************************************************/


//*********************************
//串口屏输出
//*********************************
//int main(void)
//{ 
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
//	delay_init(168);    //初始化延时函数
//	uart_init(115200);	//初始化串口波特率为115200
//	IIC_Init();
//	//OLED_Init();		//初始化OLED  
//	delay_ms(1000);
//	printf("\x00\xff\xff\xff"); 
//	while(1)
//	{
//		IICGetAngle();
//		
//		delay_ms(5);
//		USART_ShowData();
//	} 	    
//}

///********************
//CCD模块
//********************/
/*
extern u8 tsl1401_finish_flag;
extern u16 ccd_data[128]; 
int main(void)
{

	u16 adcx;
	u8 line;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);	  //延时函数初始化	  
	uart_init(115200);//串口初始化为115200
	ccd_init();
    while(1)
    {
        if(tsl1401_finish_flag)
        {
            //发送图像到上位机
            ccd_send_data(USART1, ccd_data);
            //采集完成标志位清除
            tsl1401_finish_flag = 0;
        }
    }
}

*/

/////********************
////CCD模块
////********************/
//int main(void)
//{	
//	delay_init(168);	  //延时函数初始化	  
//	uart_init(115200);//串口初始化为115200
//	tsl1401_init();
//	int i=0;
//	u16 a;
//	while(1)
//	{		
//		//printf("test for bluetooth\r\n");
//		tsl1401_gather();
//		CCD_BUFF[128]=0xff;
//		for(i=0;i<129;i++)
//		{
//			//printf("CCD_BUFF[%d]:%1.3f\tdone\r\n",i,CCD_BUFF[i]*3.3f/4096);
//			//USART_SendData(USART1,CCD_BUFF[i]);
//			uart_putchar(USART1,CCD_BUFF[i]);
////			uart_putchar(USART1,0x44);
//		}
//			uart_putchar(USART1,0xff);
////		USART_SendData(USART1,a);
//		
////		printf("%1.3f\tdone\r\n",a*3.3f/4096);
//		delay_ms(100);
//		
//	}
//}

///********************
//蓝牙模块
//********************/
//int main(void)
//{	
//	delay_init(168);	  //延时函数初始化	  
//	uart_init(115200);//串口初始化为115200
//	while(1)
//	{		
//		printf("test for bluetooth\r\n");
//		delay_ms(200);
//		
//	}
//}
//void USART1_IRQHandler(void)                	//串口1中断服务程序
//{
//	uint16_t Res;

//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
//	{
//		
//		Res =USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据
//		USART_SendData(USART1,Res);   		 
//  } 

//} 

///********************
//与openmv通信
//********************/
//void Openmv_Receive_Data(u8 data)//接收Openmv传过来的数据
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
//               			 //openmv[i]=0x00;
//            		}           
//        	}
//	}    
//	else
//	{
//		state = 0;
//		bit_number=0;
//            	for(i=0;i<18;i++)
//            	{
//               		//openmv[i]=0x00;
//           	 }
//	}
//}

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
//}

/*************************
二级菜单测试代码
*************************/
//int main(void)
//{
//  static char select = 0;
//	static char select_task = 5;
//	char flag_choose = 0;
//	char flag_back = 0;
//	delay_init(84);
//	KEY_Init();
//	OLED_Init();
//    while(1)
//    {
//			if (!flag_choose ){
//				flag_choose=0;
//				select = KEY_Scan(0,select,&flag_choose)% 6;
//				ShowMenu();
//			  ShowSelect(select);
//			}
//			else if (select !=5 && !flag_back ){
//				KEY_Scan(0,select,&flag_back);
//				ShowTask(select);
//			  ShowSelect(select_task);
//			}
//			else if (flag_back){
//				flag_back=0;
//				flag_choose=0;
//				//select=0;
//				ShowMenu();
//			  ShowSelect(select);
//			}
//			OLED_Refresh_Gram();
//			delay_ms(50);
//    }
//}
