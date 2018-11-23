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

static void HandleEvents(bool& run, SceneSettings& settings, const MSG& msg)
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
			settings._dynamicObjectY = ((float(350 - y) / 350.0));

			return;
		}

		case WM_LBUTTONDOWN:
		{
			const int x = GET_X_LPARAM(msg.lParam);
			int y = GET_Y_LPARAM(msg.lParam);

			if (y > 500)
				y = 500;

			settings._dynamicObjectX = ((float(x) / 1280.0f) * 6.0) - 3.0;
			settings._dynamicObjectY = ((float(350 - y) / 350.0));

			settings._addSphere = true;
			return;
		}

		case WM_KEYDOWN:
		{
			WPARAM key = msg.wParam;

			if (key == VK_ESCAPE)
			{
				run = false;
				return;
			}
			if (key == VK_SPACE)
			{
				settings._timeStep = !settings._timeStep;
				return;
			}
			if (key == VK_RIGHT)
			{
				settings._timeOff = 1.0f;
				return;
			}

			if (key == VK_LEFT)
			{
				settings._timeOff = -1.0f;
				return;
			}
			const TCHAR ch = (TCHAR)key;

			if (ch == 'N')
			{
				if (settings._noise == NoiseMode::NOISE)
					settings._noise = NoiseMode::NO_NOISE;
				else
					settings._noise = NoiseMode::NOISE;

				return;
			}
			if (ch == 'D')
			{
				settings._removeObject = true;
				return;
			}

			KeyToRenderMode(ch, settings._renderMode);

		}
	
	}
}

// todo: make return bool
void RunLoop(RenderEngine & engine, OSWindow & window)
{
	SceneSettings settings;
	settings._renderMode = ShaderMode::Beauty;
	settings._timeOff = 0.0;
	settings._timeStep = true;
	settings._dynamicObjectX = 0.0;
	settings._dynamicObjectY = 0.0;
	settings._noise = NoiseMode::NO_NOISE;
	settings._addSphere = false;
	settings._removeObject = false;

	MSG msg;
	bool run = true;

	while (run)
	{
		// http://msdn.microsoft.com/en-us/library/windows/desktop/ms644943%28v=vs.85%29.aspx
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0)
		{
			HandleEvents(run, settings, msg);
		
			// http://msdn.microsoft.com/en-us/library/windows/desktop/ms644934%28v=vs.85%29.aspx
			DispatchMessage(&msg);
			// http://msdn.microsoft.com/en-us/library/windows/desktop/ms644955%28v=vs.85%29.aspx
			TranslateMessage(&msg);
		}

	
		engine.UpdateScene(settings);

		// reset
		settings._timeOff = 0.0;
		settings._addSphere = false;
		settings._removeObject = false;

		const bool renderResult = engine.Render();
		if (renderResult == false)
		{
			ErrorMessage(MSG_INFO("Error on rendering."));
			run = false;
		}	

		const bool swapResult = window.Swap();
		if (swapResult == false)
			run = false;
	

		// DWORD now = GetTickCount();
	}
}

