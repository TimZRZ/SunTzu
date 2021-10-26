#include <algorithm>
#include "STRender.h"
#include "Matrix.h"

using namespace std;

STRender::STRender() {
	xres = 400;
	yres = 400;
	framebuffer = new char[3 * xres * yres];
	pixelbuffer = new STPixel[xres * xres];
	for (int i = 0; i < this->xres * this->yres; i++) {
		pixelbuffer[i].z = INT_MAX;
	}

	/* Init Xsp */
	Xsp[0][0] = xres / 2.0; Xsp[0][1] = 0;			Xsp[0][2] = 0;		Xsp[0][3] = xres / 2.0;
	Xsp[1][0] = 0;			Xsp[1][1] = -yres / 2.0;Xsp[1][2] = 0;		Xsp[1][3] = yres / 2.0;
	Xsp[2][0] = 0;			Xsp[2][1] = 0;			Xsp[2][2] = INT_MAX;Xsp[2][3] = 0;
	Xsp[3][0] = 0;			Xsp[3][1] = 0;			Xsp[3][2] = 0;		Xsp[3][3] = 1;

	/* Default Camera */
	camera.position[0] = 10.0;
	camera.position[1] = 0.0;
	camera.position[2] = 10.0;
	camera.lookat[0] = 0;
	camera.lookat[1] = 0;
	camera.lookat[2] = 0;
	camera.worldup[0] = 0;
	camera.worldup[1] = 1;
	camera.worldup[2] = 0;
	camera.FOV = 35.0;
}

void STRender::STBeginRender() {
	delete[] this->framebuffer;
	delete[] this->pixelbuffer;
	framebuffer = new char[3 * xres * yres];
	pixelbuffer = new STPixel[xres * xres];
	for (int i = 0; i < xres * yres; i++) {
		pixelbuffer[i].z = INT_MAX;
	}
	this->matlevel = 0;

	/* Define Xpi */
	float p = tan(camera.FOV * 3.1415926 / 360.0);
	camera.Xpi[0][0] = 1; camera.Xpi[0][1] = 0; camera.Xpi[0][2] = 0; camera.Xpi[0][3] = 0;
	camera.Xpi[1][0] = 0; camera.Xpi[1][1] = 1; camera.Xpi[1][2] = 0; camera.Xpi[1][3] = 0;
	camera.Xpi[2][0] = 0; camera.Xpi[2][1] = 0; camera.Xpi[2][2] = p; camera.Xpi[2][3] = 0;
	camera.Xpi[3][0] = 0; camera.Xpi[3][1] = 0; camera.Xpi[3][2] = p; camera.Xpi[3][3] = 1;

	/* Define Xiw */
	STCoord* clPtr = sub3(camera.lookat, camera.position);
	STCoord cl = { *clPtr[0], *(clPtr[0] + 1), *(clPtr[0] + 2) };
	STCoord Zvalue;
	normal3(Zvalue, cl);

	float upDotZ = dot3(this->camera.worldup, Zvalue);
	STCoord ZPrime = { camera.worldup[0] - upDotZ * Zvalue[0], camera.worldup[1] - upDotZ * Zvalue[1], camera.worldup[2] - upDotZ * Zvalue[2] };
	STCoord	Yvalue;
	normal3(Yvalue, ZPrime);

	STCoord Xvalue = {
		Yvalue[1] * Zvalue[2] - Yvalue[2] * Zvalue[1],
		Yvalue[2] * Zvalue[0] - Yvalue[0] * Zvalue[2],
		Yvalue[0] * Zvalue[1] - Yvalue[1] * Zvalue[0]
	};
	
	camera.Xiw[0][0] = Xvalue[0];	camera.Xiw[0][1] = Xvalue[1];	camera.Xiw[0][2] = Xvalue[2]; camera.Xiw[0][3] = -dot3(Xvalue, camera.position);
	camera.Xiw[1][0] = Yvalue[0];	camera.Xiw[1][1] = Yvalue[1];	camera.Xiw[1][2] = Yvalue[2]; camera.Xiw[1][3] = -dot3(Yvalue, camera.position);
	camera.Xiw[2][0] = Zvalue[0];	camera.Xiw[2][1] = Zvalue[1];	camera.Xiw[2][2] = Zvalue[2]; camera.Xiw[2][3] = -dot3(Zvalue, camera.position);
	camera.Xiw[3][0] = 0;			camera.Xiw[3][1] = 0;			camera.Xiw[3][2] = 0;		camera.Xiw[3][3] = 1;

	/* Reset transform stack */
	this->matlevel = 0;

	/* Put transform matrices into stack */
	this->STPushMatrix(this->Xsp);
	this->STPushMatrix(this->camera.Xpi);
	this->STPushMatrix(this->camera.Xiw);
}

void STRender::STPushMatrix(STMatrix m) {
	if (matlevel == MATLEVELS) {
		return;
	}

	if (matlevel == 0) {
		setMatrix(&Ximage[0], m);
	} else {
		crossMatrix(&Ximage[matlevel], Ximage[matlevel - 1], m);
	}

	matlevel++;
}

void STRender::STPushObject(STRenderObject obj) {
	if (objectNum == MAXOBJECTS) {
		return;
	}
	objects[objectNum] = obj;
	objectNum++;
}

void shade(STCoord norm, STCoord color) {
	STCoord	light;
	float		coef;

	light[0] = 0.707f;
	light[1] = 0.5f;
	light[2] = 0.5f;

	coef = light[0] * norm[0] + light[1] * norm[1] + light[2] * norm[2];
	if (coef < 0) 	coef *= -1;

	if (coef > 1.0)	coef = 1.0;
	color[0] = coef * 0.95f;
	color[1] = coef * 0.65f;
	color[2] = coef * 0.88f;
}

void STRender::STAddTriangleToObject(int id, int numAttributes, int* nameList, STPointer* valueList) {
	STCoord* posList[3];
	STCoord* normalList[3];
	for (int i = 0; i < numAttributes; i++) {
		if (nameList[i] == 0) {
			posList[0] = (STCoord*)valueList[i];
			posList[1] = (STCoord*)valueList[i] + 1;
			posList[2] = (STCoord*)valueList[i] + 2;
		} else if (nameList[i] == 1) {
			normalList[0] = (STCoord*)valueList[i];
			normalList[1] = (STCoord*)valueList[i] + 1;
			normalList[2] = (STCoord*)valueList[i] + 2;
		}
	}

	STTriangle triangle;
	triangle.vertices[0][0] = *(*posList)[0]; triangle.vertices[0][1] = *((*posList)[0] + 1); triangle.vertices[0][2] = *((*posList)[0] + 2);
	triangle.vertices[1][0] = *(*posList)[1]; triangle.vertices[1][1] = *((*posList)[1] + 1); triangle.vertices[1][2] = *((*posList)[1] + 2);
	triangle.vertices[2][0] = *(*posList)[2]; triangle.vertices[2][1] = *((*posList)[2] + 1); triangle.vertices[2][2] = *((*posList)[2] + 2);

	triangle.normals[0][0] = *(*normalList)[0]; triangle.normals[0][1] = *((*normalList)[0] + 1); triangle.normals[0][2] = *((*normalList)[0] + 2);
	triangle.normals[1][0] = *(*normalList)[1]; triangle.normals[1][1] = *((*normalList)[1] + 1); triangle.normals[1][2] = *((*normalList)[1] + 2);
	triangle.normals[2][0] = *(*normalList)[2]; triangle.normals[2][1] = *((*normalList)[2] + 1); triangle.normals[2][2] = *((*normalList)[2] + 2);

	shade(triangle.normals[0], triangle.color);
	objects[id].triangles.push_back(triangle);
}

void STRender::STPutAttribute(int numAttributes, int* nameList, STPointer* valueList) {
	for (int i = 0; i < numAttributes; i++) {
		if (nameList[i] == ST_RGB_COLOR) {
			STColor* color = (STColor*)valueList[i];
			this->flatcolor[0] = (*color)[0];
			this->flatcolor[1] = (*color)[1];
			this->flatcolor[2] = (*color)[2];
		}
	}
}

void STRender::STRendObject(int id) {
	STRenderObject object = objects[id];
	for (STTriangle & tri : object.triangles) {
		float modelA[4] = { tri.vertices[0][0], tri.vertices[0][1], tri.vertices[0][2], 1 };
		float modelB[4] = { tri.vertices[1][0], tri.vertices[1][1], tri.vertices[1][2], 1 };
		float modelC[4] = { tri.vertices[2][0], tri.vertices[2][1], tri.vertices[2][2], 1 };

		/* Change model X, Y, Z into screen X, Y, Z */
		STMatrix* x = &this->Ximage[this->matlevel - 1];
		double A[3] = { dot4(modelA, (*x)[0]) / dot4(modelA, (*x)[3]), dot4(modelA, (*x)[1]) / dot4(modelA, (*x)[3]), dot4(modelA, (*x)[2]) / dot4(modelA, (*x)[3]) };
		double B[3] = { dot4(modelB, (*x)[0]) / dot4(modelB, (*x)[3]), dot4(modelB, (*x)[1]) / dot4(modelB, (*x)[3]), dot4(modelB, (*x)[2]) / dot4(modelB, (*x)[3]) };
		double C[3] = { dot4(modelC, (*x)[0]) / dot4(modelC, (*x)[3]), dot4(modelC, (*x)[1]) / dot4(modelC, (*x)[3]), dot4(modelC, (*x)[2]) / dot4(modelC, (*x)[3]) };

		/* Precheck */
		bool xIn = false;
		bool yIn = false;
		for (double* pos : { A, B, C }) {
			if (pos == nullptr) {
				return;
			}
			if (pos[0] >= 0 && pos[0] <= this->xres) {
				xIn = true;
			}
			if (pos[1] >= 0 && pos[1] <= this->yres) {
				yIn = true;
			}
			if (pos[2] < 0) {
				return;
			}
		}
		if (!(xIn && yIn)) {
			return;
		}

		double AB[3] = { B[0] - A[0], B[1] - A[1], B[2] - A[2] };
		double AC[3] = { C[0] - A[0], C[1] - A[1], C[2] - A[2] };
		double XMul = AB[1] * AC[2] - AB[2] * AC[1];
		double YMul = AB[2] * AC[0] - AB[0] * AC[2];
		double ZMul = AB[0] * AC[1] - AB[1] * AC[0];
		double D = -(XMul * A[0] + YMul * A[1] + ZMul * A[2]);

		/* Vector Sorting */
		double* V1Ptr, * V2Ptr, * V3Ptr;
		V1Ptr = A[1] > B[1] ? (A[1] > C[1] ? A : C) : (B[1] > C[1] ? B : C);
		V3Ptr = A[1] > B[1] ? (B[1] > C[1] ? C : B) : (A[1] > C[1] ? C : A);
		V2Ptr = V1Ptr == A ? (V3Ptr == B ? C : B) : (V1Ptr == B ? (V3Ptr == A ? C : A) : (V3Ptr == A ? B : A));
		double V1[3] = { *V1Ptr, *(V1Ptr + 1), *(V1Ptr + 2) };
		double V2[3] = { *V2Ptr, *(V2Ptr + 1), *(V2Ptr + 2) };
		double V3[3] = { *V3Ptr, *(V3Ptr + 1), *(V3Ptr + 2) };

		/* Find CW Edges */
		double dV13 = (V1[1] - V3[1]) / (V1[0] - V3[0]);
		double kV13 = V1[1] - dV13 * V1[0];
		double X2 = (V2[1] - kV13) / dV13;

		//double L1[3], L2[3], R1[3], R2[3];
		double A1, B1, C1;
		double A2, B2, C2;
		double A3, B3, C3;
		bool LRR = false;
		if (X2 < V2[0]) {
			LRR = true;
			A1 = V2[1] - V1[1]; B1 = -(V2[0] - V1[0]); C1 = (V2[0] - V1[0]) * V1[1] - (V2[1] - V1[1]) * V1[0];
			A2 = V3[1] - V2[1]; B2 = -(V3[0] - V2[0]); C2 = (V3[0] - V2[0]) * V2[1] - (V3[1] - V2[1]) * V2[0];
			A3 = V1[1] - V3[1]; B3 = -(V1[0] - V3[0]); C3 = (V1[0] - V3[0]) * V3[1] - (V1[1] - V3[1]) * V3[0];
		}
		else {
			A1 = V3[1] - V1[1]; B1 = -(V3[0] - V1[0]); C1 = (V3[0] - V1[0]) * V1[1] - (V3[1] - V1[1]) * V1[0];
			A2 = V2[1] - V3[1]; B2 = -(V2[0] - V3[0]); C2 = (V2[0] - V3[0]) * V3[1] - (V2[1] - V3[1]) * V3[0];
			A3 = V1[1] - V2[1]; B3 = -(V1[0] - V2[0]); C3 = (V1[0] - V2[0]) * V2[1] - (V1[1] - V2[1]) * V2[0];
		}

		/* LEE */
		for (int i = min(min(V1[0], V2[0]), V3[0]); i <= max(max(V1[0], V2[0]), V3[0]); i++) {
			if (i < 0 || i >= this->xres) { continue; }
			for (int j = min(min(V1[1], V2[1]), V3[1]); j <= max(max(V1[1], V2[1]), V3[1]); j++) {
				if (j < 0 || j >= this->yres) { continue; }
				int z = -(XMul * i + YMul * j + D) / ZMul;
				if (z < 0 || this->pixelbuffer[j * this->yres + i].z < z) { continue; }
				bool shouldPaint = false;
				if (LRR) {
					if ((A1 * i + B1 * j + C1 < 0.0 && A2 * i + B2 * j + C2 < 0.0 && A3 * i + B3 * j + C3 <= 0.0) ||
						(A1 * i + B1 * j + C1 > 0.0 && A2 * i + B2 * j + C2 > 0.0 && A3 * i + B3 * j + C3 >= 0.0)) {
						shouldPaint = true;
					}
				}
				else {
					if ((A1 * i + B1 * j + C1 < 0.0 && A2 * i + B2 * j + C2 <= 0.0 && A3 * i + B3 * j + C3 <= 0.0) ||
						(A1 * i + B1 * j + C1 > 0.0 && A2 * i + B2 * j + C2 >= 0.0 && A3 * i + B3 * j + C3 >= 0.0)) {
						shouldPaint = true;
					}
				}
				if (shouldPaint) {
					this->pixelbuffer[j * this->yres + i].red = 4095 * tri.color[0];
					this->pixelbuffer[j * this->yres + i].green = 4095 * tri.color[1];
					this->pixelbuffer[j * this->yres + i].blue = 4095 * tri.color[2];
					this->pixelbuffer[j * this->yres + i].z = z;
				}
			}
		}
	}
}

void STRender::STFlushDisplay2FrameBuffer() {
	for (int i = 0; i < this->xres * this->yres; i++) {
		this->framebuffer[i * 3] = (char)(this->pixelbuffer[i].blue >> 4);
		this->framebuffer[i * 3 + 1] = (char)(this->pixelbuffer[i].green >> 4);
		this->framebuffer[i * 3 + 2] = (char)(this->pixelbuffer[i].red >> 4);
	}
}