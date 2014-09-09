#include "smash-telemetry.h"
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <errno.h>
#include <sys/ioctl.h>

#define DELAY 15000

int main(void){
	int i, j, k, fd = smashTelemetryInit("/dev/i2c-1", 0x03);
	char buf[128] = {0};
	
	char type = MSG_TYPE_ROTORS;
	RotorStates msg = {48, 58, 68, 78};

	memcpy(buf + 1, (char*)&msg, 4);
	buf[0] = MSG_TYPE_ROTORS;

	printf("fd: %d\n", fd);
	//if(write(fd, &type, 1) > 0)
	/*for(i = 0; i < sizeof(RotorStates); i++){
		if(write(fd, &msg + i, 1) <= 0)
			printf("Done goofed %d\n", errno);
	}*/

#define steps 1000
	for(k = steps; k--;){
		msg[0] = (char)(cos(k / steps.0f) * 180);
		smashTelSendThrottles(fd, msg);
		bzero(msg, sizeof(RotorStates));
		smashTelRecieveThrottles(fd, msg);
		for(j = 0; j < sizeof(RotorStates); j++){
			printf("%02x ", msg[j]);
		}printf("\n");	
	}

	smashTelemetryShutdown(fd);

	return 0;
}
