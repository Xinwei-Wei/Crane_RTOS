/*
*********************************************************************************************************
*
*	ģ������ : ������ģ�顣
*	�ļ����� : main.c
*	��    �� : V1.0
*	˵    �� : ��ʵ����ҪѧϰFreeRTOS����ֲ
*              ʵ��Ŀ�ģ�
*                 1. ѧϰFreeRTOS����ֲ��
*              ʵ�����ݣ�
*                 1. �����������ĸ�����
*                    vTaskTaskUserIF ����: �ӿ���Ϣ������������LED��˸	
*                    vTaskLED        ����: LED��˸
*                    vTaskMsgPro     ����: ��Ϣ��������������LED��˸
*                    vTaskStart      ����: ��������Ҳ����������ȼ�������������LED��˸
*              ע�����
*                 1. ��ʵ���Ƽ�ʹ�ô������SecureCRT��Ҫ�����ڴ�ӡЧ�������롣�������
*                    V5��������������С�
*                 2. ��ؽ��༭��������������TAB����Ϊ4���Ķ����ļ���Ҫ��������ʾ�����롣
*
*	�޸ļ�¼ :
*		�汾��    ����         ����            ˵��
*       V1.0    2016-03-15   Eric2013    1. ST�̼��⵽V1.5.0�汾
*                                        2. BSP������V1.2
*                                        3. FreeRTOS�汾V8.2.3
*
*	Copyright (C), 2016-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "includes.h"

/*
**********************************************************************************************************
											�������ȼ�����
**********************************************************************************************************
*/

#define		vTask_Control_PRIO			2
#define		vTask_Receive_PRIO			9
#define		vTask_USART_PRIO			1
#define		vTask_Mecanum_PRIO			4
#define		vTask_CCD2_PRIO				4
#define		vTask_CCD1_PRIO				4
#define		vTask_BottomStepper_PRIO	3
#define		vTask_RLStepper_PRIO		3
#define		vTask_UDStepper_PRIO		3

/*
**********************************************************************************************************
											����������
**********************************************************************************************************
*/

static	void	AppTaskCreate		(void);
static	void	vTask_Receive		(void *pvParameters);
static	void	vTask_USART			(void *pvParameters);
static	void	vTask_Mecanum		(void *pvParameters);
static	void	vTask_CCD2			(void *pvParameters);
static	void	vTask_CCD1			(void *pvParameters);
static	void	vTask_BottomStepper	(void *pvParameters);
static	void	vTask_RLStepper		(void *pvParameters);
static	void	vTask_UDStepper		(void *pvParameters);
static	void	vTask_Control		(void *pvParameters);

/*
**********************************************************************************************************
											�����������
**********************************************************************************************************
*/

static	TaskHandle_t	xHandleTask_Receive			= NULL;
static	TaskHandle_t	xHandleTask_USART			= NULL;
static	TaskHandle_t	xHandleTask_Mecanum			= NULL;
static	TaskHandle_t	xHandleTask_CCD2			= NULL;
static	TaskHandle_t	xHandleTask_CCD1			= NULL;
static	TaskHandle_t	xHandleTask_BottomStepper	= NULL;
static	TaskHandle_t	xHandleTask_RLStepper		= NULL;
static	TaskHandle_t	xHandleTask_UDStepper		= NULL;
static	TaskHandle_t	xHandleTask_Control			= NULL;

/*
**********************************************************************************************************
											�û���������
**********************************************************************************************************
*/

void	Periph_Init	(void);
static	void	TestLED(void);
static 	void 	GrabMilk(int a);
static 	void 	PutDownMilk(int a, int b);

/*
**********************************************************************************************************
											�û���������
**********************************************************************************************************
*/

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
int angle[8]={0,-1,-1,-1,-1,-1,-1,0};
int bottom_turn_judge = 1;
int stop_judge = 0, USART_judge = 0, slow_down_judge = 0, guess_judge[6] = {0};
int target_center1 = 68, target_center2 = 64;
float CCD1_p = 1, CCD2_p = 3;
int a = 37;
int work_times = -1;
int guess_angle[6] = {2,3,5,1,4,6};

/*
*********************************************************************************************************
*	�� �� ��: main
*	����˵��: ��׼c������ڡ�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/

int main(void)
{
	/* 
	  ����������ǰ��Ϊ�˷�ֹ��ʼ��STM32����ʱ���жϷ������ִ�У������ֹȫ���ж�(����NMI��HardFault)��
	  �������ĺô��ǣ�
	  1. ��ִֹ�е��жϷ����������FreeRTOS��API������
	  2. ��֤ϵͳ�������������ܱ���ж�Ӱ�졣
	  3. �����Ƿ�ر�ȫ���жϣ���Ҹ����Լ���ʵ��������ü��ɡ�
	  ����ֲ�ļ�port.c�еĺ���prvStartFirstTask�л����¿���ȫ���жϡ�ͨ��ָ��cpsie i������__set_PRIMASK(1)
	  ��cpsie i�ǵ�Ч�ġ�
     */
	__set_PRIMASK(1);
	
	/* Ӳ����ʼ�� */
	Periph_Init();
	
	/* �������� */
	AppTaskCreate();
	
    /* �������ȣ���ʼִ������ */
    vTaskStartScheduler();

	/* 
	  ���ϵͳ���������ǲ������е�����ģ����е����Ｋ�п��������ڶ�ʱ��������߿��������
	  heap�ռ䲻����ɴ���ʧ�ܣ���Ҫ�Ӵ�FreeRTOSConfig.h�ļ��ж����heap��С��
	  #define configTOTAL_HEAP_SIZE	      ( ( size_t ) ( 30 * 1024 ) )
	*/
	while(1);
}

void Periph_Init()
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		//����ϵͳ�ж����ȼ�����2
//	LED_Init();
	KEY_Init();
	uart_init(115200);
}

/*
**********************************************************************************************************
											    �û�����
**********************************************************************************************************
*/

static void vTask_Receive(void *pvParameters)
{
	int rev_num = 0;
	
	vTaskSuspend(xHandleTask_Receive);
	for(;;)
	{
		printf("a\r\n");
		vTaskDelay(30);
		if(USART_GetFlagStatus(USART1 , USART_FLAG_RXNE) != RESET)
		{
			if(Read_Bit() == 0xff)
			{
				//printf("receive 0xff\r\n");
				for(rev_num=0; rev_num<1; rev_num++)
					rev[rev_num] = Read_Bit()-48;
				//printf("receive %d\r\n",rev[0]);
//				if(rev[0] == 3)
//				{				
//					RL_stepper_turn(rev[1]*50);
//					OSTaskResume(&AppTask_RL_Stepper_TCB, &err);
//					while(RL_stepper_judge)
//						OSTimeDlyHMSM(0u, 0u, 0u, 5u, OS_OPT_TIME_HMSM_STRICT, &err);
//				}
//				else if(rev[0] == 2)
//				{	
//					if(rev[1] == 0)
//						bottom_stepper_turn(-60);
//					else
//						bottom_stepper_turn(60);
//					bottom_stepper_turn(-rev[1]*10);
//					OSTaskResume(&AppTask_Bottom_Stepper_TCB, &err);
//					while(bottom_stepper_judge)
//						OSTimeDlyHMSM(0u, 0u, 0u, 5u, OS_OPT_TIME_HMSM_STRICT, &err);
//				}
//				else if(rev[0] == 4)
//				{			
//					UD_stepper_turn(rev[1]*50);
//					OSTaskResume(&AppTask_UD_Stepper_TCB, &err);
//					while(UD_stepper_judge)
//						OSTimeDlyHMSM(0u, 0u, 0u, 5u, OS_OPT_TIME_HMSM_STRICT, &err);
//				}
//				else if(rev[0] == 5)
//				{			
//					set_servo_angle(10*rev[1]);
//				}
				if(Read_Bit() == 0xee)
				{
					//printf("receive 0xee\r\n");					
					USART_judge = 1;
					vTaskSuspend(xHandleTask_Receive);
				}
			}			
		}		
	}
}

static void	vTask_Control(void *pvParameters)
{
	static int high = 1;
	int tem_angle = 0;
	
	vTaskDelay(5000);
	
	ccd2_center = 64; //���ݳ�ʼ����޸�
	vTaskResume(xHandleTask_CCD2);
	vTaskResume(xHandleTask_CCD1);
	UD_stepper_turn(UD1);
	vTaskResume(xHandleTask_UDStepper);
	targetSpeedY = -35;
	
	for(;;)
	{	
		int i, j = 0;
		if(slow_down_judge)
		{
			slow_down_judge = 0;
			vTaskDelay(100);
			if(slow_down_judge){
				slow_down_judge = 0;
				targetSpeedY = 0;
				vTaskSuspend(xHandleTask_CCD1);	//�ر��߳�CCD1 
				vTaskSuspend(xHandleTask_CCD2);	//�ر��߳�CCD2
				target_center1 = 66;
				
				targetSpeedX = 0;
				targetSpeedW = -60;
				vTaskDelay(5000);
				CCD1_Collect();
				while(!Find_Line_first(ccd1_data, THRESHOLD))
				{
					CCD1_Collect();
					vTaskDelay(5);
				}
				
				
				vTaskResume(xHandleTask_CCD1);
				vTaskResume(xHandleTask_CCD2);
				while(targetSpeedW || targetSpeedY)
					vTaskDelay(10);			
				targetSpeedY = 30;
				vTaskDelay(1000);
				stop_judge = 0;
			}
//			else{
//				targetSpeedY = 45;
//			}
			
		}
		if(stop_judge)
		{
			work_times++;
			if(work_times==0){
				stop_judge = 0;
				UD_stepper_turn(-UD1);
				vTaskResume(xHandleTask_UDStepper);
				while(UD_stepper_judge)
					vTaskDelay(5);
				targetSpeedY = 30;
			}
			else if(work_times<=6)
			{
				
				
				stop_judge = 0;
				targetSpeedY = 0;
				vTaskDelay(1000);
				//printf("stop\r\n");
				
//				if(work_times != 6){
//					OSTaskResume(&AppTask_CCD1_TCB, &err);
//					OSTimeDlyHMSM(0u, 0u, 1u, 0u, OS_OPT_TIME_HMSM_STRICT, &err);
//					while(targetSpeedW)
//						OSTimeDlyHMSM(0u, 0u, 0u, 10u, OS_OPT_TIME_HMSM_STRICT, &err);
//					OSTaskSuspend(&AppTask_CCD1_TCB, &err);
//				}
				//USART_ClearITPendingBit(USART1, USART_IT_RXNE);
				vTaskResume(xHandleTask_Receive);
				i = 0;				
				while(!USART_judge){
					vTaskDelay(10);
					//i++;					
//					if(i > 500){
//						rev[0] = guess_angle[work_times-1];
//						break;
//					}
				}
				//OSTaskSuspend(&AppTask_Receive_TCB, &err);
				USART_judge = 0;		
				
//				printf("grab\r\n");
//				if(!guess_judge[work_times-1]){
				angle[rev[0]] = angle[0];
//				}
				GrabMilk(high);
				vTaskDelay(1000);
				if(work_times == 3){
					bottom_stepper_turn(180);
					vTaskResume(xHandleTask_BottomStepper);
					angle[0] = 60;
					targetSpeedY = 30;
					high = 2;
				}
				else if(work_times == 5){
					bottom_stepper_turn(120);
					angle[0] += 120;
					vTaskResume(xHandleTask_BottomStepper);
					targetSpeedY = 30;
					vTaskSuspend(xHandleTask_CCD1);
				}
				else if(work_times ==6){
					printf("start_turn\r\n");
					//ת90����
					vTaskSuspend(xHandleTask_CCD2);	//�ر��߳�CCD2
					targetSpeedY = -20;
					vTaskDelay(500);
					targetSpeedY = 0;
					targetSpeedX = 0;
					targetSpeedW = 60;
					vTaskDelay(2500);
					CCD2_p = 1;
					CCD1_p = 3;
					CCD1_Collect();
					while(!Find_Line_first(ccd1_data, THRESHOLD))
					{
						CCD1_Collect();
						vTaskDelay(5);
					}
					
					vTaskResume(xHandleTask_CCD1);
					targetSpeedY = 50;
					vTaskDelay(1000);
					vTaskResume(xHandleTask_CCD2);

					angle[0] = 0;
					bottom_stepper_turn(angle[1]+65);
					vTaskResume(xHandleTask_BottomStepper);
					while(UD_stepper_judge)
						vTaskDelay(5);
					targetSpeedY = 45;
					UD_stepper_turn(TWO_FLOAT_HIGH);
					vTaskResume(xHandleTask_UDStepper);
					while(UD_stepper_judge)
						vTaskDelay(5);	
				}
				else{
					bottom_stepper_turn(120);
					vTaskResume(xHandleTask_BottomStepper);
					angle[0] += 120;
					targetSpeedY = 30;
				}	
			}
			else
			{
				if(work_times == 8){
					vTaskSuspend(xHandleTask_CCD1);
				}
				stop_judge = 0;
				targetSpeedY = 0;
				printf("stop\r\n");
				
				PutDownMilk(((angle[2*(work_times-6)-1]/60)%2+1) , 1);
				//ȡ��-360
				tem_angle = angle[2*(work_times-6)]-angle[2*(work_times-6)-1]+10;
				if(tem_angle > 180)tem_angle -= 360;
				if(tem_angle < -180)tem_angle += 360;
				bottom_stepper_turn(tem_angle);
				vTaskResume(xHandleTask_BottomStepper);
				while(bottom_stepper_judge)
					vTaskDelay(5);
				
				//��֤�ϴ�ץ��
				while(UD_stepper_judge)
					vTaskDelay(5);
				PutDownMilk(((angle[2*(work_times-6)]/60)%2+1) , 2);
				
				target_center2 = 64;
				vTaskDelay(1000);
				
				if(work_times<9)
					targetSpeedY = 20;
				
				tem_angle = angle[2*(work_times-6)+1]-angle[2*(work_times-6)]+10;
				if(tem_angle > 180)tem_angle -= 360;
				if(tem_angle < -180)tem_angle += 360;
				bottom_stepper_turn(tem_angle);
				vTaskResume(xHandleTask_BottomStepper);
				
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
//			

			
		}	
		vTaskDelay(10);
	}
	
	
}


static void vTask_USART(void *pvParameters)
{
	
	vTaskDelay(1000);
	
	
	for(;;)
	{
		vTaskDelay(2000);
	}
}

static void vTask_Mecanum(void *pvParameters)
{
	static int time =20;
	TickType_t xLastWakeTime;
	
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
		//printf("x:%.2f y:%.2f w:%.2f\r\n",targetSpeedX, targetSpeedY, targetSpeedW);
		targetMecanum = moto_caculate(targetSpeedX, targetSpeedY, targetSpeedW);
		
		if(rightfront_pwm < 0)
			rightfront_pid.error = (targetMecanum[0])+((TIM2->CNT<0xffffffff-TIM2->CNT) ? TIM2->CNT : 0xffffffff-TIM2->CNT)*15/time;
		else
			rightfront_pid.error = (targetMecanum[0])-((TIM2->CNT<0xffffffff-TIM2->CNT) ? TIM2->CNT : 0xffffffff-TIM2->CNT)*15/time;	
		//printf("TIM2->CNT %d\r\n", TIM2->CNT);
		TIM2->CNT = 0;

		if(leftfront_pwm < 0)
			leftfront_pid.error = (targetMecanum[1])+((TIM3->CNT<0xffff-TIM3->CNT) ? TIM3->CNT : 0xffff-TIM3->CNT)*15/time;
		else
			leftfront_pid.error = (targetMecanum[1])-((TIM3->CNT<0xffff-TIM3->CNT) ? TIM3->CNT : 0xffff-TIM3->CNT)*15/time;	
		//printf("TIM3->CNT %d\r\n", TIM3->CNT);
		TIM3->CNT = 0;
		
		if(leftrear_pwm < 0)
			leftrear_pid.error = (targetMecanum[2])+((TIM4->CNT<0xffff-TIM4->CNT) ? TIM4->CNT : 0xffff-TIM4->CNT)*15/time;
		else
			leftrear_pid.error = (targetMecanum[2])-((TIM4->CNT<0xffff-TIM4->CNT) ? TIM4->CNT : 0xffff-TIM4->CNT)*15/time;	
		//printf("TIM4->CNT %d\r\n", TIM4->CNT);
		TIM4->CNT = 0;
		
		if(rightrear_pwm < 0)
			rightrear_pid.error = (targetMecanum[3])+((TIM5->CNT<0xffffffff-TIM5->CNT) ? TIM5->CNT : 0xffffffff-TIM5->CNT)*15/time;
		else
			rightrear_pid.error = (targetMecanum[3])-((TIM5->CNT<0xffffffff-TIM5->CNT) ? TIM5->CNT : 0xffffffff-TIM5->CNT)*15/time;	
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
		
//		vTaskDelayUntil(&xLastWakeTime, 20);
		vTaskDelay(20);
	}
}

static void vTask_CCD2(void *pvParameters)
{
	TickType_t xLastWakeTime;
	CCD_Init();
	vTaskSuspend(xHandleTask_CCD2);
	for(;;)
	{
		CCD2_Collect();
		//ccd_send_data(USART2, ccd2_data);

		ccd2_center = CCD2_find_Line(ccd2_center, THRESHOLD);
		//printf("CCD2_center %d\r\n", ccd2_center);
		
		if(ccd2_center > target_center2 + 2 || ccd2_center < target_center2 - 2)
			targetSpeedX = (ccd2_center - target_center2) * CCD2_p;
		else targetSpeedX = 0;
		//printf("targetSpeedX %d\r\n",targetSpeedX);
		//printf("CCD2_error %d\r\n",ccd2_center - target_center);
		
//		push(1,targetspeed);
//		push(2,leftrear_pid.error+100);
//		push(3,leftrear_pwm);
//		sendDataToScope();
//		vTaskDelayUntil(&xLastWakeTime, 20);
		vTaskDelay(20);
	}
}

static void vTask_CCD1(void *pvParameters)
{
	TickType_t xLastWakeTime;
	vTaskSuspend(xHandleTask_CCD1);
	for(;;)
	{
		CCD1_Collect();
		//ccd_send_data(USART2, ccd1_data);
		ccd1_center = CCD1_find_Line(ccd1_center, THRESHOLD);

		if(ccd1_center > target_center2+2 || ccd1_center < target_center2-2)
			targetSpeedW = -(ccd1_center - target_center1) * CCD1_p;
		else targetSpeedW = 0;
		//printf("CCD1_center %d\r\n",ccd1_center);
		
//		push(1,targetspeed);
//		push(2,leftrear_pid.error+100);
//		push(3,leftrear_pwm);
//		sendDataToScope();
//		vTaskDelayUntil(&xLastWakeTime, 20);
		vTaskDelay(20);
	}
}


static void vTask_BottomStepper(void *pvParameters)
{
	Stepper_EN_Init();
	Servo_Init();
	vTaskDelay(5000);
	GPIO_SetBits(GPIOB, GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12);
	
	Stepper_Init();
	vTaskSuspend(xHandleTask_BottomStepper);
	
	for(;;)
	{
		if(bottom_stepper_judge)
			PEout(5)=1;
//		if(puzzer_)
//			PEout(9)=1;
		vTaskDelay(1);
		PEout(5)=0;
		PEout(9)=0;
		vTaskDelay(1);
		Bottom_Soft_IRQ();
		if(!bottom_stepper_judge)
		{
			vTaskSuspend(xHandleTask_BottomStepper);
		}
	}
}

static void vTask_RLStepper(void *pvParameters)
{
	vTaskDelay(2000);
	
	Stepper_Init();
	vTaskSuspend(xHandleTask_RLStepper);
	
	for(;;)
	{
		if(RL_stepper_judge)
			PDout(5)=1;
		vTaskDelay(2);
		PDout(5)=0;
		vTaskDelay(2);
		RL_Soft_IRQ();
		if(!RL_stepper_judge)
		{
			vTaskSuspend(xHandleTask_RLStepper);
		}
	}
}

static void vTask_UDStepper(void *pvParameters)
{
	vTaskDelay(2000);
	
	vTaskSuspend(xHandleTask_UDStepper);
	
	for(;;)
	{
		if(UD_stepper_judge)
			PDout(7)=1;
		vTaskDelay(1);
		PDout(7)=0;
		vTaskDelay(1);
		UD_Soft_IRQ();
		if(!UD_stepper_judge)
		{
			vTaskSuspend(xHandleTask_UDStepper);
		}
	}
}

//static void AppTask_Stepper(void *pvParameters)
//{
//	
//}

/*
**********************************************************************************************************
											    �û�����
**********************************************************************************************************
*/

static void TestLED(void)
{
	
	vTaskDelay(500);
	LED1 = 0;

	vTaskDelay(500);
	LED1 = 1;
}

static void GrabMilk(int a)
{
	bottom_turn_judge = 0;
	if(a == 1)
	{
		servo_close();
		vTaskDelay(500);
		UD_stepper_turn(ONE_FLOAT_HIGH + UD1);
		vTaskResume(xHandleTask_UDStepper);
		while(UD_stepper_judge)
			vTaskDelay(5);
		RL_stepper_turn(RL1);
		vTaskResume(xHandleTask_RLStepper);
		while(RL_stepper_judge)
			vTaskDelay(5);
		vTaskDelay(500);
		UD_stepper_turn(-UD1);
		vTaskResume(xHandleTask_UDStepper);
		while(UD_stepper_judge)
			vTaskDelay(5);
		servo_open();
		vTaskDelay(500);
//		UD_stepper_turn(UD);
//		OSTaskResume(&AppTask_UD_Stepper_TCB, &err);
//		while(UD_stepper_judge)
//			OSTimeDlyHMSM(0u, 0u, 0u, 5u, OS_OPT_TIME_HMSM_STRICT, &err);
		RL_stepper_turn(-RL1);
		vTaskResume(xHandleTask_RLStepper);
		while(RL_stepper_judge)
			vTaskDelay(5);
		UD_stepper_turn(-ONE_FLOAT_HIGH);
		vTaskResume(xHandleTask_UDStepper);
//		while(UD_stepper_judge)
//			OSTimeDlyHMSM(0u, 0u, 0u, 5u, OS_OPT_TIME_HMSM_STRICT, &err);		
	}
	else if(a == 2)
	{
		servo_close();
		vTaskDelay(500);
		UD_stepper_turn(TWO_FLOAT_HIGH + UD2);
		vTaskResume(xHandleTask_UDStepper);
		while(UD_stepper_judge)
			vTaskDelay(5);
		RL_stepper_turn(RL1);
		vTaskResume(xHandleTask_RLStepper);
		while(RL_stepper_judge)
			vTaskDelay(5);
		UD_stepper_turn(-UD2);
		vTaskResume(xHandleTask_UDStepper);
		while(UD_stepper_judge)
			vTaskDelay(5);
		servo_open();
		vTaskDelay(500);
//		UD_stepper_turn(500);
//		OSTaskResume(&AppTask_UD_Stepper_TCB, &err);
//		while(UD_stepper_judge)
//			OSTimeDlyHMSM(0u, 0u, 0u, 5u, OS_OPT_TIME_HMSM_STRICT, &err);
		RL_stepper_turn(-RL1);
		vTaskResume(xHandleTask_RLStepper);
		while(RL_stepper_judge)
			vTaskDelay(5);
		UD_stepper_turn(-TWO_FLOAT_HIGH);
		vTaskResume(xHandleTask_UDStepper);
//		while(UD_stepper_judge)
//			OSTimeDlyHMSM(0u, 0u, 0u, 5u, OS_OPT_TIME_HMSM_STRICT, &err);
	}
	bottom_turn_judge = 1;
}

static void PutDownMilk(int a, int b)//��ʼ�߶�5000
{
	bottom_turn_judge = 0;
	b -= 1;
	if(a == 1)
	{
		UD_stepper_turn(ONE_FLOAT_HIGH - TWO_FLOAT_HIGH);
		vTaskResume(xHandleTask_UDStepper);
		while(UD_stepper_judge)
			vTaskDelay(5);
		while(bottom_stepper_judge)
			vTaskDelay(5);
		RL_stepper_turn(RL1);
		vTaskResume(xHandleTask_RLStepper);
		while(RL_stepper_judge)
			vTaskDelay(5);
		bottom_stepper_turn(-10);
		vTaskResume(xHandleTask_BottomStepper);
		while(bottom_stepper_judge)
			vTaskDelay(5);
//		UD_stepper_turn(-750);
//		OSTaskResume(&AppTask_UD_Stepper_TCB, &err);
//		while(UD_stepper_judge)
//			vTaskDelay(5);
		servo_close();
		vTaskDelay(500);
		UD_stepper_turn(UD1);
		vTaskResume(xHandleTask_UDStepper);
		while(UD_stepper_judge)
			vTaskDelay(5);
		RL_stepper_turn(-RL1);
		vTaskResume(xHandleTask_RLStepper);
		while(RL_stepper_judge)
			vTaskDelay(5);
		UD_stepper_turn(MILK_HIGH * b - (ONE_FLOAT_HIGH + UD1));
		vTaskResume(xHandleTask_UDStepper);
		while(UD_stepper_judge)
			vTaskDelay(5);
		servo_open();
		vTaskDelay(500);
		UD_stepper_turn(TWO_FLOAT_HIGH - MILK_HIGH * b);
		vTaskResume(xHandleTask_UDStepper);
//		while(UD_stepper_judge)
//			vTaskDelay(5);
	}
	else if(a == 2)
	{
//		UD_stepper_turn(5000);
//		OSTaskResume(&AppTask_Stepper_TCB, &err);
//		while(UD_stepper_judge)
//			vTaskDelay(5);
		while(bottom_stepper_judge)
			vTaskDelay(5);
		RL_stepper_turn(RL1);
		vTaskResume(xHandleTask_RLStepper);
		while(RL_stepper_judge)
			vTaskDelay(5);
		bottom_stepper_turn(-10);
		vTaskResume(xHandleTask_BottomStepper);
		while(bottom_stepper_judge)
			vTaskDelay(5);
//		UD_stepper_turn(UD);
//		OSTaskResume(&AppTask_UD_Stepper_TCB, &err);
//		while(UD_stepper_judge)
//			vTaskDelay(5);
		servo_close();
		vTaskDelay(500);
		
		UD_stepper_turn(UD2);
		vTaskResume(xHandleTask_UDStepper);
		while(UD_stepper_judge)
			vTaskDelay(5);
		
		RL_stepper_turn(-RL1);
		vTaskResume(xHandleTask_RLStepper);
		while(RL_stepper_judge)
			vTaskDelay(5);
		
		UD_stepper_turn(MILK_HIGH * b - (TWO_FLOAT_HIGH + UD2));
		vTaskResume(xHandleTask_UDStepper);
		while(UD_stepper_judge)
			vTaskDelay(5);
		servo_open();
		vTaskDelay(500);
		UD_stepper_turn(TWO_FLOAT_HIGH - MILK_HIGH * b);
		vTaskResume(xHandleTask_UDStepper);
//		while(UD_stepper_judge)
//			vTaskDelay(5);
	}
	bottom_turn_judge = 1;
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskCreate
*	����˵��: ����Ӧ������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void AppTaskCreate (void)
{
    xTaskCreate( vTask_Receive,				/* ������  */
                 "vTask Receive",			/* ������    */
                 512,						/* ����ջ��С����λword��Ҳ����4�ֽ� */
                 NULL,						/* �������  */
                 vTask_Receive_PRIO,		/* �������ȼ�*/
                 &xHandleTask_Receive );	/* ������  */
	
	
	xTaskCreate( vTask_USART,
                 "vTask USART",
                 512,
                 NULL,
                 vTask_USART_PRIO,
                 &xHandleTask_USART );
	
	xTaskCreate( vTask_Mecanum,
                 "vTask Mecanum",
                 512,
                 NULL,
                 vTask_Mecanum_PRIO,
                 &xHandleTask_Mecanum );
	
	
	xTaskCreate( vTask_CCD2,
                 "vTask CCD2",
                 512,
                 NULL,
                 vTask_CCD2_PRIO,
                 &xHandleTask_CCD2 );
				 
	xTaskCreate( vTask_CCD1,
                 "vTask CCD1",
                 512,
                 NULL,
                 vTask_CCD1_PRIO,
                 &xHandleTask_CCD1 );
				 
				 
	xTaskCreate( vTask_BottomStepper,
                 "vTask Bottom Stepper",
                 512,
                 NULL,
                 vTask_BottomStepper_PRIO,
                 &xHandleTask_BottomStepper );
	
	xTaskCreate( vTask_RLStepper,
                 "vTask RL Stepper",
                 512,
                 NULL,
                 vTask_RLStepper_PRIO,
                 &xHandleTask_RLStepper );
	
	
	xTaskCreate( vTask_UDStepper,
                 "vTask UD Stepper",
                 512,
                 NULL,
                 vTask_UDStepper_PRIO,
                 &xHandleTask_UDStepper );
				 
	xTaskCreate( vTask_Control,
                 "vTask Control",
                 512,
                 NULL,
                 vTask_Control_PRIO,
                 &xHandleTask_Control );
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
