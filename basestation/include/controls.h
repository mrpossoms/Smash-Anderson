#ifndef __CONTROLS
#define __CONTROLS

typedef void(throttleCallback)(float, float);

int controlsSetup(throttleCallback cb);

#endif