#ifndef VOLUME_DEMO_EVENTLOOP_H__
#define VOLUME_DEMO_EVENTLOOP_H__

#include "renderengine.h"
#include "window.h"

//---------------------------------------------------------------------------
/// Application event loop.
/// @paran[in] engine		The render engine.
/// @param[in] window		The window to show the rendering result.
//---------------------------------------------------------------------------
void RunLoop(RenderEngine & engine, OSWindow & window);

#endif
