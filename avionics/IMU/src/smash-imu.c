#include "smash-imu.h"
#include <ftdi.h>

struct ftdi_context *ftdi;

int __smashOpen(){
	struct ftdi_device_list* devList;

	if(!(ftdi = ftdi_new())){
		return -1; // allocating ftdi object failed
	}

	ftdi_set_interface(ftdi, INTERFACE_ANY);
	
	if(ftdi_usb_find_all(ftdi, &devList, 0, 0) < 0){
		return -2; // failed to find any ftdi devices
	}
	
	if(ftdi_usb_open_dev(ftdi, devList[0].dev) < 0){
		ftdi_list_free(&devList);
		return -3; // failed to open device
	}

	ftdi_list_free(&devList);

	if(ftdi_set_baudrate(ftdi, 115200) < 0){
		return -4; // failed to set baudrate
	}

	return 0;
}

int smashImuInit(){
	int ret = 0;

	if((ret = __smashOpen()) < 0){
		return ret; 
	}

	// TODO spin up the thread that polls the

	return 0;
}

int smashImuShutdown(){

}

