#ifndef SMASH_IMU
#define SMASH_IMU

#define VEC3 sizeof(float) * 3
#define VEC6 sizeof(float) * 6

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
