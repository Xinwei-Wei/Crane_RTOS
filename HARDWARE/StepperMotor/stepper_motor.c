#include "stepper_motor.h"
#include "stm32f4xx.h"

u8 ps=1, bia;
double Target_angle = 0;
double Curruent_angle = 0;
double angle_count = 0;
double stepper_count = 0;
double bu_to_angle = 1.8/16;
int stepper_flat = 0;
u16 stepper_frequency=100;
int dir, stepperstop=0;
extern unsigned int set_time;
extern unsigned int reset_time;

OS_ERR  err;

void TIM9_PWM_Init(u16 arr,u16 psc)
{		 					 
	//�˲������ֶ��޸�IO������
	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);  	//TIM9ʱ��ʹ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE); 	//ʹ��PORTBʱ��	
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource5,GPIO_AF_TIM9); //GPIOB14����Ϊ��ʱ��12
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;			//GPIOE5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //����
	GPIO_Init(GPIOE,&GPIO_InitStructure);				//��ʼ��PB14
	  
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period=arr;   //�Զ���װ��ֵ
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM9,&TIM_TimeBaseStructure);//��ʼ����ʱ��9
	
	
	//��ʼ��TIM9 Channel1 PWMģʽ
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
	TIM_OC1Init(TIM9, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM12 OC1
	
    TIM_OC1PreloadConfig(TIM9, TIM_OCPreload_Enable);  //ʹ��TIM4��CCR1�ϵ�Ԥװ�ؼĴ���

    TIM_ARRPreloadConfig(TIM9,ENABLE);//ARPEʹ��
	
	TIM_Cmd(TIM9, DISABLE);  //ʹ��TIM4
	TIM_ClearITPendingBit(TIM9, TIM_IT_Update);
	TIM_ITConfig(TIM9,TIM_IT_Update,ENABLE); //����ʱ��4�����ж�
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM1_BRK_TIM9_IRQn; //��ʱ��3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x02; //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02; //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
 									  
} 

void Stepper_Dir_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//ʹ��GPIOEʱ��

    //GPIOD13��ʼ������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;			//LED0��LED1��ӦIO��
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		//��ͨ���ģʽ
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		//����
    GPIO_Init(GPIOE, &GPIO_InitStructure);				//��ʼ��GPIOB

    GPIO_ResetBits(GPIOE, GPIO_Pin_4 | GPIO_Pin_5);					//GPIOA15���øߣ�����
}

void Stepper_Init(void)
{
	//TIM9_PWM_Init(arr, psc);
	Stepper_Dir_Init();
}



void soft_TIM_start(u16 frequency)
{
	u16 temp_arr=10000/frequency-1; 
	set_time = temp_arr/2;
	reset_time = set_time;
}


int stepper_turn(double angle, u16 frequency)
{
	printf("turn ok1\r\n");
	angle *= 20;
	stepper_frequency = frequency;
	if(frequency>500 || frequency<10)
		return 0;
	Target_angle += angle;
	if(stepper_flat == 0)
	{
		angle_count = Target_angle-Curruent_angle;
		printf("nmsl");
		if(angle_count>bu_to_angle || angle_count<-bu_to_angle)
		{
			if(angle_count<0)
			{
				dir = 1;
				angle_count=-angle_count;
			}
			else 
				dir = 0;
			PEout(4) = dir;
			stepper_count = 0;
			printf("turn ok2\r\n");
			soft_TIM_start(frequency);
			stepper_flat = 1;
			return 1;
		}
	}
	return 0;
}

//��ʱ��9�жϷ�����
void TIM1_BRK_TIM9_IRQHandler(void)
{
//	CPU_SR_ALLOC();
//	CPU_CRITICAL_ENTER();
//	OSIntEnter();
//	CPU_CRITICAL_EXIT(); 
//	if(TIM_GetITStatus(TIM9,TIM_IT_Update) == SET) //����ж�
//	{
//		stepper_count+=bu_to_angle;
//		if(stepper_count > angle_count || stepperstop==1)
//		{
//			TIM_SetCompare1(TIM9,0);	//�޸ıȽ�ֵ���޸�ռ�ձ�			
//			if(dir == 0)
//				Curruent_angle+=stepper_count;
//			else
//				Curruent_angle-=stepper_count;
//			TIM_Cmd(TIM9, DISABLE);
//			stepper_count = 0;
//			stepper_flat = 0;
//			stepper_turn(0, stepper_frequency, tcb);
//			stepperstop = 0;
//		}
//	}
//	TIM_ClearITPendingBit(TIM9,TIM_IT_Update);  //����жϱ�־λ
//	OSIntExit();
//	TIM_Cmd(TIM9, DISABLE);
}

void stepper_stop(void)
{
	stepperstop=1;
}

int soft_IRQ(void)
{
	int res = 1;
	stepper_count+=bu_to_angle;
	if(stepper_count > angle_count)
	{
		set_time = 0;
		reset_time *= 2;
		if(dir == 0)
			Curruent_angle+=stepper_count;
		else
			Curruent_angle-=stepper_count;
		//�ر��߳�
		res = 0;
		stepper_count = 0;
		stepper_flat = 0;
		stepper_turn(0, stepper_frequency);
	}
	if(stepperstop==1)
	{
		set_time = 0;
		reset_time *= 2;
		//�ر��߳�
		res = 0;
		if(dir == 0)
			Curruent_angle+=stepper_count;
		else
			Curruent_angle-=stepper_count;
		Target_angle = Curruent_angle;
		stepper_count = 0;
		stepper_flat = 0;
		stepper_turn(0, stepper_frequency);
		stepperstop = 0;
	}
	return res;
}
