#ifndef __STEPPER_H
#define __STEPPER_H
#include "sys.h"
#include "includes.h"

void Stepper_Init(void);
int stepper_turn(double angle, u16 frequency);
int soft_IRQ(void);
void soft_TIM_start(u16 frequency);

#endif
