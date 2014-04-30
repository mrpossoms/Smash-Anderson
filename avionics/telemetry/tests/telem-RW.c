#include "smash-telemetry.h"
#include <stdio.h>
#include <unistd.h>

typedef struct{
	int override;
	unsigned char rotors[4];
}basStnMsg;

int main(void){
	int i, fd = smashTelemetryInit("/dev/i2c-1", 0x60);
	char buf[128] = {0};
	basStnMsg msg = {
		.override = 1,
		{ 16, 32, 96, 128 }
	};

	if(write(fd, &msg, sizeof(basStnMsg)) != sizeof(basStnMsg)){
		printf("Done goofed\n");
	}

	sleep(1);

	printf("read: %d\n", read(fd, buf, 128));

	for(i = 0; i < 128; i++){
		printf("%02x ", buf[i]);
	}	

	smashTelemetryShutdown(fd);

	return 0;
}
