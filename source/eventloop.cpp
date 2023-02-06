#include <windows.h>
#include <windowsx.h>

#include "eventloop.h"
#include "log.h"
#include <iostream>


//---------------------------------------------------------------------------
/// Converts the key ID to a render mode ID.
/// @param[in]	key			Character ID.
/// @param[out]	renderMode	Render mode ID (0 - 9).
//---------------------------------------------------------------------------
static void KeyToRenderMode(const TCHAR& key, unsigned int& renderMode)
{
    if (key < 48)
        return;

    if (key > 57)
        return;

    // admittedly, a wild hack
    renderMode = key - 48;
}

//---------------------------------------------------------------------------
/// Converts the screen space x coordinate to world space.
/// @param[in]	x			Screen space x-coordinate.
/// @param[out]	xf			World space x-coordinate.
//---------------------------------------------------------------------------
static void GetXCoordinate(int x, float& xf)
{
    xf = ((float(x) / 1280.0f) * 6.0f) - 3.0f;
}

//---------------------------------------------------------------------------
/// Converts the screen space y coordinate to world space.
/// @param[in]	y			Screen space y-coordinate.
/// @param[out]	yf			World space y-coordinate.
//---------------------------------------------------------------------------
static void GetYCoordinate(int y, float& yf)
{
    yf = ((float(350 - y) / 350.0f));
}

//---------------------------------------------------------------------------
/// Handles key and mouse events.
/// @param[out]	run			Set to false if the application should end (ESC
/// pressed).
/// @param[out] settings	The settings object that will be edited.
/// @param[in]	msg			The current event message.
//---------------------------------------------------------------------------
static void HandleEvents(bool& run, SceneSettings& settings, const MSG& msg)
{
    switch (msg.message)
    {
    case WM_MOUSEMOVE:
    {
        const int x = GET_X_LPARAM(msg.lParam);
        int       y = GET_Y_LPARAM(msg.lParam);

        if (y > 500)
            y = 500;

        GetXCoordinate(x, settings._dynamicObjectX);
        GetYCoordinate(y, settings._dynamicObjectY);

        return;
    }

    case WM_LBUTTONDOWN:
    {
        const int x = GET_X_LPARAM(msg.lParam);
        int       y = GET_Y_LPARAM(msg.lParam);

        if (y > 500)
            y = 500;

        GetXCoordinate(x, settings._dynamicObjectX);
        GetYCoordinate(y, settings._dynamicObjectY);

        settings._addObjectClick = true;
        return;
    }

    case WM_KEYDOWN:
    {
        const WPARAM key = msg.wParam;

        if (key == VK_ESCAPE)
        {
            // ESC pressed, stop application
            run = false;
            return;
        }
        if (key == VK_SPACE)
        {
            // SPACE pressed, stop animation
            settings._timeStep = !settings._timeStep;
            return;
        }
        if (key == VK_RIGHT)
        {
            // RIGHT ARROW pressed, move animation forward
            settings._timeOff = 1.0f;
            return;
        }

        if (key == VK_LEFT)
        {
            // LEFT ARROW pressed, move animation backward
            settings._timeOff = -1.0f;
            return;
        }
        if (key == VK_BACK)
        {
            // BACKSPACE pressed, remove the last object
            settings._removeObject = true;
            return;
        }

        // character keys pressed
        const TCHAR ch = (TCHAR)key;

        if (ch == 'N')
        {
            // turn noise on/off

            if (settings._noise == NoiseMode::NOISE)
                settings._noise = NoiseMode::NO_NOISE;
            else
                settings._noise = NoiseMode::NOISE;

            return;
        }
        if (ch == 'D')
        {
            // remove last object
            settings._removeObject = true;
            return;
        }
        if (ch == 'A')
        {
            // add new abject
            settings._addObject = true;
            return;
        }

        // handle render modes
        KeyToRenderMode(ch, settings._renderMode);
    }
    }
}

void RunLoop(RenderEngine& engine, OSWindow& window)
{
    SceneSettings settings;
    settings._renderMode     = 0;
    settings._timeOff        = 0.0;
    settings._timeStep       = true;
    settings._dynamicObjectX = 0.0;
    settings._dynamicObjectY = 0.0;
    settings._noise          = NoiseMode::NO_NOISE;
    settings._addObjectClick = false;
    settings._removeObject   = false;
    settings._addObject      = false;

    MSG  msg;
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

        // update scene
        // add/remove objects
        // play animation
        engine.UpdateScene(settings);

        // reset
        settings._timeOff        = 0.0;
        settings._addObjectClick = false;
        settings._removeObject   = false;
        settings._addObject      = false;

        // render scene
        const bool renderResult = engine.Render();

        if (renderResult == false)
        {
            ErrorMessage(MSG_INFO("Error on rendering."));
            run = false;
        }

        // swap buffers
        const bool swapResult = window.Swap();
        if (swapResult == false)
            run = false;
    }
}
