#include "includes.h"
#include "mecanum_control.h"

/*
 * number :选择电机
 * mode   :模式  1：前进；2：后退
 * speed  :速度
 */

/***************
说明：
1、程序的预置小车电机编号依据象限来划分，也就是说，从顶部看小车，车头朝前，右上角的电机为1号，左上角的电机为2号，左下角的电机为3号，右下角的电机为4号。
2、该程序的预配置电机转动方向均为面对电机轴时，电机轴逆时针转动为正转，反之则为反转。
3、关于参数，输入三个变量speedx——x轴移动速度,speedy——y轴移动速度,speedw——小车移动速度。
**************/

#define valueK 1

float * moto_caculate(float pwmx, float pwmy, float pwmw)
{
    static float Vw[5] = {0,0,0,0,0};
    Vw[0] = -pwmx+pwmy+pwmw*valueK;	//电机转速计算
    Vw[1] = pwmx+pwmy+pwmw*valueK;
    Vw[2] = pwmx-pwmy+pwmw*valueK;
    Vw[3] = -pwmx-pwmy+pwmw*valueK;
    return Vw;
}

//控制函数(每个轮子的控制)
void Control_Dir(u8 number,float pwm)
{
    switch(number)
	{
		case 1:
			if(pwm>0)
				Mecanum1_Dir_Pin = 0;
			else
			{
				pwm *= -1;
				Mecanum1_Dir_Pin = 1;
			}
			Mecanum_PWM_SetDuty(1, pwm);
			break ;
		case 2:
			if(pwm>0)
				Mecanum2_Dir_Pin = 0;
			else
			{
				pwm *= -1;
				Mecanum2_Dir_Pin = 1;
			}
			Mecanum_PWM_SetDuty(2, pwm);
			break ;
		case 3:
			if(pwm>0)
				Mecanum3_Dir_Pin = 0;
			else
			{
				pwm *= -1;
				Mecanum3_Dir_Pin = 1;
			}
			Mecanum_PWM_SetDuty(3, pwm);
			break ;
		case 4:
			if(pwm>0)
				Mecanum4_Dir_Pin = 0;
			else
			{
				pwm *= -1;
				Mecanum4_Dir_Pin = 1;
			}
			Mecanum_PWM_SetDuty(4, pwm);
			break ;
		}
}

void duoji_init(void)
{
//    Camera_angle_pwm = (0+30+90)*100.0/18+25;
//    pwm_init(PWM2_CH1_A0,50,(int)Camera_angle_pwm);
}

void duoji_change(void)
{
//    Camera_angle_pwm = (Camera_angle+30+90)*100/18+25;
//    pwm_duty(PWM2_CH1_A0,(int)Camera_angle_pwm);
}
