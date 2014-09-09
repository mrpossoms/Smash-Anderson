#include "smash-imu.h"
#include "smash-imu-decoder.h"
#include <pthread.h>
#include <math.h>
#include <ardutalk.h>
#include <sys/ioctl.h>
#include <termios.h>

//#define __IMU_DEBUG

pthread_t IMU_THREAD;
int IS_POLLING_IMU = 0;
int IMU_FD = -1;
int synched = 0;
Vec3 ORIENTATION;

void print(const char* str){
	write(1, str, strlen(str));
}

int smashImuSync(const char* token){
	int len = strlen(token), bytes = 0;
	char received[len + 1];

	received[len] = '\0';

	while(bytes < len)
		ioctl(IMU_FD, FIONREAD, &bytes);
	//while(received[0] != '#'){
	//	atRead(IMU_FD, received, 1);
	//	write(1, received, 1);
	//}
	
	atRead(IMU_FD, &received[0], len);
	printf("r: %s\nt: %s\n", received, token);

	return memcmp(received, token, len) == 0;
}

void* __IMUpoller(void* params){

	while(IS_POLLING_IMU){
		atRead(IMU_FD, &ORIENTATION, sizeof(Vec3));
	}

	return NULL;
}

int smashImuInit(const char* dev){
	int ret;
	IMU_FD = atOpen(dev, 57600);
	atConfig(AT_BIN | AT_NCHKSUM);

	sleep(3);

	while(!synched){
		atWrite(IMU_FD, "#ob\n",  4); // Turn on binary output
		atWrite(IMU_FD, "#o1\n",  4); // Turn on continuous streaming output
		atWrite(IMU_FD, "#oe0\n", 5); // Disable error message output
		
		tcflush(IMU_FD, TCIFLUSH);
		atWrite(IMU_FD, "#s00\r\n", 6);
		
		// sync 
		if(!smashImuSync("#SYNCH00\r\n")) continue;
		synched = 1;
	}

	write(1, "Synched\n", 8);

	IS_POLLING_IMU = 1;
	if((ret = pthread_create(&IMU_THREAD, NULL, __IMUpoller, NULL)) < 0){
		IS_POLLING_IMU = 0;
		return ret;
	}

	return 0;
}

int smashImuShutdown(){

}

