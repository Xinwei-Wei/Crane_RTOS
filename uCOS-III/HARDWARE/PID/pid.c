#include "pid.h"

void position_pid_init(struct PositionPID* p, float kp, float ki, float kd)
{
	p->kp = kp;
	p->ki = ki;
	p->kd = kd;
	p->error = 0;
	p->last_error = 0;
	p->sum_error = 0;
}

void incremental_pid_init(struct IncrementalPID* p, float kp, float ki, float kd)
{
	p->kp = kp;
	p->ki = ki;
	p->kd = kd;
	p->error = 0;
	p->last_error = 0;
	p->last_last_error = 0;
}

float position_pid(struct PositionPID* p)
{
	float pwm;
	pwm = p->kp*p->error+ p->ki*p->sum_error+ p->kd*(p->error-p->last_error);
	p->last_error = p->error;
	p->sum_error += p->error;
	//p->sum_error = LIMIT(p->sum_error, -8000, +8000);
	return pwm;
}

float incremental_pid(struct IncrementalPID* p)
{
	float pwm;
	pwm = p->kp*(p->error-p->last_error)+p->ki*p->error+p->kd*(p->error-2*p->last_error+p->last_last_error);
	p->last_last_error = p->last_error;
	p->last_error = p->error;
	return pwm;
}


