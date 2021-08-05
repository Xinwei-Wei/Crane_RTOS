#ifndef _PWM_H
#define _PWM_H
#include "sys.h"

#define	MOTOR_OUT_MAX	10000	//Êä³öÏÞ·ù

#define Mecanum1_Dir_Pin PCout(0)
#define Mecanum2_Dir_Pin PCout(1)
#define Mecanum3_Dir_Pin PCout(2)
#define Mecanum4_Dir_Pin PCout(3)


extern int pwm_duty;
void TIM8_PWM_Init(u16 arr, u16 psc);
void Mecanum_PWM_SetDuty(int channel, double duty);
void TIM5_PWM_Init(u32 arr,u32 psc);

void Motor_IO_Init(void);

void Set_Concer(float angle);

#endif
