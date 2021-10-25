#include "filter_mathod.h"
#include "sys.h"



//一阶滤波
/*fL=a/2Pit pi为圆周率3.14… fL为采样频率
式中 a——滤波系数, t——采样间隔时间;

例如：当t=0.5s(即每秒2次)，a=1/32时;
fL=(1/32)/(2*3.14*0.5)=0.01Hz
*/
#define a   0.01                // 滤波系数a（0-1） 

u32 get_ad(void)
{
	return 0;
}

u32 baroOffset,baro,baroAlt;//本次实际值，本次滤波值，上次滤波值

u32 filter(void)
{
    baroOffset  = get_ad(); 
    baro = a * baroOffset  + (1.0 - a) * baroAlt;
    baroAlt = baro;
    return baro;  
}



//一维卡尔曼滤波
/*        
        Q:过程噪声，Q增大，动态响应变快，收敛稳定性变坏
        R:测量噪声，R增大，动态响应变慢，收敛稳定性变好        
*/

// ProcessNiose_Q;//系统噪声
// MeasureNoise_R;//测量噪声
//ResrcData;//测量值

double KalmanFilter(double ResrcData,double ProcessNiose_Q,double MeasureNoise_R)
{
        double R = MeasureNoise_R;//测量噪声
        double Q = ProcessNiose_Q;//系统噪声

        static double x_last;  //上次最优值

        double x_mid; //预测本次值
        double x_now;

        static  double p_last; //上次噪声

        double p_mid ; //预测本次噪声
        double p_now;
        double kg;        

        x_mid=x_last; //x_last=x(k-1|k-1),x_mid=x(k|k-1)
        p_mid=p_last+Q; //p_mid=p(k|k-1),p_last=p(k-1|k-1),Q=噪声
        kg=p_mid/(p_mid+R); //kg为kalman filter，R为噪声
        x_now=x_mid+kg*(ResrcData-x_mid);//估计出的最优值
                
        p_now=(1-kg)*p_mid;//最优值对应的covariance        
        p_last = p_now; //更新covariance值
        x_last = x_now; //更新系统状态值
        return x_now;                
}

