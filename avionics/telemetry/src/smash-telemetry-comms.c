#include "smash-telemetry.h"
#include <ardutalk.h>
#include <unistd.h>

#define DELAY 15000

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
