#ifndef __LED_H
#define __LED_H
#include "sys.h"



//LED端口定义
#define LED0 PAout(6)	// DS0
#define LED1 PAout(7)	// DS1	 

void LED_Init(void);//初始化
void Stepper_EN_Init(void);
#endif

