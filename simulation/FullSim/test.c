#include <ode/ode.h>
#include <drawstuff/drawstuff.h>
#include "control/pid.h"

#define PARTS 4
#define DT 0.001
// dynamics and collision objects
static dWorldID world;
static dSpaceID space;
static dBodyID body;	
static dGeomID geoms[PARTS];	
static dMass m;
static dJointGroupID contactgroup;

PidState RollControl = {0};
PidState PitchControl = {0};
PidState AltControl = {0};

static void mat3MulVec3(dVector3 r, dMatrix3 m, dVector3 v){
	for(int i = 0; i < 3; i++)
		for(int j = 0; j < 3; j++){
			r[i] += m[i * 4 + j] * v[j];
		}
}

static float min(float x, float min){
	if(x < min) return min;
	return x;
}

static float rnd(){
	return ((random() % 1000) / 1000.0) * M_PI * 2 - M_PI;
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
	dsSetViewpoint (xyz,hpr);
}

// simulation loop
static void simLoop (int pause)
{
	const dReal *pos;
	const dReal *R;
	dVector3 lens;


	dQuaternion q;
	dQfromR(q, dBodyGetRotation(body));
	pos = dBodyGetPosition(body);


float yaw = atan2(2*q[1]*q[3]-2*q[0]*q[2] , 1 - 2*q[1] * 2 - 2*q[2]*2);
float pitch = asin(2*q[0]*q[1] + 2*q[2]*q[3]);
float roll = atan2(2*q[0]*q[3]-2*q[1]*q[2] , 1 - 2*q[0]*2 - 2*q[2]*2);

if(q[0]*q[1] + q[2]*q[3] == 0.5f){ //(north pole)
	yaw = 2 * atan2(q[0],q[3]);
	roll = 0;
} 
else if(q[0]*q[1] + q[2]*q[3] == 0.5f){ //(south pole)
	yaw = 2 * atan2(q[0],q[3]);
	roll = 0;
}


	float yawT   = (0.0882f * PID(&RollControl, yaw, DT));//, 0);
	float pitchT = (0.0882f * PID(&PitchControl, pitch, DT));//, 0);
	float altT   = min(0.0882f * PID(&AltControl, pos[2], DT), 0);

	float rotorThrusts[4] = {
		altT + (yawT > 0 ? yawT : 0),
		altT + (pitchT > 0 ? pitchT : 0),
		altT + (yawT < 0 ? -yawT : 0),
		altT + (pitchT < 0 ? -pitchT : 0)
	};

	// rotorForce(0, 0, 0, 0.0882f * (yawT / 2));
	// rotorForce(2, 0, 0, 0.0882f * (-yawT / 2));
	// rotorForce(1, 0, 0, 0.0882f * (pitchT / 2));
	// rotorForce(3, 0, 0, 0.0882f * (-pitchT / 2));


	//compensate(yawT, 0);
	//compensate(pitchT, 1);

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

	dVector3 up = {0,0,1};
	dVector3 dir;

	for(int i = PARTS; i--;){
		pos = dGeomGetPosition (geoms[i]);
		R = dGeomGetRotation (geoms[i]);
		dGeomBoxGetLengths(geoms[i], lens);

		dsDrawBox (pos, R, lens);
	}

	float R2D = 180 / M_PI;

	dsPrint("P {%0.3f, %0.3f, %0.3f}\n",
		pos[0], pos[1], pos[2]
	);
	dsPrint("R {%0.3f, %0.3f, %0.3f}\n",
		yaw * R2D, pitch * R2D, roll * R2D
	);

	// dsPrint("{%0.3f, %0.3f, %0.3f, %0.3f}\n", 
	// 	q[0], q[1], q[2], q[3]
	// );
}

int main (int argc, char **argv)
{
	dMatrix3 R;
	dRSetIdentity(R);

	// setup control structures
	RollControl.Kp = PitchControl.Kp = 2.0f;
    RollControl.Ki = PitchControl.Ki = 2.0f;
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
	dRFromEulerAngles(R, rnd(), rnd(), 0);
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