#ifndef SMASH_IMU
#define SMASH_IMU
	
extern float ORIENTATION[3];

int smashImuInit(const char* dev, void (*onChange)(float*));
int smashImuShutdown();
#endif
