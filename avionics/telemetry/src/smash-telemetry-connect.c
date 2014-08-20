#include "smash-telemetry.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ardutalk.h>

int smashTelemetryInit(const char* dev){
	int fd = -1;

	if ((fd = atOpen(dev, 115200)) < 0) {
		printf("Failed to open the telemetry bus.");
		exit(1);
	}

	atConfig(fd, AT_BIN);

	return fd;
}