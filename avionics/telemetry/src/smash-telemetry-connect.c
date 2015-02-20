#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "smash-telemetry.h"

pthread_mutex_t SMASH_TELEM_LOCK;

int smashTelemetryInit(const char* dev){
	int fd = -1;

	if ((fd = atOpen(dev, 115200, AT_BIN | AT_NCHKSUM | AT_BLOCKING)) < 0) {
		printf("Failed to open the telemetry bus.\n");
		exit(1);
	}
	
	if(pthread_mutex_init(&SMASH_TELEM_LOCK, NULL)){
		printf("Initializing lock failed\n");
		exit(2);
	}

	return fd;
}