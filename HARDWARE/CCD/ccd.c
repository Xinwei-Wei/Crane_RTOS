#include "ccd.h"
#include "stm32f4xx.h"
#include "delay.h"
#include "usart.h"
#include "adc.h"

#define CCD_CLK  PAout(11)
#define CCD_SI  PAout(12)
#define threshold  300

u8 tsl1401_finish_flag;
u16 ccd_data[128]; 

//��ʼ��ADC(AO)	ADC1 ͨ��5  PA5														   
void  ccd_Init(void)
{    
	GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef       ADC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //ʹ��ADC1ʱ��

  //�ȳ�ʼ��ADC1ͨ��5 IO��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;//PA5 ͨ��5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//ģ������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��  
 
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	  //ADC1��λ
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//��λ����	 
 
	
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//����ģʽ
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//���������׶�֮����ӳ�5��ʱ��
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //DMAʧ��
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;//Ԥ��Ƶ4��Ƶ��ADCCLK=PCLK2/4=84/4=21Mhz,ADCʱ����ò�Ҫ����36Mhz 
	ADC_CommonInit(&ADC_CommonInitStructure);//��ʼ��
	
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12λģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;//��ɨ��ģʽ	
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//�ر�����ת��
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//��ֹ������⣬ʹ���������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//�Ҷ���	
	ADC_InitStructure.ADC_NbrOfConversion = 1;//1��ת���ڹ��������� Ҳ����ֻת����������1 
	ADC_Init(ADC1, &ADC_InitStructure);//ADC��ʼ��
	
	ADC_Cmd(ADC1, ENABLE);//����ADת����	

}				  

void TIM7_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE);  ///ʹ��TIM7ʱ��
	
    TIM_TimeBaseInitStructure.TIM_Period = arr; 	//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM7,&TIM_TimeBaseInitStructure);//��ʼ��TIM7
	
	TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE); //����ʱ��7�����ж�
	TIM_Cmd(TIM7,ENABLE); //ʹ�ܶ�ʱ��7
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM7_IRQn; //��ʱ��7�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x01; //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}


//PA11:��ΪCLK
//PA12:��ΪSI
void CCD_IO(void)
{    	 
	GPIO_InitTypeDef  GPIO_InitStructure;	

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��

	//GPIOA7��ʼ������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;//PA��ӦIO��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIO
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      TSL1401����CCD���ݲɼ�
//  @param      NULL
//  @return     void
//  @since      v1.0
//  Sample usage:               ��isr.c�����ȴ�����Ӧ���жϺ�����Ȼ����øú���(֮�����������жϱ�־λ)
//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------
//  @brief      TSL1401����CCD���ݲɼ�
//  @param      NULL
//  @return     void
//  @since      v1.0
//  Sample usage:               ��isr.c�����ȴ�����Ӧ���жϺ�����Ȼ����øú���(֮�����������жϱ�־λ)
//-------------------------------------------------------------------------------------------------------------------
void ccd_collect(void)
{
    u8 i = 0;

    CCD_CLK=1;
    CCD_SI=0;
    CCD_CLK=0;
    CCD_SI=1;
    CCD_CLK=1;
    CCD_SI=0;

    for(i=0;i<128;i++)
    {
        CCD_CLK=0;
        //�������ͬʱ�ɼ�����CCD����
        ccd_data[i] = Get_Adc1();
//        ccd_data_two[i] = adc_convert(AD_CHANNEL, AD_RESOLUTION);
        CCD_CLK=1;
    }

    //�ɼ���ɱ�־λ��1
    tsl1401_finish_flag = 1;

}

void ccd_init(void)     
{       
    ccd_Init();
    TIM7_Int_Init(999,83); //10ms
    CCD_IO();
}


//-------------------------------------------------------------------------------------------------------------------
//  @brief      TSL1401����CCDͼ��������λ���鿴ͼ��
//  @param      uart_n          ���ں�
//  @param      uart_n          ����CCD����ָ��
//  @return     void
//  @since      v1.0
//  Sample usage:               ���øú���ǰ���ȳ�ʼ������
//-------------------------------------------------------------------------------------------------------------------
void ccd_send_data(USART_TypeDef* uart_n ,u16 *dat)
{
   u8 i = 0;
   uart_putchar(uart_n, 0x00); 
   uart_putchar(uart_n, 0xff);
   uart_putchar(uart_n, 0x01);
   uart_putchar(uart_n, 0x00);
   
    for(i=0; i<128; i++)
    {
        uart_putchar(uart_n, dat[i]>>8);   //���͸�8λ
        uart_putchar(uart_n, dat[i]&0XFF); //���͵�8λ
    }
}

//��ʱ��7�жϷ�����
void TIM7_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM7,TIM_IT_Update)==SET) //����ж�
	{
		ccd_collect();//CCD���ݲɼ�;
	}
	TIM_ClearITPendingBit(TIM7,TIM_IT_Update);  //����жϱ�־λ
}


//Ѱ�ڵ��ߣ�������ʼλ��
int find_line(void)
{
	u8 i;
	for(i=0;i<128;i++)
	{
		if(ccd_data[i] > threshold && ccd_data[i+1]>threshold && ccd_data[i+2] > threshold)
		{
			return i;
			
		}
	}
	return -1;	
}
