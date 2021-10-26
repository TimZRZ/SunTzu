#pragma once
#include "ST.h"

const int MATLEVELS  = 100;
const int MAXOBJECTS = 100;

class STRender {

public:
	unsigned short	xres;
	unsigned short	yres;
	char*			framebuffer;
	STPixel*		pixelbuffer;
	STColor			flatcolor;
	STCamera		camera;
	int				matlevel;
	STMatrix		Ximage[MATLEVELS];
	STMatrix		Xsp;
	int				objectNum;
	STRenderObject	objects[MAXOBJECTS];

	STRender();
	void STBeginRender();
	void STPushMatrix(STMatrix matrix);
	void STPushObject(STRenderObject obj);
	void STAddTriangleToObject(int id, int numAttributes, int* nameList, STPointer* valueList);
	void STRendObject(int id);
	void STPutAttribute(int numAttributes, int* nameList, STPointer* valueList);
	void STFlushDisplay2FrameBuffer();
};