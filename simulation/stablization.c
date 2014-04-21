// Example program:
// Using SDL2 to create an application window

#include "SDL.h"
#include "linmath.h"
#include <stdio.h>
#include <math.h>
#include <unistd.h>

#define MESH_SEGMENTS 9

typedef struct{
    vec3 center_of_mass;
    float omega; // angular velocity
    float theta; // angle
    float mass;
}Body;

int WIDTH = 640;
int HEIGHT = 480;

SDL_Renderer *renderer;

void center(vec3 center, vec3* data, int verts){
    int i = verts;

    bzero(center, sizeof(vec3));
    for(;i--;){
        vec3_add(center, center, data[i]);
    } vec3_scale(center, center, 1 / verts);

    for(i = verts; i--;){
        vec3_sub(data[i], data[i], center);
    }
}

void applyForce(Body* body, vec3 point, vec3 force, float dt){
    vec3 L = {0};
    vec3_mul_cross(L, point, force);

    body->omega += (L[2] / body->mass) * dt;
}

void mesh(vec3* data, int verts, mat4x4 m){
	int li = -1; // last index
	int hw = WIDTH >> 1, hh = HEIGHT >> 1;
	
	for(;verts--;){
		int i = verts;
		
		if(li >= 0){
			vec4 lv = {1, 1, 1, 1}, v = {1, 1, 1, 1};
			vec4 tlv, tv;

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

int main(int argc, char* argv[]) {
    float baseThrottle = 1;
    vec3 rightMotor = { 1.25,-0.1, 1 };
    vec3 leftMotor = { -1.25,-0.1, 1 };
    vec3 lProp[2] = {
        {1.15,-0.15, 1},
        {1.35,-0.15, 1}
    };
    vec3 rProp[2] = {
        {-1.15,-0.15, 1},
        {-1.35,-0.15, 1}
    };
    vec3 l[MESH_SEGMENTS] = {
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
    SDL_Window   *window;                    // Declare a pointer
    Body quadRotor = {
        .omega = 0,
        .theta = 0,
        .mass  = 3 // 3 kg
    };

    SDL_Init(SDL_INIT_VIDEO);              // Initialize SDL2

    // Create an application window with the following settings:
    window = SDL_CreateWindow(
        "An SDL2 window",                  // window title
        SDL_WINDOWPOS_UNDEFINED,           // initial x position
        SDL_WINDOWPOS_UNDEFINED,           // initial y position
        640,                               // width, in pixels
        480,                               // height, in pixels
        SDL_WINDOW_OPENGL                  // flags - see below
    );
    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED
    );

    // Check that the window was successfully made
    if (window == NULL) {
        // In the event that the window could not be made...
        printf("Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    Uint32 last = SDL_GetTicks();

    center(quadRotor.center_of_mass, l, MESH_SEGMENTS);
    for(int i = 2; i--;){
        vec3_sub(lProp[i], lProp[i], quadRotor.center_of_mass);
        vec3_sub(rProp[i], rProp[i], quadRotor.center_of_mass);
    }

    // The window is open: enter program loop (see SDL_PollEvent)
    while(1){
        Uint32 ticks = SDL_GetTicks();
        mat4x4 m, r, t;
        SDL_Event e;
        float dt = (ticks - last) / 1000.0f;
        float lThrottle = 0, rThrottle = 0;

        printf("%f seconds\n", dt);
        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                break;
            }
            if(e.type == SDL_KEYDOWN || e.type == SDL_KEYUP){
                vec3 thrust = {0, -10, 0};
                if(e.key.keysym.scancode == SDL_SCANCODE_RIGHT){
                    rThrottle += 10;
                    //applyForce(&quadRotor, rightMotor, thrust, dt);
                }

                if(e.key.keysym.scancode == SDL_SCANCODE_LEFT){
                    lThrottle += 10;
                    
                } 
            }
        }
        last = ticks;


        // Physics stuff
        quadRotor.theta += dt * quadRotor.omega;

        // TODO stabilize!
        lThrottle += baseThrottle - quadRotor.theta;
        rThrottle += baseThrottle + quadRotor.theta;

        vec3 lThrust = {0, -lThrottle, 0};
        vec3 rThrust = {0, -rThrottle, 0};
        applyForce(&quadRotor, leftMotor, lThrust, dt);
        applyForce(&quadRotor, rightMotor, rThrust, dt);

        mat4x4_identity(t);
        mat4x4_scale_aniso(t, t, HEIGHT * 0.5f / (float)WIDTH, 0.5f, 1);
        mat4x4_rotate(r, t, 0, 0, 1, quadRotor.theta);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 16);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 128);
        mesh(l, MESH_SEGMENTS, r);

        SDL_SetRenderDrawColor(renderer, (int)(128 * lThrottle * lThrottle), 0, 0, 255);
        mesh(lProp, 2, r);
        SDL_SetRenderDrawColor(renderer, (int)(128 * rThrottle * lThrottle), 0, 0, 255);
        mesh(rProp, 2, r);

        SDL_RenderPresent(renderer);
    }
    // Close and destroy the window
    SDL_DestroyWindow(window);

    // Clean up
    SDL_Quit();
    return 0;
}
