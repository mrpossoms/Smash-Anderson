#include "vec3.h"
#include <stdio.h>
#include <assert.h>

void add(){
	Vec3 v1 = {
		32, 343, 44
	};

	Vec3 v2 = {
		12.3f, 0.0f, 320.0f
	};

	Vec3 out = vec3Add(&v1, &v2);

	assert(out.x == 44.3f);
	assert(out.y == 343.0f);
	assert(out.z == 364.0f);	
}

void sub(){
	Vec3 v1 = {
		32, 343, 44
	};

	Vec3 v2 = {
		12.3f, 0.0f, 320.0f
	};

	Vec3 out = vec3Sub(&v1, &v2);

	assert(out.x == 19.7f);
	assert(out.y == 343.0f);
	assert(out.z == -276.0f);	
}

void mul(){
	Vec3 v1 = {
		32, 343, 44
	};

	Vec3 v2 = {
		3.0f, 0.0f, 2.0f
	};

	Vec3 out = vec3Mul(&v1, &v2);

	assert(out.x == 96.0f);
	assert(out.y == 0.0f);
	assert(out.z == 88.0f);	
}

void div(){
	Vec3 v1 = {
		32, 343, 44
	};

	Vec3 v2 = {
		3.0f, 0.0f, 2.0f
	};

	Vec3 out = vec3Div(&v1, &v2);

	assert(out.x == 96.0f);
	assert(out.y == 0.0f);
	assert(out.z == 88.0f);	
}

void scl(){
	Vec3 v1 = {
		32, 343, 44
	};

	Vec3 out = vec3Scl(&v1, 2);

	assert(out.x == 64.0f);
	assert(out.y == 686.0f);
	assert(out.z == 88.0f);	
}

int main(void){
	add();
	sub();
	mul();
	//div();
	scl();	

	return 0;
}
