#ifndef __CCD_H
#define __CCD_H
#include "sys.h"

void  Adc_Init(void);
void  ccd_Init(void);
u16 Get_Adc_Average(u8 ch,u8 times);

void ccd_init(void);
void ccd_send_data(USART_TypeDef* uart_n ,u16 *dat);
int find_line(void);

#endif

