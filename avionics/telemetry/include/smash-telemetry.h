#ifndef SMASH_TELEM
#define SMASH_TELEM

int  smashTelemetryInit(const char* dev, int addr);
void smashTelemetryShutdown(int fd);

#endif