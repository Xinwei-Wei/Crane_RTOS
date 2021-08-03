#ifndef __HC_SR04_H
#define __HC_SR04_H
#include "sys.h"
#include "stm32f4xx.h"

void CH_SR04_int(void);
float Senor_Using(void);
u8 block(u16 distance);

#endif

