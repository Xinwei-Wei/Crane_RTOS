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
#define		AppTask_Control_PRIO		9u
#define		AppTask_Receive_PRIO		7u
#define		AppTask_USART_PRIO			8u
#define		AppTask_Mecanum_PRIO		6u
#define		AppTask_CCD2_PRIO			5u
#define		AppTask_CCD1_PRIO			4u
#define		AppTask_Bottom_Stepper_PRIO	10u
#define		AppTask_RL_Stepper_PRIO		11u
#define		AppTask_UD_Stepper_PRIO		12u

/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

/// 系统变量声明
static  OS_TCB		AppTaskStartTCB;
static  CPU_STK		AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE];

/// 任务变量声明
static  OS_TCB		AppTask_Control_TCB;
static  CPU_STK		AppTask_Control_Stk[AppTask_Common_STK_SIZE];

static  OS_TCB		AppTask_Receive_TCB;
static  CPU_STK		AppTask_Receive_Stk[AppTask_Common_STK_SIZE];

static  OS_TCB		AppTask_USART_TCB;
static  CPU_STK		AppTask_USART_Stk[AppTask_Common_STK_SIZE];

static  OS_TCB		AppTask_Mecanum_TCB;
static  CPU_STK		AppTask_Mecanum_Stk[AppTask_Common_STK_SIZE];

static  OS_TCB		AppTask_CCD1_TCB;
static  CPU_STK		AppTask_CCD1_Stk[AppTask_Common_STK_SIZE];

static  OS_TCB		AppTask_CCD2_TCB;
static  CPU_STK		AppTask_CCD2_Stk[AppTask_Common_STK_SIZE];

static  OS_TCB		AppTask_Bottom_Stepper_TCB;
static  CPU_STK		AppTask_Bottom_Stepper_Stk[AppTask_Common_STK_SIZE];

static  OS_TCB		AppTask_RL_Stepper_TCB;
static  CPU_STK		AppTask_RL_Stepper_Stk[AppTask_Common_STK_SIZE];

static  OS_TCB		AppTask_UD_Stepper_TCB;
static  CPU_STK		AppTask_UD_Stepper_Stk[AppTask_Common_STK_SIZE];


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
int bottom_stepper_judge = 0, RL_stepper_judge = 0, UD_stepper_judge = 0, stepper_judge = 0;
int control_step = 1;
int angle[8]={0,0,180,60,240,120,300,0};
int bottom_turn_judge = 1;
int stop_judge = 0, USART_judge = 0, slow_down_judge = 0, guess_judge[6] = 0;
int target_center1 = 68, target_center2 = 64;
float CCD1_p = 1, CCD2_p = 3;
int a = 30;
int work_times = 0;
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
static	void	AppTask_Bottom_Stepper	(void *p_arg);
static	void	AppTask_RL_Stepper		(void *p_arg);
static	void	AppTask_UD_Stepper		(void *p_arg);
static	void	AppTask_Control			(void *p_arg);

/// 用户函数声明
static	void	DispTaskInfo(void);
static	void	TestLED(void);
void	Periph_Init	(void);
static 	void 	grab_milk(int a);
static 	void 	put_down_milk(int a, int b);

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
	OSTaskSuspend(&AppTask_Receive_TCB, &err);
	
	for(;;)
	{
		if(USART_GetFlagStatus(USART1 , USART_FLAG_RXNE) != RESET)
		{
			if(Read_Bit() == 0xff)
			{
				printf("receive 0xff\r\n");
				for(rev_num=0; rev_num<1; rev_num++)
					rev[rev_num] = Read_Bit()-48;
				printf("receive %d\r\n",rev[0]);
				if(Read_Bit() == 0xee)
				{
					printf("receive 0xee\r\n");
					USART_judge = 1;
				}
			}
			
//			targetSpeedY = rev[2]*1000 + rev[3]*100 + rev[4]*10 + rev[5];
//			if(rev[1] == 0)targetSpeedY*=-1;
//			printf("%f\r\n",targetSpeedY);
//			if(RL_stepper_turn(targetSpeedY))
//				OSTaskResume(&AppTask_Stepper_TCB, &err);
//			if(rev[0] == 0)
//				put_down_milk(rev[1]);
//			else if(rev[0] == 1)
//				grab_milk(rev[1]);
			
			
//			leftfront_pid.kp = rev[3] + rev[4]/10.0 + rev[5]/100.0;
//			leftfront_pid.ki = rev[6] + rev[7]/10.0 + rev[8]/100.0;
//			leftfront_pid.kd = rev[9] + rev[10]/10.0 + rev[11]/100.0;
		}
        OSTimeDlyHMSM(0u, 0u, 0u, 100u, OS_OPT_TIME_HMSM_STRICT, &err);
	}
}

static void	AppTask_Control(void *p_arg)
{
	OS_ERR  err;
	(void)p_arg;
	static int high = 1;
	int tem_angle = 0;
	OSTimeDlyHMSM(0u, 0u, 5u, 0u, OS_OPT_TIME_HMSM_STRICT, &err);
	
	
	
		
	ccd2_center = 64; //根据初始情况修改
	OSTaskResume(&AppTask_CCD2_TCB, &err);
	OSTaskResume(&AppTask_CCD1_TCB, &err);
	targetSpeedY = -35;
	angle[0] = 0;
	angle[7] = 0;

	for(;;)
	{	
		int i = 0, j = 0;
		if(slow_down_judge)
		{
			targetSpeedY = 30;
			target_center1 = 66;
			//OSTaskSuspend(&AppTask_CCD1_TCB, &err);
		}
		if(stop_judge)
		{
			work_times++;
			if(work_times<=6)
			{
				CCD2_p = 1;
				stop_judge = 0;
				targetSpeedY = 0;
				//printf("stop\r\n");
				
				if(angle[0] != 300){
					OSTaskResume(&AppTask_CCD1_TCB, &err);
					OSTimeDlyHMSM(0u, 0u, 1u, 0u, OS_OPT_TIME_HMSM_STRICT, &err);
					while(targetSpeedW)
						OSTimeDlyHMSM(0u, 0u, 0u, 10u, OS_OPT_TIME_HMSM_STRICT, &err);
					OSTaskSuspend(&AppTask_CCD1_TCB, &err);
				}
				
				OSTaskResume(&AppTask_Receive_TCB, &err);				
				while(!USART_judge){
					OSTimeDlyHMSM(0u, 0u, 0u, 10u, OS_OPT_TIME_HMSM_STRICT, &err);
					i++;					
					if(i > 500){
						guess_judge[work_times-1] = 1;
						break;
					}
				}
				OSTaskSuspend(&AppTask_Receive_TCB, &err);
				USART_judge = 0;		
								
				target_center2 = a;
				a = 8;
				OSTimeDlyHMSM(0u, 0u, 0u, 50u, OS_OPT_TIME_HMSM_STRICT, &err);
				for(;;){
					if(!targetSpeedX){
						OSTimeDlyHMSM(0u, 0u, 0u, 100u, OS_OPT_TIME_HMSM_STRICT, &err);
						if(!targetSpeedX)
							break;
					}
					OSTimeDlyHMSM(0u, 0u, 0u, 10u, OS_OPT_TIME_HMSM_STRICT, &err);
				}
				
				printf("grab\r\n");
				if(!guess_judge[work_times-1]){
					angle[rev[0]] = angle[0];
				}
				grab_milk(high);
				target_center2 = 64;
				OSTimeDlyHMSM(0u, 0u, 1u, 0u, OS_OPT_TIME_HMSM_STRICT, &err);
				if(work_times == 3){
					bottom_stepper_turn(-60);
					OSTaskResume(&AppTask_Bottom_Stepper_TCB, &err);
					angle[0] -= 60;
					targetSpeedY = 30;
				}
				else if(work_times == 5){
					bottom_stepper_turn(120);
					OSTaskResume(&AppTask_Bottom_Stepper_TCB, &err);
					angle[0] -= 240;
					targetSpeedY = 30;
				}
				else if(work_times ==6){
					
					a = 105;
					printf("start_turn\r\n");
					//转90度弯
					OSTaskSuspend(&AppTask_CCD2_TCB, &err);  //关闭线程CCD2
					targetSpeedX = 0;
					targetSpeedW = 60;
					OSTimeDlyHMSM(0u, 0u, 2u, 500u, OS_OPT_TIME_HMSM_STRICT, &err);
					CCD2_p = 1;
					CCD1_p = 3;
					CCD1_Collect();
					while(!Find_Line_first(ccd1_data, THRESHOLD))
					{
						CCD1_Collect();
						OSTimeDlyHMSM(0u, 0u, 0u, 5u, OS_OPT_TIME_HMSM_STRICT, &err);
					}
					
					OSTaskResume(&AppTask_CCD1_TCB, &err);
					targetSpeedY = 50;
					OSTimeDlyHMSM(0u, 0u, 1u, 0u, OS_OPT_TIME_HMSM_STRICT, &err);
					OSTaskResume(&AppTask_CCD2_TCB, &err);

					angle[0] = 0;
					bottom_stepper_turn(angle[1]+120);
					OSTaskResume(&AppTask_Bottom_Stepper_TCB, &err);
					while(UD_stepper_judge)
						OSTimeDlyHMSM(0u, 0u, 0u, 5u, OS_OPT_TIME_HMSM_STRICT, &err);
					targetSpeedY = 60;
					UD_stepper_turn(TWO_FLOAT_HIGH+UD);
					OSTaskResume(&AppTask_UD_Stepper_TCB, &err);
					while(UD_stepper_judge)
						OSTimeDlyHMSM(0u, 0u, 0u, 5u, OS_OPT_TIME_HMSM_STRICT, &err);
					RL_stepper_turn(RIGHT_TO_LEFT);
					OSTaskResume(&AppTask_RL_Stepper_TCB, &err);
					while(RL_stepper_judge)
						OSTimeDlyHMSM(0u, 0u, 0u, 5u, OS_OPT_TIME_HMSM_STRICT, &err);	
					UD_stepper_turn(-UD);
					OSTaskResume(&AppTask_UD_Stepper_TCB, &err);
				}
				else{
					bottom_stepper_turn(120);
					OSTaskResume(&AppTask_Bottom_Stepper_TCB, &err);
					angle[0] += 120;
					targetSpeedY = 30;
				}	
			}
			else
			{
				stop_judge = 0;
				targetSpeedY = 0;
				printf("stop\r\n");
				
				put_down_milk(((angle[2*(work_times-6)-1]/60)%2+1) , 1);
				//取余-360
				tem_angle = angle[2*(work_times-6)]-angle[2*(work_times-6)-1];
				if(tem_angle > 180)tem_angle -= 360;
				if(tem_angle < -180)tem_angle += 360;
				bottom_stepper_turn(tem_angle);
				OSTaskResume(&AppTask_Bottom_Stepper_TCB, &err);
				while(bottom_stepper_judge)
					OSTimeDlyHMSM(0u, 0u, 0u, 5u, OS_OPT_TIME_HMSM_STRICT, &err);
				
				//保证上次抓完
				while(UD_stepper_judge)
					OSTimeDlyHMSM(0u, 0u, 0u, 5u, OS_OPT_TIME_HMSM_STRICT, &err);
				put_down_milk(((angle[2*(work_times-6)]/60)%2+1) , 2);
				
				target_center2 = 64;
				OSTimeDlyHMSM(0u, 0u, 1u, 0u, OS_OPT_TIME_HMSM_STRICT, &err);
				
				if(work_times<9)
					targetSpeedY = 20;
				
				bottom_stepper_turn(angle[2*(work_times-6)+1]-angle[2*(work_times-6)]);
				OSTaskResume(&AppTask_Bottom_Stepper_TCB, &err);
				
			}
//			else if(rev[0] == 0)
//			{
//				if(rev[1] == 9);
//				else
//				{
//					put_down_milk(((angle[2*rev[1]-1]/60)%2+1) , 1);
//					bottom_stepper_turn(angle[2*rev[1]]-angle[2*rev[1]-1]);
//					OSTaskResume(&AppTask_Bottom_Stepper_TCB, &err);
//					while(bottom_stepper_judge)
//						OSTimeDlyHMSM(0u, 0u, 0u, 5u, OS_OPT_TIME_HMSM_STRICT, &err);
//					
//					put_down_milk(((angle[2*rev[1]]/60)%2+1) , 2);
//					
//					bottom_stepper_turn(angle[2*rev[1]+1]-angle[2*rev[1]]);
//					OSTaskResume(&AppTask_Bottom_Stepper_TCB, &err);
//					while(bottom_stepper_judge)
//						OSTimeDlyHMSM(0u, 0u, 0u, 5u, OS_OPT_TIME_HMSM_STRICT, &err);
//					
//					targetSpeedY = 20;
//				}
//			}
			target_center2 = 64;
//			else if(rev[0] == 3)
//			{				
//				RL_stepper_turn(rev[1]*50);
//				OSTaskResume(&AppTask_RL_Stepper_TCB, &err);
//				while(RL_stepper_judge)
//					OSTimeDlyHMSM(0u, 0u, 0u, 5u, OS_OPT_TIME_HMSM_STRICT, &err);
//			}
//			else if(rev[0] == 2)
//			{	
////				if(rev[1] == 0)
////					bottom_stepper_turn(-60);
////				else
////					bottom_stepper_turn(60);
//				bottom_stepper_turn(-rev[1]*10);
//				OSTaskResume(&AppTask_Bottom_Stepper_TCB, &err);
//				while(bottom_stepper_judge)
//					OSTimeDlyHMSM(0u, 0u, 0u, 5u, OS_OPT_TIME_HMSM_STRICT, &err);
//			}
//			else if(rev[0] == 4)
//			{			
//				UD_stepper_turn(rev[1]*50);
//				OSTaskResume(&AppTask_UD_Stepper_TCB, &err);
//				while(UD_stepper_judge)
//					OSTimeDlyHMSM(0u, 0u, 0u, 5u, OS_OPT_TIME_HMSM_STRICT, &err);
//			}
//			else if(rev[0] == 5)
//			{			
//				set_servo_angle(5*rev[1]);
//			}

			
		}
		
		OSTimeDlyHMSM(0u, 0u, 0u, 10u, OS_OPT_TIME_HMSM_STRICT, &err);
	}
	
	
}


static void AppTask_USART(void *p_arg)
{
	OS_ERR  err;
	(void)p_arg;
	
	OSTimeDlyHMSM(0u, 0u, 1u, 0u, OS_OPT_TIME_HMSM_STRICT, &err);
	
	
	for(;;)
	{

		OSTimeDlyHMSM(0u, 0u, 2u, 0u, OS_OPT_TIME_HMSM_STRICT, &err);
	}
}

static void AppTask_Mecanum(void *p_arg)
{
	OS_ERR  err;
	(void)p_arg;
	
	targetSpeedY = 0;
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
		printf("x:%.2f y:%.2f w:%.2f\r\n",targetSpeedX, targetSpeedY, targetSpeedW);
		targetMecanum = moto_caculate(targetSpeedX, targetSpeedY, targetSpeedW);
		
		if(rightfront_pwm < 0)
			rightfront_pid.error = (targetMecanum[0])+((TIM2->CNT<0xffffffff-TIM2->CNT) ? TIM2->CNT : 0xffffffff-TIM2->CNT)*1.5;
		else
			rightfront_pid.error = (targetMecanum[0])-((TIM2->CNT<0xffffffff-TIM2->CNT) ? TIM2->CNT : 0xffffffff-TIM2->CNT)*1.5;	
		//printf("TIM2->CNT %d\r\n", TIM2->CNT);
		TIM2->CNT = 0;

		if(leftfront_pwm < 0)
			leftfront_pid.error = (targetMecanum[1])+((TIM3->CNT<0xffff-TIM3->CNT) ? TIM3->CNT : 0xffff-TIM3->CNT)*1.5;
		else
			leftfront_pid.error = (targetMecanum[1])-((TIM3->CNT<0xffff-TIM3->CNT) ? TIM3->CNT : 0xffff-TIM3->CNT)*1.5;	
		//printf("TIM3->CNT %d\r\n", TIM3->CNT);
		TIM3->CNT = 0;
		
		if(leftrear_pwm < 0)
			leftrear_pid.error = (targetMecanum[2])+((TIM4->CNT<0xffff-TIM4->CNT) ? TIM4->CNT : 0xffff-TIM4->CNT)*1.5;
		else
			leftrear_pid.error = (targetMecanum[2])-((TIM4->CNT<0xffff-TIM4->CNT) ? TIM4->CNT : 0xffff-TIM4->CNT)*1.5;	
		//printf("TIM4->CNT %d\r\n", TIM4->CNT);
		TIM4->CNT = 0;
		
		if(rightrear_pwm < 0)
			rightrear_pid.error = (targetMecanum[3])+((TIM5->CNT<0xffffffff-TIM5->CNT) ? TIM5->CNT : 0xffffffff-TIM5->CNT)*1.5;
		else
			rightrear_pid.error = (targetMecanum[3])-((TIM5->CNT<0xffffffff-TIM5->CNT) ? TIM5->CNT : 0xffffffff-TIM5->CNT)*1.5;	
		//printf("TIM5->CNT %d\r\n", TIM5->CNT);
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

static void AppTask_CCD2(void *p_arg)
{
	OS_ERR  err;
	(void)p_arg;
	CCD_Init();
	OSTaskSuspend(&AppTask_CCD2_TCB, &err);
	for(;;)
	{
		CCD2_Collect();
		//ccd_send_data(USART2, ccd2_data);

		ccd2_center = CCD2_find_Line(ccd2_center, THRESHOLD);
		printf("CCD2_center %d\r\n", ccd2_center);
		
		if(ccd2_center > target_center2 + 2 || ccd2_center < target_center2 - 2)
			targetSpeedX = (ccd2_center - target_center2) * CCD2_p;
		else targetSpeedX = 0;
		//printf("targetSpeedX %d\r\n",targetSpeedX);
		//printf("CCD2_error %d\r\n",ccd2_center - target_center);
		
//		push(1,targetspeed);
//		push(2,leftrear_pid.error+100);
//		push(3,leftrear_pwm);
//		sendDataToScope();
		OSTimeDlyHMSM(0u, 0u, 0u, 20u, OS_OPT_TIME_HMSM_STRICT, &err);
	}
}

static void AppTask_CCD1(void *p_arg)
{
	OS_ERR  err;
	(void)p_arg;
	OSTaskSuspend(&AppTask_CCD1_TCB, &err);
	for(;;)
	{
		CCD1_Collect();
		//ccd_send_data(USART2, ccd1_data);
		ccd1_center = CCD1_find_Line(ccd1_center, THRESHOLD);

		if(ccd1_center > target_center2+2 || ccd1_center < target_center2-2)
			targetSpeedW = -(ccd1_center - target_center1) * CCD1_p;
		else targetSpeedW = 0;
		printf("CCD1_center %d\r\n",ccd1_center);
		
//		push(1,targetspeed);
//		push(2,leftrear_pid.error+100);
//		push(3,leftrear_pwm);
//		sendDataToScope();
		OSTimeDlyHMSM(0u, 0u, 0u, 20u, OS_OPT_TIME_HMSM_STRICT, &err);
	}
}


static void AppTask_Bottom_Stepper(void *p_arg)
{
	OS_ERR  err;
	(void)p_arg;
	Stepper_EN_Init();
	Servo_Init();
	OSTimeDlyHMSM(0u, 0u, 5u, 0u, OS_OPT_TIME_HMSM_STRICT, &err);
	GPIO_SetBits(GPIOB, GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12);
	
	Stepper_Init();
	OSTaskSuspend(&AppTask_Bottom_Stepper_TCB, &err);
	
	for(;;)
	{
		if(bottom_stepper_judge)
			PEout(5)=1;
//		if(puzzer_)
//			PEout(9)=1;
		OSTimeDlyHMSM(0u, 0u, 0u, 1u, OS_OPT_TIME_HMSM_STRICT, &err);
		PEout(5)=0;
		PEout(9)=0;
		OSTimeDlyHMSM(0u, 0u, 0u, 1u, OS_OPT_TIME_HMSM_STRICT, &err);
		Bottom_Soft_IRQ();
		if(!bottom_stepper_judge)
		{
			OSTaskSuspend(&AppTask_Bottom_Stepper_TCB, &err);
		}
	}
}

static void AppTask_RL_Stepper(void *p_arg)
{
	OS_ERR  err;
	(void)p_arg;
	OSTimeDlyHMSM(0u, 0u, 2u, 0u, OS_OPT_TIME_HMSM_STRICT, &err);
	
	Stepper_Init();
	OSTaskSuspend(&AppTask_RL_Stepper_TCB, &err);
	
	for(;;)
	{
		if(RL_stepper_judge)
			PDout(5)=1;
		OSTimeDlyHMSM(0u, 0u, 0u, 2u, OS_OPT_TIME_HMSM_STRICT, &err);
		PDout(5)=0;
		OSTimeDlyHMSM(0u, 0u, 0u, 2u, OS_OPT_TIME_HMSM_STRICT, &err);
		RL_Soft_IRQ();
		if(!RL_stepper_judge)
		{
			OSTaskSuspend(&AppTask_RL_Stepper_TCB, &err);
		}
	}
}

static void AppTask_UD_Stepper(void *p_arg)
{
	OS_ERR  err;
	(void)p_arg;
	OSTimeDlyHMSM(0u, 0u, 2u, 0u, OS_OPT_TIME_HMSM_STRICT, &err);
	
	OSTaskSuspend(&AppTask_UD_Stepper_TCB, &err);
	
	for(;;)
	{
		if(UD_stepper_judge)
			PDout(7)=1;
		OSTimeDlyHMSM(0u, 0u, 0u, 1u, OS_OPT_TIME_HMSM_STRICT, &err);
		PDout(7)=0;
		OSTimeDlyHMSM(0u, 0u, 0u, 1u, OS_OPT_TIME_HMSM_STRICT, &err);
		UD_Soft_IRQ();
		if(!UD_stepper_judge)
		{
			OSTaskSuspend(&AppTask_UD_Stepper_TCB, &err);
		}
	}
}

//static void AppTask_Stepper(void *p_arg)
//{
//	
//}

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
				 
	OSTaskCreate((OS_TCB      *)&AppTask_CCD1_TCB,
                 (CPU_CHAR    *)"CCD1 Read",
                 (OS_TASK_PTR  ) AppTask_CCD1,
                 (void        *) 0,
                 (OS_PRIO      ) AppTask_CCD1_PRIO,
                 (CPU_STK     *)&AppTask_CCD1_Stk[0],
                 (CPU_STK_SIZE ) AppTask_CCD1_Stk[AppTask_Common_STK_SIZE / 10u],
                 (CPU_STK_SIZE ) AppTask_Common_STK_SIZE,
                 (OS_MSG_QTY   ) 0u,
                 (OS_TICK      ) 0u,
                 (void        *) 0,
                 (OS_OPT       )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                 (OS_ERR      *)&os_err);
				 
	OSTaskCreate((OS_TCB      *)&AppTask_CCD2_TCB,
                 (CPU_CHAR    *)"CCD2 Read",
                 (OS_TASK_PTR  ) AppTask_CCD2,
                 (void        *) 0,
                 (OS_PRIO      ) AppTask_CCD2_PRIO,
                 (CPU_STK     *)&AppTask_CCD2_Stk[0],
                 (CPU_STK_SIZE ) AppTask_CCD2_Stk[AppTask_Common_STK_SIZE / 10u],
                 (CPU_STK_SIZE ) AppTask_Common_STK_SIZE,
                 (OS_MSG_QTY   ) 0u,
                 (OS_TICK      ) 0u,
                 (void        *) 0,
                 (OS_OPT       )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                 (OS_ERR      *)&os_err);
				 
	OSTaskCreate((OS_TCB      *)&AppTask_Bottom_Stepper_TCB,
                 (CPU_CHAR    *)"Bottom_Stepper Control",
                 (OS_TASK_PTR  ) AppTask_Bottom_Stepper,
                 (void        *) 0,
                 (OS_PRIO      ) AppTask_Bottom_Stepper_PRIO,
                 (CPU_STK     *)&AppTask_Bottom_Stepper_Stk[0],
                 (CPU_STK_SIZE ) AppTask_Bottom_Stepper_Stk[AppTask_Common_STK_SIZE / 10u],
                 (CPU_STK_SIZE ) AppTask_Common_STK_SIZE,
                 (OS_MSG_QTY   ) 0u,
                 (OS_TICK      ) 0u,
                 (void        *) 0,
                 (OS_OPT       )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                 (OS_ERR      *)&os_err);
				 
	OSTaskCreate((OS_TCB      *)&AppTask_Control_TCB,
                 (CPU_CHAR    *)"Control",
                 (OS_TASK_PTR  ) AppTask_Control,
                 (void        *) 0,
                 (OS_PRIO      ) AppTask_Control_PRIO,
                 (CPU_STK     *)&AppTask_Control_Stk[0],
                 (CPU_STK_SIZE ) AppTask_Control_Stk[AppTask_Common_STK_SIZE / 10u],
                 (CPU_STK_SIZE ) AppTask_Common_STK_SIZE,
                 (OS_MSG_QTY   ) 0u,
                 (OS_TICK      ) 0u,
                 (void        *) 0,
                 (OS_OPT       )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                 (OS_ERR      *)&os_err);
				 
	OSTaskCreate((OS_TCB      *)&AppTask_RL_Stepper_TCB,
                 (CPU_CHAR    *)"RL_Stepper Control",
                 (OS_TASK_PTR  ) AppTask_RL_Stepper,
                 (void        *) 0,
                 (OS_PRIO      ) AppTask_RL_Stepper_PRIO,
                 (CPU_STK     *)&AppTask_RL_Stepper_Stk[0],
                 (CPU_STK_SIZE ) AppTask_RL_Stepper_Stk[AppTask_Common_STK_SIZE / 10u],
                 (CPU_STK_SIZE ) AppTask_Common_STK_SIZE,
                 (OS_MSG_QTY   ) 0u,
                 (OS_TICK      ) 0u,
                 (void        *) 0,
                 (OS_OPT       )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                 (OS_ERR      *)&os_err);
				 
	OSTaskCreate((OS_TCB      *)&AppTask_UD_Stepper_TCB,
                 (CPU_CHAR    *)"UD_Stepper Control",
                 (OS_TASK_PTR  ) AppTask_UD_Stepper,
                 (void        *) 0,
                 (OS_PRIO      ) AppTask_UD_Stepper_PRIO,
                 (CPU_STK     *)&AppTask_UD_Stepper_Stk[0],
                 (CPU_STK_SIZE ) AppTask_UD_Stepper_Stk[AppTask_Common_STK_SIZE / 10u],
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

static void grab_milk(int a)
{
	OS_ERR  err;
	bottom_turn_judge = 0;
	if(a == 1)
	{
		servo_close();
		OSTimeDlyHMSM(0u, 0u, 0u, 500u, OS_OPT_TIME_HMSM_STRICT, &err);
		UD_stepper_turn(ONE_FLOAT_HIGH + UD);
		OSTaskResume(&AppTask_UD_Stepper_TCB, &err);
		while(UD_stepper_judge)
			OSTimeDlyHMSM(0u, 0u, 0u, 5u, OS_OPT_TIME_HMSM_STRICT, &err);
		while(bottom_stepper_judge)
			OSTimeDlyHMSM(0u, 0u, 0u, 5u, OS_OPT_TIME_HMSM_STRICT, &err);
		RL_stepper_turn(RL1);
		OSTaskResume(&AppTask_RL_Stepper_TCB, &err);
		while(RL_stepper_judge)
			OSTimeDlyHMSM(0u, 0u, 0u, 5u, OS_OPT_TIME_HMSM_STRICT, &err);
		OSTimeDlyHMSM(0u, 0u, 0u, 500u, OS_OPT_TIME_HMSM_STRICT, &err);
		UD_stepper_turn(-UD);
		OSTaskResume(&AppTask_UD_Stepper_TCB, &err);
		while(UD_stepper_judge)
			OSTimeDlyHMSM(0u, 0u, 0u, 5u, OS_OPT_TIME_HMSM_STRICT, &err);
		servo_open();
		OSTimeDlyHMSM(0u, 0u, 0u, 500u, OS_OPT_TIME_HMSM_STRICT, &err);
//		UD_stepper_turn(UD);
//		OSTaskResume(&AppTask_UD_Stepper_TCB, &err);
//		while(UD_stepper_judge)
//			OSTimeDlyHMSM(0u, 0u, 0u, 5u, OS_OPT_TIME_HMSM_STRICT, &err);
		RL_stepper_turn(-RL1);
		OSTaskResume(&AppTask_RL_Stepper_TCB, &err);
		while(RL_stepper_judge)
			OSTimeDlyHMSM(0u, 0u, 0u, 5u, OS_OPT_TIME_HMSM_STRICT, &err);
		UD_stepper_turn(-ONE_FLOAT_HIGH);
		OSTaskResume(&AppTask_UD_Stepper_TCB, &err);
//		while(UD_stepper_judge)
//			OSTimeDlyHMSM(0u, 0u, 0u, 5u, OS_OPT_TIME_HMSM_STRICT, &err);		
	}
	else if(a == 2)
	{
		servo_close();
		OSTimeDlyHMSM(0u, 0u, 0u, 500u, OS_OPT_TIME_HMSM_STRICT, &err);
		UD_stepper_turn(TWO_FLOAT_HIGH + UD);
		OSTaskResume(&AppTask_UD_Stepper_TCB, &err);
		while(UD_stepper_judge)
			OSTimeDlyHMSM(0u, 0u, 0u, 5u, OS_OPT_TIME_HMSM_STRICT, &err);
		while(bottom_stepper_judge)
			OSTimeDlyHMSM(0u, 0u, 0u, 5u, OS_OPT_TIME_HMSM_STRICT, &err);
		RL_stepper_turn(RL1);
		OSTaskResume(&AppTask_RL_Stepper_TCB, &err);
		while(RL_stepper_judge)
			OSTimeDlyHMSM(0u, 0u, 0u, 5u, OS_OPT_TIME_HMSM_STRICT, &err);
		UD_stepper_turn(-UD);
		OSTaskResume(&AppTask_UD_Stepper_TCB, &err);
		while(UD_stepper_judge)
			OSTimeDlyHMSM(0u, 0u, 0u, 5u, OS_OPT_TIME_HMSM_STRICT, &err);
		servo_open();
		OSTimeDlyHMSM(0u, 0u, 0u, 500u, OS_OPT_TIME_HMSM_STRICT, &err);
//		UD_stepper_turn(500);
//		OSTaskResume(&AppTask_UD_Stepper_TCB, &err);
//		while(UD_stepper_judge)
//			OSTimeDlyHMSM(0u, 0u, 0u, 5u, OS_OPT_TIME_HMSM_STRICT, &err);
		RL_stepper_turn(-RL1);
		OSTaskResume(&AppTask_RL_Stepper_TCB, &err);
		while(RL_stepper_judge)
			OSTimeDlyHMSM(0u, 0u, 0u, 5u, OS_OPT_TIME_HMSM_STRICT, &err);
		UD_stepper_turn(-TWO_FLOAT_HIGH);
		OSTaskResume(&AppTask_UD_Stepper_TCB, &err);
//		while(UD_stepper_judge)
//			OSTimeDlyHMSM(0u, 0u, 0u, 5u, OS_OPT_TIME_HMSM_STRICT, &err);
	}
	bottom_turn_judge = 1;
}

static void put_down_milk(int a, int b)//初始高度5000
{
	OS_ERR  err;
	bottom_turn_judge = 0;
	b -= 1;
	if(a == 1)
	{
		UD_stepper_turn(ONE_FLOAT_HIGH - TWO_FLOAT_HIGH);
		OSTaskResume(&AppTask_UD_Stepper_TCB, &err);
		while(UD_stepper_judge)
			OSTimeDlyHMSM(0u, 0u, 0u, 5u, OS_OPT_TIME_HMSM_STRICT, &err);
		RL_stepper_turn(-RL0);
		OSTaskResume(&AppTask_RL_Stepper_TCB, &err);
		while(RL_stepper_judge)
			OSTimeDlyHMSM(0u, 0u, 0u, 5u, OS_OPT_TIME_HMSM_STRICT, &err);
//		UD_stepper_turn(-750);
//		OSTaskResume(&AppTask_UD_Stepper_TCB, &err);
//		while(UD_stepper_judge)
//			OSTimeDlyHMSM(0u, 0u, 0u, 5u, OS_OPT_TIME_HMSM_STRICT, &err);
		servo_close();
		OSTimeDlyHMSM(0u, 0u, 0u, 500u, OS_OPT_TIME_HMSM_STRICT, &err);
		UD_stepper_turn(UD);
		OSTaskResume(&AppTask_UD_Stepper_TCB, &err);
		while(UD_stepper_judge)
			OSTimeDlyHMSM(0u, 0u, 0u, 5u, OS_OPT_TIME_HMSM_STRICT, &err);
		RL_stepper_turn(RL0);
		OSTaskResume(&AppTask_RL_Stepper_TCB, &err);
		while(RL_stepper_judge)
			OSTimeDlyHMSM(0u, 0u, 0u, 5u, OS_OPT_TIME_HMSM_STRICT, &err);
		UD_stepper_turn(MILK_HIGH * b - (ONE_FLOAT_HIGH + UD));
		OSTaskResume(&AppTask_UD_Stepper_TCB, &err);
		while(UD_stepper_judge)
			OSTimeDlyHMSM(0u, 0u, 0u, 5u, OS_OPT_TIME_HMSM_STRICT, &err);
		servo_open();
		OSTimeDlyHMSM(0u, 0u, 0u, 500u, OS_OPT_TIME_HMSM_STRICT, &err);
		UD_stepper_turn(TWO_FLOAT_HIGH - MILK_HIGH * b);
		OSTaskResume(&AppTask_UD_Stepper_TCB, &err);
//		while(UD_stepper_judge)
//			OSTimeDlyHMSM(0u, 0u, 0u, 5u, OS_OPT_TIME_HMSM_STRICT, &err);
	}
	else if(a == 2)
	{
//		UD_stepper_turn(5000);
//		OSTaskResume(&AppTask_Stepper_TCB, &err);
//		while(UD_stepper_judge)
//			OSTimeDlyHMSM(0u, 0u, 0u, 5u, OS_OPT_TIME_HMSM_STRICT, &err);
		RL_stepper_turn(-RL0);
		OSTaskResume(&AppTask_RL_Stepper_TCB, &err);
		while(RL_stepper_judge)
			OSTimeDlyHMSM(0u, 0u, 0u, 5u, OS_OPT_TIME_HMSM_STRICT, &err);
		
//		UD_stepper_turn(UD);
//		OSTaskResume(&AppTask_UD_Stepper_TCB, &err);
//		while(UD_stepper_judge)
//			OSTimeDlyHMSM(0u, 0u, 0u, 5u, OS_OPT_TIME_HMSM_STRICT, &err);
		servo_close();
		OSTimeDlyHMSM(0u, 0u, 0u, 500u, OS_OPT_TIME_HMSM_STRICT, &err);
		
		UD_stepper_turn(UD);
		OSTaskResume(&AppTask_UD_Stepper_TCB, &err);
		while(UD_stepper_judge)
			OSTimeDlyHMSM(0u, 0u, 0u, 5u, OS_OPT_TIME_HMSM_STRICT, &err);
		
		RL_stepper_turn(RL0);
		OSTaskResume(&AppTask_RL_Stepper_TCB, &err);
		while(RL_stepper_judge)
			OSTimeDlyHMSM(0u, 0u, 0u, 5u, OS_OPT_TIME_HMSM_STRICT, &err);
		
		UD_stepper_turn(MILK_HIGH * b - (TWO_FLOAT_HIGH + UD));
		OSTaskResume(&AppTask_UD_Stepper_TCB, &err);
		while(UD_stepper_judge)
			OSTimeDlyHMSM(0u, 0u, 0u, 5u, OS_OPT_TIME_HMSM_STRICT, &err);
		servo_open();
		OSTimeDlyHMSM(0u, 0u, 0u, 500u, OS_OPT_TIME_HMSM_STRICT, &err);
		UD_stepper_turn(TWO_FLOAT_HIGH - MILK_HIGH * b);
		OSTaskResume(&AppTask_UD_Stepper_TCB, &err);
//		while(UD_stepper_judge)
//			OSTimeDlyHMSM(0u, 0u, 0u, 5u, OS_OPT_TIME_HMSM_STRICT, &err);
	}
	bottom_turn_judge = 1;
}


