#include "smash-imu.h"

int main(void){
	smashImuInit("/dev/ttyUSB0");

	while(1){
		
		usleep(10000);	
		printf("%f, %f, %f\n", ORIENTATION.x, ORIENTATION.y, ORIENTATION.z);

	}

	return 0;
}
