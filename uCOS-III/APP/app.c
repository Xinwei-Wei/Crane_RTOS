/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                             (c) Copyright 2013; Micrium, Inc.; Weston, FL
*
*                   All rights reserved.  Protected by international copyright laws.
*                   Knowledge of the source code may not be used to write a similar
*                   product.  This file may only be used in accordance with a license
*                   and should not be redistributed in any way.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                            EXAMPLE CODE
*
*                                       IAR Development Kits
*                                              on the
*
*                                    STM32F429II-SK KICKSTART KIT
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : YS
*                 DC
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include  <includes.h>

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

/// 任务优先级
#define		AppTask_Receive_PRIO		6u
#define		AppTask_USART_PRIO			7u
#define		AppTask_Mecanum_PRIO		5u
#define		AppTask_CCD_PRIO			4u
#define		AppTask_Stepper_PRIO		8u

/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

/// 系统变量声明
static  OS_TCB		AppTaskStartTCB;
static  CPU_STK		AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE];

/// 任务变量声明
static  OS_TCB		AppTask_Receive_TCB;
static  CPU_STK		AppTask_Receive_Stk[AppTask_Common_STK_SIZE];

static  OS_TCB		AppTask_USART_TCB;
static  CPU_STK		AppTask_USART_Stk[AppTask_Common_STK_SIZE];

static  OS_TCB		AppTask_Mecanum_TCB;
static  CPU_STK		AppTask_Mecanum_Stk[AppTask_Common_STK_SIZE];

static  OS_TCB		AppTask_CCD_TCB;
static  CPU_STK		AppTask_CCD_Stk[AppTask_Common_STK_SIZE];

static  OS_TCB		AppTask_Stepper_TCB;
static  CPU_STK		AppTask_Stepper_Stk[AppTask_Common_STK_SIZE];

/// 用户变量声明
u8 key;
int pwm_x, pwm_y, pwm_w;
float *pwm_mecanum;
int targetspeed = 0;
struct IncrementalPID rightfront_pid, leftfront_pid, rightrear_pid, leftrear_pid;
float rightfront_pwm, leftfront_pwm, rightrear_pwm, leftrear_pwm;
char rev[12];
extern u16 ccd1_data[128];
extern u16 ccd2_data[128];
float *targetMecanum;
float targetSpeedX = 0, targetSpeedY = 0, targetSpeedW = 0;
int ccd1_center = 64, ccd2_center = 64;
unsigned int set_time = 0, reset_time = 100;


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

/// 系统函数声明
static  void  	AppTaskStart	(void *p_arg);
static  void  	AppTaskCreate	(void);
static  void  	AppObjCreate	(void);

/// 任务函数声明
static	void  	AppTask_Receive			(void *p_arg);
static	void	AppTask_USART			(void *p_arg);
static	void	AppTask_Mecanum			(void *p_arg);
static	void	AppTask_Stepper			(void *p_arg);

/// 用户函数声明
static	void	DispTaskInfo(void);
static	void	TestLED(void);
void	Periph_Init	(void);

/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/

int main(void)
{
    OS_ERR  err;

    BSP_IntDisAll();                                            /* Disable all interrupts.                              */
    
    CPU_Init();                                                 /* Initialize the uC/CPU Services                       */
    Mem_Init();                                                 /* Initialize Memory Management Module                  */
    Math_Init();                                                /* Initialize Mathematical Module                       */

    OSInit(&err);                                               /* Init uC/OS-III.                                      */

    OSTaskCreate((OS_TCB       *)&AppTaskStartTCB,              /* Create the start task                                */
                 (CPU_CHAR     *)"App Task Start",
                 (OS_TASK_PTR   )AppTaskStart,
                 (void         *)0u,
                 (OS_PRIO       )APP_CFG_TASK_START_PRIO,
                 (CPU_STK      *)&AppTaskStartStk[0u],
                 (CPU_STK_SIZE  )AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE / 10u],
                 (CPU_STK_SIZE  )APP_CFG_TASK_START_STK_SIZE,
                 (OS_MSG_QTY    )0u,
                 (OS_TICK       )0u,
                 (void         *)0u,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);

    OSStart(&err);                                              /* Start multitasking (i.e. give control to uC/OS-III). */

    (void)&err;

    return (0u);
}

/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  AppTaskStart (void *p_arg)
{
    OS_ERR  err;
	(void)p_arg;

    BSP_Init();                                                 /* Initialize BSP functions                             */
	BSP_Tick_Init();                                            /* Initialize Tick Services.                            */


#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif                                           				/* Turn Off LEDs after initialization                   */

    APP_TRACE_DBG(("Creating Application Kernel Objects\n\r"));
    AppObjCreate();                                             /* Create Applicaiton kernel objects                    */

    APP_TRACE_DBG(("Creating Application Tasks\n\r"));
    AppTaskCreate();                                           	/* Create Application tasks                             */
	
	Periph_Init();

    while (DEF_TRUE)
	{                                          					/* Task body, always written as an infinite loop.       */
		LED0 = 0;
        OSTimeDlyHMSM(0u, 0u, 0u, 500u, OS_OPT_TIME_HMSM_STRICT, &err);

		LED0 = 1;
        OSTimeDlyHMSM(0u, 0u, 0u, 500u, OS_OPT_TIME_HMSM_STRICT, &err);
    }
}

void Periph_Init()
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);				//设置系统中断优先级分组2
//	LED_Init();
	KEY_Init();
	uart_init(115200);
}

/*
*********************************************************************************************************
*
*                                          CUSTOMIZE TASKS
*
*********************************************************************************************************
*/

static void AppTask_Receive(void *p_arg)
{
	OS_ERR  err;
	(void)p_arg;
	
	int rev_num = 0;
	
	for(;;)
	{
		if(USART_GetFlagStatus(USART1 , USART_FLAG_RXNE) != RESET)
		{
			for(rev_num=0; rev_num<=2; rev_num++)
				rev[rev_num] = Read_Bit()-48;
			targetSpeedY = rev[0]*100 + rev[1]*10 + rev[2]-50;
//			leftfront_pid.kp = rev[3] + rev[4]/10.0 + rev[5]/100.0;
//			leftfront_pid.ki = rev[6] + rev[7]/10.0 + rev[8]/100.0;
//			leftfront_pid.kd = rev[9] + rev[10]/10.0 + rev[11]/100.0;
		}
        OSTimeDlyHMSM(0u, 0u, 0u, 100u, OS_OPT_TIME_HMSM_STRICT, &err);
	}
}

static void AppTask_USART(void *p_arg)
{
	OS_ERR  err;
	(void)p_arg;
	
	OSTimeDlyHMSM(0u, 0u, 1u, 0u, OS_OPT_TIME_HMSM_STRICT, &err);
	
	printf("Hello\r\n");
	
	for(;;)
	{
		DispTaskInfo();
		
//		push(1,targetMecanum[1]);
//		push(2,leftfront_pid.error+100);
//		push(3,leftfront_pwm);
//		sendDataToScope();
		
//		printf("%d	%.2f	%.2f	%.2f	%.2f\r\n",targetspeed, leftrear_pid.kp, leftrear_pid.ki, leftrear_pid.kd, incremental_pid(&leftrear_pid));
		OSTimeDlyHMSM(0u, 0u, 2u, 0u, OS_OPT_TIME_HMSM_STRICT, &err);
	}
}

static void AppTask_Mecanum(void *p_arg)
{
	OS_ERR  err;
	(void)p_arg;
	
	Motor_IO_Init();
	TIM8_PWM_Init(500-1, 33-1);
	Encoder_Init_TIM2();
	Encoder_Init_TIM3();
	Encoder_Init_TIM4();
	Encoder_Init_TIM5();
	
	incremental_pid_init(&rightfront_pid, 0.06, 0.1, 0.06);
	incremental_pid_init(&leftfront_pid,  0.06, 0.1, 0.06);
	incremental_pid_init(&rightrear_pid,  0.06, 0.1, 0.06);
	incremental_pid_init(&leftrear_pid,   0.06, 0.1, 0.06);
	
	for(;;)
	{
		targetMecanum = moto_caculate(targetSpeedX, targetSpeedY, targetSpeedW);
		
		if(rightfront_pwm < 0)
			rightfront_pid.error = (targetMecanum[0])+((TIM2->CNT<0xffffffff-TIM2->CNT) ? TIM2->CNT : 0xffffffff-TIM2->CNT)*1.5;
		else
			rightfront_pid.error = (targetMecanum[0])-((TIM2->CNT<0xffffffff-TIM2->CNT) ? TIM2->CNT : 0xffffffff-TIM2->CNT)*1.5;		
		TIM2->CNT = 0;
		
		if(leftfront_pwm < 0)
			leftfront_pid.error = (targetMecanum[1])+((TIM3->CNT<0xffff-TIM3->CNT) ? TIM3->CNT : 0xffff-TIM3->CNT)*1.5;
		else
			leftfront_pid.error = (targetMecanum[1])-((TIM3->CNT<0xffff-TIM3->CNT) ? TIM3->CNT : 0xffff-TIM3->CNT)*1.5;		
		TIM3->CNT = 0;
		
		if(leftrear_pwm < 0)
			leftrear_pid.error = (targetMecanum[2])+((TIM4->CNT<0xffff-TIM4->CNT) ? TIM4->CNT : 0xffff-TIM4->CNT)*1.5;
		else
			leftrear_pid.error = (targetMecanum[2])-((TIM4->CNT<0xffff-TIM4->CNT) ? TIM4->CNT : 0xffff-TIM4->CNT)*1.5;		
		TIM4->CNT = 0;
		
		if(rightrear_pwm < 0)
			rightrear_pid.error = (targetMecanum[3])+((TIM5->CNT<0xffffffff-TIM5->CNT) ? TIM5->CNT : 0xffffffff-TIM5->CNT)*1.5;
		else
			rightrear_pid.error = (targetMecanum[3])-((TIM5->CNT<0xffffffff-TIM5->CNT) ? TIM5->CNT : 0xffffffff-TIM5->CNT)*1.5;		
		TIM5->CNT = 0;

		rightfront_pwm += incremental_pid(&rightfront_pid);
		leftfront_pwm  += incremental_pid(&leftfront_pid);
		leftrear_pwm   += incremental_pid(&leftrear_pid);
		rightrear_pwm  += incremental_pid(&rightrear_pid);

		Control_Dir(1, LIMIT(-99, rightfront_pwm, 99));
		Control_Dir(2, LIMIT(-99, leftfront_pwm,  99));
		Control_Dir(3, LIMIT(-99, leftrear_pwm,   99));
		Control_Dir(4, LIMIT(-99, rightrear_pwm,  99));
		
		OSTimeDlyHMSM(0u, 0u, 0u, 10u, OS_OPT_TIME_HMSM_STRICT, &err);
	}
}

static void AppTask_CCD(void *p_arg)
{
	OS_ERR  err;
	(void)p_arg;
	
	CCD_Init();
	OSTimeDlyHMSM(0u, 0u, 2u, 0u, OS_OPT_TIME_HMSM_STRICT, &err);
	
	for(;;)
	{
		CCD_Collect();
//		ccd_send_data(USART2, ccd1_data);
		ccd1_center = Find_Line(ccd1_data, ccd1_center, 3200);
		ccd2_center = Find_Line(ccd2_data, ccd2_center, 3200);
		
		if(ccd2_center > 68 || ccd2_center < 60)
			targetSpeedX =  (ccd2_center - 64) * 1.0;
		if(ccd1_center > 68 || ccd1_center < 60)
			targetSpeedW = -(ccd1_center - 64) * 3.0;
		
//		push(1,targetspeed);
//		push(2,leftrear_pid.error+100);
//		push(3,leftrear_pwm);
//		sendDataToScope();
		OSTimeDlyHMSM(0u, 0u, 0u, 20u, OS_OPT_TIME_HMSM_STRICT, &err);
	}
}

static void AppTask_Stepper(void *p_arg)
{
	OS_ERR  err;
	(void)p_arg;
	
	Stepper_Init();
	OSTimeDlyHMSM(0u, 0u, 2u, 0u, OS_OPT_TIME_HMSM_STRICT, &err);
	
	for(;;)
	{
		PEout(5)=1;
		OSTimeDlyHMSM(0u, 0u, 0u, set_time, OS_OPT_TIME_HMSM_STRICT, &err);
		PEout(5)=0;
		OSTimeDlyHMSM(0u, 0u, 0u, reset_time, OS_OPT_TIME_HMSM_STRICT, &err);
		soft_IRQ();
	}
}

/*
*********************************************************************************************************
*                                          AppTaskCreate()
*
* Description : Create application tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : AppTaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  AppTaskCreate (void)
{
    OS_ERR  os_err;
				 
	OSTaskCreate((OS_TCB      *)&AppTask_Receive_TCB,
                 (CPU_CHAR    *)"USART Receive",
                 (OS_TASK_PTR  ) AppTask_Receive,
                 (void        *) 0,
                 (OS_PRIO      ) AppTask_Receive_PRIO,
                 (CPU_STK     *)&AppTask_Receive_Stk[0],
                 (CPU_STK_SIZE ) AppTask_Receive_Stk[AppTask_Common_STK_SIZE / 10u],
                 (CPU_STK_SIZE ) AppTask_Common_STK_SIZE,
                 (OS_MSG_QTY   ) 0u,
                 (OS_TICK      ) 0u,
                 (void        *) 0,
                 (OS_OPT       )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                 (OS_ERR      *)&os_err);
				 
	OSTaskCreate((OS_TCB      *)&AppTask_USART_TCB,
                 (CPU_CHAR    *)"USART Send",
                 (OS_TASK_PTR  ) AppTask_USART,
                 (void        *) 0,
                 (OS_PRIO      ) AppTask_USART_PRIO,
                 (CPU_STK     *)&AppTask_USART_Stk[0],
                 (CPU_STK_SIZE ) AppTask_USART_Stk[AppTask_Common_STK_SIZE / 10u],
                 (CPU_STK_SIZE ) AppTask_Common_STK_SIZE,
                 (OS_MSG_QTY   ) 0u,
                 (OS_TICK      ) 0u,
                 (void        *) 0,
                 (OS_OPT       )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                 (OS_ERR      *)&os_err);
				 
	OSTaskCreate((OS_TCB      *)&AppTask_Mecanum_TCB,
                 (CPU_CHAR    *)"Mecanum Control",
                 (OS_TASK_PTR  ) AppTask_Mecanum,
                 (void        *) 0,
                 (OS_PRIO      ) AppTask_Mecanum_PRIO,
                 (CPU_STK     *)&AppTask_Mecanum_Stk[0],
                 (CPU_STK_SIZE ) AppTask_Mecanum_Stk[AppTask_Common_STK_SIZE / 10u],
                 (CPU_STK_SIZE ) AppTask_Common_STK_SIZE,
                 (OS_MSG_QTY   ) 0u,
                 (OS_TICK      ) 0u,
                 (void        *) 0,
                 (OS_OPT       )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                 (OS_ERR      *)&os_err);
				 
	OSTaskCreate((OS_TCB      *)&AppTask_CCD_TCB,
                 (CPU_CHAR    *)"CCD Read",
                 (OS_TASK_PTR  ) AppTask_CCD,
                 (void        *) 0,
                 (OS_PRIO      ) AppTask_CCD_PRIO,
                 (CPU_STK     *)&AppTask_CCD_Stk[0],
                 (CPU_STK_SIZE ) AppTask_CCD_Stk[AppTask_Common_STK_SIZE / 10u],
                 (CPU_STK_SIZE ) AppTask_Common_STK_SIZE,
                 (OS_MSG_QTY   ) 0u,
                 (OS_TICK      ) 0u,
                 (void        *) 0,
                 (OS_OPT       )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                 (OS_ERR      *)&os_err);
				 
	OSTaskCreate((OS_TCB      *)&AppTask_Stepper_TCB,
                 (CPU_CHAR    *)"Stepper Control",
                 (OS_TASK_PTR  ) AppTask_Stepper,
                 (void        *) 0,
                 (OS_PRIO      ) AppTask_Stepper_PRIO,
                 (CPU_STK     *)&AppTask_Stepper_Stk[0],
                 (CPU_STK_SIZE ) AppTask_Stepper_Stk[4096u / 10u],
                 (CPU_STK_SIZE ) 4096u,
                 (OS_MSG_QTY   ) 0u,
                 (OS_TICK      ) 0u,
                 (void        *) 0,
                 (OS_OPT       )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                 (OS_ERR      *)&os_err);
				 
}

/*
*********************************************************************************************************
*                                          AppObjCreate()
*
* Description : Create application kernel objects tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : AppTaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  AppObjCreate (void)
{

}

/*
*********************************************************************************************************
*	函 数 名: DispTaskInfo
*	功能说明: 将uCOS-III任务信息通过串口打印出来
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/

static void DispTaskInfo(void)
{
	OS_TCB      *p_tcb;	        /* 定义一个任务控制块指针, TCB = TASK CONTROL BLOCK */
	float CPU = 0.0f;
	CPU_SR_ALLOC();

	CPU_CRITICAL_ENTER();
    p_tcb = OSTaskDbgListPtr;
    CPU_CRITICAL_EXIT();
	
	/* 打印标题 */
	printf("==============================================\r\n");
	printf(" 优先级 使用栈 剩余栈 百分比 利用率   任务名\r\n");
	printf("  Prio   Used   Free   Per    CPU    Taskname\r\n");

	/* 遍历任务控制块列表(TCB list)，打印所有的任务的优先级和名称 */
	while (p_tcb != (OS_TCB *)0)
	{
		CPU = (float)p_tcb->CPUUsage / 100;
		printf("   %2d  %5d  %5d    %02d%%  %5.2f%%   %s\r\n",
		p_tcb->Prio,
		p_tcb->StkUsed,
		p_tcb->StkFree,
		(p_tcb->StkUsed * 100) / (p_tcb->StkUsed + p_tcb->StkFree),
		CPU,
		p_tcb->NamePtr);
	 	
		CPU_CRITICAL_ENTER();
        p_tcb = p_tcb->DbgNextPtr;
        CPU_CRITICAL_EXIT();
	}
	printf("\r\n");
}

static void TestLED(void)
{
	OS_ERR  err;
	
	OSTimeDlyHMSM(0u, 0u, 0u, 500u, OS_OPT_TIME_HMSM_STRICT, &err);
	LED1 = 0;

	OSTimeDlyHMSM(0u, 0u, 0u, 500u, OS_OPT_TIME_HMSM_STRICT, &err);
	LED1 = 1;
}
