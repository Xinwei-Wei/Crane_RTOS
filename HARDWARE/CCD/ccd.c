#include "ccd.h"
#include "stm32f4xx.h"
#include "delay.h"
#include "usart.h"
#include "adc.h"

#define CCD1_CLK  	PAout(11)
#define CCD1_SI		PAout(12)
#define CCD2_CLK
#define CCD2_CLK
#define threshold  300

u8 ccd1_finish_flag;
u16 ccd1_data[128];

void CCD_Init(void)
{
	Adc_Init();
	CCD_IO();
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
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//100MHz
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
void CCD_Collect(void)
{
    u8 i = 0;

    CCD1_CLK=1;
    CCD1_SI=0;
    CCD1_CLK=0;
    CCD1_SI=1;
    CCD1_CLK=1;
    CCD1_SI=0;

    for(i=0;i<128;i++)
    {
        CCD1_CLK=0;
        //�������ͬʱ�ɼ�����CCD����
        ccd1_data[i] = Get_Adc1();
//        ccd_data_two[i] = adc_convert(AD_CHANNEL, AD_RESOLUTION);
        CCD1_CLK=1;
    }

    //�ɼ���ɱ�־λ��1
    ccd1_finish_flag = 1;
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

}


//Ѱ�ڵ��ߣ�������ʼλ��
int find_line(void)
{
	u8 i;
	for(i=0;i<128;i++)
	{
		if(ccd1_data[i] > threshold && ccd1_data[i+1]>threshold && ccd1_data[i+2] > threshold)
		{
			return i;
			
		}
	}
	return -1;	
}
