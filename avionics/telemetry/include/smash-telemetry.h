#ifndef SMASH_TELEM
#define SMASH_TELEM

#define MSG_TYPE_ROTORS    0
#define MSG_TYPE_ORI       1
#define MSG_TYPE_LOC       2
#define MSG_TYPE_ELEVATION 3

typedef unsigned char byte;

typedef byte  OrientationStates[3];
typedef byte  RotorStates[4];
typedef float LocationStates[3];


union SmashMessage{
	OrientationStates orientation,
	RotorStates       throttles,
	LocationStates    location,
	byte              elevation
};

int  smashTelemetryInit(const char* dev);
void smashTelemetryShutdown(int fd);

int  smashTelSendThrottles  (int fd, RotorStates       throttles);
int  smashTelSendOrientation(int fd, OrientationStates orientation);
int  smashTelSendLocation   (int fd, LocationStates    location);

int  smashTelRecieveThrottles  (int fd, RotorStates    throttles);
int  smashTelRecieveElevation  (int fd, byte*          elevation);

#endif
