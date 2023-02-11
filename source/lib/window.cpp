#include "window.h"
#include "log.h"
#include <gl\GL.h>
#include <gl\glu.h>
#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <wingdi.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT            ps;
        [[maybe_unused]] auto* hdc = BeginPaint(hwnd, &ps);

        // FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
        // we should do rendering here

        EndPaint(hwnd, &ps);

        return 0;
    }
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

OSWindow::OSWindow()
{
    _handle     = nullptr;
    _oglContext = nullptr;
    _hdc        = nullptr;
}

OSWindow::~OSWindow()
{
    Close();
}

bool OSWindow::Init(int w, int h)
{
    // getting the hinstance
    auto* module    = GetModuleHandle(NULL);
    auto* hinstance = (HINSTANCE)module;

    HWND windowHandle;

    // window class
    WNDCLASSEX windowClass;

    windowClass = {};

    windowClass.cbSize      = sizeof(WNDCLASSEX);
    windowClass.style       = 0;
    windowClass.lpfnWndProc = WindowProc; // window function

    const auto* className = "VolumeDemo Window";

    windowClass.cbClsExtra    = 0;
    windowClass.cbWndExtra    = 0;
    windowClass.hInstance     = hinstance;
    windowClass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    windowClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    windowClass.lpszMenuName  = NULL;
    windowClass.lpszClassName = className; // window class anme
    windowClass.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    // http://msdn.microsoft.com/en-us/library/windows/desktop/ms633587%28v=vs.85%29.aspx
    const auto resultRegister = RegisterClassEx(&windowClass);
    if (IsFalse(resultRegister, MSG_INFO("Could not register window class")))
        return false;

    // //
    // http://msdn.microsoft.com/en-us/library/windows/desktop/ms632680%28v=vs.85%29.aspx
    windowHandle = CreateWindowEx(0,              // Optional window styles.
                                  className,      // Window class name
                                  "VolumeWindow", // Window Title
                                  WS_OVERLAPPEDWINDOW,
                                  CW_USEDEFAULT, // position x
                                  CW_USEDEFAULT, // position y
                                  w,             // size x
                                  h,             // size y
                                  NULL,          // parent
                                  NULL,          // meune
                                  hinstance,     // hinstance
                                  NULL);

    if (IsNullptr(windowHandle, MSG_INFO("Could not create window handle")))
        return false;

    _handle = windowHandle;

    SetWindowPos(windowHandle, NULL, 0, 0, w, h, NULL);

    return true;
}

bool OSWindow::CreateOglContext()
{
    if (IsNullptr(_handle, MSG_INFO("Handle not set.")))
        return false;

    auto* hdc = GetDC((HWND)_handle);

    if (IsNullptr(hdc, MSG_INFO("Could not get device context.")))
        return false;

    // set pixel format
    // http://msdn.microsoft.com/en-us/library/windows/desktop/dd368826%28v=vs.85%29.aspx
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR), // size of this pfd
        1,                             // version number
        PFD_DRAW_TO_WINDOW |           // support window
            PFD_SUPPORT_OPENGL |       // support OpenGL
            PFD_DOUBLEBUFFER,          // double buffered
        PFD_TYPE_RGBA,                 // RGBA type
        24,                            // 24-bit color depth
        0,
        0,
        0,
        0,
        0,
        0, // color bits ignored
        0, // no alpha buffer
        0, // shift bit ignored
        0, // no accumulation buffer
        0,
        0,
        0,
        0,              // accum bits ignored
        32,             // 32-bit z-buffer
        0,              // no stencil buffer
        0,              // no auxiliary buffer
        PFD_MAIN_PLANE, // main layer
        0,              // reserved
        0,
        0,
        0 // layer masks ignored
    };

    int iPixelFormat;

    // get the best available match of pixel format for the device context
    // http://msdn.microsoft.com/en-us/library/windows/desktop/dd318284%28v=vs.85%29.aspx
    iPixelFormat = ChoosePixelFormat(hdc, &pfd);

    // make that the pixel format of the device context
    // http://msdn.microsoft.com/en-us/library/windows/desktop/dd369049%28v=vs.85%29.aspx
    SetPixelFormat(hdc, iPixelFormat, &pfd);

    // starts openGL Context
    // http://msdn.microsoft.com/en-us/library/windows/desktop/dd374379%28v=vs.85%29.aspx
    _oglContext = wglCreateContext(hdc);

    if (IsNullptr(_oglContext, MSG_INFO("Could not create OGL context.")))
        return false;

    _hdc = hdc;

    return true;
}

bool OSWindow::MakeCurrentContext()
{
    if (IsNullptr(_hdc, MSG_INFO("HDC not set")))
        return false;
    if (IsNullptr(_oglContext, MSG_INFO("OGL context not set")))
        return false;

    const auto res = wglMakeCurrent(_hdc, _oglContext);

    if (IsNotValue(res, TRUE,
                   MSG_INFO("Could not make OGL contex the current context.")))
        return false;

    return true;
}

bool OSWindow::RemoveContext()
{
    // kill OpenGL context
    wglMakeCurrent(NULL, NULL);

    auto* hglrc = _oglContext;
    wglDeleteContext(hglrc);
    auto* hwnd = (HWND)_handle;
    auto* hdc  = _hdc;
    ReleaseDC(hwnd, hdc);

    return true;
}

bool OSWindow::Swap() const
{
    const auto res = SwapBuffers(_hdc);

    if (IsNotValue(res, TRUE, MSG_INFO("Could not swap buffers.")))
        return false;

    const auto resInvaidate = InvalidateRect((HWND)_handle, nullptr, false);

    if (IsNotValue(resInvaidate, TRUE,
                   MSG_INFO("Could not invalidate rectangle.")))
        return false;

    return true;
}

bool OSWindow::Show()
{
    if (IsNullptr(_handle, MSG_INFO("Window handle not set")))
        return false;

    // https://docs.microsoft.com/en-us/windows/desktop/api/winuser/nf-winuser-showwindow
    // return value does NOT indicate success
    ShowWindow((HWND)_handle, SW_SHOW);

    return true;
}

bool OSWindow::Close()
{
    this->RemoveContext();

    DestroyWindow((HWND)_handle);

    const auto* className = "VolumeDemo Window";

    auto* module    = GetModuleHandle(NULL);
    auto* hinstance = (HINSTANCE)module;

    UnregisterClass(className, hinstance);

    _handle = nullptr;

    return true;
}
