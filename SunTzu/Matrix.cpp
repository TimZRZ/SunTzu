#include <math.h>
#include "Matrix.h"

STCoord* sub3(STCoord v1, STCoord v2) {
	STCoord result = { v1[0] - v2[0], v1[1] - v2[1], v1[2] - v2[2] };
	return &result;
}

void normal3(float* v, float* v1) {
	float d = sqrt(pow(v1[0], 2) + pow(v1[1], 2) + pow(v1[2], 2));
	v[0] = v1[0] / d;
	v[1] = v1[1] / d;
	v[2] = v1[2] / d;
}

float dot3(float* v1, float* v2) {
	float result = v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
	return result;
}

float dot4(float* v1, float* v2) {
	float result = v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2] + v1[3] * v2[3];
	return result;
}

void setMatrix(STMatrix* m1, STMatrix m2) {
	(*m1)[0][0] = m2[0][0]; (*m1)[0][1] = m2[0][1]; (*m1)[0][2] = m2[0][2]; (*m1)[0][3] = m2[0][3];
	(*m1)[1][0] = m2[1][0]; (*m1)[1][1] = m2[1][1]; (*m1)[1][2] = m2[1][2]; (*m1)[1][3] = m2[1][3];
	(*m1)[2][0] = m2[2][0]; (*m1)[2][1] = m2[2][1]; (*m1)[2][2] = m2[2][2]; (*m1)[2][3] = m2[2][3];
	(*m1)[3][0] = m2[3][0]; (*m1)[3][1] = m2[3][1]; (*m1)[3][2] = m2[3][2]; (*m1)[3][3] = m2[3][3];
}

void crossMatrix(STMatrix* m, STMatrix m1, STMatrix m2) {
	STMatrix t = {
		{m2[0][0], m2[1][0], m2[2][0], m2[3][0] },
		{m2[0][1], m2[1][1], m2[2][1], m2[3][1] },
		{m2[0][2], m2[1][2], m2[2][2], m2[3][2] },
		{m2[0][3], m2[1][3], m2[2][3], m2[3][3] },
	};

	(*m)[0][0] = dot4(m1[0], t[0]); (*m)[0][1] = dot4(m1[0], t[1]); (*m)[0][2] = dot4(m1[0], t[2]); (*m)[0][3] = dot4(m1[0], t[3]);
	(*m)[1][0] = dot4(m1[1], t[0]); (*m)[1][1] = dot4(m1[1], t[1]); (*m)[1][2] = dot4(m1[1], t[2]); (*m)[1][3] = dot4(m1[1], t[3]);
	(*m)[2][0] = dot4(m1[2], t[0]); (*m)[2][1] = dot4(m1[2], t[1]); (*m)[2][2] = dot4(m1[2], t[2]); (*m)[2][3] = dot4(m1[2], t[3]);
	(*m)[3][0] = dot4(m1[3], t[0]); (*m)[3][1] = dot4(m1[3], t[1]); (*m)[3][2] = dot4(m1[3], t[2]); (*m)[3][3] = dot4(m1[3], t[3]);
}

void crossMatrix(float* m, STMatrix m1, float* m2, int n) {
	if (n == 4) {
		m[0] = dot4(m1[0], m2);
		m[1] = dot4(m1[1], m2);
		m[2] = dot4(m1[2], m2);
		m[3] = dot4(m1[3], m2);
	} else if (n == 3) {
		m[0] = dot3(m1[0], m2);
		m[1] = dot3(m1[1], m2);
		m[2] = dot3(m1[2], m2);
	}
}

void rotationReverse(STMatrix* m, STMatrix m2) {
	(*m)[0][0] = m2[0][0]; (*m)[0][1] = m2[1][0]; (*m)[0][2] = m2[2][0]; (*m)[0][3] = m2[0][3];
	(*m)[1][0] = m2[0][1]; (*m)[1][1] = m2[1][1]; (*m)[1][2] = m2[2][1]; (*m)[1][3] = m2[1][3];
	(*m)[2][0] = m2[0][2]; (*m)[2][1] = m2[1][2]; (*m)[2][2] = m2[2][2]; (*m)[2][3] = m2[2][3];
	(*m)[3][0] = m2[3][0]; (*m)[3][1] = m2[3][1]; (*m)[3][2] = m2[3][2]; (*m)[3][3] = m2[3][3];
}