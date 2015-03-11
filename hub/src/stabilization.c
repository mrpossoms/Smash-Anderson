#include "stabilization.h"
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define MAX(a, b) (a) > (b) ? (a) : (b)

unsigned char MIN_THROTTLE = 28;

struct PidState{
	float Kp, Ki, Kd;
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
		gettimeofday(&lastTime);
		usleep(10000);
	}

	gettimeofday(&now, NULL);

	time_t dSec       = now.tv_sec - lastTime.tv_sec;
	suseconds_t dusec = now.tv_usec - lastTime.tv_usec;

	return (float)(dusec * 1E6 + dSec); 
}

void stblInit()
{
	bzero(&pitchState, sizeof(struct PidState));
	bzero(&rollState,  sizeof(struct PidState));

    	pitchState.Kp = 2.0f;
    	pitchState.Ki = 2.0f;
    	pitchState.Kd = 0.75f;

    	rollState.Kp = 2.0f;
    	rollState.Ki = 2.0f;
    	rollState.Kd = 0.75f;

	pitchState.target = rollState.target = MIN_THROTTLE;
}

int  stblUpdate(struct SmashState* state)
{
	unsigned char THROTTLE_RANGE = 80 - MIN_THROTTLE;
	float dt = elapsedTime();
	float pitchErr = state->imuAngles[1];
	float rollErr  = state->imuAngles[2];

	float pitchOut, rollOut;
	float derivative, output;

	if(dt == 0.0) return 1;

	// PID for the pitch
	pitchState.integral += pitchErr * dt;
	derivative = (pitchErr - pitchState.lastError) / dt;
	pitchOut = pitchState.Kp * pitchErr + pitchState.Ki * pitchState.integral + pitchState.Kd * derivative;
	pitchState.lastError = pitchErr;

	// PID for the roll
	rollState.integral += rollErr * dt;
	derivative = (rollErr - rollState.lastError) / dt;
	rollOut = rollState.Kp * rollErr + rollState.Ki * rollState.integral + rollState.Kd * derivative;
	rollState.lastError = rollErr;

	// Tweak the delta needed for correction
	float rollT  = MAX(0.0882f * rollOut,  0.2);//, 0);
	float pitchT = MAX(0.0882f * pitchOut, 0.2);//, 0);

	// TODO, set this up correctly
	unsigned char rotorThrusts[4] = {
		MIN_THROTTLE + (rollT  > 0  ?  rollT  * THROTTLE_RANGE : 0),
		MIN_THROTTLE + (pitchT > 0  ?  pitchT * THROTTLE_RANGE : 0),
		MIN_THROTTLE + (rollT  < 0  ? -rollT  * THROTTLE_RANGE : 0),
		MIN_THROTTLE + (pitchT < 0  ? -pitchT * THROTTLE_RANGE : 0),
	};

	printf("%f %f - %d %d %d %d\n", rollErr, pitchErr, rotorThrusts[0], rotorThrusts[1], rotorThrusts[2], rotorThrusts[3]);

//	memcpy(state->speedTargets, rotorThrusts, sizeof(rotorThrusts));

	return 0;
}
