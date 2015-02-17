
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include <indicurses.h>
#include <libNEMA.h>
#include <ardutalk.h>
#include "smash-telemetry.h"

#include "smash-hub.h"

#define USE_INDICURSES

static float YPR[3];
static struct SmashState* state = NULL;
static struct GpsHandler gps_st = {0};

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

//	assert(0);

	while(1){
		char MSG_TYPE_BUF[128];
		
#ifndef USE_INDICURSES
		write(1, ".", 1);
		smashReceiveMsg(fd_radio, &msgType, buf);
#endif
		sprintf(MSG_TYPE_BUF, "Message type %x\n", msgType);
		icText(1, 1, MSG_TYPE_BUF);

		switch(msgType){
			case MSG_CODE_THROTTLE:
				{
					RotorStates temp = {0};
					memcpy(state->speedTargets, buf, sizeof(state->speedTargets));
					sprintf(buf, "Rotors = {%d, %d, %d, %d}\n",
						(int)state->speedTargets[0],
						(int)state->speedTargets[1],
						(int)state->speedTargets[2],
						(int)state->speedTargets[3]
					);
					icText(2,3,buf);
				}
				break;
			case MSG_CODE_STATUS_REQ:
				{
					struct SmashState tempState;
					memcpy(&tempState, state, sizeof(struct SmashState));
					smashSendMsg(fd_radio, MSG_CODE_STATUS, &tempState);
					icText(2, 3, "STAT REQ");
				}
				break;
#ifndef USE_INDICURSES
			default:
				printf("Unrecognized message!\n");
#endif
		}
	}

	return NULL;
}

int main(int argc, const char* argv[]){
	int fd_radio = 0;
	pthread_t commThread;

	if(argc != 2){
		printf("Missing radio device path parameter\n");
		return -1;
	}

	fd_radio = smashTelemetryInit(argv[1]);
	AT_RXTX_SCRAM = 0;
	
	printf("Attaching to, or creating shared memory segment...");
	state = createAndAttach(SMASH_SHM_KEY);	
	if(state){
		printf("Attached!\n");
	}
	else{
		return -2;
	}

#ifdef USE_INDICURSES
	assert(!icInit());
#endif

	pthread_create(&commThread, NULL, commHandler,&fd_radio);

	while(1){
		char buf[128];
		GpsState* gps = &gps_st.state;		

		clear();
		sprintf(buf, 
			"ypr = ( %.5f, %.5f, %.5f )",
			state->imuAngles[0],
			state->imuAngles[1],
			state->imuAngles[2]
		);

		icText(2, 2, buf);
		 	
		//smashSpeedSet(fd_rotors, rotor_st);
		//if(lnReadMsg(buf, 255)){
		//	lnParseMsg(gps_st, buf); 
		//}

		//if(gps->Fix)
		//	printGpsCoords(gps);		

		++state->subSystemLife[SMASH_HUB_I];
	
		usleep(10000);	
#ifdef USE_INDICURSES
		icPresent();
#endif
	}
	
	printf("Done\n");

	return 0;
}
