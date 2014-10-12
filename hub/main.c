
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include <indicurses.h>
#include <libNEMA.h>
#include <ardutalk.h>
#include <Controls.h>
#include <smash-telemetry.h>

#include "smash-hub.h"

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

void* commHandler(void* args)
{
	int fd_radio = *((int*)args);
	byte msgType;
	byte buf[128];

	while(1){
		smashReceiveMsg(fd_radio, &msgType, buf);
		//if(msgType == 0) continue;
		printf("Message type %x\n", msgType);

		switch(msgType){
			case MSG_CODE_THROTTLE:
				{
					RotorStates temp = {0};
					memcpy(buf, &temp, sizeof(RotorStates));

					sprintf(buf, "Rotors = {%d, %d, %d, %d}\n",
						(int)temp[0],
						(int)temp[1],
						(int)temp[2],
						(int)temp[3]
					); 
					icText(3,3,buf);
				}
				break;
			case MSG_CODE_STATUS_REQ:
				{
					struct SmashState tempState;
					memcpy(&tempState, state, sizeof(struct SmashState));
					tempState.imuAngles[0] = 13.37f;
					smashSendMsg(fd_radio, MSG_CODE_STATUS, &tempState);
				}
				break;
			default:;
				printf("Unrecognized message!\n");
		}
	}
}

int main(int argc, const char* argv[]){
	int fd_radio = 0;
	pthread_t commThread;

	if(argc != 2){
		printf("Missing radio device path parameter\n");
		return -1;
	}

	fd_radio = smashTelemetryInit(argv[1]);

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

	//assert(!icInit());

	pthread_create(&commThread, NULL, commHandler,&fd_radio);

	while(1){
		char buf[128];
		clear();
		sprintf(buf, 
			"ypr = ( %f, %f, %f )",
			state->imuAngles[0],
			state->imuAngles[1],
			state->imuAngles[2]
		);

		icText(2, 2, buf);
		 	
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
