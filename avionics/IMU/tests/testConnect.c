#include "smash-imu.h"
#include <indicurses.h>

int main(void){
	smashImuInit();
	icInit();

	while(1){
		int hx = IC_TERM_WIDTH >> 1, hy = IC_TERM_HEIGHT >> 1;
		int qx = hx >> 1;

		clear();
		icCurrentChar = '.';

		icCurrentChar = 'o';
		icDial(qx, hy, 10, ORIENTATION.x);
		icDial(hx, hy, 10, ORIENTATION.y);
		icDial(hx + qx, hy, 10, ORIENTATION.z);
		
		usleep(1000);	
		icPresent();
//		printf("%f, %f, %f\n", ORIENTATION.x, ORIENTATION.y, ORIENTATION.z);
	}

	return 0;
}
