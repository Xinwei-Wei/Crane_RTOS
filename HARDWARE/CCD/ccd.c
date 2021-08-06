#include "ccd.h"
#include "stm32f4xx.h"
#include "delay.h"
#include "usart.h"
#include "adc.h"

#define CCD1_CLK  	PAout(11)
#define CCD1_SI		PAout(12)
#define CCD2_CLK	PCout(11)
#define CCD2_SI		PCout(12)
#define threshold  300

u8 ccd_finish_flag;
u16 ccd1_data[128];
u16 ccd2_data[128];

void CCD_Init(void)
{
	Adc_Init();
	CCD_IO();
}

//PA11:作为CLK
//PA12:作为SI
void CCD_IO(void)
{    	 
	GPIO_InitTypeDef  GPIO_InitStructure;	

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);	//使能GPIOA时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);	//使能GPIOD时钟

	//GPIOA初始化设置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;//CCD1对应IO口
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;			//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//50MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;			//上拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//初始化GPIOA
	GPIO_Init(GPIOC, &GPIO_InitStructure);					//初始化GPIOD
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
void CCD_Collect(void)
{
    u8 i = 0;

    CCD1_CLK=1;
	CCD2_CLK=1;
    CCD1_SI=0;
	CCD2_SI=0;
    CCD1_CLK=0;
	CCD2_CLK=0;
    CCD1_SI=1;
	CCD2_SI=1;
    CCD1_CLK=1;
	CCD2_CLK=1;
    CCD1_SI=0;
	CCD2_SI=0;

    for(i=0;i<128;i++)
    {
        CCD1_CLK=0;
		CCD2_CLK=0;
        //这里可以同时采集两个CCD数据
        ccd1_data[i] = Get_Adc1();
		ccd2_data[i] = Get_Adc2();
        CCD1_CLK=1;
		CCD2_CLK=1;
    }

    //采集完成标志位置1
    ccd_finish_flag = 1;
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

}


//寻黑单线，返回起始位置
int find_line(void)
{
	u8 i;
	for(i=0;i<128;i++)
	{
		if(ccd1_data[i] > threshold && ccd1_data[i+1]>threshold && ccd1_data[i+2] > threshold)
		{
			return i;
			
		}
	}
	return -1;	
}
