#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "smash-speed.h"
#include "smash-hub.h"

#define MAX(a, b) (a) > (b) ? (a) : (b)

static const unsigned char ZERO = 18; // bottom throttle value
static struct SmashState* state = NULL;

int main(int argc, char* argv[])
{
	int fd_rotors = 0;
	unsigned char lastRotorStates[] = { ZERO, ZERO, ZERO, ZERO };
	unsigned char lastHubState;
	time_t lastContact;

	// start the servo driver
	if(system("servod --p1pins=11,12,15,16")){
		printf("Error! Failed to start servo driver daemon\n");
		return -2;
	}
	printf("Driver started!\n");

	usleep(10000);
	smashSpeedSet(fd_rotors, lastRotorStates);
	
	// attache to the shared memory segment
	if(!(state = (struct SmashState*)createAndAttach(SMASH_SHM_KEY))){
		printf("Failed to attach to shared memory segment\n");
		return -3;
	}
	assert(state);
	printf("Shared memory attached!\n");

	// say the system is offline for now..
	state->subSystemOnline &= ~SMASH_HUB_MSK_SPEED;
	
	// initialize the control system
	printf("Initializing control system...");
	sleep(1);
	if(!(fd_rotors = smashSpeedInit("/dev/servoblaster"))){
		printf("Failed to open servo device\n");
		return -4;
	}
	printf("OK!\n");

	// wait until the hub comes online before beginning operation
	printf("Waiting for hub to start...");
	while(!state->subSystemOnline & SMASH_HUB_MSK){
		usleep(10000);
	}

	lastContact  = time(NULL);
	lastHubState = state->subSystemLife[SMASH_HUB_I];
	state->subSystemOnline |= SMASH_HUB_MSK_SPEED;
	printf("hub started!\n");

	// keep updating the speed as it changes
	while(!(state->subSystemShouldShutdown & SMASH_HUB_MSK_SPEED)){
		time_t now = time(NULL);
		unsigned char hubState = state->subSystemLife[SMASH_HUB_I];
		int i;

		if(lastHubState != hubState){ // we are hearing back from the hub, all is well
			memcpy(lastRotorStates, state->speedTargets, sizeof(lastRotorStates));

			// update safety variables
			lastHubState = hubState;
			lastContact  = now;
		}
		else{ // we haven't heard from the hub in a second...
			// Slowly power down
			// TODO figure out the decay for realz
			for(i = 4; i--;){
				lastRotorStates[i] *= 0.95f;
			}			
		}

		for(i = 4; i--;) lastRotorStates[i] = MAX(lastRotorStates[i], ZERO);
		printf("%u %u %u %u\n", lastRotorStates[0], lastRotorStates[1], lastRotorStates[2], lastRotorStates[3]);
		smashSpeedSet(fd_rotors, lastRotorStates);
		usleep(10000);
	}

	printf("Exited successfully\n");

	return 0;
}
