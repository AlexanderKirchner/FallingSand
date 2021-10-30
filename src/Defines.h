#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define WINDOW_X 800
#define WINDOW_Y 600

#define SAND_ID 0
#define STONE_ID 2
#define WATER_ID 1
#define AIR_ID 255

typedef struct {
	float x; // coordinates
	float y;
	float z;
	
	float u; // texture coordinates
	float v; 
} Vertex;

typedef struct {
	int x;
	int y;
} Vec2i;