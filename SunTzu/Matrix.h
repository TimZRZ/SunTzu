#pragma once
#include "ST.h"

STCoord* sub3(STCoord v1, STCoord v2);

void normal3(float* v, float* v1);

float dot3(float* v1, float* v2);

float dot4(float* v1, float* v2);

void setMatrix(STMatrix* m1, STMatrix m2);

void crossMatrix(STMatrix* m, STMatrix m1, STMatrix m2);

void crossMatrix(float* m, STMatrix m1, float* m2, int n);

void rotationReverse(STMatrix* m, STMatrix m2);