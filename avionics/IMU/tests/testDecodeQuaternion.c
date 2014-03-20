#include <stdio.h>
#include "smash-imu-decoder.h"

int main(void){
	char* buf = "21FBB43D,27AE733F,0E862A3D,BC3C94BE,6AB92C3E,2E3EC8BC,F3EE783F,406D3A3D,ADB67B3F,499B353D,B0F226BE,05562E3E,49EEE3BC,A3C4763F,0A6DB13A,9E407F3F,F4863D3D,6EE403BD,F3422D3E,5F88AEBC,47CC7A3F,2C482DBD,A8E97D3F,7CBD413D,1008C83D,7D3F223E,A621D2BC,28D0783";
	float Q[4] = {0};
	int i = 0;

	smashImuDecodeQuaternion(buf, Q);

	printf("{ ");
	for(i = 0; i < 4; i++){
		printf("%f ", Q[i]);
	}
	printf("}\n");	

	return 0;
}
