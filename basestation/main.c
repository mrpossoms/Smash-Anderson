#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <GLFW/glfw3.h>
#include <smash-telemetry.h>
#include <ardutalk.h>
#include "controls.h"

static GLFWwindow* window = NULL;
static int radio_fd;
static unsigned char statusTimer;

static void updateView(){
	float ratio;
	int width, height;

	glfwGetFramebufferSize(window, &width, &height);
	ratio = width / (float) height;

	glViewport(0, 0, width, height);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

static void throttle_callback(float x, float y){
	y = y > 0 ? 0 : -y;
	byte t = (byte)(y * 255.0f);
	RotorStates throttle = {
		t, t, t, t
	};
	printf("Sending message to %d %f %f %d\n", radio_fd, x, y, throttle[0]);
	smashSendMsg(radio_fd, MSG_CODE_THROTTLE, &throttle);
}

int main(int argc, char* argv[]){

	if(argc != 2){
		printf("Error: missing argument for radio device.\n\tUsage: basestation [device path]\n");
		return -1;
	}

	if(!glfwInit()){
		return 1;
	}

	if(!(window = glfwCreateWindow(640, 480, "Smash-Controller", NULL, NULL))){
		glfwTerminate();
		printf("Error!\n");
		return 2;
	}

	if(!(radio_fd = smashTelemetryInit(argv[1]))){
		return 3;
	}

	sleep(1);

	glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);
	glPointSize(3);
	glEnable(GL_DEPTH_TEST);

	//if(controlsSetup(throttle_callback)){
	//	return 4;
	//}

	printf("Stick selected!\n");

	while(!glfwWindowShouldClose(window)){
		byte msgBuf[128];

		updateView();
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);


		if(atAvailable(radio_fd)){
			byte msgType = 0;
			int bytes = smashReceiveMsg(radio_fd, &msgType, msgBuf);
			printf("resulted in %x\n", bytes);
			assert(bytes > 0);
			//if(msgType == 0) continue;

			switch(msgType){
				case MSG_CODE_THROTTLE:
				{
					printf(">>>throttle message\n");
				}
				break;
				case MSG_CODE_STATUS_REQ:
				{
					printf(">>>Status message request\n");
				}
				break;
				case MSG_CODE_STATUS:
				{
					printf(">>>Status message\n");

					float* angles =((struct SmashState*)msgBuf)->imuAngles;
					printf("ypr = ( %f, %f, %f )\n", angles[0], angles[1], angles[2]);
					return 0;
				}
				break;
				default:;
				printf("Unrecognized message! %x\n", msgType);
			}
		}
		//controlsPoll();

		if(!(statusTimer--)){
			printf("Requesting status...\n");
			smashSendMsg(radio_fd, MSG_CODE_STATUS_REQ, NULL);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

}
