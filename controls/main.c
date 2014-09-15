#include <stdio.h>
#include "smash-speed.h"
#include "smash-hub.h"

static struct SmashState* state = NULL;

int main(int argc, char* argv[])
{
	int fd_rotors = 0;

	if(argc != 2){
		printf("Incorrect number of arguments. Please provided path to the servoblaster device\n");
		return -1;
	}

	// start the servo driver
	if(system("servod --p1pins=11,12,15,16")){
		printf("Error! Failed to start servo driver daemon\n");
		return -2;
	}

	// attache to the shared memory segment
	if(!(state = (struct SmashState*)createAndAttach(SMASH_SHM_KEY))){
		printf("Failed to attach to shared memory segment\n");
		return -3;
	}
	state->subSystemOnline |= SMASH_HUB_MSK_SPEED;
	
	// initialize the control system
	if(!(fd_rotors = smashSpeedInit(argv[1]))){
		printf("Failed to open servo device\n");
		return -4;
	}

	// keep updating the speed as it changes
	while(!(state->subSystemShouldShutdown & SMASH_HUB_MSK_SPEED)){
		smashSpeedSet(fd_rotors, state->speedTargets);
		usleep(10000);
	}

	printf("Exited successfully\n");

	return 0;
}
