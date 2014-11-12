#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <GLFW/glfw3.h>
#include "controls.h"

struct JoystickState
{
	int id;
	int axes;
	float* values;
};

static int CONTROLS_JOYSTICK_COUNT = 0;
static struct JoystickState CONTROLS_JOYSTICKS[10] = {0};
static struct JoystickState* CONTROLS_THROTTLE_STICK = NULL;
static struct JoystickState* CONTROLS_BALANCE_STICK = NULL;
void (*CONTROLS_THROTTLE_CALLBACK)(float, float);
void (*CONTROLS_BALANCE_CALLBACK)(float, float);

void selectStick(struct JoystickState* stickOut)
{
	// reset the stick measurements
	for(int i = CONTROLS_JOYSTICK_COUNT; i--;){
		struct JoystickState* stick = CONTROLS_JOYSTICKS + i;
		const float* values = glfwGetJoystickAxes(stick->id, &stick->axes);
		memcpy(stick->values, values, sizeof(float) * stick->axes);
	}

	while(1){
		for(int i = CONTROLS_JOYSTICK_COUNT; i--;){
			struct JoystickState* stick = CONTROLS_JOYSTICKS + i;
			const float* values = glfwGetJoystickAxes(stick->id, &stick->axes);

			// dot the current and last joystick readings
			float lCurrent = 0, lLast = 0;
			for(int j = stick->axes; j--;){
				lCurrent += values[j] * values[j];
				lLast    += stick->values[j] * stick->values[j]; 
			}

			// retain the new measurement
			memcpy(stick->values, values, sizeof(float) * stick->axes);

			// this stick probably moved!
			if(fabs(lCurrent - lLast) > 0.01f){
				stickOut = stick;
				return;
			}
		}
	}
}

void pollStick(struct JoystickState* stick, void(*cb)(float, float))
{
	const float* values = glfwGetJoystickAxes(stick->id, &stick->axes);

	assert(stick->axes >= 2);

	// dot the current and last joystick readings
	float lCurrent = 0, lLast = 0;
	for(int j = stick->axes; j--;){
		lCurrent += values[j] * values[j];
		lLast    += stick->values[j] * stick->values[j]; 
	}

	// retain the new measurement
	memcpy(stick->values, values, sizeof(float) * stick->axes);

	// this stick probably moved!
	if(fabs(lCurrent - lLast) > 0.001f){
		cb(values[0], values[1]);
		return;
	}
}

int controlsSetup(stickCallback throttleCb, stickCallback balanceCb){
	CONTROLS_THROTTLE_CALLBACK = throttleCb;
	CONTROLS_BALANCE_CALLBACK = balanceCb;

	// find the number of joysticks connected
	for(int i = 0; i < 10; ++i){
		if(glfwJoystickPresent(i)){
			int axes = 0;
			const float* values = glfwGetJoystickAxes(i, &axes);

			struct JoystickState stick = {
				.id = i,
				.axes = axes,
				.values = NULL
			};
			stick.values = (float*)malloc(sizeof(float) * axes);
			memcpy(stick.values, values, sizeof(float) * axes);

			CONTROLS_JOYSTICKS[CONTROLS_JOYSTICK_COUNT++] = stick;
		}	
	}
	printf("%d joysticks found\n", CONTROLS_JOYSTICK_COUNT);

	if(CONTROLS_JOYSTICK_COUNT == 0){
		return -1;
	}

	printf("Please move the throttle stick to select it...");
	fflush(stdout);
	selectStick(CONTROLS_THROTTLE_STICK);
	printf("Selected!\n");
	sleep(1);

	printf("Please move the balance stick to select it...");
	fflush(stdout);
	selectStick(CONTROLS_BALANCE_STICK);
	printf("Selected!\n");
}

int controlsPoll(){
	pollStick(CONTROLS_THROTTLE_STICK, CONTROLS_THROTTLE_CALLBACK);
	pollStick(CONTROLS_BALANCE_STICK, CONTROLS_BALANCE_CALLBACK);

	return 0;
}