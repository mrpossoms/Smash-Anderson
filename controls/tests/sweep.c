#include "smash-speed.h"
#include <math.h>

int main(){
	int fd = smashSpeedInit("/dev/servoblaster");
	float f = 0;

	if(fd <= 0) return;

	while(1){
		float t = (sin(f) + 1) * 128;
		unsigned char speeds[4] = {
			(unsigned char)t,		
			(unsigned char)t,		
			(unsigned char)t,		
			(unsigned char)t
		};
		f += 0.001f;

		smashSpeedSet(fd, speeds);
		usleep(10000);
	}

	return 0;
}
