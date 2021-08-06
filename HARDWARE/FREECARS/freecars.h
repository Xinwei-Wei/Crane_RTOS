#ifndef __FREECARS_H
#define __FREECARS_H

#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "sys.h"

#define   MYUART    	 USART2
#define   SerialBaud 	 115200
#define   ScopeChaNum      17       //��λ��ͨ����
//#define   LineDataNum      128      //CCDͼ����

extern char uSendBuf[ScopeChaNum * 2]; //�����͸���λ��������
extern double UartData[9];          //����λ�����յ�������


#define USART2_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART2_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����

extern u8  USART2_RX_BUF[USART2_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з�
extern u16 USART2_RX_STA;         		//����״̬���

void Initial_USART2(u32 baudrate);

extern void push(char chanel, u16 data);              //��ĳ��ͨ��������������ݺ���
extern void sendDataToScope(void);                 //���ݷ��ͺ���
extern void uartSendChar(USART_TypeDef* USARTx, u8 sendData); ////����һ���ַ�������
//extern void pushLineData(Int16 ccd,Byte *data); //����CCD����
#endif

//------------------End of File----------------------------

