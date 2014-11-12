#ifndef __CONTROLS
#define __CONTROLS

typedef void(stickCallback)(float, float);

int controlsSetup(stickCallback cb, stickCallback balanceCb);
int controlsPoll();

#endif