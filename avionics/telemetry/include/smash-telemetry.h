#ifndef SMASH_TELEM
#define SMASH_TELEM

#define MSG_TYPE_ROTORS 0
#define MSG_TYPE_ORI    1
#define MSG_TYPE_LOC    2

typedef unsigned char OrientationStates[3];
typedef unsigned char RotorStates[4];
typedef float         LocationStates[3];

int  smashTelemetryInit(const char* dev, int addr);
void smashTelemetryShutdown(int fd);

#endif
