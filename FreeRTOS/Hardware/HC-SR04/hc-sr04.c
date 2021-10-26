#include "hc-sr04.h"
#include "usart.h"
#include "includes.h"

#define	delay_ms(x) vTaskDelay(x)

static void delay_us(u32 nus)
{
	nus *= 14;	// ��Ƶ168MHZ
	for(; nus--; );
}

int overcount;
//��ʱ��5��ʼ��
void TIM5_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);  ///ʹ��TIM5ʱ��
	RCC_APB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);  ///
	
    TIM_TimeBaseInitStructure.TIM_Period = 999; 	//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=83;  //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseInitStructure);//��ʼ��TIM5
	
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE); //����ʱ��5�����ж�
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM5_IRQn; //��ʱ��5�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02; //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);	


	TIM_Cmd(TIM5, DISABLE);
}



void CH_SR04_IO(void)
{    	 
	GPIO_InitTypeDef  GPIO_InitStructure;	
	GPIO_InitTypeDef  GPIO_InitStructure1;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//ʹ��GPIOFʱ��

  //GPIOC4��ʼ������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;//PC4��ӦIO��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIO
	
	/*ECOH�����ź�*///PC1
	
	GPIO_InitStructure1.GPIO_Pin = GPIO_Pin_1; // ��Ӧ����
	GPIO_InitStructure1.GPIO_Mode = GPIO_Mode_IN;//��ͨ����ģʽ
	GPIO_InitStructure1.GPIO_Speed = GPIO_Speed_100MHz;//100M
	GPIO_InitStructure1.GPIO_PuPd = GPIO_PuPd_NOPULL  ;//����
	GPIO_Init(GPIOC, &GPIO_InitStructure1);//��ʼ��PC1

}


void CH_SR04_int(void)
{
	TIM5_Init();
	CH_SR04_IO();
}


float Senor_Using(void)
{
	float length=0,sum=0;
	u16 tim;
	u8 i=0;
	/*��5�����ݼ���һ��ƽ��ֵ*/
	while(i!=5)
	{
		PCout(4)=1;  //�����źţ���Ϊ�����ź�
		delay_us(20);  //�ߵ�ƽ�źų���10us
		PCout(4)=0;
		/*�ȴ������ź�*/	
		while(PCin(1)==0);
		TIM_Cmd(TIM5,ENABLE);//�����źŵ�����������ʱ������
		i+=1;  //ÿ�յ�һ�λ����ź�+1���յ�5�ξͼ����ֵ
		while(PCin(1)==1);//�����ź���ʧ
		TIM_Cmd(TIM5,DISABLE);//�رն�ʱ��
		tim=TIM_GetCounter(TIM5);//��ȡ��TIM2���Ĵ����еļ���ֵ��һ�߼�������ź�ʱ��

		length=(tim+overcount*1000)*340/1000/4;//ͨ�������źż������
		sum=length+sum;
		TIM_SetCounter(TIM5, 0);//����������  //��TIM5�����Ĵ����ļ���ֵ����
		overcount=0;  //�ж������������
	}
	delay_ms(100);
	length=sum/5;
	return length;//������Ϊ��������ֵ
}



//�ϰ������
//���ϰ��0
//���ϰ��1
u8 block(u16 distance)
{
	u8 flag=0;
	float length=0;
	u16 tim;
	PCout(4)=1;  //�����źţ���Ϊ�����ź�
	delay_us(20);  //�ߵ�ƽ�źų���10us
	PCout(4)=0;
	/*�ȴ������ź�*/	
	while(PCin(1)==0);
	TIM_Cmd(TIM5,ENABLE);//�����źŵ�����������ʱ������
	while(PCin(1)==1);//�����ź���ʧ
	TIM_Cmd(TIM5,DISABLE);//�رն�ʱ��

	tim=TIM_GetCounter(TIM5);//��ȡ��TIM2���Ĵ����еļ���ֵ��һ�߼�������ź�ʱ��
	length=(tim+overcount*1000)*340/1000/4;//ͨ�������źż������
	if(length>distance)
	{
		flag=1;
	}
	TIM_SetCounter(TIM5, 0);//����������  //��TIM5�����Ĵ����ļ���ֵ����
	overcount=0;  //�ж������������
	return flag;//������Ϊ��������ֵ
}
	

void TIM5_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM5,TIM_IT_Update)==SET) //����ж�
	{
		overcount++;
	}
	TIM_ClearITPendingBit(TIM5,TIM_IT_Update);  //����жϱ�־λ
}

