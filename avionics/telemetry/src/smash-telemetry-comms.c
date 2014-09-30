#include "smash-telemetry.h"
#include <ardutalk.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#define DELAY 18000

void __dump(char* buf, int len){
	int i = 0;	
	for(i = 0; i < len; i++){
		char str[16];
		sprintf(str, "%02x ", buf[i] & 0xff);
		write(1, str, strlen(str));
	}write(1, "\n", 1);
}
//-----------------------------------------------------------------------
void smashTelemetryShutdown(int fd){
	close(fd);
}
//-----------------------------------------------------------------------
int smashReceiveMsg(int fd, byte* type, void* msg){
	size_t msgSize = 0;
	byte   msgType = *type;

	// wait for the peer to indicate what message is incomming
	int result = atRead(fd, type, sizeof(byte));

	switch((int)msgType){
		case MSG_CODE_THROTTLE:
			msgSize = sizeof(RotorStates);
			break;
		case MSG_CODE_STATUS:
			msgSize = sizeof(struct SmashState);
			break;
		case MSG_CODE_STATUS_REQ:
			msgSize = 0;
		case MSG_CODE_DATA:
			msgSize = sizeof(struct SmashData);
			break;
		default:
			if(result > 0){
				return TELEM_ERR_BAD_CODE;
			}
			else{
				return TELEM_ERR_BAD_CODE & TELEM_ERR_TIMEOUT;
			}
	}

	if(msgSize){
		// read the expected message
		result = atRead(fd, msg, msgSize);

		// check the message size
		if(result != msgSize){
			return TELEM_ERR_BAD_MSG & TELEM_ERR_TIMEOUT;
		}
	}

	// everything is ok to this point, ack the message
	msgType |= MSG_CODE_ACK;
	atWrite(fd, &msgType, sizeof(byte));

	return result;
}
//-----------------------------------------------------------------------
int smashSendMsg(int fd, byte type, void* msg){
	int    result  = 0;
	byte   msgType = type;
	byte   ackType = 0;
	size_t msgSize = 0;

	// tell the peer what kind of message is on it's way
	result = atWrite(fd, &msgType, sizeof(byte));	

	switch(msgType){
		case MSG_CODE_THROTTLE:
			msgSize = sizeof(RotorStates);
			break;
		case MSG_CODE_STATUS:
			msgSize = sizeof(struct SmashState);
			break;
		case MSG_CODE_STATUS_REQ:
			msgSize = 0;
		case MSG_CODE_DATA:
			msgSize = sizeof(struct SmashData);
			break;
		default:
			if(result > 0){
				return TELEM_ERR_BAD_CODE;
			}
			else{
				return TELEM_ERR_BAD_CODE & TELEM_ERR_TIMEOUT;
			}
	}

	if(msgSize){
		// send the message
		atWrite(fd, msg, msgSize);
	}

	// read an ack from the peer
	result = atRead(fd, &ackType, sizeof(byte));

	// ensure the ack recieved was for the expected message
	// and confirm that it was indeed an ack
	if(msgType & ackType && ackType & MSG_CODE_ACK){
		return result;
	}
	
	return TELEM_ERR_MSG_NACK;
}
