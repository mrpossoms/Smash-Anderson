#include "smash-imu.h"
#include <GLFW/glfw3.h>
#include <math.h>

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

int main(int argc, char* argv[]){
	if(argc != 2) return 1;

	if(!glfwInit()){
		return 2;
	}

	if(!(window = glfwCreateWindow(640, 480, "Smash-IMU", NULL, NULL))){
		glfwTerminate();
		printf("Error!\n");
		return 1;
	}

	glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);
	glPointSize(3);

	smashImuInit(argv[1], onChange);

	while(!glfwWindowShouldClose(window)){
		float ratio;
		int width, height;

		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float) height;

		glViewport(0, 0, width, height);

		glClear(GL_COLOR_BUFFER_BIT);
		
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotatef(-ypr[0] * RAD_TO_DEG, 0, 1, 0);
		glRotatef(-ypr[1] * RAD_TO_DEG, 1, 0, 0);
		glRotatef(ypr[2] * RAD_TO_DEG, 0, 0, 1);
		glBegin(GL_TRIANGLES);
		glColor3f(1.f, 0.f, 0.f);
		glVertex3f(-0.6f, -0.4f, 0.f);
		glColor3f(0.f, 1.f, 0.f);
		glVertex3f(0.6f, -0.4f, 0.f);
		glColor3f(0.f, 0.f, 1.f);
		glVertex3f(0.f, 0.6f, 0.f);
		glEnd();

		usleep(10000);	

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	return 0;
}
