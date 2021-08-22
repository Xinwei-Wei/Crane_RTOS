#include "stepper_motor.h"
#include "stm32f4xx.h"
#include "includes.h"
u8 ps=1, bia;
double Target_angle = 0;
double Curruent_angle = 0;
double angle_count = 0;
double stepper_count = 0;
double bu_to_angle = 1.8/16;
int stepper_flat = 1;
u16 stepper_frequency=100;
int dir, stepperstop;

void TIM9_PWM_Init(u16 arr,u16 psc)
{		 					 
	//此部分需手动修改IO口设置
	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);  	//TIM9时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE); 	//使能PORTB时钟	
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource5,GPIO_AF_TIM9); //GPIOB14复用为定时器12
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;			//GPIOE5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
	GPIO_Init(GPIOE,&GPIO_InitStructure);				//初始化PB14
	  
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseStructure.TIM_Period=arr;   //自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM9,&TIM_TimeBaseStructure);//初始化定时器9
	
	
	//初始化TIM9 Channel1 PWM模式
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OC1Init(TIM9, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM12 OC1
	
    TIM_OC1PreloadConfig(TIM9, TIM_OCPreload_Enable);  //使能TIM4在CCR1上的预装载寄存器

    TIM_ARRPreloadConfig(TIM9,ENABLE);//ARPE使能
	
	TIM_Cmd(TIM9, DISABLE);  //使能TIM4
	TIM_ClearITPendingBit(TIM9, TIM_IT_Update);
	TIM_ITConfig(TIM9,TIM_IT_Update,ENABLE); //允许定时器4更新中断
	
//	NVIC_InitStructure.NVIC_IRQChannel=TIM1_BRK_TIM9_IRQn; //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x02; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
 									  
} 

void Stepper_Dir_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//使能GPIOE时钟

    //GPIOD13初始化设置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;			//LED0和LED1对应IO口
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		//普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		//上拉
    GPIO_Init(GPIOE, &GPIO_InitStructure);				//初始化GPIOB

    GPIO_ResetBits(GPIOE, GPIO_Pin_4);					//GPIOA15设置高，灯灭
}

void Stepper_Init(u16 arr, u16 psc)
{
	TIM9_PWM_Init(arr, psc);
	Stepper_Dir_Init();
}



void TIM9_start(u16 frequency)
{
	u16 temp_arr=10000/frequency-1; 
	TIM_SetAutoreload(TIM9,temp_arr);//设定自动重装值	
	TIM_SetCompare1(TIM9,temp_arr>>1); //匹配值2等于重装值一半，是以占空比为50%	
	TIM_SetCounter(TIM9,0);//计数器清零
	TIM_Cmd(TIM9, ENABLE);  //使能TIM12
}


void stepper_turn(double angle, u16 frequency)
{
	angle *= 20;
	stepper_frequency = frequency;
	if(frequency>10000 || frequency<10)return;
	Target_angle += angle;
	if(stepper_flat == 0)
	{
		angle_count = Target_angle-Curruent_angle;
		if(angle_count>bu_to_angle || angle_count<-bu_to_angle)
		{
			if(angle_count<0)
			{
				dir = 1;
				angle_count=-angle_count;
			}
			else dir = 0;
			PEout(4) = dir;
			stepper_count = 0;
			TIM9_start(frequency);
			stepper_flat = 1;
		}
	}
}

//定时器9中断服务函数
void TIM1_BRK_TIM9_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM9,TIM_IT_Update) == SET) //溢出中断
	{
		stepper_count+=bu_to_angle;
		if(stepper_count > angle_count || stepperstop==1)
		{
			TIM_SetCompare1(TIM9,0);	//修改比较值，修改占空比			
			if(dir == 0)
				Curruent_angle+=stepper_count;
			else
				Curruent_angle-=stepper_count;
			TIM_Cmd(TIM9, DISABLE);
			stepper_count = 0;
			stepper_flat = 0;
			stepper_turn(0, stepper_frequency);
			stepperstop = 0;
		}
	}
	TIM_ClearITPendingBit(TIM9,TIM_IT_Update);  //清除中断标志位
}

void stepper_stop()
{
	stepperstop=1;
}
