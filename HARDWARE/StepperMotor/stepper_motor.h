#ifndef __STEPPER_H
#define __STEPPER_H
#include "sys.h"
#include "includes.h"

void Stepper_Init(void);
int bottom_stepper_turn(double angle);
int RL_stepper_turn(double angle);
int UD_stepper_turn(double angle);
void soft_IRQ(void);
void soft_TIM_start(u16 frequency);

#endif
