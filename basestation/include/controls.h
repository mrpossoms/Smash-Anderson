#ifndef __CONTROLS
#define __CONTROLS

typedef void(stickCallback)(int, const float*);

int controlsSetup(stickCallback cb, stickCallback balanceCb);
int controlsPoll();

#endif