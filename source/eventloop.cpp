#include "eventloop.h"

#include <windows.h>
#include <windowsx.h>

#include "log.h"

#include <iostream>

static void KeyToRenderMode(const TCHAR & key, unsigned int& renderMode)
{
	if (key < 48)
		return;

	if (key > 57)
		return;

	// admittedly, a wild hack
	renderMode = key - 48;
}

// todo: make return bool
void RunLoop(RenderEngine & engine, OSWindow & window)
{
	bool run = true;

	MSG msg;


	SceneSettings settings;
	settings._renderMode = ShaderMode::Beauty;
	settings._sphereMode = true;
	settings._timeOff = 0.0;
	settings._timeStep = true;
	settings._objectMode = ObjectMode::SPHERE;
	settings._dynamicObjectX = 0.0;

	bool render = true;
	
	while (run)
	{
		float timeOff = 0.0;

		// http://msdn.microsoft.com/en-us/library/windows/desktop/ms644943%28v=vs.85%29.aspx
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0)
		{

			switch (msg.message)
			{

			case WM_MOUSEMOVE:
			{
				

				const int x = GET_X_LPARAM(msg.lParam);
				int y = GET_Y_LPARAM(msg.lParam);

				if (y > 500)
					y = 500;

				settings._dynamicObjectX = ((float(x) / 1280.0f) * 6.0) - 3.0;
				settings._dynamicObjectY = ((float(350 - y ) / 350.0));

				break;
			}
			case WM_KEYDOWN:
			{
				WPARAM key = msg.wParam;

				if (key == VK_ESCAPE)
				{
					run = false;
				}


				const TCHAR ch = (TCHAR)key;
				if (ch == 'P')
				{
					// pause / restart animation
					render = !render;
				}
				if ( key == VK_SPACE)
				{
					settings._timeStep = !settings._timeStep;
				}

				if (ch == 'S')
				{
					settings._objectMode = ObjectMode::SPHERE;
				}

				if (ch == 'M')
				{
					settings._objectMode = ObjectMode::METABALL;
				}

				if (ch == 'D')
				{
					settings._objectMode = ObjectMode::DEFORMED_SPHERE;
				}

				if (key == VK_RIGHT)
				{
					settings._timeOff = 0.01f;
				}

				if (key == VK_LEFT)
				{
					settings._timeOff = -0.01f;
				}

				KeyToRenderMode(ch, settings._renderMode);
			}
			}

			// http://msdn.microsoft.com/en-us/library/windows/desktop/ms644934%28v=vs.85%29.aspx
			DispatchMessage(&msg);
			// http://msdn.microsoft.com/en-us/library/windows/desktop/ms644955%28v=vs.85%29.aspx
			TranslateMessage(&msg);
		}

		if (render)
		{
			engine.UpdateScene(settings);

			settings._timeOff = 0.0;

			const bool renderResult = engine.Render();
			if (renderResult == false)
			{
				ERROR_MSG("Error on rendering.");
				run = false;
			}	

			const bool swapResult = window.Swap();
			if (swapResult == false)
				run = false;
		}

		// DWORD now = GetTickCount();
	}
}

