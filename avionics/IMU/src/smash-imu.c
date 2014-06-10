#include "smash-imu.h"
#include "smash-imu-decoder.h"
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <ardutalk.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <arpa/inet.h>
#include <assert.h>

//#define __IMU_DEBUG
#define VEC3 sizeof(float) * 3

const float DEG_TO_RAD = (M_PI / 180.0f);

pthread_t IMU_THREAD;
int IS_POLLING_IMU = 0;
int IMU_FD = -1;
int synched = 0;
float ORIENTATION[3];
void (*changeCallback)(float*);

void endianSwap(float* f){
	uint32_t i = ntohl(*((uint32_t*)f));
	memcmp(f, &i, sizeof(float));
}

int smashImuSync(const char* token){
	int len = strlen(token), off = 0;
	char received;

	while(1){
		usleep(10000);
		atRead(IMU_FD, &received, 1);
		if(received == token[off]){
			++off;

			if(off == len)
				return 1;

			continue;
		}
	
		off = 0;
	}
}

void* __IMUpoller(void* params){
	int i = 0, bytes = 0;
	float temp[3];

	while(IS_POLLING_IMU){
		
		if(atAvailable(IMU_FD) < VEC3){
			usleep(10000);
			continue;
		}

		if((bytes = atRead(IMU_FD, temp, VEC3)) == VEC3){
			//endianSwap(&ORIENTATION[0]);
			//endianSwap(&ORIENTATION[1]);
			//endianSwap(&ORIENTATION[2]);
			memcpy(ORIENTATION, temp, VEC3);

			ORIENTATION[0] *= DEG_TO_RAD;
			ORIENTATION[1] *= DEG_TO_RAD;
			ORIENTATION[2] *= DEG_TO_RAD;

			printf("OK %d (%f, %f, %f)\n", bytes, ORIENTATION[0], ORIENTATION[1], ORIENTATION[2]);
			changeCallback(ORIENTATION);
		}
		else
			printf("Read %d (%f, %f, %f)\n", bytes, ORIENTATION[0], ORIENTATION[1], ORIENTATION[2]);
	}

	return NULL;
}

int smashImuInit(const char* dev, void (*onChange)(float*)){
	int ret;
	IMU_FD = atOpen(dev, 57600);
	atConfig(IMU_FD, AT_BIN | AT_NCHKSUM);

	assert((changeCallback = onChange) != NULL);

	sleep(1);

	atWrite(IMU_FD, "#ob\n",  4); // Turn on binary output
	atWrite(IMU_FD, "#o1\n",  4); // Turn on continuous streaming output
	atWrite(IMU_FD, "#oe0\n", 5); // Disable error message output
	
	tcflush(IMU_FD, TCIFLUSH);
	atWrite(IMU_FD, "#s00\n", 5);
	
	// sync 
	smashImuSync("#SYNCH00\r\n");
	synched = 1;

	write(1, "Synched\n", 8);

	IS_POLLING_IMU = 1;
	if((ret = pthread_create(&IMU_THREAD, NULL, __IMUpoller, NULL)) < 0){
		IS_POLLING_IMU = 0;
		return ret;
	}

	return 0;
}

int smashImuShutdown(){
	return 0;
}

