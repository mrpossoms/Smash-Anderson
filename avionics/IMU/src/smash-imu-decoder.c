#include <stdio.h>
#include <string.h>
#include "smash-imu-decoder.h"

int smashImuDecodeQuaternion(unsigned char* buf, float* Q){
	char *delim = ", ";
	char *brkt, *word;
	char temp[1024];
	int i = 0;

	// make a copy of the buffer, then get the first word.
	strcpy(temp, buf);
	word = strtok(temp, delim); 

	do{
		int temp = 0;

		// for now, read only a single quaternion
		if(i >= 4) return 0;

		// decode the hex string for this float
		sscanf(word, "%x", &temp);

		temp = ((temp>>24)&0xff) |    // move byte 3 to byte 0
		       ((temp<<8)&0xff0000) | // move byte 1 to byte 2
		       ((temp>>8)&0xff00) |   // move byte 2 to byte 1
		       ((temp<<24)&0xff000000);
	
		memcpy(&Q[i++], &temp, sizeof(float));

	}while(word = strtok(NULL, delim));
		
	return -1; // ran out of data, before a quaternion was read
}
