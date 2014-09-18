#include "smash-telemetry.h"
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include "smash-hub.h"

#define DELAY 15000

static struct SmashState state = {0xFF};

void dump(char* buf, int len){
	int i = 0;	
	for(i = 0; i < len; i++){
		char str[16];
		sprintf(str, "%02x ", buf[i] & 0xff);
		write(1, str, strlen(str));
	}write(1, "\n", 1);
}

int main(void){
	int i, fd = smashTelemetryInit("/dev/tty.usbserial-A5025TWM");
	
	printf("fd: %d\n", fd);
	//if(write(fd, &type, 1) > 0)
	/*for(i = 0; i < sizeof(RotorStates); i++){
		if(write(fd, &msg + i, 1) <= 0)
			printf("Done goofed %d\n", errno);
	}*/

	while(1){
		byte msgType = 0;
		byte buf[128] = {0};
		if(smashReceiveCode(fd, &msgType) < 0){
			usleep(DELAY);
			continue;
		}

		switch(msgType){
			case MSG_CODE_STATUS:
			{
				printf("Status requested\n");
				smashSendStatus(fd, &state);
			}
				break;
			default:
			{
				int read = smashReceiveMsg(fd, buf);
				if(read > 0){
					dump(buf, read);
				}
			}
				break;
		}
	
		usleep(DELAY);
	}

#define steps 1000

	smashTelemetryShutdown(fd);

	return 0;
}
