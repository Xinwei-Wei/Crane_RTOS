#ifndef __STEPPER_H
#define __STEPPER_H
#include "sys.h"

void Stepper_Init(void);
void stepper_turn(double angle, u16 frequency);
void soft_IRQ(void);
#endif
