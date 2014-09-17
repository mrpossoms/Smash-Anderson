#include "smash-telemetry.h"
#include <ardutalk.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#define DELAY 18000

int  __readyForMsg = 0;
byte __msgCode;

void __dump(char* buf, int len){
	int i = 0;	
	for(i = 0; i < len; i++){
		char str[16];
		sprintf(str, "%02x ", buf[i] & 0xff);
		write(1, str, strlen(str));
	}write(1, "\n", 1);
}

int __send(int fd, void* msg, size_t size){
	if(atWrite(fd, msg, size) != size){
		// ERROR
	}
	
	usleep(DELAY);

	return 0;
}
//-----------------------------------------------------------------------
int __receieve(int fd, void* msg, size_t size){
	// read the response message
	int read = atRead(fd, msg, size);
	if(read < 0 || read < size){
		// ERROR
		lseek(fd, 0, SEEK_END);
		return -2;
	}
	
	usleep(DELAY);
	
	return 0;
}
//-----------------------------------------------------------------------
void smashTelemetryShutdown(int fd){
	close(fd);
}
//-----------------------------------------------------------------------
int smashSendStatus(int fd, struct SmashState* status){
	int result = 0;
	int statusCode = MSG_CODE_STATUS;

	// inform the receiver of the message type
	if((result = __send(fd, &statusCode, sizeof(int)))){
		return result;
	}

	// send the status message itself
	if((result = __send(fd, status, sizeof(struct SmashState)))){
		return result;
	}

	return 0;
}
//-----------------------------------------------------------------------
int smashReceiveCode(int fd, byte* type){
	atPrepare(fd, sizeof(byte));
	if(__receieve(fd, type, sizeof(byte))){
		return -1;
	}

	__msgCode = *type;
	__readyForMsg = 1;

	return 0;
}
//-----------------------------------------------------------------------
int smashReceiveMsg (int fd, void* msg){
	int result = 0;
	if(!__readyForMsg) return -1;

	switch(__msgCode){
		case MSG_CODE_THROTTLE:
			result = __receieve(fd, msg, sizeof(RotorStates));
			break;
		case MSG_CODE_STATUS:
			result = __receieve(fd, msg, sizeof(struct SmashState));
			break;
	}

	__readyForMsg = 0;

	return result;
}
//-----------------------------------------------------------------------
int smashSendMessage(int fd, int type, void* msg){
	int result = 0;

	if(__send(fd, &type, sizeof(int))){
		return -1;
	}

	switch(type){
		case MSG_CODE_THROTTLE:
			result = __send(fd, msg, sizeof(RotorStates));
			break;
		case MSG_CODE_STATUS:
			result = __send(fd, msg, sizeof(struct SmashState));
			break;	
	}

	return result;
}
