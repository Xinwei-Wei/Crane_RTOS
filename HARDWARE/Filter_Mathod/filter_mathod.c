#include "filter_mathod.h"
#include "sys.h"



//һ���˲�
/*fL=a/2Pit piΪԲ����3.14�� fLΪ����Ƶ��
ʽ�� a�����˲�ϵ��, t�����������ʱ��;

���磺��t=0.5s(��ÿ��2��)��a=1/32ʱ;
fL=(1/32)/(2*3.14*0.5)=0.01Hz
*/
#define a   0.01                // �˲�ϵ��a��0-1�� 

u32 get_ad(void)
{
	return 0;
}

u32 baroOffset,baro,baroAlt;//����ʵ��ֵ�������˲�ֵ���ϴ��˲�ֵ

u32 filter(void)
{
    baroOffset  = get_ad(); 
    baro = a * baroOffset  + (1.0 - a) * baroAlt;
    baroAlt = baro;
    return baro;  
}



//һά�������˲�
/*        
        Q:����������Q���󣬶�̬��Ӧ��죬�����ȶ��Ա仵
        R:����������R���󣬶�̬��Ӧ�����������ȶ��Ա��        
*/

// ProcessNiose_Q;//ϵͳ����
// MeasureNoise_R;//��������
//ResrcData;//����ֵ

double KalmanFilter(double ResrcData,double ProcessNiose_Q,double MeasureNoise_R)
{
        double R = MeasureNoise_R;//��������
        double Q = ProcessNiose_Q;//ϵͳ����

        static double x_last;  //�ϴ�����ֵ

        double x_mid; //Ԥ�Ȿ��ֵ
        double x_now;

        static  double p_last; //�ϴ�����

        double p_mid ; //Ԥ�Ȿ������
        double p_now;
        double kg;        

        x_mid=x_last; //x_last=x(k-1|k-1),x_mid=x(k|k-1)
        p_mid=p_last+Q; //p_mid=p(k|k-1),p_last=p(k-1|k-1),Q=����
        kg=p_mid/(p_mid+R); //kgΪkalman filter��RΪ����
        x_now=x_mid+kg*(ResrcData-x_mid);//���Ƴ�������ֵ
                
        p_now=(1-kg)*p_mid;//����ֵ��Ӧ��covariance        
        p_last = p_now; //����covarianceֵ
        x_last = x_now; //����ϵͳ״ֵ̬
        return x_now;                
}

