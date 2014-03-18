#ifndef SMASH_VEC3
#define SMASH_VEC3
typedef struct{
	float x;
	float y;
	float z;
} Vec3;

extern inline Vec3 vec3Add(Vec3* v1, Vec3* v2){
	Vec3 out = {
		v1->x + v2->x,
		v1->y + v2->y,
		v1->z + v2->z
	};
	return out;
}

extern inline Vec3 vec3Sub(Vec3* v1, Vec3* v2){
	Vec3 out = {
		v1->x - v2->x,
		v1->y - v2->y,
		v1->z - v2->z
	};
	return out;
}

extern inline Vec3 vec3Mul(Vec3* v1, Vec3* v2){
	Vec3 out = {
		v1->x * v2->x,
		v1->y * v2->y,
		v1->z * v2->z
	};
	return out;
}

extern inline Vec3 vec3Div(Vec3* v1, Vec3* v2){
	Vec3 out = {
		v1->x * v2->x,
		v1->y * v2->y,
		v1->z * v2->z
	};
	return out;
}

extern inline Vec3 vec3Scl(Vec3* v1, float scl){
	Vec3 out = {
		v1->x * scl,
		v1->y * scl,
		v1->z * scl
	};
	return out;
}

extern inline float vec3dot(Vec3* v1, Vec3* v2){
	float out = 0;
	int i = 3;

	for(;i--;){
		out += ((float*)v1)[i] * ((float*)v2)[i];
	}
	
	return out;
}
#endif
