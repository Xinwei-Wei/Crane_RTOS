#ifndef __STEPPER_H
#define __STEPPER_H
#include "sys.h"

void Stepper_Init(u16 arr,u16 psc);
void stepper_turn(double angle, u16 frequency);
#endif
