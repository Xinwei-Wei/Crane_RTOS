#include "sys.h"
#include "usart.h"
#include <stdio.h>
#include "hc05.h"
#include "led.h"
#include "oled.h"

void uart3_init(void) {
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); //ʹ��GPIOAʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); //ʹ��USART1ʱ��
//����2��Ӧ���Ÿ���ӳ��
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3); //GPIOA9����ΪUSART1
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3); //GPIOA10����ΪUSART1
//USART2�˿�����
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; //GPIOA9��GPIOA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //�ٶ�50MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
    GPIO_Init(GPIOB, &GPIO_InitStructure); //��ʼ��PA9��PA10
//USART2 ��ʼ������
    USART_InitStructure.USART_BaudRate = 9600;//����������
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //�շ�ģʽ
    USART_Init(USART3, &USART_InitStructure); //��ʼ������1
    USART_Cmd(USART3, ENABLE);  //ʹ�ܴ���1
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//��������ж�
    //Usart2 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;//����1�ж�ͨ��
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //��ռ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //�����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;   //IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure); //����ָ���Ĳ�����ʼ��VIC�Ĵ�����
		
}

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
//		 
//}

//void Openmv_Receive_Data(int16_t data)//����Openmv������������
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

