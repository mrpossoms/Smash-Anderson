#ifndef __Controls_smash-speed 
#define __Controls_smash-speed 

int  smashSpeedInit(const char* dev);
void smashSpeedSet(int fd, unsigned char* rotors);
void smashSpeedClose(int fd);

#endif
