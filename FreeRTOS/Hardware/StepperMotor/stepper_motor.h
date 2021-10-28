#ifndef __STEPPER_H
#define __STEPPER_H
#include "sys.h"
#include "includes.h"

//high 1600 1800 3000 4200 5400
#define RL1 1050
#define RL0 900
#define RIGHT_TO_LEFT 2700

#define MILK_HIGH 600
#define ONE_FLOAT_HIGH 700
#define TWO_FLOAT_HIGH 1450
#define UD1 350
#define UD2 500

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
