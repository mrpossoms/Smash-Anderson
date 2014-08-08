#include <ode/ode.h>
#include <drawstuff/drawstuff.h>
#include "control/pid.h"
#include "./../../avionics/IMU/include/smash-imu.h"

#define PARTS 4
#define DT 0.01
#define RAND8() ((unsigned char)(rand() % 255))\

// dynamics and collision objects
static dWorldID world;
static dSpaceID space;
static dBodyID body;	
static dGeomID geoms[PARTS];	
static dMass m;
static dJointGroupID contactgroup;
static float hpr[3] = {140.000f,-17.0000f,0.0000f};

float R2D = 180 / M_PI;
float YPR[3];
float* YPROFF = NULL;
dQuaternion imuRot = {0};
dQuaternion qI = {0};
dQuaternion qJ = {0};
dQuaternion qK = {0};

PidState RollControl = {0};
PidState PitchControl = {0};
PidState AltControl = {0};

float __randf(){
	unsigned char b = RAND8();
	return b / 255.0f;
}

static void mat3MulVec3(dVector3 r, const dReal* m, const dReal* v){
	for(int i = 0; i < 3; i++)
		for(int j = 0; j < 3; j++){
			r[i] += m[i * 4 + j] * v[j];
		}
}

static float min(float x, float min){
	if(x < min) return min;
	return x;
}

static float max(float x, float max){
	if(x > max) return max;
	return x;
}

static float rnd(){
	return ((__randf() * M_PI * 2) - M_PI) / 2.0f;
}

static void quatFromEuler(dQuaternion q, float* ypr){
	float hy = ypr[0] / 2, hp = ypr[1] / 2, hr = ypr[2] / 2;

	q[0] = cos(hy) * cos(hp) * cos(hr) + sin(hy) * sin(hp) * sin(hr);
	q[1] = sin(hy) * cos(hp) * cos(hr) - cos(hy) * sin(hp) * sin(hr);
	q[2] = cos(hy) * sin(hp) * cos(hr) + sin(hy) * cos(hp) * sin(hr);
	q[3] = cos(hy) * cos(hp) * sin(hr) + sin(hy) * sin(hp) * cos(hr);
}

static float quatMag(dQuaternion q){
	return sqrt(q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3]);
}

static float quatDot(dQuaternion q1, dQuaternion q2){
	float m1 = quatMag(q1);
	float m2 = quatMag(q2);

	return acos(
		(q1[0]*q2[0] + q1[1]*q2[1] + q1[2]*q2[2] + q1[3]*q2[3]) /
		(m1 * m2)
	);
}

// this is called by dSpaceCollide when two objects in space are
// potentially colliding.
static void nearCallback (void *data, dGeomID o1, dGeomID o2)
{
	dBodyID b1 = dGeomGetBody(o1);
	dBodyID b2 = dGeomGetBody(o2);
	dContact contact;  
	contact.surface.mode = dContactBounce | dContactSoftCFM;
	// friction parameter
	contact.surface.mu = dInfinity;
	// bounce is the amount of "bouncyness".
	contact.surface.bounce = 0.6;
	// bounce_vel is the minimum incoming velocity to cause a bounce
	contact.surface.bounce_vel = 0.1;
	// constraint force mixing parameter
	contact.surface.soft_cfm = 0.001;

	if(b1 == b2) return;

	if (dCollide (o1,o2,1,&contact.geom,sizeof(dContact))) {
	    dJointID c = dJointCreateContact (world,contactgroup,&contact);
	    dJointAttach (c,b1,b2);
	}
}

static void rotorForce(int i, float fx, float fy, float fz){
	float theta = i * (M_PI / 2.0);
	dVector3 pos = {
		cos(theta) * 0.3,
		sin(theta) * 0.3,
		0.0
	};

	dBodyAddRelForceAtRelPos(
		body,
		fx, fy, fz,
		pos[0], pos[1], pos[2]
	);
}

// start simulation - set viewpoint
static void start()
{
	static float xyz[3] = {2.0f,-2.0f,1.7600f};
	static float hpr[3] = {140.000f,-17.0000f,0.0000f};
	static float I[3] = { 0, 0, 0 };
	static float J[3] = { 0, 0, M_PI / 2 };
	static float K[3] = { 0, M_PI / 2, 0 };

	dsSetViewpoint (xyz,hpr);

	// set the orthoganal quats
	quatFromEuler(qI, I);
	quatFromEuler(qJ, J);
	quatFromEuler(qK, K);

	qI[0] = 0;
}

// simulation loop
static void simLoop (int pause)
{
	const dReal *pos;
	const dReal *vel;
	const dReal *R;
	dVector3 lens;
	dVector3 relVel = {0};

	dQuaternion q;

	dQfromR(q, R = dBodyGetRotation(body));
	pos = dBodyGetPosition(body);
	vel = dBodyGetLinearVel(body);

	float yaw = atan2(2*q[1]*q[3]-2*q[0]*q[2] , 1 - 2*q[1] * 2 - 2*q[2]*2);
	float pitch = asin(2*q[0]*q[1] + 2*q[2]*q[3]);
	float roll = atan2(2*q[0]*q[3]-2*q[1]*q[2] , 1 - 2*q[0]*2 - 2*q[2]*2);

	if(q[0]*q[1] + q[2]*q[3] == 0.5f){ //(north pole)
		yaw = 2 * atan2(q[0],q[3]);
		roll = 0;
	} 
	else if(q[0]*q[1] + q[2]*q[3] == -0.5f){ //(south pole)
		yaw = -2 * atan2(q[0],q[3]);
		roll = 0;
	}

	// float yaw = quatDot(imuRot, qI) * R2D;
	// float pitch = quatDot(imuRot, qJ) * R2D;
	// float roll = quatDot(imuRot, qK) * R2D;

	// dsPrint("dy: %f\n", yaw);
	// dsPrint("dp: %f\n", pitch);
	// dsPrint("dr: %f\n\n", roll);

	yaw += YPR[2] - YPROFF[2];
	pitch -= YPR[1] - YPROFF[1];

	mat3MulVec3(relVel, R, vel);

	float yawT   = max(0.0882f * PID(&RollControl, yaw, DT), 0.2);//, 0);
	float pitchT = max(0.0882f * PID(&PitchControl, pitch, DT), 0.2);//, 0);
	float altT   = max(0.0882f * PID(&AltControl, pos[2], DT), 0.2);

	float rotorThrusts[4] = {
		min(altT + (yawT > 0 ? yawT : 0), 0),
		min(altT + (pitchT > 0 ? pitchT : 0), 0),
		min(altT + (yawT < 0 ? -yawT : 0), 0),
		min(altT + (pitchT < 0 ? -pitchT : 0), 0)
	};

	for(int i = 4; i--;){
		rotorForce(i, 0, 0, rotorThrusts[i]);
	}

	//rotorForce(0, 0, 0, 0.0882f);
	//rotorForce(2, 0, 0, 0.0882f);


	// find collisions and add contact joints
	dSpaceCollide (space,0,&nearCallback);
	// step the simulation
	dWorldQuickStep (world,DT);  
	// remove all contact joints
	dJointGroupEmpty (contactgroup);
	// redraw sphere at new location

	dVector3 off = {2.0f,-2.0f,1.0f};
	dVector3 P;

	dAddVectors3(P, pos, off);

	dsSetViewpoint (P, hpr);
	for(int i = PARTS; i--;){
		pos = dGeomGetPosition (geoms[i]);
		R = dGeomGetRotation (geoms[i]);
		dGeomBoxGetLengths(geoms[i], lens);

		dsDrawBox (pos, R, lens);
	}

	// dsPrint("P {%0.3f, %0.3f, %0.3f}\n",
	// 	pos[0], pos[1], pos[2]
	// );
	// dsPrint("V {%0.3f, %0.3f, %0.3f}\n",
	// 	relVel[0], relVel[1], relVel[2]
	// );
	// dsPrint("R {%0.3f, %0.3f, %0.3f}\n",
	// 	yaw, pitch, roll
	// );
}

void onTiltRefresh(float* ypr){
	memcpy(YPR, ypr, sizeof(float) * 3);
	int i = 3;

	if(!YPROFF){
		YPROFF = (float*)malloc(sizeof(float) * 3);
		memcpy(YPROFF, YPR, sizeof(float) * 3);
	}

	quatFromEuler(imuRot, ypr);

	if(qI[0] == 0){
		memcpy(qI, imuRot, sizeof(dQuaternion));
	}

	//dsPrint("Q {%0.2f, %0.2f, %0.2f, %0.2f}\n", imuRot[0], imuRot[1], imuRot[2], imuRot[3]);
}

int main (int argc, char **argv)
{
	dMatrix3 R;
	dRSetIdentity(R);

	/* seed the rng */
	srand(((unsigned int)time(NULL)) % 1024);

	// setup control structures
	RollControl.Kp = PitchControl.Kp = 4.0f;
    RollControl.Ki = PitchControl.Ki = 1.0f;
    RollControl.Kd = PitchControl.Kd = 0.75f;

	AltControl.Kp = 4.0f;
    AltControl.Ki = 4.0f;
    AltControl.Kd = 1.0f;

    AltControl.target = 1;


	// setup pointers to drawstuff callback functions
	dsFunctions fn;
	fn.version = DS_VERSION;
	fn.start = &start;
	fn.step = &simLoop;
	fn.stop = 0;
	fn.command = 0;
	fn.path_to_textures = "./drawstuff/textures";

	dInitODE ();
	smashImuInit("/dev/tty.usbserial-FTGCHM6G", onTiltRefresh);
	// create world
	world = dWorldCreate ();
	space = dHashSpaceCreate (0);
	dWorldSetGravity (world,0,0,-9.8);
	dWorldSetCFM (world,1e-5);
	dCreatePlane (space,0,0,1,0);
	contactgroup = dJointGroupCreate (0);
	// create object
	body = dBodyCreate (world);

	for(int i = PARTS; i--;){
		float theta = i * (M_PI / 2);
		float x = cos(theta) * 0.16;
		float y = sin(theta) * 0.16;
		float z = 0;
		
		geoms[i] = dCreateBox (space, 0.3, 0.01, 0.1);
		dGeomSetBody (geoms[i], body);

		dGeomSetOffsetPosition(geoms[i], x, y, z);
		dRFromEulerAngles(R, 0, 0, theta);
		dGeomSetOffsetRotation(geoms[i], R);
	}

	dRSetIdentity(R);
	dMassSetBox (&m, 1, 0.6, 0.6, 0.1);
	//dRFromEulerAngles(R, rnd(), rnd(), rnd());
	dBodySetRotation(body, R);
	dBodySetMass (body,&m);

	// set initial position
	dBodySetPosition (body,0,0,1);
	// run simulation
	dsSimulationLoop (argc,argv,800,600,&fn);
	// clean up
	dJointGroupDestroy (contactgroup);
	dSpaceDestroy (space);
	dWorldDestroy (world);
	dCloseODE();
	return 0;
}
