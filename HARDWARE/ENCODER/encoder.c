#include "encoder.h"
#include "freecars.h"
#include "stm32f4xx_gpio.h"
#include "math.h"
#include "led.h"
#include "key.h"
#include "adc.h"
#include "usart.h"
#include "oled.h"
#include "beep.h"
#include "delay.h"
#include "encoder.h"

u8 H, L;
u16 value;
int count = 0;
float voltage1, voltage0;
float v;
float sum, SUM;
float val[50], max, min, MAX, MIN;
int j, k, l;
float value_current[12];
u8 Last_count;

/**************************************************************************
函数功能：把TIM2初始化为编码器接口模式
入口参数：无
返回  值：无
**************************************************************************/
void Encoder_Init_TIM2(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  //时基设置
    TIM_ICInitTypeDef TIM_ICInitStructure;  //输入捕获
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//使能定时器2的时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能PA端口时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能PA端口时钟
//GPIO设置为输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	//端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF ; //浮空输入
    GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_100MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);					      //根据设定参数初始化GPIOA
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	//端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF ; //浮空输入
    GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_100MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);					      //根据设定参数初始化GPIOA

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_TIM2); //
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_TIM2); //

//定时器设置  （时基）
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);//配置为默认模式
    TIM_TimeBaseStructure.TIM_Prescaler = 0; // 预分频器
    TIM_TimeBaseStructure.TIM_Period = 0xffffffff; //设定计数器自动重装值65535
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//选择时钟分频：不分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;////TIM向上计数
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
//编码器模式设置	（输入捕获）
    TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);//使用编码器模式3
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 10;//每多少微秒采样一次，采样N次，如果都是高电平或低电平则记一次输出
    TIM_ICInit(TIM2, &TIM_ICInitStructure);
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);//清除TIM的更新标志位
    //溢出中断设置
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    //Reset counter
    TIM_SetCounter(TIM2, 0);
    TIM_Cmd(TIM2, ENABLE);
}

void Encoder_Init_TIM3(void)//PB4,PB5
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  //时基设置
    TIM_ICInitTypeDef TIM_ICInitStructure;  //输入捕获
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//使能定时器2的时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能PA端口时钟
//GPIO设置为输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;	//端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF ; //浮空输入
    GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_100MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);					      //根据设定参数初始化GPIOA

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_TIM3); //
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_TIM3); //

//定时器设置  （时基）
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);//配置为默认模式
    TIM_TimeBaseStructure.TIM_Prescaler = 0; // 预分频器
    TIM_TimeBaseStructure.TIM_Period = ENCODER_TIM_PERIOD; //设定计数器自动重装值65535
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//选择时钟分频：不分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;////TIM向上计数
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
//编码器模式设置	（输入捕获）
    TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);//使用编码器模式3
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 10;//每多少微秒采样一次，采样N次，如果都是高电平或低电平则记一次输出
    TIM_ICInit(TIM3, &TIM_ICInitStructure);
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);//清除TIM的更新标志位
    //溢出中断设置
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

    //Reset counter
    TIM_SetCounter(TIM3, 0);
    TIM_Cmd(TIM3, ENABLE);
}

void Encoder_Init_TIM2_back(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  //时基设置
    TIM_ICInitTypeDef TIM_ICInitStructure;  //输入捕获
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//使能定时器2的时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能PA端口时钟
//GPIO设置为输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;	//端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF ; //浮空输入
    GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_100MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);					      //根据设定参数初始化GPIOA

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_TIM2); //
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM2); //

//定时器设置  （时基）
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);//配置为默认模式
    TIM_TimeBaseStructure.TIM_Prescaler = 0; // 预分频器
    TIM_TimeBaseStructure.TIM_Period = ENCODER_TIM_PERIOD; //设定计数器自动重装值65535
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//选择时钟分频：不分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;////TIM向上计数
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
//编码器模式设置	（输入捕获）
    TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI12, TIM_ICPolarity_Falling, TIM_ICPolarity_Falling);//使用编码器模式3
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 10;//每多少微秒采样一次，采样N次，如果都是高电平或低电平则记一次输出
    TIM_ICInit(TIM2, &TIM_ICInitStructure);
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);//清除TIM的更新标志位
    //溢出中断设置
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    //Reset counter
    TIM_SetCounter(TIM2, 0);
    TIM_Cmd(TIM2, ENABLE);
}
/**************************************************************************
函数功能：把TIM4初始化为编码器接口模式
入口参数：无
返回  值：无
**************************************************************************/
void Encoder_Init_TIM4(void)//PD12,PD13
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);//使能定时器4的时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//使能PB端口时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;	//端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; //浮空输入
    GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_100MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);					      //根据设定参数初始化GPIOB

    GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4); //第三路
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4); //第四路

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = 0x0; // 预分频器
    TIM_TimeBaseStructure.TIM_Period = ENCODER_TIM_PERIOD; //设定计数器自动重装值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//选择时钟分频：不分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;////TIM向上计数
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);//使用编码器模式3
    TIM_ICStructInit(&TIM_ICInitStructure);   //初始填值
    TIM_ICInitStructure.TIM_ICFilter = 10;     //输入捕获筛选
    TIM_ICInit(TIM4, &TIM_ICInitStructure);
    TIM_ClearFlag(TIM4, TIM_FLAG_Update);//清除TIM的更新标志位
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
    //Reset counter
    TIM_SetCounter(TIM4, 0);
    TIM_Cmd(TIM4, ENABLE);
}

void Encoder_Init_TIM5(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);//使能定时器4的时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能PB端口时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;	//端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; //浮空输入
    GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_100MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);					      //根据设定参数初始化GPIOB

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_TIM5); //第三路
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM5); //第四路

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = 0x0; // 预分频器
    TIM_TimeBaseStructure.TIM_Period = 0xffffffff; //设定计数器自动重装值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//选择时钟分频：不分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;////TIM向上计数
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

    TIM_EncoderInterfaceConfig(TIM5, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);//使用编码器模式3
    TIM_ICStructInit(&TIM_ICInitStructure);   //初始填值
    TIM_ICInitStructure.TIM_ICFilter = 10;     //输入捕获筛选
    TIM_ICInit(TIM5, &TIM_ICInitStructure);
    TIM_ClearFlag(TIM5, TIM_FLAG_Update);//清除TIM的更新标志位
    TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
    //Reset counter
    TIM_SetCounter(TIM5, 0);
    TIM_Cmd(TIM5, ENABLE);
}

/**************************************************************************
函数功能：单位时间读取编码器计数
入口参数：定时器
返回  值：速度值
**************************************************************************/
int Read_Encoder(u8 TIMX)
{
    int Encoder_TIM;
    switch(TIMX)
    {
    case 2:
        Encoder_TIM = TIM2 -> CNT;
        TIM2 -> CNT = 0;
        break;
    case 3:
        Encoder_TIM = TIM3 -> CNT;
        TIM3 -> CNT = 0;
        break;
    case 4:
        Encoder_TIM = TIM4 -> CNT;
        TIM4 -> CNT = 0;
        break;
	case 5:
        Encoder_TIM = TIM5 -> CNT;
        TIM5 -> CNT = 0;
        break;
    default:
        Encoder_TIM = 0;
    }
    return Encoder_TIM;
}

