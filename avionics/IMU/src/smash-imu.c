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
#define VEC6 sizeof(float) * 6


const float DEG_TO_RAD = (M_PI / 180.0f);

pthread_t IMU_THREAD;
int IS_POLLING_IMU = 0;
int IMU_FD = -1;
int synched = 0;
float ORIENTATION[6];
void (*changeCallback)(float*);

void endianSwap(float* f){
	uint32_t i = ntohl(*((uint32_t*)f));
	memcmp(f, &i, sizeof(float));
}

int smashImuSync(const char* token){
	int len = strlen(token), off = 0;
	char received;
	unsigned char timeOut = 0;

	while(1){
		usleep(10000);
		atRead(IMU_FD, &received, 1);
		if(received == token[off]){
			++off; 
			timeOut = 0;

#ifdef __IMU_DEBUG
			write(1, &received, 1);
#endif

			if(off == len){
				return 0;
			}

			continue;
		}

 		++timeOut;
		off = 0;

#ifdef __IMU_DEBUG
		printf("%u\n", timeOut);
#endif

		if(timeOut > len){
#ifdef __IMU_DEBUG
			write(1, ".", 1);
#endif
			return 1;
		}
	}
}

void* __IMUpoller(void* params){
	int i = 0, bytes = 0;
	float temp[6];

	while(IS_POLLING_IMU){
		
		if(atAvailable(IMU_FD) < VEC6){
			usleep(1000);
			continue;
		}

		if((bytes = atRead(IMU_FD, temp, VEC6)) == VEC6){
			memcpy(ORIENTATION, temp, VEC6);

			ORIENTATION[0] *= DEG_TO_RAD;
			ORIENTATION[1] *= DEG_TO_RAD;
			ORIENTATION[2] *= DEG_TO_RAD;

#ifdef __IMU_DEBUG
			printf("OK %d (%f, %f, %f) w(%f, %f, %f)\n", bytes, ORIENTATION[0], ORIENTATION[1], ORIENTATION[2], ORIENTATION[3], ORIENTATION[4], ORIENTATION[5]);
#endif

			changeCallback(ORIENTATION);
		}
		else{
#ifdef __IMU_DEBUG
			printf("Read %d (%f, %f, %f)\n", bytes, ORIENTATION[0], ORIENTATION[1], ORIENTATION[2]);
#endif
		}
	}

	return NULL;
}

int smashImuInit(const char* dev, void (*onChange)(float*)){
	int ret;
	unsigned char cbuf[] = {0x06, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xb1, 0x18, 0x00, 0x00, 0x30, 0x8a, 0x00, 0x00, 0x00, 
	0x03, 0x1c, 0x7f, 0x08, 0x04, 0x02, 0x64, 0x00, 0x11, 0x13, 0x1a, 0x00, 0x12, 0x0f, 0x17, 0x16, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x85, 0x00, 0x00, 0x01, 0x10, 0x00, 0x00, 0x01, 0x10, 0x00, 0x00};

	IMU_FD = atOpen(dev, 57600);
	tcsetattr(IMU_FD, TCSANOW, (struct termios*)cbuf);	

	atConfig(IMU_FD, AT_BIN | AT_NCHKSUM);

	assert((changeCallback = onChange) != NULL);

	sleep(3);

	do{

		atWrite(IMU_FD, "#ob",  3); // Turn on binary output
		atWrite(IMU_FD, "#o1",  3); // Turn on continuous streaming output
		atWrite(IMU_FD, "#o2",  3); // Turn on inclusion of omega
		atWrite(IMU_FD, "#oe0", 4); // Disable error message output
		
		tcflush(IMU_FD, TCIFLUSH);
		atWrite(IMU_FD, "#s00", 4);
		
	// sync 
	}while(smashImuSync("#SYNCH00\r\n"));
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

