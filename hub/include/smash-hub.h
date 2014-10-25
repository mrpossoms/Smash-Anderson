#ifndef __smash_hub_smash_hub 
#define __smash_hub_smash_hub

#include <unistd.h>
#include <stdio.h>
#include <libNEMA.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SMASH_HUB_MSK_IMU   0x01
#define SMASH_HUB_MSK_TELEM 0x02
#define SMASH_HUB_MSK_SPEED 0x04
#define SMASH_HUB_MSK_GPS   0x08
#define SMASH_HUB_MSK       0x0F

#define SMASH_IMU_I    0
#define SMASH_TELEM_I  1
#define SMASH_SPEED_I  2
#define SMASH_GPS_I    3
#define SMASH_HUB_I    4

#define SMASH_SUBSYSTEMS    SMASH_IMU_I  + SMASH_TELEM_I  + SMASH_SPEED_I  + SMASH_GPS_I  + SMASH_HUB_I  + 1

// smash 001
#define SMASH_SHM_KEY 5345001

struct SmashState{
	int8_t   subSystemLife[SMASH_SUBSYSTEMS];   
	int32_t  subSystemShouldShutdown;
	int32_t  subSystemOnline; // bit mask collection indicating which systems are functioning
	float    imuAngles[3];    // orientation
	float    imuOmegas[3];    // angular accelerations
	GpsState gpsState;        // gps location fix and status
	unsigned char     speedTargets[4]; // target rotor speeds
};

static inline void* createAndAttach(key_t key)
{
	void* data = NULL;
	int shmid = 0;

	if ((shmid = shmget(key, sizeof(struct SmashState), IPC_CREAT | 0666)) < 0) {
		perror("shmget");
		return NULL;
	}

	if ((data = shmat(shmid, NULL, 0)) == (void *) -1) {
        perror("shmat");
        return NULL;
    }

    return data;
}

#endif
