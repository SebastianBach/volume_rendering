#ifndef VOLUME_DEMO_EVENTLOOP_H__
#define VOLUME_DEMO_EVENTLOOP_H__

class RenderEngine;
class OSWindow;

//---------------------------------------------------------------------------
/// Application event loop.
/// @paran[in] engine		The render engine.
/// @param[in] window		The window to show the rendering result.
//---------------------------------------------------------------------------
void RunLoop(RenderEngine& engine, OSWindow& window);

#endif // VOLUME_DEMO_EVENTLOOP_H__
