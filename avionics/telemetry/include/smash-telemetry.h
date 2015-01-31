#ifndef SMASH_TELEM
#define SMASH_TELEM

#include "smash-hub.h"
#include <ardutalk.h>

// message codes, LSb is 0. Reserved as ACK flag
#define MSG_CODE_THROTTLE   0xC2
#define	MSG_CODE_STATUS     0xCE
#define MSG_CODE_STATUS_REQ 0x6A
#define MSG_CODE_DATA       0xF4
#define MSG_CODE_ACK        0x01

#define TELEM_ERR_BAD_CODE ~0x00000001
#define TELEM_ERR_TIMEOUT  ~0x00000002
#define TELEM_ERR_BAD_MSG  ~0x00000004
#define TELEM_ERR_MSG_NACK ~0x00000008

typedef unsigned char byte;

typedef byte  OrientationStates[3];
typedef byte  RotorStates[4];
typedef float LocationStates[3];

struct SmashData{
	unsigned char len;
	byte buf[128];
};

int  smashTelemetryInit(const char* dev);
void smashTelemetryShutdown(int fd);

int smashReceiveMsg (int fd, byte* type, void* msg);
int smashSendMsg(int fd, byte type, void* msg);

#endif
