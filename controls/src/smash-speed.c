#include <smash-speed.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

int smashSpeedInit(const char* dev){
	int fd = -1;
	if((fd = open(dev, O_WRONLY)) < 0){
		return -1;
	}

	return fd;
}

void smashSpeedSet(int fd, unsigned char* rotors){
	int i = 0;
	for(i = 4; i--;){
		char buf[10] = {0};
		sprintf(buf, "%d=%d%\n", i, (int)(rotors[i] / 2.55f));
		write(fd, buf, strlen(buf));
	}	
}

void smashSpeedClose(int fd){
	close(fd);
}
