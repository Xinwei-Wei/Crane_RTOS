#ifndef __FREECARS_H
#define __FREECARS_H

#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "sys.h"

#define   MYUART    	 USART2
#define   SerialBaud 	 115200
#define   ScopeChaNum      17       //上位机通道数
//#define   LineDataNum      128      //CCD图像宽度

extern char uSendBuf[ScopeChaNum * 2]; //待发送给上位机的数据
extern double UartData[9];          //从上位机接收到的数据


#define USART2_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_USART2_RX 			1		//使能（1）/禁止（0）串口1接收

extern u8  USART2_RX_BUF[USART2_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符
extern u16 USART2_RX_STA;         		//接收状态标记

void Initial_USART2(u32 baudrate);

extern void push(char chanel, u16 data);              //向某个通道缓冲区填充数据函数
extern void sendDataToScope(void);                 //数据发送函数
extern void uartSendChar(USART_TypeDef* USARTx, u8 sendData); ////发送一个字符给串口
//extern void pushLineData(Int16 ccd,Byte *data); //发送CCD数据
#endif

//------------------End of File----------------------------

