
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <indicurses.h>
#include <libNEMA.h>
#include <ardutalk.h>
#include <Controls.h>
#include <smash-telemetry.h>
#include <assert.h>
#include "smash-hub.h"

#define GPS_DEV   "/dev/ttyAMA0"
//#define IMU_DEV   "/dev/ttyUSB0"
#define RADIO_DEV "/dev/ttyUSB1"
#define ROTORS_DEV "/dev/servoblaster"

static float YPR[3];
static struct SmashState* state = NULL;
static GpsState gps_st = {0};

void printGpsCoords(GpsState* state){
	char buf[256];

	sprintf(buf, "%f N", state->Lat);
	icText(2, 2, buf);
	sprintf(buf, "%f W", state->Lon);
	icText(2, 4, buf);
}

int main(int argc, const char* argv[]){
	int fd_radio = smashTelemetryInit(RADIO_DEV);

	assert(!icInit());

	// start servo driver and check the status
	printf("Preparing servo driver...");
	if(system("sh ./servo.sh")){
		printf("Error!\n");

		if(argc <= 1) return -1;
		else{
			printf("Ignoring servo driver startup.\n");
		}
	}
	printf("OK!\n");

	printf("Attaching to, or creating shared memory segment...");
	state = createAndAttach(SMASH_SHM_KEY);	
	if(state){
		printf("Attached!\n");
	}
	else{
		return -2;
	}

	while(1){
		int msgType = 0;
		char buf[128] = {0};

		clear();

		sprintf(buf, "ypr = ( %f, %f, %f )", state->imuAngles[0], state->imuAngles[1], state->imuAngles[2]);
		icText(2, 2, buf);

		if(!smashReceiveCode(fd_radio, &msgType)){
			//printf("Message type %d\n", msgType);

			switch(msgType){
				case MSG_CODE_THROTTLE:
					{
						RotorStates temp = {0};
						if(!smashReceiveMsg(fd_radio, &temp)){
							//memcpy(&rotor_st, &temp, sizeof(RotorStates));

							unsigned char t = 0;//rotor_st[0];
							printf("Rotors = {%d, %d, %d, %d}\n", (int)t, (int)t, (int)t, (int)t); 
						}
					}
					break;
				default:;
					//printf("Unrecognized message!\n");
			}
		} 	


		//smashSpeedSet(fd_rotors, rotor_st);
		/*if(lnReadMsg(gps_buf, 255)){
			lnParseMsg(&gps_st, gps_buf); 
		}*/

		if(gps_st.Fix)
			printGpsCoords(&gps_st);		

	
		usleep(10000);	
		icPresent();
	}
	
	printf("Done\n");

	return 0;
}
