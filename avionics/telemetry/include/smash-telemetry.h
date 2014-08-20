#ifndef SMASH_TELEM
#define SMASH_TELEM

#define MSG_CODE_THROTTLE 0x01
#define	MSG_CODE_STATUS   0x02

typedef unsigned char byte;

typedef byte  OrientationStates[3];
typedef byte  RotorStates[4];
typedef float LocationStates[3];

typedef struct{
	float       yawPitchRoll[3];
	float       latLongAlt[3];
	RotorStates throttles;
} SmashStatusMsg;

int  smashTelemetryInit(const char* dev);
void smashTelemetryShutdown(int fd);

int smashTelSendStatus(int fd, SmashStatusMsg* status);

int smashReceiveCode(int fd, int* type);
int smashReceiveMsg (int fd, void* msg);

#endif
