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
void (*CONTROLS_THROTTLE_CALLBACK)(float, float);

int controlsSetup(throttleCallback cb){
	CONTROLS_THROTTLE_CALLBACK = cb;

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

	printf("Please move the throttle stick to select it.\n");

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
				CONTROLS_THROTTLE_STICK = stick;
				return 0;
			}
		}

		usleep(10000);
	}
}

int controlsPoll(){
	struct JoystickState* stick = CONTROLS_THROTTLE_STICK;
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
		CONTROLS_THROTTLE_CALLBACK(values[0], values[1]);
		return 0;
	}

	return -1;
}