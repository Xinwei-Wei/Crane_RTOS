#ifndef __ADC_H
#define __ADC_H	
#include "sys.h" 
 							   
void Adc_Init(void); 				//ADCͨ����ʼ��
u16  Get_Adc1(void); 				//���ĳ��ͨ��ֵ 
u16  Get_Adc2(void); 				//���ĳ��ͨ��ֵ 
u16 Get_Adc_Average(u8 ch,u8 times);//�õ�ĳ��ͨ����������������ƽ��ֵ  
#endif 















