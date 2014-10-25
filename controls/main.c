#include <stdio.h>
#include <string.h>
#include "smash-speed.h"
#include "smash-hub.h"

static struct SmashState* state = NULL;

int main(int argc, char* argv[])
{
	int fd_rotors = 0;
	unsigned char lastRotorStates[4];
	unsigned char lastHubState;
	time_t lastContact;

	if(argc != 2){
		printf("Incorrect number of arguments. Please provided path to the servoblaster device\n");
		return -1;
	}

	// start the servo driver
	if(system("servod --p1pins=11,12,15,16")){
		printf("Error! Failed to start servo driver daemon\n");
		return -2;
	}
	printf("Driver started!\n");

	// attache to the shared memory segment
	if(!(state = (struct SmashState*)createAndAttach(SMASH_SHM_KEY))){
		printf("Failed to attach to shared memory segment\n");
		return -3;
	}
	printf("Shared memory attached!\n");

	state->subSystemOnline |= SMASH_HUB_MSK_SPEED;
	
	// initialize the control system
	printf("Initializing control system...");
	sleep(1);
	if(!(fd_rotors = smashSpeedInit(argv[1]))){
		printf("Failed to open servo device\n");
		return -4;
	}
	printf("OK!\n");

	// wait until the hub comes online before beginning operation
	write(1, ".", 1);
	printf("Waiting for hub to start...");
	while(!state->subSystemOnline & SMASH_HUB_MSK){
		usleep(10000);
	}
	lastContact  = time();
	lastHubState = state->subSystemLife[SMASH_HUB_I];
	printf("hub started!\n");

	// keep updating the speed as it changes
	while(!(state->subSystemShouldShutdown & SMASH_HUB_MSK_SPEED)){
		time_t now = time();
		unsigned char hubState = state->subSystemLife[SMASH_HUB_I];

		if(lastHubState != hubState){ // we are hearing back from the hub, all is well
			memcpy(lastRotorStates, state->speedTargets, sizeof(lastRotorStates));

			// update safety variables
			lastHubState = hubState;
			lastContact  = now;
		}
		else{ // we haven't heard from the hub in a second...
			// Slowly power down
			// TODO figure out the decay for realz
			int i =4;
			for(;i--;){
				lastRotorStates[i] *= 0.95f;
			}			
		}
		
		smashSpeedSet(fd_rotors, lastRotorStates);
		usleep(10000);
	}

	printf("Exited successfully\n");

	return 0;
}
