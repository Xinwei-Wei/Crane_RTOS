#ifndef __MOTOR_CONTROL_H
#define __MOTOR_CONTROL_H
#include "includes.h"

extern float Camera_angle_pwm;
extern int Camera_angle;
float * moto_caculate(float pwmx, float pwmy, float pwmw);
void Control_Dir(u8 number,float speed);
void duoji_init(void);
void duoji_change(void);

#endif
