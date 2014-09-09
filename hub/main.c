#include <smash-imu.h>
#include <libNEMA.h>
#include <ardutalk.h>
#include <indicurses.h>

#define GPS_DEV "/dev/ttyAMA0"
#define IMU_DEV "/dev/ttyUSB0"
#define I2C_DEV "/dev/i2c-1"

void printGpsCoords(GpsState* state){
	char buf[256];

	sprintf(buf, "%f N", state->Lat);
	icText(2, 2, buf);
	sprintf(buf, "%f W", state->Lon);
	icText(2, 4, buf);
}

int main(int argc, const char* argv[]){
	int fd_gps = -1; //lnConnect(GPS_DEV, B57600);
	char gps_buf[255] = {0};	
	GpsState gps_st = {0};

	smashImuInit(IMU_DEV);
	fd_gps = lnConnect(GPS_DEV, B57600);
	
	icInit();

	while(1){
		int hx = IC_TERM_WIDTH >> 1, hy = IC_TERM_HEIGHT >> 1;
		int qx = hx >> 1;

		if(lnReadMsg(gps_buf, 255)){
			lnParseMsg(&gps_st, gps_buf); 
		}
		
		clear();

		if(gps_st.fix)
			printGpsCoords(&gps_st);		

		icCurrentChar = '.';

		icCurrentChar = 'o';
		icDial(qx, hy, 10, ORIENTATION.x);
		icDial(hx, hy, 10, ORIENTATION.y);
		icDial(hx + qx, hy, 10, ORIENTATION.z);
		
		usleep(10000);	
		icPresent();
	}
	
	printf("Done\n");

	return 0;
}
