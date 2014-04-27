#include "momentCalculator.h"

#define SWP(arr, a, b){\
	vec2 tmp = {arr[a][0], arr[a][1]};\
	memcpy(arr[a], arr[b], sizeof(vec2));\
	memcpy(arr[b], tmp, sizeof(vec2));\
}\

int _sorted(vec2* arr, int size){
	float last = arr[0][0];
	for(int i = 0; i < size; i++){
		if(last > arr[i][0]) return 0;
		last = arr[i][0];
	}

	return 1;
}

int _intersections(vec3* data, int verts, vec2* intersections, ray2 r){
	int i, intInd = 0, li = -1; // intersection index

	for(i = 0; i < verts; i++){
		if(li >= 0){
			vec2 v1 = {data[li][0], data[li][1]}, v2 = {data[i][0], data[i][1]};
			vec2 inter = {0};

			if(vec2_ray_line(inter, r, v1, v2)){
				memcpy(intersections[intInd++], inter, sizeof(vec2));
			}
		}


		li = i;
	}

	while(!_sorted(intersections, intInd)){
		int last = 0;
		for(i = 0; i < intInd; i++){
			if(intersections[i][0] < intersections[last][0])
				SWP(intersections, i, last);
			last = i;
		}
	}

	return intInd;
}

float momentCalculator(vec3 CoM, vec3* data, int verts, int iterations){
	float moment = 0, delta = 0;
	int totalPoints = 0, i = verts, ints;
	vec3 min = {data[0][0], data[0][1], data[0][2]};
	vec3 max = {data[0][0], data[0][1], data[0][2]};
	vec2 intersections[10];

	// find the min and max y values
	for(;i--;){
		if(data[i][0] < min[0]){
			min[0] = data[i][0];
		}
		else if(data[i][0] > max[0]){
			max[0] = data[i][0];
		}

		if(data[i][1] < min[1]){
			min[1] = data[i][1];
		}
		else if(data[i][1] > max[1]){
			max[1] = data[i][1];
		}
	}

	printf("min %0.3f %0.3f %0.3f\n", min[0], min[1], min[2]);
	printf("max %0.3f %0.3f %0.3f\n", max[0], max[1], max[2]);

	delta = (max[1] - min[1]) / (float)iterations;

	for(i = iterations; i--;){
		ray2 ray = {
			.p = {min[0] - 0.1f, min[1] + delta * i},
			.n = {1, 0}
		};

		ints = _intersections(data, verts, intersections, ray);
		if(!(ints & 0x01)){
			int points = 0;
			float x = intersections[0][0];
			printf("intersections %d ", ints);

			while(x < intersections[ints-1][0]){
				vec2 point = {x, intersections[0][1]};
				float dx = point[0] - CoM[0], dy = point[1] - CoM[1];
				++points;
				++totalPoints;

				moment += sqrt((dx * dx) + (dy * dy));
				x += delta;
			}

			printf("Point masses: %d\n", points);
		}
	}

	return moment / (float)totalPoints;
}