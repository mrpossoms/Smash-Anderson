#ifndef __SHAPES__
#define __SHAPES__

#include "./../linmath.h"
#include "SDL.h"

#define MESH_SEGMENTS 9

extern int WIDTH;
extern int HEIGHT;
extern SDL_Renderer *renderer;

vec3 lProp[2];
vec3 rProp[2];
vec3 Frame[MESH_SEGMENTS];

void drawRay(ray2 ray);
void point(vec3 v, mat4x4 m);
void mesh(vec3* data, int verts, mat4x4 m);

#endif