#ifndef SMASH_IMU
#define SMASH_IMU
	
extern float ORIENTATION[6];

int smashImuInit(const char* dev, void (*onChange)(float*));
int smashImuShutdown();
#endif
