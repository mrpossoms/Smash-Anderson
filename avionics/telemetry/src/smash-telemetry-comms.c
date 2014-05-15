#include "smash-telemetry.h"
#include <unistd.h>

#define DELAY 15000

byte __chksum(byte* data, size_t len){
	byte sum = 0;
	for(;len--;){
		sum ^= data[len];
	}

	return sum;
}

int __send(int fd, byte msgType, void* msg, size_t size){
	int msgSize = 0;
	byte buf[msgSize = (size + 2)];

	// setup the message packet, including the
	// message type, data and checksum
	buf[0] = MSG_TYPE_ROTORS;
	memcpy(buf + 1, msg, size);
	(buf + 1)[size] = __chksum((byte*), size);

	if(write(fd, buf, msgSize) != msgSize){
		// ERROR
	}
	
	usleep(DELAY);

	return 0;
}
//-----------------------------------------------------------------------
int __receieve(int fd, byte msgType, void* msg, size_t size){
	byte buf[size + 1];
	if(write(fd, &msgType, 1) != 1){
		// ERROR
		return -1;
	}
	usleep(DELAY);

	// read the response message
	if(read(fd, buf, size) < size){
		// ERROR
		return -2;
	}

	// recalculate the checksum, check against
	// the transmitted checksum
	if(__chksum(buf, size) != buf[size]){
		return -3;
	}

	// copy the verified message to the destination
	memcpy(msg, buf, size);
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
