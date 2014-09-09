#include <math.h>
#include <smash-imu.h>
#include <libNEMA.h>
#include <ardutalk.h>
#include <indicurses.h>
#include <Controls.h>

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
	int fd_gps    = -1; //lnConnect(GPS_DEV, B57600);
	int fd_rotors = -1;
	char gps_buf[255] = {0};	
	GpsState gps_st = {0};

	smashImuInit(IMU_DEV);
	//fd_gps    = lnConnect(GPS_DEV, B57600);
	fd_rotors = smashSpeedInit("/dev/servoblaster");	

	icInit();

	while(1){
		int hx = IC_TERM_WIDTH >> 1, hy = IC_TERM_HEIGHT >> 1;
		int qx = hx >> 1;
		unsigned char t = (unsigned char)((ORIENTATION.x + M_PI) * 40.58f);
		unsigned char rotors[4] = {
			t, t, t, t
		};

		char buf[100];
		sprintf(buf, "rotors=%d %d %f\n", fd_rotors, t, ORIENTATION.y);
		smashSpeedSet(fd_rotors, rotors);
		/*if(lnReadMsg(gps_buf, 255)){
			lnParseMsg(&gps_st, gps_buf); 
		}*/
		
		clear();

		icText(2, 2, buf);
		if(gps_st.Fix)
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
