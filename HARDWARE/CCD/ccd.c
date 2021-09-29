#include "ccd.h"
#include "stm32f4xx.h"
#include "delay.h"
#include "usart.h"
#include "adc.h"
#include "pid.h"



#define ture true
#define CCD1_CLK  	PAout(11)
#define CCD1_SI		PAout(12)
#define CCD2_CLK	PCout(11)
#define CCD2_SI		PCout(12)
#define threshold1  2700
#define line3_wide  30
#define line5_wide  70

u8 ccd_finish_flag;
u16 ccd1_data[128];
u16 ccd2_data[128];
u8 stop_line = line5_wide;
extern float targetSpeedY;
extern int stop_judge, slow_down_judge;
int EN_stop = 0;


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
void CCD1_Collect(void)
{
    u8 i = 0;

    CCD1_CLK=1;
    CCD1_SI=0;
    CCD1_CLK=0;
    CCD1_SI=1;
    CCD1_CLK=1;
    CCD1_SI=0;

    for(i=0;i<128;i++)
    {
        CCD1_CLK=0;
        //这里可以同时采集两个CCD数据
        ccd1_data[i] = Get_Adc1();
        CCD1_CLK=1;
    }

    //采集完成标志位置1
    ccd_finish_flag = 1;
}

void CCD2_Collect(void)
{
    u8 i = 0;

	CCD2_CLK=1;
	CCD2_SI=0;
	CCD2_CLK=0;
	CCD2_SI=1;
	CCD2_CLK=1;
	CCD2_SI=0;

    for(i=0;i<128;i++)
    {
		CCD2_CLK=0;
        //这里可以同时采集两个CCD数据
		ccd2_data[i] = Get_Adc2();
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
		if(ccd1_data[i] > 2700 && ccd1_data[i+1]>2700 && ccd1_data[i+2] > 2700)
		{
			return i;			
		}
	}
	return -1;	
}

int Find_Line(u16 *data, int center, int threshold)
{
	int i, emergency_flag = 0, edge_count = 0, edge_left = 0, edge_right = 127;
	int emergency_count = 0, emergency_max = 0, emergency_right = 0;
	
	for(i=center-2; i<=center+2; i++)
	{
		if(data[i] > threshold)
		{
			emergency_flag = 1;
			break;
		}
	}
	
	if(emergency_flag == 0)
	{
		for(i=center-3; i>=0; i--)
		{
			if(data[i] > threshold)
				edge_count++;
			if(edge_count == 3)
				break;
		}
		edge_left = i+3;
		edge_count = 0;
		
		for(i=center+3; i<=127; i++)
		{
			if(data[i] > threshold)
				edge_count++;
			if(edge_count == 3)
				break;
		}
		
			
		edge_right = i-3;
		edge_count = 0;
		
		if(edge_right - edge_left > stop_line)
		{
			if(data == ccd2_data){
				if(EN_stop)
				{
					if(targetSpeedY == 60)
						targetSpeedY = 30;
					else
					{
						stop_line = line3_wide;
						targetSpeedY = 0;
						stop_judge = 1;
						EN_stop = 0;
					}					
				}
			}
		}
		else EN_stop = 1;
		
		center = (edge_left + edge_right) / 2 + 0.5;
		center = LIMIT(3, center, 124);
		return center;
	}
	else
	{
		emergency_max = 0;
		emergency_flag = 0;
		emergency_count = 0;
		for(i=0; i<=127; i++)
		{
			if(data[i] <= threshold)
			{
				emergency_count++;
			}
			else
			{				
				if(emergency_count > emergency_max)
				{
					emergency_max = emergency_count;
					emergency_right = i-1;
				}
				emergency_count = 0;
			}
		}
		printf("%d\r\n", emergency_max);
		if(emergency_max >= 3)
			center = emergency_right - emergency_max/2 - 0.5;
		center = LIMIT(3, center, 124);
		return center;
	}
}

int CCD1_find_Line(int center, int threshold)
{
	int i, emergency_flag = 0, edge_count = 0, edge_left = 0, edge_right = 127;
	int emergency_count = 0, emergency_max = 0, emergency_right = 0;
	
	for(i=center-2; i<=center+2; i++)
	{
		if(ccd1_data[i] > threshold)
		{
			emergency_flag = 1;
			break;
		}
	}
	
	if(emergency_flag == 0)
	{
		for(i=center-3; i>=0; i--)
		{
			if(ccd1_data[i] > threshold)
				edge_count++;
			if(edge_count == 3)
				break;
		}
		edge_left = i+3;
		edge_count = 0;
		
		for(i=center+3; i<=127; i++)
		{
			if(ccd1_data[i] > threshold)
				edge_count++;
			if(edge_count == 3)
				break;
		}
		
			
		edge_right = i-3;
		edge_count = 0;
		
		if(edge_right - edge_left > line5_wide)
		{
			return center;
		}
		
		center = (edge_left + edge_right) / 2 + 0.5;
		center = LIMIT(3, center, 124);
		return center;
	}
	else
	{
		emergency_max = 0;
		emergency_flag = 0;
		emergency_count = 0;
		for(i=0; i<=127; i++)
		{
			if(ccd1_data[i] <= threshold)
			{
				emergency_count++;
			}
			else
			{				
				if(emergency_count > emergency_max)
				{
					emergency_max = emergency_count;
					emergency_right = i-1;
				}
				emergency_count = 0;
			}
		}
		if(emergency_max >= 3)
			center = emergency_right - emergency_max/2 - 0.5;
		center = LIMIT(3, center, 124);
		return center;
	}
}

int CCD2_find_Line(int center, int threshold)
{
	int i, emergency_flag = 0, edge_count = 0, edge_left = 0, edge_right = 127;
	int emergency_count = 0, emergency_max = 0, emergency_right = 0;
	static int time = 0;
	
	for(i=center-2; i<=center+2; i++)
	{
		if(ccd2_data[i] > threshold)
		{
			emergency_flag = 1;
			break;
		}
	}
	
	if(emergency_flag == 0)
	{
		for(i=center-3; i>=0; i--)
		{
			if(ccd2_data[i] > threshold)
				edge_count++;
			if(edge_count == 3)
				break;
		}
		
		edge_left = i+3;
		edge_count = 0;
		
		for(i=center+3; i<=127; i++)
		{
			if(ccd2_data[i] > threshold)
				edge_count++;
			if(edge_count == 3)
				break;
		}
		
			
		edge_right = i-3;
		edge_count = 0;
		
		if(edge_right - edge_left > stop_line)
		{
			if(EN_stop)
			{
				if(targetSpeedY == 60){
					if(edge_right - edge_left > 50)
					{
						targetSpeedY = 20;
						EN_stop = 0;
						slow_down_judge = 1;
					}
				}
				else
				{
					stop_line = line3_wide;
					targetSpeedY = 0;
					stop_judge = 1;
					EN_stop = 0;
				}	
			}
			time = 0;
		}
		else{
			if(targetSpeedY != 0){
				if(time > 5){
					EN_stop = 1;
				}
				else{
					time += 1;
				}
			}
		}
		
		center = (edge_left + edge_right) / 2 + 0.5;
		center = LIMIT(3, center, 124);
		if(edge_left<4)
			center = edge_right-15;
		return center;
	}
	else
	{
		emergency_max = 0;
		emergency_flag = 0;
		emergency_count = 0;
		for(i=0; i<=127; i++)
		{
			if(ccd2_data[i] <= threshold)
			{
				emergency_count++;
			}
			else
			{				
				if(emergency_count > emergency_max)
				{
					emergency_max = emergency_count;
					emergency_right = i-1;
				}
				emergency_count = 0;
			}
		}
		if(emergency_max >= 3)
			center = emergency_right - emergency_max/2 - 0.5;
		center = LIMIT(3, center, 124);
		return center;
	}
}

int Find_Line_first(u16 *data, int threshold)
{
	int i;
	for(i=62; i<=66; i++)
	{
		if(data[i] > threshold)
		{
			return 0;
		}
	}
	return 1;
}




