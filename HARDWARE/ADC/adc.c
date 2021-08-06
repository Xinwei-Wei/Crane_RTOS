#include "adc.h"
#include "delay.h"		 

/****************************
ADC			1 	2 	3
通道 0  PA0 PA0 PA0
通道 1  PA1 PA1 PA1
通道 2  PA2 PA2 PA2
通道 3  PA3 PA3 PA3
通道 4  PA4 PA4 PF6
通道 5  PA5 PA5 PF7
通道 6  PA6 PA6 PF8
通道 7  PA7 PA7 PF9
通道 8  PB0 PB0 PF10
通道 9  PB1 PB1 PF3
通道 10 PC0 PC0 PC0
通道 11 PC1 PC1 PC1
通道 12 PC2 PC2 PC2
通道 13 PC3 PC3 PC3
通道 14 PC4 PC4 PF4
通道 15 PC5 PC5 PF5
*****************************/


/****************************************************************
函数功能：初始化adc
入口参数：无
返回值	：无
*****************************************************************/
void Adc_Init(void)
{    
	GPIO_InitTypeDef  		GPIO_InitStructure;
	ADC_CommonInitTypeDef 	ADC_CommonInitStructure;
	ADC_InitTypeDef       	ADC_InitStructure;
	ADC_StructInit(&ADC_InitStructure);
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);	//使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); 	//使能ADC1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE); 	//使能ADC2时钟

	//先初始化ADC1通道4 IO口
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;			//模拟输入
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;		//不带上下拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//初始化GPIO

	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	  	//ADC1复位
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//复位结束
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC2,ENABLE);	  	//ADC2复位
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC2,DISABLE);	//复位结束		
 
	
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;						//独立模式
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;	//两个采样阶段之间的延迟5个时钟
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;			//DMA失能
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;						//预分频4分频。ADCCLK=PCLK2/4=84/4=21Mhz,ADC时钟最好不要超过36Mhz 
	ADC_CommonInit(&ADC_CommonInitStructure);										//初始化
	
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;	//12位模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;			//非扫描模式	
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;		//关闭连续转换
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;	//禁止触发检测，使用软件触发
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//右对齐
	
	ADC_InitStructure.ADC_NbrOfConversion = 1;				//使用规则通道1
	ADC_Init(ADC1, &ADC_InitStructure);						//ADC1初始化
	ADC_InitStructure.ADC_NbrOfConversion = 2;				//使用规则通道2
	ADC_Init(ADC2, &ADC_InitStructure);						//ADC2初始化
	
	ADC_Cmd(ADC1, ENABLE);	//开启ADC1
	ADC_Cmd(ADC2, ENABLE);	//开启ADC2

}	

/****************************************************************
函数功能：获得ADC值
入口参数：ch:通道值 0~16取值范围为：ADC_Channel_0~ADC_Channel_16
返回值	：转换结果
*****************************************************************/
u16 Get_Adc1(void)
{
	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, 0, 1, ADC_SampleTime_480Cycles);	//ADC1通道0,480个周期,提高采样时间可以提高精确度			    
  
	ADC_SoftwareStartConv(ADC1);					//使能指定的ADC1的软件转换启动功能	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));	//等待转换结束

	return ADC_GetConversionValue(ADC1);			//返回最近一次ADC1规则组的转换结果
}

u16 Get_Adc2(void)
{
	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC2, 1, 2, ADC_SampleTime_480Cycles );	//ADC2通道1,480个周期,提高采样时间可以提高精确度			    
  
	ADC_SoftwareStartConv(ADC2);					//使能指定的ADC2的软件转换启动功能	
	 
	while(!ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC ));	//等待转换结束

	return ADC_GetConversionValue(ADC2);			//返回最近一次ADC2规则组的转换结果
}

/****************************************************************
函数功能：获取通道ch的转换值，取times次,然后平均 
入口参数：ch:通道编号
					times:获取次数
					通道值 0~16取值范围为：ADC_Channel_0~ADC_Channel_16
返回值	：通道ch的times次转换结果平均值
*****************************************************************/
u16 Get_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc1();
		delay_ms(5);
	}
	return temp_val/times;
}
