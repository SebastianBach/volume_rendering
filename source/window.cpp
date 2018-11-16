#include "window.h"

#include <iostream>
#include <windows.h>
#include <wingdi.h>
#include <stdio.h>

// OpenGL includes
#include <gl\GL.h>
#include <gl\glu.h>

#include "log.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

		EndPaint(hwnd, &ps);
		return 0;
	}
	

	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


OSWindow::OSWindow()
{
	_handle = nullptr;
	_oglContext = nullptr;
}
OSWindow::~OSWindow()
{
	Close();
}

bool OSWindow::Init(int w, int h)
{
	// getting the hinstance
	HMODULE module = GetModuleHandle(NULL);
	HINSTANCE hinstance = (HINSTANCE)module;

	HWND windowHandle;

	// window class
	WNDCLASSEX windowClass;

	windowClass = {};

	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = 0;
	windowClass.lpfnWndProc = WindowProc;									// window function					

	LPCWSTR className = L"VolumeDemo Window";

	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = hinstance;
	windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = className;			// window class anme
	windowClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	// http://msdn.microsoft.com/en-us/library/windows/desktop/ms633587%28v=vs.85%29.aspx
	const bool resultRegister = RegisterClassEx(&windowClass);
	if (IsFalse(resultRegister, "Could not register window class", ERROR_CONTEXT))
		return false;

	// // http://msdn.microsoft.com/en-us/library/windows/desktop/ms632680%28v=vs.85%29.aspx
	windowHandle = CreateWindowEx(
		0,				// Optional window styles.
		className,						// Window class name
		L"VolumeWindow",					// Window Title
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,					// position x
		CW_USEDEFAULT,					// position y
		w,						// size x
		h,						// size y
		NULL,							// parent
		NULL,							// meune
		hinstance,						// hinstance
		NULL);

	if (IsNullptr(windowHandle, "Could not create window handle", ERROR_CONTEXT))
		return false;

	_handle = windowHandle;


	SetWindowPos(
		windowHandle,
		NULL,
		0,
		0,
		w,
		h,
		NULL);
	

	return true;
}

bool OSWindow::CreateOglContext()
{
	if (IsNullptr(_handle, "Handle not set.", ERROR_CONTEXT)) return false;

	HDC   hdc = GetDC((HWND)_handle);

	if (IsNullptr(hdc, "Could not get device context.", ERROR_CONTEXT)) return false;

	// set pixel format
	// http://msdn.microsoft.com/en-us/library/windows/desktop/dd368826%28v=vs.85%29.aspx
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),   // size of this pfd  
		1,                     // version number  
		PFD_DRAW_TO_WINDOW |   // support window  
		PFD_SUPPORT_OPENGL |   // support OpenGL  
		PFD_DOUBLEBUFFER,      // double buffered  
		PFD_TYPE_RGBA,         // RGBA type  
		24,                    // 24-bit color depth  
		0, 0, 0, 0, 0, 0,      // color bits ignored  
		0,                     // no alpha buffer  
		0,                     // shift bit ignored  
		0,                     // no accumulation buffer  
		0, 0, 0, 0,            // accum bits ignored  
		32,                    // 32-bit z-buffer  
		0,                     // no stencil buffer  
		0,                     // no auxiliary buffer  
		PFD_MAIN_PLANE,        // main layer  
		0,                     // reserved  
		0, 0, 0                // layer masks ignored  
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

	if (IsNullptr(_oglContext, "Could not create OGL context.", ERROR_CONTEXT)) return false;

	_hdc = hdc;

	return true;
}

bool OSWindow::MakeCurrentContext()
{
	if (IsNullptr(_hdc, "HDC not set", ERROR_CONTEXT)) return false;
	if (IsNullptr(_oglContext, "OGL context not set", ERROR_CONTEXT)) return false;

	const BOOL res = wglMakeCurrent(_hdc, _oglContext);

	if (IsNotValue(res, TRUE, "Could not make OGL contex the current context.", ERROR_CONTEXT))
		return false;

	return true;
}

bool OSWindow::RemoveContext()
{
	// kill OpenGL context
	wglMakeCurrent(NULL, NULL);

	HGLRC hglrc = _oglContext;
	wglDeleteContext(hglrc);
	HWND hwnd = (HWND)_handle;
	HDC hdc = _hdc;
	ReleaseDC(hwnd, hdc);

	return true;
}

bool OSWindow::Swap() const
{
	const BOOL res = SwapBuffers(_hdc);

	if (IsNotValue(res, TRUE, "Could not swap buffers.", ERROR_CONTEXT))
		return false;

	const BOOL resInvaidate = InvalidateRect((HWND)_handle, nullptr, false);

	if (IsNotValue(resInvaidate, TRUE, "Could not invalidate rectangle.", ERROR_CONTEXT))
		return false;

	return true;
}

bool OSWindow::Show()
{
	if (IsNullptr(_handle, "Window handle not set", ERROR_CONTEXT))
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

	LPCWSTR className = L"VolumeDemo Window";

	

	HMODULE module = GetModuleHandle(NULL);
	HINSTANCE hinstance = (HINSTANCE)module;

	UnregisterClass(className, hinstance);

	_handle = nullptr;

	return true;
}
