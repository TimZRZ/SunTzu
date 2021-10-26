#pragma once
#include <vector>

using namespace std;

const int ST_RGB_COLOR = 99;
const float PI = 3.1415926535897;

typedef void*	STPointer;
typedef float   STCoord[3];
typedef float   STColor[3];
typedef float	STMatrix[4][4];

typedef	struct {
	short    red;
	short    green;
	short    blue;
	short    alpha;
	int	 z;
} STPixel;

typedef struct {
    STMatrix    Xiw;
    STMatrix    Xpi;
    STCoord     position;
    STCoord     lookat;
    STCoord     worldup;
    float       FOV;
} STCamera;

typedef struct {
    STCoord vertices[3];
    STCoord normals[3];
    STColor color;
} STTriangle;

typedef struct {
    vector<STTriangle>  triangles;
} STRenderObject;