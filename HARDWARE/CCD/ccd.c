#include "ccd.h"
#include "stm32f4xx.h"
#include "delay.h"
#include "usart.h"
#include "adc.h"

#define CCD_CLK  PAout(11)
#define CCD_SI  PAout(12)
#define threshold  300

u8 tsl1401_finish_flag;
u16 ccd_data[128]; 

//初始化ADC(AO)	ADC1 通道5  PA5														   
void  ccd_Init(void)
{    
	GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef       ADC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //使能ADC1时钟

  //先初始化ADC1通道5 IO口
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;//PA5 通道5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//模拟输入
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//不带上下拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化  
 
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	  //ADC1复位
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//复位结束	 
 
	
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//独立模式
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//两个采样阶段之间的延迟5个时钟
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //DMA失能
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;//预分频4分频。ADCCLK=PCLK2/4=84/4=21Mhz,ADC时钟最好不要超过36Mhz 
	ADC_CommonInit(&ADC_CommonInitStructure);//初始化
	
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12位模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;//非扫描模式	
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//关闭连续转换
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//禁止触发检测，使用软件触发
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//右对齐	
	ADC_InitStructure.ADC_NbrOfConversion = 1;//1个转换在规则序列中 也就是只转换规则序列1 
	ADC_Init(ADC1, &ADC_InitStructure);//ADC初始化
	
	ADC_Cmd(ADC1, ENABLE);//开启AD转换器	

}				  

void TIM7_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE);  ///使能TIM7时钟
	
    TIM_TimeBaseInitStructure.TIM_Period = arr; 	//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM7,&TIM_TimeBaseInitStructure);//初始化TIM7
	
	TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE); //允许定时器7更新中断
	TIM_Cmd(TIM7,ENABLE); //使能定时器7
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM7_IRQn; //定时器7中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x01; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}


//PA11:作为CLK
//PA12:作为SI
void CCD_IO(void)
{    	 
	GPIO_InitTypeDef  GPIO_InitStructure;	

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA时钟

	//GPIOA7初始化设置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;//PA对应IO口
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      TSL1401线阵CCD数据采集
//  @param      NULL
//  @return     void
//  @since      v1.0
//  Sample usage:               在isr.c里面先创建对应的中断函数，然后调用该函数(之后别忘记清除中断标志位)
//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------
//  @brief      TSL1401线阵CCD数据采集
//  @param      NULL
//  @return     void
//  @since      v1.0
//  Sample usage:               在isr.c里面先创建对应的中断函数，然后调用该函数(之后别忘记清除中断标志位)
//-------------------------------------------------------------------------------------------------------------------
void ccd_collect(void)
{
    u8 i = 0;

    CCD_CLK=1;
    CCD_SI=0;
    CCD_CLK=0;
    CCD_SI=1;
    CCD_CLK=1;
    CCD_SI=0;

    for(i=0;i<128;i++)
    {
        CCD_CLK=0;
        //这里可以同时采集两个CCD数据
        ccd_data[i] = Get_Adc1();
//        ccd_data_two[i] = adc_convert(AD_CHANNEL, AD_RESOLUTION);
        CCD_CLK=1;
    }

    //采集完成标志位置1
    tsl1401_finish_flag = 1;

}

void ccd_init(void)     
{       
    ccd_Init();
    TIM7_Int_Init(999,83); //10ms
    CCD_IO();
}


//-------------------------------------------------------------------------------------------------------------------
//  @brief      TSL1401线阵CCD图像发送至上位机查看图像
//  @param      uart_n          串口号
//  @param      uart_n          线性CCD数据指针
//  @return     void
//  @since      v1.0
//  Sample usage:               调用该函数前请先初始化串口
//-------------------------------------------------------------------------------------------------------------------
void ccd_send_data(USART_TypeDef* uart_n ,u16 *dat)
{
   u8 i = 0;
   uart_putchar(uart_n, 0x00); 
   uart_putchar(uart_n, 0xff);
   uart_putchar(uart_n, 0x01);
   uart_putchar(uart_n, 0x00);
   
    for(i=0; i<128; i++)
    {
        uart_putchar(uart_n, dat[i]>>8);   //发送高8位
        uart_putchar(uart_n, dat[i]&0XFF); //发送低8位
    }
}

//定时器7中断服务函数
void TIM7_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM7,TIM_IT_Update)==SET) //溢出中断
	{
		ccd_collect();//CCD数据采集;
	}
	TIM_ClearITPendingBit(TIM7,TIM_IT_Update);  //清除中断标志位
}


//寻黑单线，返回起始位置
int find_line(void)
{
	u8 i;
	for(i=0;i<128;i++)
	{
		if(ccd_data[i] > threshold && ccd_data[i+1]>threshold && ccd_data[i+2] > threshold)
		{
			return i;
			
		}
	}
	return -1;	
}
