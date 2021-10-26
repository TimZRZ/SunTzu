#pragma once
#include <afxwin.h>
#include "MainEditor.h"

class SunTzuApp : public CWinApp {

public:
	/** Entry point of Application*/
	virtual BOOL InitInstance();

};

class SunTzuFrame : public CFrameWnd {

public:
	SunTzuFrame();

private:
	MainEditor editor;
};