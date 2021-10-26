#include "hc-sr04.h"
#include "usart.h"
#include "includes.h"

#define	delay_ms(x) vTaskDelay(x)

static void delay_us(u32 nus)
{
	nus *= 14;	// 主频168MHZ
	for(; nus--; );
}

int overcount;
//定时器5初始化
void TIM5_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);  ///使能TIM5时钟
	RCC_APB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);  ///
	
    TIM_TimeBaseInitStructure.TIM_Period = 999; 	//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=83;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseInitStructure);//初始化TIM5
	
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE); //允许定时器5更新中断
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM5_IRQn; //定时器5中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);	


	TIM_Cmd(TIM5, DISABLE);
}



void CH_SR04_IO(void)
{    	 
	GPIO_InitTypeDef  GPIO_InitStructure;	
	GPIO_InitTypeDef  GPIO_InitStructure1;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//使能GPIOF时钟

  //GPIOC4初始化设置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;//PC4对应IO口
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIO
	
	/*ECOH回响信号*///PC1
	
	GPIO_InitStructure1.GPIO_Pin = GPIO_Pin_1; // 对应引脚
	GPIO_InitStructure1.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
	GPIO_InitStructure1.GPIO_Speed = GPIO_Speed_100MHz;//100M
	GPIO_InitStructure1.GPIO_PuPd = GPIO_PuPd_NOPULL  ;//浮空
	GPIO_Init(GPIOC, &GPIO_InitStructure1);//初始化PC1

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
	/*测5次数据计算一次平均值*/
	while(i!=5)
	{
		PCout(4)=1;  //拉高信号，作为触发信号
		delay_us(20);  //高电平信号超过10us
		PCout(4)=0;
		/*等待回响信号*/	
		while(PCin(1)==0);
		TIM_Cmd(TIM5,ENABLE);//回响信号到来，开启定时器计数
		i+=1;  //每收到一次回响信号+1，收到5次就计算均值
		while(PCin(1)==1);//回响信号消失
		TIM_Cmd(TIM5,DISABLE);//关闭定时器
		tim=TIM_GetCounter(TIM5);//获取计TIM2数寄存器中的计数值，一边计算回响信号时间

		length=(tim+overcount*1000)*340/1000/4;//通过回响信号计算距离
		sum=length+sum;
		TIM_SetCounter(TIM5, 0);//计数器清零  //将TIM5计数寄存器的计数值清零
		overcount=0;  //中断溢出次数清零
	}
	delay_ms(100);
	length=sum/5;
	return length;//距离作为函数返回值
}



//障碍物测试
//有障碍物：0
//无障碍物：1
u8 block(u16 distance)
{
	u8 flag=0;
	float length=0;
	u16 tim;
	PCout(4)=1;  //拉高信号，作为触发信号
	delay_us(20);  //高电平信号超过10us
	PCout(4)=0;
	/*等待回响信号*/	
	while(PCin(1)==0);
	TIM_Cmd(TIM5,ENABLE);//回响信号到来，开启定时器计数
	while(PCin(1)==1);//回响信号消失
	TIM_Cmd(TIM5,DISABLE);//关闭定时器

	tim=TIM_GetCounter(TIM5);//获取计TIM2数寄存器中的计数值，一边计算回响信号时间
	length=(tim+overcount*1000)*340/1000/4;//通过回响信号计算距离
	if(length>distance)
	{
		flag=1;
	}
	TIM_SetCounter(TIM5, 0);//计数器清零  //将TIM5计数寄存器的计数值清零
	overcount=0;  //中断溢出次数清零
	return flag;//距离作为函数返回值
}
	

void TIM5_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM5,TIM_IT_Update)==SET) //溢出中断
	{
		overcount++;
	}
	TIM_ClearITPendingBit(TIM5,TIM_IT_Update);  //清除中断标志位
}

