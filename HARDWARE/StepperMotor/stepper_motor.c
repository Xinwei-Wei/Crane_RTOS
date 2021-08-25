#include "stepper_motor.h"
#include "stm32f4xx.h"

double bu_to_angle = 1.8/2;
double bottom_target_angle = 0, RL_target_angle = 0, UD_target_angle = 0;
double bottom_curruent_angle = 0, RL_curruent_angle = 0, UD_curruent_angle = 0;
double bottom_angle_count = 0, RL_angle_count = 0, UD_angle_count = 0;
double bottom_stepper_count = 0, RL_stepper_count = 0, UD_stepper_count = 0;
int stepper_flat = 0;
u16 stepper_frequency=100;
int bottom_dir, RL_dir, UD_dir, stepperstop=0;
extern unsigned int set_time;
extern unsigned int reset_time;
double bottom_bu_to_angle = 1.8/2, RL_bu_to_angle = 1.8/32, UD_bu_to_angle = 1.8/1;
extern int bottom_stepper_judge, RL_stepper_judge, UD_stepper_judge, stepper_judge;
int bottom_target_change = 0, RL_target_change = 0, UD_target_change = 0;

OS_ERR  err;

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
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM1_BRK_TIM9_IRQn; //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x02; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
 									  
} 

void Stepper_Dir_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//使能GPIOE时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//使能GPIOD时钟

    //GPIOD13初始化设置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;			//LED0和LED1对应IO口
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		//普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		//上拉
    GPIO_Init(GPIOE, &GPIO_InitStructure);				//初始化GPIOB
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_Init(GPIOD, &GPIO_InitStructure);				//初始化GPIOB

    GPIO_ResetBits(GPIOE, GPIO_Pin_4 | GPIO_Pin_5);					//GPIOA15设置高，灯灭
	GPIO_ResetBits(GPIOD, GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);
}

void Stepper_Init(void)
{
	//TIM9_PWM_Init(arr, psc);
	Stepper_Dir_Init();
}



void soft_TIM_start(u16 frequency)
{
	u16 temp_arr=1000/frequency; 
	set_time = temp_arr/2;
	reset_time = set_time;
}


int bottom_stepper_turn(double angle)//, u16 frequency)
{
	angle *= 10;
	//stepper_frequency = frequency;
	//if(frequency>500 || frequency<10)
	//	return 0;
	bottom_target_angle += angle;
	if(bottom_stepper_judge == 0)
	{
		bottom_angle_count = bottom_target_angle-bottom_curruent_angle;
		if(bottom_angle_count>bottom_bu_to_angle || bottom_angle_count<-bottom_bu_to_angle)
		{
			if(bottom_angle_count<0)
			{
				bottom_dir = 1;
				bottom_angle_count = -bottom_angle_count;
			}
			else 
				bottom_dir = 0;
			PEout(4) = bottom_dir;
			bottom_stepper_count = 0;
			//soft_TIM_start(frequency);
			bottom_stepper_judge = 1;
			return 1;
		}
	}
	bottom_target_change = 1;
	return 0;
}

int RL_stepper_turn(double angle)
{
	RL_target_angle += angle;
	if(RL_stepper_judge == 0)
	{
		RL_angle_count = RL_target_angle - RL_curruent_angle;
		if(RL_angle_count>RL_bu_to_angle || RL_angle_count<-RL_bu_to_angle)
		{
			if(RL_angle_count<0)
			{
				RL_dir = 1;
				RL_angle_count = -RL_angle_count;
			}
			else 
				RL_dir = 0;
			PDout(4) = RL_dir;
			RL_stepper_count = 0;
			RL_stepper_judge = 1;
			return 1;
		}
	}
	RL_target_change = 1;
	return 0;
}

int UD_stepper_turn(double angle)
{
	UD_target_angle += angle;
	if(UD_stepper_judge == 0)
	{
		printf("ok1");
		UD_angle_count = UD_target_angle - UD_curruent_angle;
		if(UD_angle_count>UD_bu_to_angle || UD_angle_count<-UD_bu_to_angle)
		{
			printf("ok2");
			if(UD_angle_count<0)
			{
				UD_dir = 1;
				UD_angle_count = -RL_angle_count;
			}
			else 
				UD_dir = 0;
			PDout(6) = UD_dir;
			UD_stepper_count = 0;
			UD_stepper_judge = 1;
			return 1;
		}
	}
	UD_target_change = 1;
	return 0;
}

//定时器9中断服务函数
void TIM1_BRK_TIM9_IRQHandler(void)
{
//	CPU_SR_ALLOC();
//	CPU_CRITICAL_ENTER();
//	OSIntEnter();
//	CPU_CRITICAL_EXIT(); 
//	if(TIM_GetITStatus(TIM9,TIM_IT_Update) == SET) //溢出中断
//	{
//		stepper_count+=bu_to_angle;
//		if(stepper_count > angle_count || stepperstop==1)
//		{
//			TIM_SetCompare1(TIM9,0);	//修改比较值，修改占空比			
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
//	TIM_ClearITPendingBit(TIM9,TIM_IT_Update);  //清除中断标志位
//	OSIntExit();
//	TIM_Cmd(TIM9, DISABLE);
}

void stepper_stop(void)
{
	stepperstop=1;
}

void soft_IRQ(void)
{
	if(bottom_stepper_judge)
	{
		bottom_stepper_count += bottom_bu_to_angle;
		if(bottom_stepper_count > bottom_angle_count || bottom_target_change)
		{
//			set_time = 0;
//			reset_time *= 2;
			if(bottom_dir == 0)
				bottom_curruent_angle += bottom_stepper_count;
			else
				bottom_curruent_angle -= bottom_stepper_count;
			bottom_stepper_judge = 0;
			bottom_stepper_count = 0;
			bottom_stepper_turn(0);
		}
	}
	if(RL_stepper_judge)
	{
		RL_stepper_count += RL_bu_to_angle;
		if(RL_stepper_count > RL_angle_count || RL_target_change)
		{
//			set_time = 0;
//			reset_time *= 2;
			if(RL_dir == 0)
				RL_curruent_angle += RL_stepper_count;
			else
				RL_curruent_angle -= RL_stepper_count;
			RL_stepper_judge = 0;
			RL_stepper_count = 0;
			RL_stepper_turn(0);
		}
	}
	if(UD_stepper_judge)
	{
		printf("ok?");
		UD_stepper_count += UD_bu_to_angle;
		if(UD_stepper_count > UD_angle_count || UD_target_change)
		{
//			set_time = 0;
//			reset_time *= 2;
			if(UD_dir == 0)
				UD_curruent_angle += UD_stepper_count;
			else
				UD_curruent_angle -= UD_stepper_count;
			UD_stepper_judge = 0;
			UD_stepper_count = 0;
			UD_stepper_turn(0);
		}
	}
//	if(stepperstop==1)
//	{
//		set_time = 0;
//		reset_time *= 2;
//		res = 0;
//		Curruent_angle = 0;
//		Target_angle = Curruent_angle;
//		stepper_count = 0;
//		stepper_flat = 0;
//		stepper_turn(0, stepper_frequency);
//		stepperstop = 0;
//	}
}
