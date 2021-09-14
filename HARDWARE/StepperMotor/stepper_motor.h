#ifndef __STEPPER_H
#define __STEPPER_H
#include "sys.h"
#include "includes.h"

//high 1600 1800 3000 4200 5400
#define RL1 800
#define RL0 800
#define RIGHT_TO_LEFT 2600

#define MILK_HIGH 1600
#define ONE_FLOAT_HIGH 1800
#define TWO_FLOAT_HIGH 4200
#define UD 1200

void Stepper_Init(void);
void bottom_stepper_turn(double angle);
void RL_stepper_turn(double angle);
void UD_stepper_turn(double angle);
void Bottom_Soft_IRQ(void);
void RL_Soft_IRQ(void);
void UD_Soft_IRQ(void);
void soft_TIM_start(u16 frequency);
void bottom_turn_to(double angle);

#endif
