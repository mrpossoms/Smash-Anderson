#include "smash-imu.h"
#include "smash-imu-decoder.h"
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <ardutalk.h>
#include <sys/ioctl.h>
#include <termios.h>

#define __IMU_DEBUG

#define __TO_RAD(x){ x *= M_PI / 180.0; }

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
	atRead(IMU_FD, received, len);
	return memcmp(received, token, len) == 0;
}

void* __IMUpoller(void* params){

	while(IS_POLLING_IMU){
		atRead(IMU_FD, &ORIENTATION, sizeof(Vec3));
		__TO_RAD(ORIENTATION.x);
		__TO_RAD(ORIENTATION.y);
		__TO_RAD(ORIENTATION.z);
	}

	return NULL;
}

int smashImuInit(const char* dev){
	int ret;
	IMU_FD = atOpen(dev, 57600);
	atConfig(IMU_FD, AT_BIN | AT_NCHKSUM);

	sleep(1);

	atWrite(IMU_FD, "#ob", 3); // Turn on binary output
	atWrite(IMU_FD, "#o1", 3); // Turn on continuous streaming output
	atWrite(IMU_FD, "#oe0", 4); // Disable error message output

	tcdrain(IMU_FD);	
	//tcflush(IMU_FD, TCIFLUSH);
	atWrite(IMU_FD, "#s", 2);
	tcflush(IMU_FD, TCIFLUSH);
	atWrite(IMU_FD, "00", 2);

	while(!synched){
		write(1, ".", 1);
		sleep(1);	
	
		// sync 
		if(!smashImuSync("#SYNCH00\r\n")){
			write(1, ".", 1);
			atWrite(IMU_FD, "#s", 2);
			tcflush(IMU_FD, TCIFLUSH);
			atWrite(IMU_FD, "00", 2);
			continue;
		}
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

