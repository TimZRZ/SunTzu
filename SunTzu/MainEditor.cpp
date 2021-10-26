#include "MainEditor.h"
#include "Matrix.h"
#include "CameraMove.h"
#include <string>

BEGIN_MESSAGE_MAP(MainEditor, CWnd)
    ON_WM_PAINT()
    ON_WM_CREATE()
    ON_WM_TIMER()
    ON_WM_MOUSEWHEEL()
    ON_WM_MOUSEMOVE()
    //ON_WM_LBUTTONDOWN()
    //ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

void MainEditor::createEditor(CRect rect, CWnd* parent) {
    CString className = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_OWNDC, NULL, (HBRUSH)GetStockObject(BLACK_BRUSH), NULL);
    CString windowName = "Editor";
    CreateEx(0, className, windowName, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, rect, parent, 0);

    this->originRect = rect;
    this->hWnd = parent;

    campos = new CStatic;
    campos->Create(L"Present", WS_CHILD | WS_VISIBLE,
        CRect(550, 50, 750, 100), hWnd);

    camworldup = new CStatic;
    camworldup->Create(L"Present", WS_CHILD | WS_VISIBLE,
        CRect(550, 150, 750, 200), hWnd);
}

void MainEditor::initEditor() {

    this->hdc = GetDC()->m_hDC;

    /* Send draw request */
    OnDraw(NULL);

    this->render = new STRender();
    this->render->STBeginRender();

    FILE* infile;
    char dummy[256];
    if ((infile = fopen("pot4.asc", "r")) == NULL) {
        AfxMessageBox(_T("The input file was not opened\n"));
    }
    float vertexList[3][3];
    float normalList[3][3];
    float uvList[3][3];
    int nameList[2];
    STPointer valueList[2];
    STRenderObject obj;
    render->STPushObject(obj);
    while (fscanf(infile, "%s", dummy) == 1) {
        fscanf(infile, "%f %f %f %f %f %f %f %f",
            &(vertexList[0][0]), &(vertexList[0][1]),
            &(vertexList[0][2]),
            &(normalList[0][0]), &(normalList[0][1]),
            &(normalList[0][2]),
            &(uvList[0][0]), &(uvList[0][1]));
        fscanf(infile, "%f %f %f %f %f %f %f %f",
            &(vertexList[1][0]), &(vertexList[1][1]),
            &(vertexList[1][2]),
            &(normalList[1][0]), &(normalList[1][1]),
            &(normalList[1][2]),
            &(uvList[1][0]), &(uvList[1][1]));
        fscanf(infile, "%f %f %f %f %f %f %f %f",
            &(vertexList[2][0]), &(vertexList[2][1]),
            &(vertexList[2][2]),
            &(normalList[2][0]), &(normalList[2][1]),
            &(normalList[2][2]),
            &(uvList[2][0]), &(uvList[2][1]));

        nameList[0] = 0;
        valueList[0] = vertexList;
        nameList[1] = 1;
        valueList[1] = normalList;
        render->STAddTriangleToObject(0, 2, nameList, valueList);
    }

    render->STRendObject(0);
    render->STFlushDisplay2FrameBuffer();
}

int MainEditor::OnCreate(LPCREATESTRUCT lpCreateStruct) {
    if (CWnd::OnCreate(lpCreateStruct) == -1) {
        return -1;
    }

    initEditor();

    return 0;
}

void MainEditor::OnPaint() {
    ValidateRect(NULL);
}

afx_msg void MainEditor::OnDraw(CDC* pDC) {
    /* Camera */
}


void MainEditor::OnTimer(UINT nIDEvent) {
    switch (nIDEvent) {
        case 1:
        {
            // Clear color and depth buffer bits  
            //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            renderFrameBuffer(GetDC());

            // Swap buffers  
            SwapBuffers(hdc);

            break;
        }
    }

	CWnd::OnTimer(nIDEvent);
}

afx_msg void MainEditor::OnMouseMove(UINT nFlags, CPoint point) {
    if (nFlags != MK_LBUTTON && nFlags != MK_RBUTTON) {
        if (lastMousePos.x >= 0) {
            lastMousePos.x = -1;
        }
        return;
    }

    if (lastMousePos.x < 0) {
        lastMousePos = point;
    } else {
        if (nFlags == MK_LBUTTON) {
            float deltaX = point.x - lastMousePos.x;
            verticalRotate(&render->camera, deltaX);
            CString pstr = (to_string(render->camera.position[0]) + ", " + to_string(render->camera.position[1]) + ", " + to_string(render->camera.position[2])).c_str();
            campos->SetWindowTextW(pstr);
            CString wstr = (to_string(render->camera.worldup[0]) + ", " + to_string(render->camera.worldup[1]) + ", " + to_string(render->camera.worldup[2])).c_str();
            camworldup->SetWindowTextW(wstr);
        } else {
            float deltaY = point.y - lastMousePos.y;
            horizontalRotate(&render->camera, -deltaY);
            CString pstr = (to_string(render->camera.position[0]) + ", " + to_string(render->camera.position[1]) + ", " + to_string(render->camera.position[2])).c_str();
            campos->SetWindowTextW(pstr);
            CString wstr = (to_string(render->camera.worldup[0]) + ", " + to_string(render->camera.worldup[1]) + ", " + to_string(render->camera.worldup[2])).c_str();
            camworldup->SetWindowTextW(wstr);
        }
        

        /*render->camera.position[0] = dot3(rotateX[0], newPos) + render->camera.lookat[0];
        render->camera.position[1] = dot3(rotateX[1], newPos) + render->camera.lookat[1];
        render->camera.position[2] = dot3(rotateX[2], newPos) + render->camera.lookat[2];*/

        render->STBeginRender();
        render->STRendObject(0);
        render->STFlushDisplay2FrameBuffer();
        renderFrameBuffer(GetDC());
        lastMousePos = point;
    }
}

afx_msg BOOL MainEditor::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {
    if (zDelta > 0) {
        render->camera.position[2] += 1;
        render->camera.lookat[2] += 1;
        render->STBeginRender();
        render->STRendObject(0);
        render->STFlushDisplay2FrameBuffer();
        renderFrameBuffer(GetDC());
    }

    if (zDelta < 0) {
        render->camera.position[2] -= 1;
        render->camera.lookat[2] -= 1;
        render->STBeginRender();
        render->STRendObject(0);
        render->STFlushDisplay2FrameBuffer();
        renderFrameBuffer(GetDC());
    }

    return true;
}

void MainEditor::renderFrameBuffer(CDC* pDC) {
    if (this->render->framebuffer == NULL) {
        return;
    }

    HDC hdc;
    hdc = ::CreateCompatibleDC(pDC->m_hDC);
    HBITMAP m_bitmap;

    // Display the current image
    char buffer[sizeof(BITMAPINFO)];
    BITMAPINFO* binfo = (BITMAPINFO*)buffer;
    memset(binfo, 0, sizeof(BITMAPINFOHEADER));
    binfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

    // Create the bitmap
    BITMAPINFOHEADER* bih = &binfo->bmiHeader;
    bih->biBitCount = 3 * 8;
    bih->biWidth = this->render->xres;
    bih->biHeight = this->render->yres;
    bih->biPlanes = 1;
    bih->biCompression = BI_RGB;
    bih->biSizeImage = 0;

    m_bitmap = CreateDIBSection(hdc, binfo, 0, 0, 0, DIB_RGB_COLORS);

    int colors = DIB_RGB_COLORS;

    SelectObject(hdc, m_bitmap);
    binfo->bmiHeader.biBitCount = 0;
    GetDIBits(hdc, m_bitmap, 0, 0, 0, binfo, colors);
    binfo->bmiHeader.biBitCount = 24;
    binfo->bmiHeader.biHeight = -abs(binfo->bmiHeader.biHeight);
    SetDIBits(hdc, m_bitmap, 0, this->render->yres, render->framebuffer, binfo, colors);

    ::SetStretchBltMode(pDC->m_hDC, COLORONCOLOR);
    CRect client;
    GetClientRect(&client);
    ::BitBlt(pDC->m_hDC, 0, 0, this->render->xres, this->render->yres,
        hdc, 0, 0, SRCCOPY);
    ::DeleteDC(hdc);
    DeleteObject(m_bitmap);

    CString pstr = (to_string(render->camera.position[0]) + ", " + to_string(render->camera.position[1]) + ", " + to_string(render->camera.position[2])).c_str();
    campos->SetWindowTextW(pstr);
    CString wstr = (to_string(render->camera.worldup[0]) + ", " + to_string(render->camera.worldup[1]) + ", " + to_string(render->camera.worldup[2])).c_str();
    camworldup->SetWindowTextW(wstr);

}