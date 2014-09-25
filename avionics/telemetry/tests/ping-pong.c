#include "smash-telemetry.h"
#include "smash-hub.h"
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <fcntl.h>
 
#define DELAY 1000
 
static int fd_radio = 0, fd_file = 0;
 
int main(int argc, char* argv[])
{
        switch(argc){
                case 3:
                        fd_file  = open(argv[2], O_RDONLY);
                        if(!fd_file){
                                printf("Error! '%s' could not be opened\n", argv[2]);
                        }
                        else{
                                printf("'%s' opened with fd %d\n", argv[2], fd_file);
                        }
                case 2:
                        fd_radio = smashTelemetryInit(argv[1]);
                        break;
                default:
                        printf("Error: usage\n./ping-pong 'device' ['file']\n");
                        return -1;
                        break;
        }
 
        byte msgType = 0;
        struct SmashData packet = {0};
        int fd_out = open("./out", O_CREAT | O_TRUNC | O_WRONLY);

        printf(fd_file ? "Writing\n" : "Reading\n");
        while(1){
                if(!fd_file){
                        msgType = MSG_CODE_DATA;
                        int res = smashReceiveMsg(fd_radio, &msgType, &packet);
                        
			if(res > 0){
				write(fd_out, packet.buf, packet.len);
				printf("res %d\n", res);

				if(packet.len < 128){
					printf("DONE!\n");
					close(fd_out);
				}
			}
			else
				printf("err %d\n", res);
                }
                else{
                        struct SmashData data;

                        data.len = read(fd_file, data.buf, 128);

                        if(data.len){
                                msgType = MSG_CODE_DATA;
                                smashSendMsg(fd_radio, msgType, &data);
                                write(1, ".", 1);
                        }
                }
        }
 
        return 0;
}
