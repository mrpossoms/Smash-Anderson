#include "smash-speed.h"
#include <math.h>

const char PINS[] =  " --p1pins=11,12,15,16";
const char CONFIG[] =  " --cycle-time=30000us --min=1500us --max=2500us";

int main(){
	const char cmd[] = "servod --p1pins=11,12,15,16";// --cycle-time=30000us --min=1500us --max=25000us";
	float f = 0;
	int i = 0;
	unsigned char speeds[4] = { 18, 18, 18, 18 };
	
	printf("About to run %s\n", cmd);
        if(system(cmd)){
                printf("Error! Failed to start servo driver daemon\n");
                return -2;
        }
        printf("Driver started!\n");
	int fd = smashSpeedInit("/dev/servoblaster");
	if(fd <= 0) return;
	smashSpeedSet(fd, speeds);

	while(1){
/*
		float t = (sin(f) + 1) * 128;
		unsigned char speeds[4] = {
			(unsigned char)t,		
			(unsigned char)t,		
			(unsigned char)t,		
			(unsigned char)t
		};
		f += 0.001f;

		printf("%f\n", t);
*/
		unsigned int t, i;

		scanf("%u", &t);
		for(i = 0; i < 4; ++i)
		speeds[i] = t;	

		smashSpeedSet(fd, speeds);
		usleep(10000);
	}

	return 0;
}
