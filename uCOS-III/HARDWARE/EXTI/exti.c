#include "key.h"
#include "exti.h"
#include "led.h"
#include "beep.h"
#include "oled.h"

void exti_Init()
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	KEY_Init();
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource3);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line3;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn; //ʹ�ܰ����ⲿ�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; //��ռ���ȼ� 2��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02; //��Ӧ���ȼ� 2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource4);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn; //ʹ�ܰ����ⲿ�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; //��ռ���ȼ� 2��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02; //��Ӧ���ȼ� 2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);
	
}

void EXTI3_IRQHandler(void)  //KEY1
{

}

void EXTI4_IRQHandler(void)
{

}

