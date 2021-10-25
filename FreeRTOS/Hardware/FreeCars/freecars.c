#include "freecars.h"
char uSendBuf[ScopeChaNum * 2]; //�����͸���λ��������

#if EN_USART2_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���
u8 USART2_RX_BUF[USART2_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART2_RX_STA = 0;     //����״̬���

#define UartRxBufferLen  100
#define UartRxDataLen    41           //��λ�����͸�������MCU���գ���Ҫ��
#define UartRxCmdLen     7	      //��λ�������������ݳ��ȣ���Ҫ��
float a1, a2, b1, b2, c1, c2;
typedef struct
{
    int Stack;   //��ջ��¼ ����ָ����һ������λ
    u8 Data;     //��¼�ӱ��ν��յ�������
    u8 PreData;
    u8 Buffer[UartRxBufferLen];  //����λ�����յ�������
    u8 Enable;
    u8 Check;
} SerialPortType;

SerialPortType SerialPortRx;
double UartData[9];          //����λ�����յ�������

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
    /* ʹ�� UART1 ģ���ʱ��  ʹ�� UART1��Ӧ�����Ŷ˿�PA��ʱ��*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//ʹ��GPIODʱ��5,6(TX,RX)

    GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_USART2); //GPIOA9����ΪUSART1      TX
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_USART2); //GPIOA10����ΪUSART1     RX
    //USART1�˿�����
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6; //GPIOA9��GPIOA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
    GPIO_Init(GPIOD, &GPIO_InitStructure); //��ʼ��PA9��PA10
    //USART3��ʼ������
    USART_InitStructure.USART_BaudRate = baudrate;//����������
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
    USART_Init(USART2, &USART_InitStructure); //��ʼ������1

    USART_ClearFlag(USART2, USART_FLAG_TC);
    USART_Cmd(USART2, ENABLE);  //ʹ�ܴ���1

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//��������ж�
    //Usart1 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//����1�ж�ͨ��
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //��ռ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//�����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����
    //USART_ITConfig(USART3, USART_IT_TXE, DISABLE);

}

/******************************************************************
*��λ���������ݸ���Ƭ����
*4bytesУ����ͷ
*4*9bytes����λ������ͨ��1--9�����ݣ�ÿͨ������4λ��
*1byteУ��λ
*******************************************************************/
//void USART2_IRQHandler(void)
//{
//    u32 i = 0, b = 0, d;
//    u32 dat_temp;

////    Buzzer_GetOffSetTmOver();//�������������û��
//    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //���յ�������
//    {
//        SerialPortRx.Data = USART_ReceiveData(USART2);
//        if( SerialPortRx.Stack < UartRxBufferLen )  //Stackȡ��0~Len-1��
//            SerialPortRx.Buffer[SerialPortRx.Stack++] = SerialPortRx.Data;//stack��ָ����һ��Ҫ������ݵĵط�
//        //UartRxDataLen 41����Ϊһ֡   41=4(У��λ4bytes)+9*4(9��ͨ��double��)+1��byteУ��λ��
//        if( SerialPortRx.Stack >= UartRxDataLen
//                && SerialPortRx.Buffer[SerialPortRx.Stack - UartRxDataLen]  == 0xff //У����ͷ
//                && SerialPortRx.Buffer[SerialPortRx.Stack - UartRxDataLen + 1] == 0x55
//                && SerialPortRx.Buffer[SerialPortRx.Stack - UartRxDataLen + 2] == 0xaa
//                && SerialPortRx.Buffer[SerialPortRx.Stack - UartRxDataLen + 3] == 0x10 )
//        {   //double data 9��ͨ������У��   (���յ�һ������ͨ��32λ)
//            SerialPortRx.Check = 0;
//            b = SerialPortRx.Stack - UartRxDataLen; //��ʼλ����У����ͷ����λ

//            for(i = b; i < SerialPortRx.Stack - 1; i++) //��ȥУ��λ���λ����У��
//            {
//                SerialPortRx.Check += SerialPortRx.Buffer[i];//У��
//            }

//            //�Լ������У��λ����յ���У��λ���жԱ�
//            if( SerialPortRx.Check == SerialPortRx.Buffer[SerialPortRx.Stack - 1] )
//            {
//                for(i = 0; i < 9; i++)
//                {
//                    //32λ ��16λ���������֣���16λ��С������
//                    dat_temp = SerialPortRx.Buffer[b + i * 4 + 4] * 0x1000000L
//                               + SerialPortRx.Buffer[b + i * 4 + 5] * 0x10000L
//                               + SerialPortRx.Buffer[b + i * 4 + 6] * 0x100L
//                               + SerialPortRx.Buffer[b + i * 4 + 7];

//                    if(dat_temp > 0x7FFFFFFF)  d = 0x7FFFFFFF - dat_temp  ; //����
//                    else       d = dat_temp  ;

//                    UartData[i] = d;
//                    UartData[i] /= 65536.0;

//                }
//                UartDebug();  //תȥ�������ܵ������ݸ�������

//            }
//            SerialPortRx.Stack = 0;
//        }
//        //��λ���������������û�õ�
//        else if(   SerialPortRx.Stack >= UartRxCmdLen //UartRxCmdLen = 7����Ϊһ֡
//                   && SerialPortRx.Buffer[SerialPortRx.Stack - UartRxCmdLen]  == 0xff
//                   && SerialPortRx.Buffer[SerialPortRx.Stack - UartRxCmdLen + 1] == 0x55
//                   && SerialPortRx.Buffer[SerialPortRx.Stack - UartRxCmdLen + 2] == 0xaa
//                   && SerialPortRx.Buffer[SerialPortRx.Stack - UartRxCmdLen + 3] == 0x77 ) //cmd
//        {
//            SerialPortRx.Check = 0;
//            b = SerialPortRx.Stack - UartRxCmdLen; //��ʼλ
//            for(i = b; i < SerialPortRx.Stack - 1; i++) //��У��λ���λ����У��
//            {
//                SerialPortRx.Check += SerialPortRx.Buffer[i];//У��
//            }
//            if( SerialPortRx.Check == SerialPortRx.Buffer[SerialPortRx.Stack - 1] )
//            {   //У��ɹ�
//                //UartCmd(UartCmdNum,UartCmdData);//������յ����������MCU�������
//            }
//            SerialPortRx.Stack = 0;
//        }
//    }

//    else        //���յ������ݳ��������������ջ���
//    {
//        SerialPortRx.Stack = 0;
//    }

//}

#endif

//��Ҫ������λ��������ѹ�뻺��
void push(char chanel, u16 data)
{
    uSendBuf[chanel * 2] = data / 256;
    uSendBuf[chanel * 2 + 1] = data % 256;
}


/*--------------------------��Ƭ����������------------------------*/
//����һ���ַ�������
void uartSendChar(USART_TypeDef* USARTx, u8 sendData)
{
    USART_SendData(USARTx, sendData);         //�򴮿�1��������
    while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) != SET); //�ȴ����ͽ���
}

//���������ݷ��͸���λ��
void sendDataToScope(void)
{
    u8 i, sum = 0;

    //ʹ����ѯ�ķ�ʽ�������ݣ�������δ���ͣ�����ͣ�ڴ˴�ֱ���������
    uartSendChar(MYUART, 251);
    uartSendChar(MYUART, 109);
    uartSendChar(MYUART, 37); //֪ͨ��λ��Ҫ����������

    sum += (251);    //ȫ�����ݼ���У��
    sum += (109);
    sum += (37);

    for(i = 0; i < ScopeChaNum * 2; i++) //��ѹ�뻺���е�����һ�θ�ͨ��0--16
    {
        uartSendChar(MYUART, uSendBuf[i]);
        sum += uSendBuf[i];       //ȫ�����ݼ���У��
    }

    uartSendChar(MYUART, sum); //���һλ������У��λ
}

///*-------------------------------------------------------------------------------*/

////void pushLineData(Int16 ccd,Byte *data)
////{
////	Int16 i;
////	Int16 head = ScopeChaNum*2;
////	uSendBuf[ScopeChaNum*2] = ccd;//CCDѡ��ѡ����ʾ����CCD������
////	for(i=0;i<LineDataNum;i++) //ת�Ƶ����ͻ�����
////	{
////		uSendBuf[head + i + 2] = data[i];
////	}
////}

///*-------------------------------------------------------------------------------*/
