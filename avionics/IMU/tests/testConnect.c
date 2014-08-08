#include "smash-imu.h"
#include <indicurses.h>
#include <stdio.h>

void drawDials(float* ori){
	int hx = IC_TERM_WIDTH >> 1, hy = IC_TERM_HEIGHT >> 1;
	int qx = hx >> 1;
	
	clear();
	icCurrentChar = '.';

	icCurrentChar = 'o';
	icDial(qx, hy, 10, ori[0]);
	icDial(hx, hy, 10, ori[1]);
	icDial(hx + qx, hy, 10, ori[2]);
		
	icPresent();
}

void onChange(float* ori){
	int i = 3;
	char buf[128];
	for(;i--;){
		if(ori[i] > 4.0f || ori[i] < -4.0f){
			sprintf(buf, ">>%f, %f, %f<<\n", ori[0], ori[1], ori[2]);
			write(1, buf, strlen(buf));
			//sleep(1);
			return;
		}
	}

	drawDials(ori);
}

int main(int argc, char* argv[]){
	if(argc != 2) return 1;
	smashImuInit(argv[1], onChange);
	icInit();

	while(1){
		//drawDials();

		usleep(10000);	
	}

	return 0;
}
