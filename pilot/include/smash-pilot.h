#ifndef __smash-pilot_smash-pilot 
#define __smash-pilot_smash-pilot 

#include "vec3.h"

#define PRIOR_LOW  0
#define PRIOR_MED  1
#define PRIPR_HIGH 2

// Represents a position and orientation along with
// desired exec times and a priority
typedef struct{
	vec3  position;
	float heading;
	float targetDuration;
	float currentDuration;
	char  priority;
}SmashAction;

int smashPilotInit(const char* gpxPath);
int smashPilotTakeoff(SmashAction* takeOffVector);
int smashPilotLand(SmashAction* landingVector);

int smashPilotUpdate(void);

#endif
