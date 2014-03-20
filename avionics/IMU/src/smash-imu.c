#include "smash-imu.h"
#include "smash-imu-decoder.h"
#include <ftdi.h>
#include <pthread.h>
#include <math.h>
#include <string.h>

#define __IMU_DEBUG

pthread_t IMU_THREAD;
struct ftdi_context *ftdi;
int IS_POLLING_IMU = 0;

Vec3 ORIENTATION;

void print(const char* str){
	write(1, str, strlen(str));
}

int __smashOpen(void){
	struct ftdi_device_list* devList;

	if(!(ftdi = ftdi_new())){
		return -1; // allocating ftdi object failed
	}

	ftdi_set_interface(ftdi, INTERFACE_ANY);
	
	if(ftdi_usb_find_all(ftdi, &devList, 0, 0) < 0){
		return -2; // failed to find any ftdi devices
	}

#ifdef __IMU_DEBUG
	{
		char manufacturer[128], description[128];
		ftdi_usb_get_strings(ftdi, devList->dev, manufacturer, 128, description, 128, NULL, 0);
		print(manufacturer); print(" "); print(description); print("\n");
	}
#endif
	
	if(ftdi_usb_open_dev(ftdi, devList[0].dev) < 0){
		ftdi_list_free(&devList);
		return -3; // failed to open device
	}

	ftdi_list_free(&devList);

	if(ftdi_set_baudrate(ftdi, 115200) < 0){
		return -4; // failed to set baudrate
	}


	if(ftdi_set_line_property(ftdi, 8, STOP_BIT_1, NONE) < 0){
		return -5; // failed to set byte format
	}

	return 0;
}

void* __IMUpoller(void* params){
	unsigned char ftdiBuf[1024], imuBuf[1024];
	int bytes = 0, bufOff = 0;
	int Qsize = 65;//(sizeof(float) * 2 + 1) * 6;
	float Q[4] = {0};
	char burst = 1;

#ifdef __IMU_DEBUG
	print("Polling thread started!\n");
	print("+\n");
#endif
	while(1){//IS_POLLING_IMU){
		
		if(bufOff == 0){
			ftdi_write_data_submit(ftdi, "q", 1);
			ftdi_write_data_submit(ftdi, &burst, 1);
		}	
	
#ifdef __IMU_DEBUG
		print("writing query\n");
#endif
		usleep(100000); // wait a bit for the message
		
		if((bytes = ftdi_read_data(ftdi, ftdiBuf, Qsize)) > 0){
			memcpy(imuBuf + bufOff, ftdiBuf, bytes);
			bufOff += bytes;
			
#ifdef __IMU_DEBUG
			{
				char temp[128];
				sprintf(temp, "\nOff %d\n", bufOff);
				print(temp);
			}			
#endif

			usleep(4000); // wait a bit for the message
		}

		if(bufOff >= Qsize){
#ifdef __IMU_DEBUG
			print(imuBuf);print("\n");
#endif

			smashImuDecodeQuaternion(imuBuf, Q); 

			ORIENTATION.x = atan2(2 * Q[1] * Q[2] - 2 * Q[0] * Q[3], 2 * Q[0]*Q[0] + 2 * Q[1] * Q[1] - 1); // psi
			ORIENTATION.y = -asin(2 * Q[1] * Q[3] + 2 * Q[0] * Q[2]); // theta
			ORIENTATION.z = atan2(2 * Q[2] * Q[3] - 2 * Q[0] * Q[1], 2 * Q[0] * Q[0] + 2 * Q[3] * Q[3] - 1); // phi
			printf("{%f, %f, %f}\n", ORIENTATION.x, ORIENTATION.y, ORIENTATION.z);
			bzero(imuBuf, 1024);
			bufOff = 0; 
				
			ftdi_usb_purge_buffers(ftdi);
		}
	}

#ifdef __IMU_DEBUG
	print("Poller exiting");
#endif
	return NULL;
}

int smashImuInit(){
	int ret = 0;

	if((ret = __smashOpen()) < 0){
		return ret; 
	}

	// spin up the thread that polls the IMU
	IS_POLLING_IMU = 1;
	if((ret = pthread_create(&IMU_THREAD, NULL, __IMUpoller, NULL)) < 0){
		IS_POLLING_IMU = 0;
		return ret;
	}

	return 0;
}

int smashImuShutdown(){

}

