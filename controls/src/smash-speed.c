#include <smash-speed.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#define SMASH_DEBUG_SPEED

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
		char buf[8] = {0};
		sprintf(buf, "%d=%d%\n", i, (int)(rotors[i] / 2.55f));

#ifdef SMASH_DEBUG_SPEED
		printf("%s\n", buf);
#endif
		write(fd, buf, strlen(buf));
	}	
}

void smashSpeedClose(int fd){
	close(fd);
}
