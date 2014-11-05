#include "stabilization.h"

struct PidState{
	float lastError;
	float integral;
	float target;
};

int pitchRotors[] = { 0, 1 };
int rollRotors[]  = { 2, 3 };

struct PidState pitchState;
struct PidState rollState;
struct timeval lastTime = { 0 };

float elapsedTime()
{
	struct timeval now;

	if(!lastTime.tv_sec){
		gettimeofday(&lastTimep)
		usleep(10000);
	}

	gettimeofday(&now, NULL);

	time_t dSec       = now.tv_sec - lastTime.tv_sec;
	suseconds_t dusec = now.tv_usec - lastTime.tv_usec;

	return (float)(dusec * 1E6 + dSec); 
}

int stabilize(struct SmashState* state, unsigned char* throttleTargets)
{
	float dt = elapsedTime();
	float pitchErr = state->imuAngles[1];
	float rollErr  = state->imuAngles[2];

	float pitchOut, rollOut;

	// PID for the pitch
	pitchState.integral += pitchErr * dt;
	pitchState.derivative = (pitchErr - pitchState.lastError) / dt;
	pitchOut = Kp * pitchErr + Ki * pitchState.integral + Kd * derivative;
	pitchState.lastError = pitchErr;

	// PID for the roll
	rollState.integral += rollErr * dt;
	rollState.derivative = (rollErr - rollState.lastError) / dt;
	rollOut = Kp * rollErr + Ki * rollState.integral + Kd * derivative;
	rollState.lastError = pitchErr;

	// Tweak the delta needed for correction
	float rollT  = max(0.0882f * rollOut,  0.2);//, 0);
	float pitchT = max(0.0882f * pitchOut, 0.2);//, 0);

	// TODO, set this up correctly
	unsigned char rotorThrusts[4] = {
		min(altT + (rollT > 0 ? rollT * 255 : 0), 0),
		min(altT + (pitchT > 0 ? pitchT * 255 : 0), 0),
		min(altT + (rollT < 0 ? -rollT * 255 : 0), 0),
		min(altT + (pitchT < 0 ? -pitchT * 255 : 0), 0)
	};

	memcpy(throttleTargets, rotorThrusts, sizeof(rotorThrusts));

	return 0;
}