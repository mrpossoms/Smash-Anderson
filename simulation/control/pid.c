#include "pid.h"

float PID(PidState* state, float measurement, float dt){
	float error = state->target - measurement;
	float derivative, output;
	float Kp = state->Kp, Ki = state->Ki, Kd = state->Kd;

	state->integral += error * dt;
	derivative = (error - state->lastError) / dt;
	output = Kp * error + Ki * state->integral + Kd * derivative;
	state->lastError = error;

	return output;
}