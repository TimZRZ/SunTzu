#include "App.h"

SunTzuApp app;

BOOL SunTzuApp::InitInstance() {
	
	/* Create main window */
	SunTzuFrame* frame = new SunTzuFrame();

	/* Show and update window */
	frame->ShowWindow(SW_NORMAL);
	frame->UpdateWindow();

	/* Save the main window */
	this->m_pMainWnd = frame;

	return true;
}

SunTzuFrame::SunTzuFrame() {
	Create(NULL, TEXT("SunTzu Game Engine"));

	CRect rect = new CRect(0, 0, 500, 500);

	/* Create main editor */
	editor.createEditor(rect, this);
	editor.timer = editor.SetTimer(1, 1, 0);
}
