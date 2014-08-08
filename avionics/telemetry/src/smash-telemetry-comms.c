#include "smash-telemetry.h"
#include <ardutalk.h>
#include <unistd.h>

#define DELAY 18000

void __dump(char* buf, int len){
	int i = 0;	
	for(i = 0; i < len; i++){
		char str[16];
		sprintf(str, "%02x ", buf[i]);
		write(1, str, strlen(str));
	}write(1, "\n", 1);
}

int __send(int fd, byte msgType, void* msg, size_t size){
	if(atWrite(fd, msg, size) != size){
		// ERROR
	}
	
	usleep(DELAY);

	return 0;
}
//-----------------------------------------------------------------------
int __receieve(int fd, byte msgType, void* msg, size_t size){
	// read the response message
	if(atRead(fd, msg, size) < size){
		// ERROR
		lseek(fd, 0, SEEK_END);
		return -2;
	}

	usleep(DELAY);
	
	return 0;
}
//-----------------------------------------------------------------------
int smashTelSendThrottles(int fd, RotorStates throttles){
	return __send(
		fd,
		MSG_TYPE_ROTORS,
		(void*)throttles,
		sizeof(RotorStates)
	);
}
//-----------------------------------------------------------------------
int smashTelSendOrientation(int fd, OrientationStates orientation){
	return __send(
		fd,
		MSG_TYPE_ORI,
		(void*)orientation,
		sizeof(OrientationStates)
	);
}
//-----------------------------------------------------------------------
int smashTelSendLocation(int fd, LocationStates location){
	return __send(
		fd,
		MSG_TYPE_LOC,
		(void*)location,
		sizeof(LocationStates)
	);
}
//-----------------------------------------------------------------------
int smashTelRecieveThrottles(int fd, RotorStates throttles){
	return __receieve(
		fd,
		MSG_TYPE_ROTORS,
		(void*)throttles,
		sizeof(RotorStates)
	);
}
//-----------------------------------------------------------------------
int smashTelRecieveElevation(int fd, byte* elevation){
	return __receieve(
		fd,
		MSG_TYPE_ELEVATION,
		(void*)elevation,
		sizeof(byte)
	);
}
