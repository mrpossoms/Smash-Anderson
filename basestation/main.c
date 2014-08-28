#include <stdio.h>
#include <unistd.h>
#include <GLFW/glfw3.h>
#include <smash-telemetry.h>
#include "controls.h"

GLFWwindow* window = NULL;

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

	sleep(1);

	glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);
	glPointSize(3);
	glEnable(GL_DEPTH_TEST);

	controlsSetup(NULL);

	while(!glfwWindowShouldClose(window)){
		updateView();
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

}
