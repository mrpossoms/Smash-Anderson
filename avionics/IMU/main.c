#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "smash-imu.h"
#include "smash-hub.h"

struct SmashState* state = NULL;

void onChange(float* ori){
	
	memcpy(state->imuAngles, ori, sizeof(float) * 3);
	memcpy(state->imuOmegas, ori + 3, sizeof(float) * 3);
}

int main(int argc, char* argv[])
{
	if(argc < 2){
		perror("Missing IMU device parameter");
		return -1;
	}

	if(!(state = (struct SmashState*)createAndAttach(SMASH_SHM_KEY))){
		perror("Could not create or attach to IMU shared mem seg");
		return -2;
	}

	if(smashImuInit(argv[1], onChange)){
		perror("Could not initialize IMU device");
	}

	// indicate that the imu system is up and running
	state->subSystemOnline |= SMASH_HUB_MSK_IMU;

	// wait for a signal to shutdown the system
	while(!(state->subSystemShouldShutdown & SMASH_HUB_MSK_IMU)){
		++state->subSystemLife[SMASH_IMU_I];
		usleep(10000);
	}

	return 0;
}
