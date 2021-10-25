#ifndef __FILTER_MATHOD_H
#define __FILTER_MATHOD_H
#include "sys.h"

double KalmanFilter(double ResrcData,double ProcessNiose_Q,double MeasureNoise_R);
u32 filter(void);


#endif
