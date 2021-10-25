#include "encoder.h"
#include "freecars.h"
#include "stm32f4xx_gpio.h"
#include "math.h"
#include "led.h"
#include "key.h"
#include "adc.h"
#include "usart.h"
#include "oled.h"
#include "beep.h"

u8 H, L;
u16 value;
int count = 0;
float voltage1, voltage0;
float v;
float sum, SUM;
float val[50], max, min, MAX, MIN;
int j, k, l;
float value_current[12];
u8 Last_count;

/**************************************************************************
�������ܣ���TIM2��ʼ��Ϊ�������ӿ�ģʽ
��ڲ�������
����  ֵ����
**************************************************************************/
void Encoder_Init_TIM2(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  //ʱ������
    TIM_ICInitTypeDef TIM_ICInitStructure;  //���벶��
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//ʹ�ܶ�ʱ��2��ʱ��
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��PA�˿�ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��PA�˿�ʱ��
//GPIO����Ϊ����
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	//�˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF ; //��������
    GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_100MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);					      //�����趨������ʼ��GPIOA
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	//�˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF ; //��������
    GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_100MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);					      //�����趨������ʼ��GPIOA

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_TIM2); //
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_TIM2); //

//��ʱ������  ��ʱ����
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);//����ΪĬ��ģʽ
    TIM_TimeBaseStructure.TIM_Prescaler = 0; // Ԥ��Ƶ��
    TIM_TimeBaseStructure.TIM_Period = 0xffffffff; //�趨�������Զ���װֵ65535
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//ѡ��ʱ�ӷ�Ƶ������Ƶ
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;////TIM���ϼ���
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
//������ģʽ����	�����벶��
    TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);//ʹ�ñ�����ģʽ3
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 10;//ÿ����΢�����һ�Σ�����N�Σ�������Ǹߵ�ƽ��͵�ƽ���һ�����
    TIM_ICInit(TIM2, &TIM_ICInitStructure);
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);//���TIM�ĸ��±�־λ
    //����ж�����
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    //Reset counter
    TIM_SetCounter(TIM2, 0);
    TIM_Cmd(TIM2, ENABLE);
}

void Encoder_Init_TIM3(void)//PB4,PB5
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  //ʱ������
    TIM_ICInitTypeDef TIM_ICInitStructure;  //���벶��
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//ʹ�ܶ�ʱ��2��ʱ��
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��PA�˿�ʱ��
//GPIO����Ϊ����
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;	//�˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF ; //��������
    GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_100MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);					      //�����趨������ʼ��GPIOA

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_TIM3); //
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_TIM3); //

//��ʱ������  ��ʱ����
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);//����ΪĬ��ģʽ
    TIM_TimeBaseStructure.TIM_Prescaler = 0; // Ԥ��Ƶ��
    TIM_TimeBaseStructure.TIM_Period = ENCODER_TIM_PERIOD; //�趨�������Զ���װֵ65535
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//ѡ��ʱ�ӷ�Ƶ������Ƶ
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;////TIM���ϼ���
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
//������ģʽ����	�����벶��
    TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);//ʹ�ñ�����ģʽ3
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 10;//ÿ����΢�����һ�Σ�����N�Σ�������Ǹߵ�ƽ��͵�ƽ���һ�����
    TIM_ICInit(TIM3, &TIM_ICInitStructure);
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);//���TIM�ĸ��±�־λ
    //����ж�����
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

    //Reset counter
    TIM_SetCounter(TIM3, 0);
    TIM_Cmd(TIM3, ENABLE);
}

void Encoder_Init_TIM2_back(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  //ʱ������
    TIM_ICInitTypeDef TIM_ICInitStructure;  //���벶��
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//ʹ�ܶ�ʱ��2��ʱ��
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��PA�˿�ʱ��
//GPIO����Ϊ����
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;	//�˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF ; //��������
    GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_100MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);					      //�����趨������ʼ��GPIOA

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_TIM2); //
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM2); //

//��ʱ������  ��ʱ����
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);//����ΪĬ��ģʽ
    TIM_TimeBaseStructure.TIM_Prescaler = 0; // Ԥ��Ƶ��
    TIM_TimeBaseStructure.TIM_Period = ENCODER_TIM_PERIOD; //�趨�������Զ���װֵ65535
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//ѡ��ʱ�ӷ�Ƶ������Ƶ
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;////TIM���ϼ���
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
//������ģʽ����	�����벶��
    TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI12, TIM_ICPolarity_Falling, TIM_ICPolarity_Falling);//ʹ�ñ�����ģʽ3
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 10;//ÿ����΢�����һ�Σ�����N�Σ�������Ǹߵ�ƽ��͵�ƽ���һ�����
    TIM_ICInit(TIM2, &TIM_ICInitStructure);
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);//���TIM�ĸ��±�־λ
    //����ж�����
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    //Reset counter
    TIM_SetCounter(TIM2, 0);
    TIM_Cmd(TIM2, ENABLE);
}
/**************************************************************************
�������ܣ���TIM4��ʼ��Ϊ�������ӿ�ģʽ
��ڲ�������
����  ֵ����
**************************************************************************/
void Encoder_Init_TIM4(void)//PD12,PD13
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);//ʹ�ܶ�ʱ��4��ʱ��
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//ʹ��PB�˿�ʱ��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;	//�˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; //��������
    GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_100MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);					      //�����趨������ʼ��GPIOB

    GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4); //����·
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4); //����·

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = 0x0; // Ԥ��Ƶ��
    TIM_TimeBaseStructure.TIM_Period = ENCODER_TIM_PERIOD; //�趨�������Զ���װֵ
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//ѡ��ʱ�ӷ�Ƶ������Ƶ
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;////TIM���ϼ���
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);//ʹ�ñ�����ģʽ3
    TIM_ICStructInit(&TIM_ICInitStructure);   //��ʼ��ֵ
    TIM_ICInitStructure.TIM_ICFilter = 10;     //���벶��ɸѡ
    TIM_ICInit(TIM4, &TIM_ICInitStructure);
    TIM_ClearFlag(TIM4, TIM_FLAG_Update);//���TIM�ĸ��±�־λ
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
    //Reset counter
    TIM_SetCounter(TIM4, 0);
    TIM_Cmd(TIM4, ENABLE);
}

void Encoder_Init_TIM5(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);//ʹ�ܶ�ʱ��4��ʱ��
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��PB�˿�ʱ��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;	//�˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; //��������
    GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_100MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);					      //�����趨������ʼ��GPIOB

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_TIM5); //����·
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM5); //����·

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = 0x0; // Ԥ��Ƶ��
    TIM_TimeBaseStructure.TIM_Period = 0xffffffff; //�趨�������Զ���װֵ
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//ѡ��ʱ�ӷ�Ƶ������Ƶ
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;////TIM���ϼ���
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

    TIM_EncoderInterfaceConfig(TIM5, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);//ʹ�ñ�����ģʽ3
    TIM_ICStructInit(&TIM_ICInitStructure);   //��ʼ��ֵ
    TIM_ICInitStructure.TIM_ICFilter = 10;     //���벶��ɸѡ
    TIM_ICInit(TIM5, &TIM_ICInitStructure);
    TIM_ClearFlag(TIM5, TIM_FLAG_Update);//���TIM�ĸ��±�־λ
    TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
    //Reset counter
    TIM_SetCounter(TIM5, 0);
    TIM_Cmd(TIM5, ENABLE);
}

/**************************************************************************
�������ܣ���λʱ���ȡ����������
��ڲ�������ʱ��
����  ֵ���ٶ�ֵ
**************************************************************************/
int Read_Encoder(u8 TIMX)
{
    int Encoder_TIM;
    switch(TIMX)
    {
    case 2:
        Encoder_TIM = TIM2 -> CNT;
        TIM2 -> CNT = 0;
        break;
    case 3:
        Encoder_TIM = TIM3 -> CNT;
        TIM3 -> CNT = 0;
        break;
    case 4:
        Encoder_TIM = TIM4 -> CNT;
        TIM4 -> CNT = 0;
        break;
	case 5:
        Encoder_TIM = TIM5 -> CNT;
        TIM5 -> CNT = 0;
        break;
    default:
        Encoder_TIM = 0;
    }
    return Encoder_TIM;
}

