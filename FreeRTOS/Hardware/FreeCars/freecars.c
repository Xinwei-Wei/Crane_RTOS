#include "freecars.h"
char uSendBuf[ScopeChaNum * 2]; //待发送给上位机的数据

#if EN_USART2_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误
u8 USART2_RX_BUF[USART2_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART2_RX_STA = 0;     //接收状态标记

#define UartRxBufferLen  100
#define UartRxDataLen    41           //上位机发送浮点数据MCU接收，不要改
#define UartRxCmdLen     7	      //上位机接收命令数据长度，不要改
float a1, a2, b1, b2, c1, c2;
typedef struct
{
    int Stack;   //堆栈记录 总是指向下一个数据位
    u8 Data;     //记录从本次接收到的数据
    u8 PreData;
    u8 Buffer[UartRxBufferLen];  //从上位机接收到的数据
    u8 Enable;
    u8 Check;
} SerialPortType;

SerialPortType SerialPortRx;
double UartData[9];          //从上位机接收到的数据

void Page0_debug(void)
{

}

void Page1_debug(void)
{

}

void Page2_debug(void)
{

}

void Page3_debug(void)
{

}

void Page4_debug(void)
{
    //    CarSpeedCtr.Ki=UartData[4];
}

void Page5_debug(void)
{
    //  CarSpeedCtr.Kd=UartData[5];
}

void Page6_debug(void)
{
    //   k=UartData[6];
}

void Page7_debug(void)
{
    //  CarSpeedCtr.SpeedSet=UartData[7];
}

void UartDebug(void)
{
    switch((u16)UartData[8])
    {
    case 0:
        Page0_debug();
        break;
    case 1:
        Page1_debug();
        break;
    case 2:
        Page2_debug();
        break;
    case 3:
        Page3_debug();
        break;
    case 4:
        Page4_debug();
        break;
    case 5:
        Page5_debug();
        break;
    case 6:
        Page6_debug();
        break;
    case 7:
        Page7_debug();
        break;
    }

}

void Initial_USART2(u32 baudrate)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    /* 使能 UART1 模块的时钟  使能 UART1对应的引脚端口PA的时钟*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//使能GPIOD时钟5,6(TX,RX)

    GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_USART2); //GPIOA9复用为USART1      TX
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_USART2); //GPIOA10复用为USART1     RX
    //USART1端口配置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6; //GPIOA9与GPIOA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
    GPIO_Init(GPIOD, &GPIO_InitStructure); //初始化PA9，PA10
    //USART3初始化设置
    USART_InitStructure.USART_BaudRate = baudrate;//波特率设置
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
    USART_Init(USART2, &USART_InitStructure); //初始化串口1

    USART_ClearFlag(USART2, USART_FLAG_TC);
    USART_Cmd(USART2, ENABLE);  //使能串口1

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启相关中断
    //Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//串口1中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、
    //USART_ITConfig(USART3, USART_IT_TXE, DISABLE);

}

/******************************************************************
*上位机发送数据给单片机，
*4bytes校验字头
*4*9bytes数据位（发送通道1--9的数据，每通道数据4位）
*1byte校验位
*******************************************************************/
//void USART2_IRQHandler(void)
//{
//    u32 i = 0, b = 0, d;
//    u32 dat_temp;

////    Buzzer_GetOffSetTmOver();//看进入这个部分没有
//    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //接收到了数据
//    {
//        SerialPortRx.Data = USART_ReceiveData(USART2);
//        if( SerialPortRx.Stack < UartRxBufferLen )  //Stack取（0~Len-1）
//            SerialPortRx.Buffer[SerialPortRx.Stack++] = SerialPortRx.Data;//stack是指向下一个要存放数据的地方
//        //UartRxDataLen 41个数为一帧   41=4(校验位4bytes)+9*4(9个通道double型)+1（byte校验位）
//        if( SerialPortRx.Stack >= UartRxDataLen
//                && SerialPortRx.Buffer[SerialPortRx.Stack - UartRxDataLen]  == 0xff //校验字头
//                && SerialPortRx.Buffer[SerialPortRx.Stack - UartRxDataLen + 1] == 0x55
//                && SerialPortRx.Buffer[SerialPortRx.Stack - UartRxDataLen + 2] == 0xaa
//                && SerialPortRx.Buffer[SerialPortRx.Stack - UartRxDataLen + 3] == 0x10 )
//        {   //double data 9个通道数据校验   (接收的一个数据通道32位)
//            SerialPortRx.Check = 0;
//            b = SerialPortRx.Stack - UartRxDataLen; //起始位，即校验字头所在位

//            for(i = b; i < SerialPortRx.Stack - 1; i++) //除去校验位外的位进行校验
//            {
//                SerialPortRx.Check += SerialPortRx.Buffer[i];//校验
//            }

//            //自己算出的校验位与接收到的校验位进行对比
//            if( SerialPortRx.Check == SerialPortRx.Buffer[SerialPortRx.Stack - 1] )
//            {
//                for(i = 0; i < 9; i++)
//                {
//                    //32位 高16位存整数部分，低16位存小数部分
//                    dat_temp = SerialPortRx.Buffer[b + i * 4 + 4] * 0x1000000L
//                               + SerialPortRx.Buffer[b + i * 4 + 5] * 0x10000L
//                               + SerialPortRx.Buffer[b + i * 4 + 6] * 0x100L
//                               + SerialPortRx.Buffer[b + i * 4 + 7];

//                    if(dat_temp > 0x7FFFFFFF)  d = 0x7FFFFFFF - dat_temp  ; //负数
//                    else       d = dat_temp  ;

//                    UartData[i] = d;
//                    UartData[i] /= 65536.0;

//                }
//                UartDebug();  //转去处理，把受到的数据付给变量

//            }
//            SerialPortRx.Stack = 0;
//        }
//        //上位机发过来命令，这里没用到
//        else if(   SerialPortRx.Stack >= UartRxCmdLen //UartRxCmdLen = 7个数为一帧
//                   && SerialPortRx.Buffer[SerialPortRx.Stack - UartRxCmdLen]  == 0xff
//                   && SerialPortRx.Buffer[SerialPortRx.Stack - UartRxCmdLen + 1] == 0x55
//                   && SerialPortRx.Buffer[SerialPortRx.Stack - UartRxCmdLen + 2] == 0xaa
//                   && SerialPortRx.Buffer[SerialPortRx.Stack - UartRxCmdLen + 3] == 0x77 ) //cmd
//        {
//            SerialPortRx.Check = 0;
//            b = SerialPortRx.Stack - UartRxCmdLen; //起始位
//            for(i = b; i < SerialPortRx.Stack - 1; i++) //除校验位外的位进行校验
//            {
//                SerialPortRx.Check += SerialPortRx.Buffer[i];//校验
//            }
//            if( SerialPortRx.Check == SerialPortRx.Buffer[SerialPortRx.Stack - 1] )
//            {   //校验成功
//                //UartCmd(UartCmdNum,UartCmdData);//处理接收到的命令，付给MCU命令变量
//            }
//            SerialPortRx.Stack = 0;
//        }
//    }

//    else        //接收到的数据超过接收上限则堆栈清空
//    {
//        SerialPortRx.Stack = 0;
//    }

//}

#endif

//将要发送上位机的数据压入缓存
void push(char chanel, u16 data)
{
    uSendBuf[chanel * 2] = data / 256;
    uSendBuf[chanel * 2 + 1] = data % 256;
}


/*--------------------------单片机发送数据------------------------*/
//发送一个字符给串口
void uartSendChar(USART_TypeDef* USARTx, u8 sendData)
{
    USART_SendData(USARTx, sendData);         //向串口1发送数据
    while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) != SET); //等待发送结束
}

//将缓冲数据发送给上位机
void sendDataToScope(void)
{
    u8 i, sum = 0;

    //使用轮询的方式发送数据，当数据未发送，程序停在此处直到发送完成
    uartSendChar(MYUART, 251);
    uartSendChar(MYUART, 109);
    uartSendChar(MYUART, 37); //通知上位机要发送数据了

    sum += (251);    //全部数据加入校验
    sum += (109);
    sum += (37);

    for(i = 0; i < ScopeChaNum * 2; i++) //把压入缓存中的数据一次给通道0--16
    {
        uartSendChar(MYUART, uSendBuf[i]);
        sum += uSendBuf[i];       //全部数据加入校验
    }

    uartSendChar(MYUART, sum); //最后一位数据是校验位
}

///*-------------------------------------------------------------------------------*/

////void pushLineData(Int16 ccd,Byte *data)
////{
////	Int16 i;
////	Int16 head = ScopeChaNum*2;
////	uSendBuf[ScopeChaNum*2] = ccd;//CCD选择，选择显示发送CCD的数据
////	for(i=0;i<LineDataNum;i++) //转移到发送缓冲区
////	{
////		uSendBuf[head + i + 2] = data[i];
////	}
////}

///*-------------------------------------------------------------------------------*/
