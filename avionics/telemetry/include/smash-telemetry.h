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

int  smashTelemetryInit(const char* dev, int addr);
void smashTelemetryShutdown(int fd);

void smashTelSendThrottles  (int fd, RotorStates       throttles);
void smashTelSendOrientation(int fd, OrientationStates orientation);
void smashTelSendLocation   (int fd, LocationStates    location);

void smashTelRecieveThrottles  (int fd, RotorStates       throttles);
void smashTelRecieveElevation  (int fd, byte*             elevation);

#endif
