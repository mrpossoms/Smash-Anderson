#ifndef __PID__
#define __PID__

typedef struct{
	float Kp, Ki, Kd;
	float lastError;
	float integral;
	float target;
} PidState;

float PID(PidState* state, float measurement, float dt);

#endif