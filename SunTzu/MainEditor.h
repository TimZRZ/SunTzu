#pragma once
#include <afxwin.h>
#include "STRender.h"

class MainEditor : public CWnd {

public:
	UINT_PTR timer;
	STRender* render;
	BOOL LButtonDown;
	CPoint lastMousePos;

	CStatic* campos;
	CStatic* camworldup;

	virtual void createEditor(CRect rect, CWnd* parent);
	virtual void initEditor();
	afx_msg void OnDraw(CDC* pDC);
	virtual void renderFrameBuffer(CDC* pDC);

	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnPaint();
	virtual void OnTimer(UINT nIDEvent);

	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

private:
	CWnd*	hWnd;
	HDC     hdc;
	CRect   originRect;

protected:
	DECLARE_MESSAGE_MAP();
};