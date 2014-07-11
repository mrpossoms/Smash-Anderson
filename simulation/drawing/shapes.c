#include "shapes.h"

int WIDTH;
int HEIGHT;
SDL_Renderer *renderer;

vec3 lProp[2] = {
    {1.15,-0.15, 1},
    {1.35,-0.15, 1}
};
vec3 rProp[2] = {
    {-1.15,-0.15, 1},
    {-1.35,-0.15, 1}
};
vec3 Frame[MESH_SEGMENTS] = {
    { 1.25,-0.1, 1},
    {-1.25,-0.1, 1},
    {-1.25, 0, 1},
    {-0.25, 0.3, 1},
    { 0, 0.1, 1},

    { 0, 0.1, 1},
    { 0.25, 0.3, 1},
    { 1.25, 0, 1},
    { 1.25,-0.1, 1}
};

void drawRay(ray2 ray){
    int hw = WIDTH >> 1, hh = HEIGHT >> 1;

    SDL_RenderDrawLine(
        renderer,
        (int)(ray.p[0] * hw + hw),
        (int)(ray.p[1] * hh + hh),
        (int)((ray.n[0] + ray.p[0]) * hw + hw),
        (int)((ray.n[1] + ray.p[1]) * hh + hh)
    );  
}

void point(vec3 v, mat4x4 m){
    int hw = WIDTH >> 1, hh = HEIGHT >> 1;
    vec4 lv = {1}, tv = {1}; 

    memcpy(lv, v, sizeof(vec3));
    mat4x4_mul_vec4(tv, m, lv);

    SDL_RenderDrawLine(
        renderer,
        (int)(tv[0] * hw + hw),
        (int)(tv[1] * hh + hh),
        (int)(tv[0] * hw + hw + 1),
        (int)(tv[1] * hh + hh + 1)
    );
}

void mesh(vec3* data, int verts, mat4x4 m){
	int li = -1; // last index
	int hw = WIDTH >> 1, hh = HEIGHT >> 1;
	
	for(;verts--;){
		int i = verts;
		
		if(li >= 0){
			vec4 lv = {1, 1, 1, 1}, v = {1, 1, 1, 1};
			vec4 tlv, tv;
            vec2 inter = {0};

			memcpy(lv, data[li], sizeof(vec3));
			memcpy(v, data[i], sizeof(vec3));

			mat4x4_mul_vec4(tlv, m, lv);
			mat4x4_mul_vec4(tv, m, v);

			SDL_RenderDrawLine(
				renderer,
				(int)(tlv[0] * hw + hw),
				(int)(tlv[1] * hh + hh),
				(int)(tv[0] * hw + hw),
				(int)(tv[1] * hh + hh)
			);
		}

		li = i;
	}
}