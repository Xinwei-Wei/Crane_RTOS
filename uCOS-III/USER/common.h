#ifndef _COMMON_H_
#define _COMMON_H_

/**************************************
SYSCLK(ϵͳʱ��) =168MHz
AHB ����ʱ��(HCLK=SYSCLK) =168MHz
APB1 ����ʱ��(PCLK1=SYSCLK/4) =42MHz
APB2 ����ʱ��(PCLK2=SYSCLK/2) =84MHz
PLL ��ʱ�� =168MHz
***************************************/

#include "stm32f4xx_conf.h"
#include "stdio.h"
/** \brief  Enable IRQ Interrupts

  This function enables IRQ interrupts by clearing the I-bit in the CPSR.
  Can only be executed in Privileged modes.
 */
#define EnableInterrupts   __enable_irq()     //�����ж�
#define DisableInterrupts  __disable_irq()    //�����ж�

/*****************************������������*****************************/
typedef 	unsigned 	char		u8;	  //�޷�����
typedef     u8                      Byte;
typedef     u8                      bool;
typedef 	unsigned 	short int	u16;
//typedef 	unsigned 	long  int	u32;

typedef 	signed char			    s8;	  //�з�����
typedef 	short  int				s16;
//typedef 	long   int				s32;
/*****************************��C#����*********************************/
typedef     u8                      byte;
typedef     s16                     Int16;
typedef     double                  Double;
typedef     u16                     UInt16;
typedef     u32                     uint;
typedef     u32                     UInt32;
typedef     s32                     Int32;
/**********************************************************************/

#define ERROR       0
#define SUCCESS     1

#define true        1
#define false       0
#define none        2

//0,��֧��ucos
//1,֧��ucos
#define SYSTEM_SUPPORT_UCOS		0		//����ϵͳ�ļ����Ƿ�֧��UCOS


//λ������,ʵ��51���Ƶ�GPIO���ƹ���
//����ʵ��˼��,�ο�<<CM3Ȩ��ָ��>>������(87ҳ~92ҳ).M4ͬM3����,ֻ�ǼĴ�����ַ����.
//IO�ڲ����궨��
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2))
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr))
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum))
//IO�ڵ�ַӳ��
#define GPIOA_ODR_Addr    (GPIOA_BASE+20) //0x40020014
#define GPIOB_ODR_Addr    (GPIOB_BASE+20) //0x40020414 
#define GPIOC_ODR_Addr    (GPIOC_BASE+20) //0x40020814 
#define GPIOD_ODR_Addr    (GPIOD_BASE+20) //0x40020C14 
#define GPIOE_ODR_Addr    (GPIOE_BASE+20) //0x40021014 
#define GPIOF_ODR_Addr    (GPIOF_BASE+20) //0x40021414    
#define GPIOG_ODR_Addr    (GPIOG_BASE+20) //0x40021814   
#define GPIOH_ODR_Addr    (GPIOH_BASE+20) //0x40021C14    
#define GPIOI_ODR_Addr    (GPIOI_BASE+20) //0x40022014     

#define GPIOA_IDR_Addr    (GPIOA_BASE+16) //0x40020010 
#define GPIOB_IDR_Addr    (GPIOB_BASE+16) //0x40020410 
#define GPIOC_IDR_Addr    (GPIOC_BASE+16) //0x40020810 
#define GPIOD_IDR_Addr    (GPIOD_BASE+16) //0x40020C10 
#define GPIOE_IDR_Addr    (GPIOE_BASE+16) //0x40021010 
#define GPIOF_IDR_Addr    (GPIOF_BASE+16) //0x40021410 
#define GPIOG_IDR_Addr    (GPIOG_BASE+16) //0x40021810 
#define GPIOH_IDR_Addr    (GPIOH_BASE+16) //0x40021C10 
#define GPIOI_IDR_Addr    (GPIOI_BASE+16) //0x40022010 

//IO�ڲ���,ֻ�Ե�һ��IO��!
//ȷ��n��ֵС��16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //��� 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //���� 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //��� 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //���� 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //��� 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //���� 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //��� 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //���� 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //��� 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //����

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //��� 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //����

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //��� 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //����

#define PHout(n)   BIT_ADDR(GPIOH_ODR_Addr,n)  //��� 
#define PHin(n)    BIT_ADDR(GPIOH_IDR_Addr,n)  //����

#define PIout(n)   BIT_ADDR(GPIOI_ODR_Addr,n)  //��� 
#define PIin(n)    BIT_ADDR(GPIOI_IDR_Addr,n)  //����

#define ABS(x) ( (x)>0?(x):-(x) )
#define LIMIT( x,min,max ) ( (x) < (min)  ? (min) : ( (x) > (max) ? (max) : (x) ) )   //�޷�
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#endif

