#include "adc.h"
#include "includes.h"

#define	delay_ms(x) vTaskDelay(x)

/****************************
ADC			1 	2 	3
ͨ�� 0  PA0 PA0 PA0
ͨ�� 1  PA1 PA1 PA1
ͨ�� 2  PA2 PA2 PA2
ͨ�� 3  PA3 PA3 PA3
ͨ�� 4  PA4 PA4 PF6
ͨ�� 5  PA5 PA5 PF7
ͨ�� 6  PA6 PA6 PF8
ͨ�� 7  PA7 PA7 PF9
ͨ�� 8  PB0 PB0 PF10
ͨ�� 9  PB1 PB1 PF3
ͨ�� 10 PC0 PC0 PC0
ͨ�� 11 PC1 PC1 PC1
ͨ�� 12 PC2 PC2 PC2
ͨ�� 13 PC3 PC3 PC3
ͨ�� 14 PC4 PC4 PF4
ͨ�� 15 PC5 PC5 PF5
*****************************/


/****************************************************************
�������ܣ���ʼ��adc
��ڲ�������
����ֵ	����
*****************************************************************/
void Adc_Init(void)
{    
	GPIO_InitTypeDef  		GPIO_InitStructure;
	ADC_CommonInitTypeDef 	ADC_CommonInitStructure;
	ADC_InitTypeDef       	ADC_InitStructure;
	ADC_StructInit(&ADC_InitStructure);
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	//ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); 	//ʹ��ADC1ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE); 	//ʹ��ADC2ʱ��

	//�ȳ�ʼ��ADC1ͨ��4 IO��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;			//ģ������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;		//����������
	GPIO_Init(GPIOB, &GPIO_InitStructure);					//��ʼ��GPIO

	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	  	//ADC1��λ
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//��λ����
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC2,ENABLE);	  	//ADC2��λ
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC2,DISABLE);	//��λ����		
 
	
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;						//����ģʽ
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;	//���������׶�֮����ӳ�5��ʱ��
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;			//DMAʧ��
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;						//Ԥ��Ƶ4��Ƶ��ADCCLK=PCLK2/4=84/4=21Mhz,ADCʱ����ò�Ҫ����36Mhz 
	ADC_CommonInit(&ADC_CommonInitStructure);										//��ʼ��
	
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;	//12λģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;			//��ɨ��ģʽ	
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;		//�ر�����ת��
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;	//��ֹ������⣬ʹ���������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//�Ҷ���
	
	ADC_InitStructure.ADC_NbrOfConversion = 1;				//ʹ�ù���ͨ��1
	ADC_Init(ADC1, &ADC_InitStructure);						//ADC1��ʼ��
//	ADC_InitStructure.ADC_NbrOfConversion = 1;				//ʹ�ù���ͨ��1
	ADC_Init(ADC2, &ADC_InitStructure);						//ADC2��ʼ��
	
	ADC_Cmd(ADC1, ENABLE);	//����ADC1
	ADC_Cmd(ADC2, ENABLE);	//����ADC2

}	

/****************************************************************
�������ܣ����ADCֵ
��ڲ�����ch:ͨ��ֵ 0~16ȡֵ��ΧΪ��ADC_Channel_0~ADC_Channel_16
����ֵ	��ת�����
*****************************************************************/
u16 Get_Adc1(void)
{
	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	ADC_RegularChannelConfig(ADC1, 8, 1, ADC_SampleTime_480Cycles);	//ADC1ͨ��0,480������,��߲���ʱ�������߾�ȷ��			    
  
	ADC_SoftwareStartConv(ADC1);					//ʹ��ָ����ADC1�����ת����������	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));	//�ȴ�ת������

	return ADC_GetConversionValue(ADC1);			//�������һ��ADC1�������ת�����
}

u16 Get_Adc2(void)
{
	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	ADC_RegularChannelConfig(ADC2, 9, 1, ADC_SampleTime_480Cycles );	//ADC2ͨ��1,480������,��߲���ʱ�������߾�ȷ��			    
  
	ADC_SoftwareStartConv(ADC2);					//ʹ��ָ����ADC2�����ת����������	
	 
	while(!ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC ));	//�ȴ�ת������

	return ADC_GetConversionValue(ADC2);			//�������һ��ADC2�������ת�����
}

/****************************************************************
�������ܣ���ȡͨ��ch��ת��ֵ��ȡtimes��,Ȼ��ƽ�� 
��ڲ�����ch:ͨ�����
					times:��ȡ����
					ͨ��ֵ 0~16ȡֵ��ΧΪ��ADC_Channel_0~ADC_Channel_16
����ֵ	��ͨ��ch��times��ת�����ƽ��ֵ
*****************************************************************/
u16 Get_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc1();
		delay_ms(5);
	}
	return temp_val/times;
}
