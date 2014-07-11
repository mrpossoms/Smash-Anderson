// Example program:
// Using SDL2 to create an application window

#include "SDL.h"
#include "linmath.h"
#include "momentCalculator.h"
#include "./drawing/shapes.h"
#include "./control/pid.h"
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <assert.h>

#define KP 4.0f
#define KI 2.0f
#define KD 0.75f

typedef struct{
    vec3 center_of_mass;
    vec3 position;
    vec3 velocity;
    float omega; // angular velocity
    float theta; // angle
    float mass;
    float moment;
}Body;

WIDTH = 640;
HEIGHT = 480;

PidState altState = {
    .Kp = KP,
    .Ki = KI,
    .Kd = KD,
    .lastError = 0,
    .integral = 0,
    .target = 0
};

PidState rollState = {
    .Kp = 2.0f,
    .Ki = 2.0f,
    .Kd = 0.75f,
    .lastError = 0,
    .integral = 0,
    .target = 0
};

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

// F = m * a
void applyForce(Body* body, vec3 point, vec3 force, float dt){
    vec3 L = {0}, linForce;
    mat4x4 i, r;

    mat4x4_identity(i);
    mat4x4_rotate(r, i, 0, 0, 1, body->theta);
    vec3_mul_cross(L, point, force);

    body->omega += body->moment * (L[2] / body->mass) * dt;

    // v += (F / m) / (L[2] + 1)
    // v += F / m

    vec3_scale(linForce, force, dt / -body->mass);

    {
        vec4 temp, trans; memcpy(temp, linForce, sizeof(vec3));
        temp[3] = 1.f;

        mat4x4_mul_vec4(trans, r, temp);

        memcpy(linForce, trans, sizeof(vec3));

        printf("F -> {%0.3f, %0.3f, %0.3f}\n",
            linForce[0],
            linForce[1],
            linForce[2]
        );

    }

    // v += acc
    vec3_add(body->velocity, body->velocity, linForce);
}

void applyGravity(Body* body, float dt){
    body->velocity[1] -= 9.8 * dt;
}

void applyDrag(Body* body, float dt){
    vec3 temp;
    vec3_scale(temp, body->velocity, dt * -0.9f);
    vec3_add(body->velocity, body->velocity, temp);
    body->omega += body->omega * dt * -0.9f;

    printf("v -> {%0.3f, %0.3f, %0.3f}\n",
        body->velocity[0],
        body->velocity[1],
        body->velocity[2]
    );
}

int main(int argc, char* argv[]) {
    float baseThrottle = 1;
    vec3 rightMotor = { 1.25,-0.1, 1 };
    vec3 leftMotor = { -1.25,-0.1, 1 };

    SDL_Window   *window;                    // Declare a pointer
    SDL_Rect screen = {0, 0, WIDTH, HEIGHT};
    Body quadRotor = {
        .omega = 0,
        .theta = 0,
        .mass  = 3 // 3 kg
    };

    center(quadRotor.center_of_mass, Frame, MESH_SEGMENTS);
    quadRotor.moment = quadRotor.mass * momentCalculator(quadRotor.center_of_mass, Frame, MESH_SEGMENTS, 100);
    printf("Moment %f\n", quadRotor.moment);

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

    for(int i = 2; i--;){
        vec3_sub(lProp[i], lProp[i], quadRotor.center_of_mass);
        vec3_sub(rProp[i], rProp[i], quadRotor.center_of_mass);
    }

    // The window is open: enter program loop (see SDL_PollEvent)
    float ang = 0;
    while(1){

        int disabled = 0;
        Uint32 ticks = SDL_GetTicks();
        mat4x4 m, r, t;
        SDL_Event e;
        float dt = (ticks - last) / 1000.0f;
        float lThrottle = 0, rThrottle = 0;


        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                break;
            }
            if(e.type == SDL_KEYDOWN || e.type == SDL_KEYUP){
                vec3 thrust = {0, -10, 0};
                if(e.key.keysym.scancode == SDL_SCANCODE_SPACE){
                    quadRotor.theta = ((random() % 1000) / 1000.0) * M_PI * 2 - M_PI;
                }
                if(e.key.keysym.scancode == SDL_SCANCODE_RIGHT){
                    rThrottle += 3;
                }

                if(e.key.keysym.scancode == SDL_SCANCODE_LEFT){
                    lThrottle += 3;
                } 
            }
        }
        last = ticks;


        // Physics stuff
        {
            vec3 temp; memcpy(temp, quadRotor.velocity, sizeof(vec3));

            // rotation
            quadRotor.theta += dt * quadRotor.omega;
            
            // translation
            vec3_scale(temp, temp, dt);
            vec3_add(
                quadRotor.position,
                quadRotor.position,
                temp
            );
        }
        while(quadRotor.theta >  M_PI * 2) quadRotor.theta -= M_PI * 2;
        while(quadRotor.theta < -M_PI * 2) quadRotor.theta += M_PI * 2;
/*
        ____ a ____
        ---__-__---

        f=ma
        t=r
*/


        // TODO stabilize!
        float a = 0, theta = quadRotor.theta, w = quadRotor.omega;
        float cancel = 0;
        float bias = 0.01f;

        if(dt <= 0) continue;

        assert(dt > 0);

        float out = PID(&rollState, quadRotor.theta, dt);
        float alt = PID(&altState, quadRotor.position[1], dt);

        lThrottle += out < 0 ? out : 0;
        rThrottle += out > 0 ? -out : 0;

        lThrottle += alt > 0 ? alt : 0;
        rThrottle += alt > 0 ? alt : 0;

        printf("%f alt %f %f\n", lThrottle, alt, rThrottle);

        vec3 lThrust = {0, -lThrottle * 10, 0};
        vec3 rThrust = {0, -rThrottle * 10, 0};
        vec2 inter = {0};

        applyForce(&quadRotor, leftMotor, lThrust, dt);
        applyForce(&quadRotor, rightMotor, rThrust, dt);
        applyGravity(&quadRotor, dt);
        applyDrag(&quadRotor, dt);

        printf("{%0.3f, %0.3f, %0.3f}\n",
            quadRotor.position[0],
            quadRotor.position[1],
            quadRotor.position[2]
        );

        mat4x4_identity(t);
        mat4x4_scale_aniso(t, t, HEIGHT * 0.5f / (float)WIDTH, 0.5f, 1);
        mat4x4_rotate(r, t, 0, 0, 1, quadRotor.theta);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 128);
        mesh(Frame, MESH_SEGMENTS, r);

        SDL_SetRenderDrawColor(renderer, (int)(64 + 64 * lThrottle), 0, 0, 255);
        mesh(lProp, 2, r);
        SDL_SetRenderDrawColor(renderer, 0, (int)(64 + 64 * rThrottle), 0, 255);
        mesh(rProp, 2, r);

        SDL_RenderPresent(renderer);
    }
    // Close and destroy the window
    SDL_DestroyWindow(window);

    // Clean up
    SDL_Quit();
    return 0;
}
