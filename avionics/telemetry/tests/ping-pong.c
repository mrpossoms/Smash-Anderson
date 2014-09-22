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
                        int res = atRead(fd_radio, &msgType, sizeof(byte));
                        // write(1, &msgType, sizeof(byte));
                        //if(smashReceiveCode(fd_radio, &msgType)){
                        if(res){
                                //printf("+ %c\n", msgType);
                                switch(msgType){
                                        case MSG_CODE_DATA:
                                        {
                                                int r2 = atRead(fd_radio, &packet, sizeof(struct SmashData));
                                                //printf("(%d) Data len = %d\n", r2, packet.len);
                                                //write(1, packet.buf, packet.len);
                                                write(fd_out, packet.buf, packet.len);

                                                if(packet.len < 128){
                                                        printf("DONE!\n");
                                                        close(fd_out);
                                                }
                                        }
                                                break;
                                        default:
                                                break;
                                }
                        }
                }
                else{
                        struct SmashData data;

                        data.len = read(fd_file, data.buf, 128);

                        if(data.len){
                                msgType = MSG_CODE_DATA;
                                smashSendMsg(fd_radio, msgType, &data);
                                //printf(" [LEN:%u]\n", data.len);
                                //printf(" Wrote %d\n", smashSendMsg(fd_radio, msgType, &data));
                        }
                }
                //printf(".");
                usleep(DELAY);
                //sleep(1);
        }
 
        return 0;
}