#ifndef SMASH_TELEM
#define SMASH_TELEM

#include "smash-hub.h"

#define MSG_CODE_THROTTLE 0x61
#define	MSG_CODE_STATUS   0x67
#define MSG_CODE_DATA     0x7A

typedef unsigned char byte;

typedef byte  OrientationStates[3];
typedef byte  RotorStates[4];
typedef float LocationStates[3];

struct SmashData{
	unsigned char len;
	byte buf[256];
};

int  smashTelemetryInit(const char* dev);
void smashTelemetryShutdown(int fd);

int smashRequestStatus(int fd);
int smashSendStatus(int fd, struct SmashState* status);

int smashReceiveCode(int fd, byte* type);
int smashReceiveMsg (int fd, void* msg);

int smashSendMessage(int fd, int type, void* msg);

#endif
