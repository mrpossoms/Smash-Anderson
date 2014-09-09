#include "smash-telemetry.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int smashTelemetryInit(const char* dev, int addr){
	int fd = -1;

	if ((fd = open(dev, O_RDWR)) < 0) {
		printf("Failed to open the bus.");
		exit(1);
	}

	printf("%x set as slave address\n", addr);
	if (ioctl(fd, I2C_SLAVE, addr) < 0) {
		printf("Failed to acquire bus access and/or talk to slave.\n");
		exit(1);
	}

	return fd;
}
//-----------------------------------------------------------------------------
void smashTelemetryShutdown(int fd){
	close(fd);
}
