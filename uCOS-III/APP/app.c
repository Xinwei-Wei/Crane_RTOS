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
#define		AppTask_LED_PRIO			4u
#define		AppTask_USART_PRIO			6u

/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

/// 系统变量声明
static  OS_TCB		AppTaskStartTCB;
static  CPU_STK		AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE];

/// 任务变量声明
static  OS_TCB		AppTask_LED_TCB;
static  CPU_STK		AppTask_LED_Stk[AppTask_Common_STK_SIZE];

static  OS_TCB		AppTask_USART_TCB;
static  CPU_STK		AppTask_USART_Stk[AppTask_Common_STK_SIZE];

/// 用户变量声明


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
static	void  	AppTask_LED				(void *p_arg);
static	void	AppTask_USART			(void *p_arg);

/// 用户函数声明
static	void	DispTaskInfo(void);
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
	LED_Init();
}

/*
*********************************************************************************************************
*
*                                          CUSTOMIZE TASKS
*
*********************************************************************************************************
*/

static void AppTask_LED(void *p_arg)
{
	OS_ERR  err;
	(void)p_arg;
	
	OSTimeDlyHMSM(0u, 0u, 0u, 500u, OS_OPT_TIME_HMSM_STRICT, &err);
	
	for(;;)
	{
		LED1 = 0;
        OSTimeDlyHMSM(0u, 0u, 0u, 500u, OS_OPT_TIME_HMSM_STRICT, &err);

		LED1 = 1;
        OSTimeDlyHMSM(0u, 0u, 0u, 500u, OS_OPT_TIME_HMSM_STRICT, &err);
	}
}

static void AppTask_USART(void *p_arg)
{
	OS_ERR  err;
	(void)p_arg;
	
	uart_init(115200);	// 初始化串口波特率为115200
	OSTimeDlyHMSM(0u, 0u, 1u, 0u, OS_OPT_TIME_HMSM_STRICT, &err);
	
	printf("Hello\r\n");
	
	while(1)
	{
		DispTaskInfo();
		OSTimeDlyHMSM(0u, 0u, 5u, 0u, OS_OPT_TIME_HMSM_STRICT, &err);
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
				 
	OSTaskCreate((OS_TCB      *)&AppTask_LED_TCB,
                 (CPU_CHAR    *)"Task LED",
                 (OS_TASK_PTR  ) AppTask_LED, 
                 (void        *) 0,
                 (OS_PRIO      ) AppTask_LED_PRIO,
                 (CPU_STK     *)&AppTask_LED_Stk[0],
                 (CPU_STK_SIZE ) AppTask_LED_Stk[AppTask_Common_STK_SIZE / 10u],
                 (CPU_STK_SIZE ) AppTask_Common_STK_SIZE,
                 (OS_MSG_QTY   ) 0u,
                 (OS_TICK      ) 0u,
                 (void        *) 0,
                 (OS_OPT       )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                 (OS_ERR      *)&os_err);
				 
	OSTaskCreate((OS_TCB      *)&AppTask_USART_TCB,
                 (CPU_CHAR    *)"Task USART",
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
		printf("   %2d  %5d  %5d   %02d%%   %5.2f%%   %s\r\n", 
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
}
