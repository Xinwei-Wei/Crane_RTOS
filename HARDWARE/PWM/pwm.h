#ifndef _PWM_H
#define _PWM_H
#include "sys.h"

#define   MOTOR_OUT_MAX                     10000   //Êä³öÏÞ·ù


#define motor_l PCout(1)
#define motor_r PCout(4)
#define down PCout(3)

#define led PCout(5)


extern int pwm_duty;
void TIM8_PWM_Init(u16 arr, u16 psc);
void TIM8_PWM_SetDuty(int channel, double duty);
void TIM5_PWM_Init(u32 arr,u32 psc);

void Motor_IO_Init(void);

void Set_Concer(float angle);

#endif
