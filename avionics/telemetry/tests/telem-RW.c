#include "smash-telemetry.h"
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>

#define DELAY 15000

int main(void){
	int i, fd = smashTelemetryInit("/dev/ttyUSB0");
	
	printf("fd: %d\n", fd);
	//if(write(fd, &type, 1) > 0)
	/*for(i = 0; i < sizeof(RotorStates); i++){
		if(write(fd, &msg + i, 1) <= 0)
			printf("Done goofed %d\n", errno);
	}*/

	while(1){
		byte msgType = 0;
		if(smashReceiveCode(fd, &msgType)){
			usleep(DELAY);
			continue;
		}

		printf("Got code of type %x\n", msgType);		
	}

#define steps 1000

	smashTelemetryShutdown(fd);

	return 0;
}
