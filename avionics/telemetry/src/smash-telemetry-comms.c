#include "smash-telemetry.h"
#include <unistd.h>

#define DELAY 15000

int __send(int fd, byte msgType, void* msg, size_t size){
	int msgSize = 0;
	byte buf[msgSize = (size + 1)];

	buf[0] = MSG_TYPE_ROTORS;
	memcpy(buf + 1, msg, size);
	if(write(fd, buf, msgSize) != msgSize){
		// ERROR
	}
	
	usleep(DELAY);

	return 0;
}
//-----------------------------------------------------------------------
int __receieve(int fd, byte msgType, void* msg, size_t size){
	if(write(fd, &msgType, 1) != 1){
		// ERROR
		return -11;
	}
	usleep(DELAY);

	if(read(fd, msg, size) < size){
		// ERROR
		return -2;
	}
	usleep(DELAY);
	
	return 0;
}
//-----------------------------------------------------------------------
void smashTelSendThrottles(int fd, RotorStates throttles){
	__send(
		fd,
		MSG_TYPE_ROTORS,
		(void*)throttles,
		sizeof(RotorStates)
	);
}
//-----------------------------------------------------------------------
void smashTelSendOrientation(int fd, OrientationStates orientation){
	__send(
		fd,
		MSG_TYPE_ORI,
		(void*)orientation,
		sizeof(OrientationStates)
	);
}
//-----------------------------------------------------------------------
void smashTelSendLocation(int fd, LocationStates location){
	__send(
		fd,
		MSG_TYPE_LOC,
		(void*)location,
		sizeof(LocationStates)
	);
}
//-----------------------------------------------------------------------
void smashTelRecieveThrottles(int fd, RotorStates throttles){
	__receieve(
		fd,
		MSG_TYPE_ROTORS,
		(void*)throttles,
		sizeof(RotorStates)
	);
}
//-----------------------------------------------------------------------
void smashTelRecieveElevation(int fd, byte* elevation){
	__receieve(
		fd,
		MSG_TYPE_ELEVATION,
		(void*)elevation,
		sizeof(byte)
	);
}
