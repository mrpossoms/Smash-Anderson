#include "smash-imu.h"
#include <GLFW/glfw3.h>
#include <math.h>

#define SIDE 100
#define RAD_TO_DEG 180 / M_PI

GLFWwindow* window = NULL;
float ypr[3];

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void onChange(float* ori){
	int i = 3;
	char buf[128];

	for(;i--;){
		if(ori[i] > 4.0f || ori[i] < -4.0f){
			sprintf(buf, ">>%f, %f, %f<<\n", ori[0], ori[1], ori[2]);
			write(1, buf, strlen(buf));
			//sleep(1);
			return;
		}
	}

	memcpy(ypr, ori, sizeof(float) * 3);
}

void display(float* data){
		int y = SIDE;

		glBegin(GL_POINTS);

		for(;y--;){
			int x = SIDE;
			for(;x--;){
				float u = x / (float)SIDE, v = y / (float)SIDE;
				float X = u - 0.5f, Y = v - 0.5f;
				float z = data[y * SIDE + x];
				glColor3f(u, v, z * 0.5f + 0.5f);
				glVertex3f(X, Y, z);
			}
		}

		glEnd();
}

int main(int argc, char* argv[]){
	float data[SIDE * SIDE];

	if(!glfwInit()){
		return 2;
	}

	smashImuInit("/dev/tty.usbserial-FTGCHM6G", onChange);

	if(!(window = glfwCreateWindow(640, 480, "Smash-Formula", NULL, NULL))){
		glfwTerminate();
		printf("Error!\n");
		return 1;
	}

	for (int i = 0; i < SIDE * SIDE; ++i){
		float x = i % SIDE, y = (int)(i / SIDE);
		float t = (x / (float)SIDE) - 0.5f;
		float w = (y / (float)SIDE) - 0.5f;
		
		t *= M_PI * 2; w *= M_PI * 2;

		data[i] = sin((w + t)) * -sin(t/2);
	}
	sleep(1);

	glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);
	glPointSize(3);
	glEnable(GL_DEPTH_TEST);

	while(!glfwWindowShouldClose(window)){
		float ratio;
		int width, height;

		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float) height;

		glViewport(0, 0, width, height);

		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glRotatef(-ypr[0] * RAD_TO_DEG, 0, 1, 0);
		glRotatef(-ypr[1] * RAD_TO_DEG, 1, 0, 0);
		glRotatef(ypr[2] * RAD_TO_DEG, 0, 0, 1);

		display(data);

		usleep(10000);	

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	return 0;
}
