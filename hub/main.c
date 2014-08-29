#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <smash-imu.h>
#include <libNEMA.h>
#include <ardutalk.h>
#include <Controls.h>
#include <smash-telemetry.h>

#define GPS_DEV "/dev/ttyAMA0"
#define IMU_DEV "/dev/ttyUSB0"

void printGpsCoords(GpsState* state){
	char buf[256];

	sprintf(buf, "%f N", state->Lat);
	icText(2, 2, buf);
	sprintf(buf, "%f W", state->Lon);
	icText(2, 4, buf);
}

int main(int argc, const char* argv[]){
	int fd_gps    = -1; //lnConnect(GPS_DEV, B57600);
	int fd_rotors = -1;
	int fd_radio  = smashTelemetryInit("/dev/ttyUSB0");
	char gps_buf[255] = {0};	
	GpsState gps_st = {0};
	RotorStates rotor_st = {0};

	// start servo driver and check the status
	printf("Preparing servo driver...");
	if(system("sh ./servo.sh")){
		printf("Error!\n");
		return -1;
	}
	printf("OK!\n");

	smashImuInit(IMU_DEV);
	//fd_gps    = lnConnect(GPS_DEV, B57600);
	fd_rotors = smashSpeedInit("/dev/servoblaster");	

	while(1){
		int msgType = -1;
		if(smashReceiveCode(fd_radio, &msgType)){
			usleep(10000);

			printf("Message type %d\n", msgType);

			switch(msgType){
				case MSG_CODE_THROTTLE:
					{
						RotorStates temp = {0};
						if(!smashReceiveMsg(fd_radio, &temp)){
							memcpy(&rotor_st, &temp, sizeof(RotorStates));
						}
					}
					break;
				default:
					printf("Unrecognized message!\n");
			}
		} 	
	
		unsigned char t = rotor_st[0];
		printf("Rotors = {%d, %d, %d, %d}\n", (int)t); 

		smashSpeedSet(fd_rotors, rotor_st);
		/*if(lnReadMsg(gps_buf, 255)){
			lnParseMsg(&gps_st, gps_buf); 
		}*/

		if(gps_st.Fix)
			printGpsCoords(&gps_st);		

	
		usleep(10000);	
	}
	
	printf("Done\n");

	return 0;
}
