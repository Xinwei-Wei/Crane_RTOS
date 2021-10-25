#include "includes.h"
#include "mecanum_control.h"

/*
 * number :ѡ����
 * mode   :ģʽ  1��ǰ����2������
 * speed  :�ٶ�
 */

/***************
˵����
1�������Ԥ��С���������������������֣�Ҳ����˵���Ӷ�����С������ͷ��ǰ�����Ͻǵĵ��Ϊ1�ţ����Ͻǵĵ��Ϊ2�ţ����½ǵĵ��Ϊ3�ţ����½ǵĵ��Ϊ4�š�
2���ó����Ԥ���õ��ת�������Ϊ��Ե����ʱ���������ʱ��ת��Ϊ��ת����֮��Ϊ��ת��
3�����ڲ�����������������speedx����x���ƶ��ٶ�,speedy����y���ƶ��ٶ�,speedw����С���ƶ��ٶȡ�
**************/

#define valueK 1

float * moto_caculate(float pwmx, float pwmy, float pwmw)
{
    static float Vw[5] = {0,0,0,0,0};
    Vw[0] = -pwmx+pwmy+pwmw*valueK;	//���ת�ټ���
    Vw[1] = pwmx+pwmy+pwmw*valueK;
    Vw[2] = pwmx-pwmy+pwmw*valueK;
    Vw[3] = -pwmx-pwmy+pwmw*valueK;
    return Vw;
}

//���ƺ���(ÿ�����ӵĿ���)
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
