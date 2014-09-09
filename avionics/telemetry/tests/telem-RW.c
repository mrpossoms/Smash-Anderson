#include "smash-telemetry.h"
#include <stdio.h>
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
	if((i = write(fd, buf, sizeof(RotorStates) + 1)) != sizeof(RotorStates) + 1){
		printf("Done goofed %d\n", errno);
	}
	printf("Wrote %d size %d\n", i, sizeof(RotorStates));
	usleep(DELAY);

	for(k = 1000; k--;){
		char rotorReq = 0;
		int waiting = 0;
		bzero(msg, sizeof(RotorStates));		
		if(write(fd, &rotorReq, 1) != 1)
			printf("Something went wrong\n");
		usleep(10000);
/*		
		while(waiting != sizeof(RotorStates)){
			char buf[128];
			ioctl(fd, FIONREAD, &waiting);
			sprintf(buf, "%d\n", waiting);
			write(1, buf, strlen(buf));
		}
*/
		if((i = read(fd, msg, sizeof(RotorStates))) <= 0){
			printf("%d\n", i);
			write(1, ".", 1);
		}
		usleep(DELAY);
		
		for(j = 0; j < i; j++){
			printf("%02x ", msg[j]);
		}printf("\n");	

	}

	smashTelemetryShutdown(fd);

	return 0;
}
