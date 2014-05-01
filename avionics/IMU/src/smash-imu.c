#include "smash-imu.h"
#include "smash-imu-decoder.h"
#include <pthread.h>
#include <math.h>
#include <ardutalk.h>

//#define __IMU_DEBUG

pthread_t IMU_THREAD;
int IS_POLLING_IMU = 0;
int IMU_FD = -1;

Vec3 ORIENTATION;

void print(const char* str){
	write(1, str, strlen(str));
}

void* __IMUpoller(void* params){

	while(IS_POLLING_IMU){
		atRead(IMU_FD, &ORIENTATION, sizeof(Vec3));
	}

	return NULL;
}

int smashImuInit(const char* dev){
	IMU_FD = atOpen(dev, 57600);
	atConfig(AT_BIN | AT_NCHKSUM);

	atWrite(IMU_FD, "#ob",  3); // Turn on binary output
	atWrite(IMU_FD, "#o1",  3); // Turn on continuous streaming output
	atWrite(IMU_FD, "#oe0", 4); // Disable error message output

	sleep(3);

	IS_POLLING_IMU = 1;
	if((ret = pthread_create(&IMU_THREAD, NULL, __IMUpoller, NULL)) < 0){
		IS_POLLING_IMU = 0;
		return ret;
	}

	return 0;
}

int smashImuShutdown(){

}

