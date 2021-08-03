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
//���������
//*********************************
//int main(void)
//{ 
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
//	delay_init(168);    //��ʼ����ʱ����
//	uart_init(115200);	//��ʼ�����ڲ�����Ϊ115200
//	IIC_Init();
//	//OLED_Init();		//��ʼ��OLED  
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
//CCDģ��
//********************/
/*
extern u8 tsl1401_finish_flag;
extern u16 ccd_data[128]; 
int main(void)
{

	u16 adcx;
	u8 line;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);	  //��ʱ������ʼ��	  
	uart_init(115200);//���ڳ�ʼ��Ϊ115200
	ccd_init();
    while(1)
    {
        if(tsl1401_finish_flag)
        {
            //����ͼ����λ��
            ccd_send_data(USART1, ccd_data);
            //�ɼ���ɱ�־λ���
            tsl1401_finish_flag = 0;
        }
    }
}

*/

/////********************
////CCDģ��
////********************/
//int main(void)
//{	
//	delay_init(168);	  //��ʱ������ʼ��	  
//	uart_init(115200);//���ڳ�ʼ��Ϊ115200
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
//����ģ��
//********************/
//int main(void)
//{	
//	delay_init(168);	  //��ʱ������ʼ��	  
//	uart_init(115200);//���ڳ�ʼ��Ϊ115200
//	while(1)
//	{		
//		printf("test for bluetooth\r\n");
//		delay_ms(200);
//		
//	}
//}
//void USART1_IRQHandler(void)                	//����1�жϷ������
//{
//	uint16_t Res;

//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
//	{
//		
//		Res =USART_ReceiveData(USART1);//(USART1->DR);	//��ȡ���յ�������
//		USART_SendData(USART1,Res);   		 
//  } 

//} 

///********************
//��openmvͨ��
//********************/
//void Openmv_Receive_Data(u8 data)//����Openmv������������
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
//	else if(state==3)		//����Ƿ���ܵ�������־
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

//void USART3_IRQHandler(void)                 //����2�жϷ������
//{
//    u8 com_data;
//	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //�����ж�
//    {
//			USART_ClearITPendingBit(USART3,USART_IT_RXNE);
//			LED0 = !LED0;		
//			com_data = USART_ReceiveData(USART3);
//		  Openmv_Receive_Data(com_data);//openmv���ݴ�����
//    }	 
//}

/*************************
�����˵����Դ���
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
