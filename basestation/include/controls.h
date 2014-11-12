#ifndef __CONTROLS
#define __CONTROLS

typedef void(stickCallback)(int, float*);

int controlsSetup(stickCallback cb, stickCallback balanceCb);
int controlsPoll();

#endif