#ifndef SMASH_STABILIZATION
#define SMASH_STABILIZATION

#include "smash-hub.h"

extern unsigned char MIN_THROTTLE;

void stblInit();
int  stblUpdate(struct SmashState* state);

#endif
