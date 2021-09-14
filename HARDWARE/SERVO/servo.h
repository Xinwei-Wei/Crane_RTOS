#ifndef __SERVO_H
#define __SERVO_H
#include "sys.h"

void Servo_Init(void);
void set_servo_angle(int angle);
void servo_open(void);
void servo_close(void);


#endif
