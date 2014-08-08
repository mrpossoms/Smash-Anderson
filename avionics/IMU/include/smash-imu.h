#ifndef SMASH_IMU
#define SMASH_IMU

#ifdef __cplusplus
extern "C"{
#endif 
		
	extern float ORIENTATION[6];

	int smashImuInit(const char* dev, void (*onChange)(float*));
	int smashImuShutdown();

#ifdef __cplusplus
}
#endif 

#endif
