#ifndef _PID_H
#define _PID_H
#include "sys.h"

#define MIN(x,y) (x<y)?x:y
#define LIMIT(min,x,max) (min>x)?min:((max<x)?max:x)

struct PositionPID{
	float kp, ki, kd;
	float error, last_error, sum_error;
};


struct IncrementalPID{
	float kp, ki, kd;
	float error, last_error, last_last_error;
};


void position_pid_init(struct PositionPID* p, float kp, float ki, float kd);
void incremental_pid_init(struct IncrementalPID* p, float kp, float ki, float kd);
float position_pid(struct PositionPID* p);
float incremental_pid(struct IncrementalPID* p);

#endif
