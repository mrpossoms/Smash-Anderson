#include <math.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <GLFW/glfw3.h>
#include <glut.h>
#include <glu.h>
#include <smash-telemetry.h>
#include <ardutalk.h>
#include "controls.h"

#ifndef M_PI
#define M_PI 3.14159
#endif

static GLFWwindow* window = NULL;
static int radioFd;
static int radioEnabled;
static int joystickAvailable;
static unsigned char statusTimer;
static struct SmashState state;
static float displayAngles[3];
static float throttle;

static void drawThrottleLine(float x, float y, float z, int i){
	float p = state.speedTargets[i] / 255.0f;
	glColor3f(0, p, 1);
	glVertex3f(x, y, z);
	glVertex3f(x, y + 0.5f * p, z);
}

static void drawModel(){
		static const float d = 0.2f;

		glBegin(GL_QUADS);
			glColor3f(1, 0, 0);
			glVertex3f(-1, 0, 0 );
			glVertex3f( 1, 0, 0 );

			glVertex3f( 1, 0, 0 );
			glVertex3f( 1, d, 0 );

			glVertex3f(-1, 0, 0 );
			glVertex3f(-1, d, 0 );
			
			glVertex3f(-1, d, 0 );
			glVertex3f( 1, d, 0 );

			glColor3f(0.5, 0, 0);

			glVertex3f(0, 0, 1);
			glVertex3f(0, 0, -1);

			glVertex3f(0, 0, -1);
			glVertex3f(0, d, -1);

			glVertex3f(0, 0, 1);
			glVertex3f(0, d, 1);

			glVertex3f(0, d,  1);
			glVertex3f(0, d, -1);
		glEnd();

		glBegin(GL_LINES);
			drawThrottleLine(-1, d, 0, 0);
			drawThrottleLine( 1, d, 0, 1);
			drawThrottleLine( 0, d, 1, 2);
			drawThrottleLine( 0, d,-1, 3);
		glEnd();
}

void drawString(int x, int y, char *string, void* font){
	int len, i;

	glRasterPos2f(x, y);
	len = (int) strlen(string);
	for (i = 0; i < len; i++) {
		glutBitmapCharacter(font, string[i]);
	}
}

static void throttle_callback(int axes, const float* values)
{
	float x = values[0], y = values[1];

	//printf("Sending message to %d %f %f %d\n", radioFd, x, y, throttle[0]);
	//smashSendMsg(radioFd, MSG_CODE_THROTTLE, &throttle);

	if(joystickAvailable){
		float trigger = values[4] * 0.5f + 0.5f;
		float t = 126.0f * trigger;

		state.speedTargets[0] = (unsigned char)((-x < 0 ? 0 : -x) * 127.0f + t);
		state.speedTargets[1] = (unsigned char)((x < 0 ? 0 : x) * 127.0f + t);

		state.speedTargets[2] = (unsigned char)((-y < 0 ? 0 : -y) * 127.0f + t);
		state.speedTargets[3] = (unsigned char)((y < 0 ? 0 : y) * 127.0f + t);
	}
}

int w, h;
static void resize(int width, int height)
{
	const float ar = (float) width / (float) height;
	w = width;
	h = height;
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
} 

static void update(int value)
{
	if(atAvailable(radioFd)){
		unsigned char msgBuf[128];
		byte msgType = 0;
		int bytes = smashReceiveMsg(radioFd, &msgType, msgBuf);
		//assert(bytes > 0);
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

				state = *((struct SmashState*)msgBuf);
				//printf("ypr = ( %f, %f, %f )\n", angles[0], angles[1], angles[2]);
				//return 0;
			}
			break;
			default:;
			printf("Unrecognized message! %x\n", msgType);
		}
	}

	if(!radioEnabled){
		state.imuAngles[0] += 0.01f;
		state.imuAngles[1] += 0.01f;
	}

	if(joystickAvailable){
		controlsPoll();		
	}

	if(radioEnabled){
		if((statusTimer--) == 250){
			//printf("Requesting status...\n");
			smashSendMsg(radioFd, MSG_CODE_STATUS_REQ, NULL);
			statusTimer = 0xFF;
		}
	}

    double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    int time = (int)t;
    glutTimerFunc(16, update, 0);
    glutPostRedisplay();
}

void setOrthographicProjection() {
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h);
	glScalef(1, -1, 1);
	glTranslatef(0, -h, 0);
	glMatrixMode(GL_MODELVIEW);
} 

void printGPS(GpsState* state, int x, int y){
	char buf[256];

	sprintf(buf, "Position: %fN, %fW\n", state->Lat, state->Lon);
	drawString(x, y + 30, buf, GLUT_BITMAP_HELVETICA_12);

	sprintf(buf, "Altitude: %f\n", state->Altitude);
	drawString(x, y + 45, buf, GLUT_BITMAP_HELVETICA_12);

	sprintf(buf, "Speed: %f\n", state->Speed);
	drawString(x, y + 60, buf, GLUT_BITMAP_HELVETICA_12);

	sprintf(buf, "Update Time: %02d:%02d:%02d\n",
		state->Hour,
		state->Minute,
		state->Second
	);
	drawString(x, y + 75, buf, GLUT_BITMAP_HELVETICA_12);

	sprintf(buf, "Satellites: %d\n", state->Satellites);
	drawString(x, y + 90, buf, GLUT_BITMAP_HELVETICA_12);

	sprintf(buf, "HDOP: %f\n", state->HDOP);
	drawString(x, y + 105, buf, GLUT_BITMAP_HELVETICA_12);
}

static void display(void){
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	glFrustum (-1.0, 1.0, -1.0, 1.0, 1.5, 20.0);

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
	gluLookAt (
		0.0, 0.0, -5.0, // eye position
		0.0, 0.0, 0.0,  // target
		0.0, 1.0, 0.0   // up
	);

	for(int i = 3; i--;){
		float delta = state.imuAngles[i] - displayAngles[i];
		if(delta == 0) continue;
		displayAngles[i] = state.imuAngles[i];//+= delta * 0.1f;
	}

	glRotatef(-displayAngles[0] * (180.0 / M_PI) + 45, 0.0f, 1.0f, 0.0f);
	glRotatef(-displayAngles[1] * (180.0 / M_PI), 1.0f, 0.0f, 0.0f);
	glRotatef(displayAngles[2] * (180.0 / M_PI), 0.0f, 0.0f, 1.0f);

	drawModel();

	setOrthographicProjection();
    glPushMatrix();
    glLoadIdentity();
	glColor3f(1, 0, 0);
	drawString(10, 20, "Smash-Anderson Basestation", GLUT_BITMAP_HELVETICA_18);
	printGPS(&state.gpsState, 20, 10);

	{
		char buf[128];
		sprintf(buf, "SubSys Online: 0x%08x", state.subSystemOnline);
		drawString(20, h - 20, buf, GLUT_BITMAP_HELVETICA_12);
	}

    glPopMatrix();
    //resetPerspectiveProjection();

	glutSwapBuffers();
} 

int main(int argc, char* argv[]){
	radioEnabled = 1;
	joystickAvailable = 0;

	if(argc != 2){
		printf("Error: missing argument for radio device.\n\tUsage: basestation [device path]\n");
		printf("Proceeding without radio\n");
		radioEnabled = 0;
	}

	if(!glfwInit()){
		return 1;
	}

	// if(!(window = glfwCreateWindow(640, 480, "Smash-Controller", NULL, NULL))){
	// 	glfwTerminate();
	// 	printf("Error!\n");
	// 	return 2;
	// }
	if(!controlsSetup(throttle_callback, NULL)){
		joystickAvailable = 1;
	}

	if(radioEnabled && !(radioFd = smashTelemetryInit(argv[1]))){
		return 3;
	}

    glutInit(&argc, argv);
    glutInitWindowSize(640,480);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("SmashBase");
    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutTimerFunc(16, update, 0);
	glEnable(GL_DEPTH_TEST);
    glutMainLoop();

    return 0;
}
