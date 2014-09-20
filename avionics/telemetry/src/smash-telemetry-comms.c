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
	int wrote = atWrite(fd, msg, size);

	//printf("wrote %d\n", wrote);
	if(wrote != size){
		// ERROR
	}
	
	usleep(DELAY);

	return wrote;
}
//-----------------------------------------------------------------------
int __receieve(int fd, void* msg, size_t size){
	// read the response message
	if(atAvailable(fd) < size)
		return -1;

	int read = atRead(fd, msg, size);
	
	usleep(DELAY);
	
	return read;
}
//-----------------------------------------------------------------------
void smashTelemetryShutdown(int fd){
	close(fd);
}
//-----------------------------------------------------------------------
int smashRequestStatus(int fd){
	int result = 0;
	byte statusCode = MSG_CODE_STATUS;

	// inform the receiver of the message type
	if((result = __send(fd, &statusCode, sizeof(byte)))){
		__msgCode = MSG_CODE_STATUS;
		__readyForMsg = 1;
		//printf("Code error\n");
		return result;
	}
	return 0;
}
//-----------------------------------------------------------------------
int smashSendStatus(int fd, struct SmashState* status){
	int result = 0;
	byte statusCode = MSG_CODE_STATUS;

	// inform the receiver of the message type
	if((result = __send(fd, &statusCode, sizeof(byte)))){
		__readyForMsg = 0;
		//printf("Code error\n");
		return result;
	}

	// send the status message itself
	if((result = __send(fd, status, sizeof(struct SmashState)))){
		__readyForMsg = 0;
		//printf("Msg error\n");
		return result;
	}

	__readyForMsg = 0;

	return 0;
}
//-----------------------------------------------------------------------
int smashReceiveCode(int fd, byte* type){
	//printf("rfm %d ", __readyForMsg);
	if(__readyForMsg) return 0;

	atPrepare(fd, sizeof(byte));
	if(__receieve(fd, type, sizeof(byte)) <= 0){
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
			atPrepare(fd, sizeof(RotorStates));
			result = __receieve(fd, msg, sizeof(RotorStates));
			break;
		case MSG_CODE_STATUS:
			atPrepare(fd, sizeof(struct SmashState));
			result = __receieve(fd, msg, sizeof(struct SmashState));
			break;
		case MSG_CODE_DATA:
			atPrepare(fd, sizeof(struct SmashData));
			result = __receieve(fd, msg, sizeof(struct SmashData));
	}

	if(result <= 0) return -2;

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
