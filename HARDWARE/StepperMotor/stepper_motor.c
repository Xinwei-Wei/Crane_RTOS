#include "stepper_motor.h"
#include "stm32f4xx.h"
u32 stepper_count = 0;
u8 ps=1, bia;

void TIM12_PWM_Init(u16 arr,u16 psc)
{		 					 
	//此部分需手动修改IO口设置
	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE);  	//TIM12时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 	//使能PORTB时钟
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource14,GPIO_AF_TIM12); //GPIOB14复用为定时器12
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;			//GPIOB14
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
	GPIO_Init(GPIOB,&GPIO_InitStructure);				//初始化PB14
	  
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseStructure.TIM_Period=arr;   //自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM12,&TIM_TimeBaseStructure);//初始化定时器5
	
	
	//初始化TIM12 Channel1 PWM模式
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OC1Init(TIM12, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM12 OC1
	
    TIM_OC4PreloadConfig(TIM12, TIM_OCPreload_Enable);  //使能TIM4在CCR1上的预装载寄存器

    TIM_ARRPreloadConfig(TIM12,ENABLE);//ARPE使能
	
	TIM_Cmd(TIM12, ENABLE);  //使能TIM4
	TIM_ITConfig(TIM12,TIM_IT_Update,ENABLE); //允许定时器4更新中断
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM8_BRK_TIM12_IRQn; //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
 									  
} 

void Stepper_Dir_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOB时钟

    //GPIOD13初始化设置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;			//LED0和LED1对应IO口
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		//普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		//上拉
    GPIO_Init(GPIOB, &GPIO_InitStructure);				//初始化GPIOA

    GPIO_ResetBits(GPIOB, GPIO_Pin_15);					//GPIOA15设置高，灯灭
}

void Stepper_Init(u16 arr, u16 psc)
{
	TIM12_PWM_Init(arr, psc);
	Stepper_Dir_Init();
}

//定时器12中断服务函数
void TIM12_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM12,TIM_IT_Update) == SET) //溢出中断
	{
		stepper_count++;
		if(stepper_count > 100*ps)
		{
			stepper_count = 0;
			TIM_SetCompare1(TIM12,0);	//修改比较值，修改占空比
		}
	}
	TIM_ClearITPendingBit(TIM12,TIM_IT_Update);  //清除中断标志位
}
