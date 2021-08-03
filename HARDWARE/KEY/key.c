#include "key.h"
#include "delay.h" 


void KEY_Init(void)
{

	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//使能GPIOA,GPIOE时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10 |GPIO_Pin_11 |GPIO_Pin_12; //KEY 对应引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;//下拉
	GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化GPIOE4
	 
} 



//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，KEY0按下
//2，KEY1按下
//3，KEY2按下 
//4，WKUP按下 WK_UP
//注意此函数有响应优先级,KEY0>KEY1>KEY2>WK_UP!!
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//按键按松开标志
	if(mode)key_up=1;  //支持连按		  
	if(key_up&&(key1==0||key2==0 || key3==0 || key4==0))
	{
		delay_ms(10);//去抖动 
		key_up=0;
		if(key1==0)return  1;
		else if(key2==0)return 2;
		else if(key3==0)return 3;
		else if(key4==0)return 4;
	}else if(key1==1&&key2==1&&key3==1&&key4==1)key_up=1; 	    
 	return 0;// 无按键按下
}




















