#ifndef VOLUME_DEMO_WINDOW_H__
#define VOLUME_DEMO_WINDOW_H__

#include <windows.h>

//---------------------------------------------------------------------------
/// A window provided by the OS.
//---------------------------------------------------------------------------
class OSWindow
{
public:
    //---------------------------------------------------------------------------
    /// Constructor.
    //---------------------------------------------------------------------------
    OSWindow();

    //---------------------------------------------------------------------------
    /// Destructor.
    //---------------------------------------------------------------------------
    ~OSWindow();

    //---------------------------------------------------------------------------
    /// Creates the window with the given size.
    /// @param[in]  w       Window width.
    /// @param[in]  h       Window height.
    /// @return             False if an error occurred.
    //---------------------------------------------------------------------------
    bool Init(int w, int h);

    //---------------------------------------------------------------------------
    /// Displays the window.
    /// @return             False if an error occurred.
    //---------------------------------------------------------------------------
    bool Show();

    //---------------------------------------------------------------------------
    /// Closes the window.
    /// @return             False if an error occurred.
    //---------------------------------------------------------------------------
    bool Close();

    //---------------------------------------------------------------------------
    /// Creates an OpenGL context.
    /// @return             False if an error occurred.
    //---------------------------------------------------------------------------
    bool CreateOglContext();

    //---------------------------------------------------------------------------
    /// Makes the creates context the current context.
    /// @return             False if an error occurred.
    //---------------------------------------------------------------------------
    bool MakeCurrentContext();

    //---------------------------------------------------------------------------
    /// Removes the created context.
    /// @return             False if an error occurred.
    //---------------------------------------------------------------------------
    bool RemoveContext();

    //---------------------------------------------------------------------------
    /// Swaps windows buffers.
    /// @return             False if an error occurred.
    //---------------------------------------------------------------------------
    bool Swap() const;

private:
    void* _handle;     ///< window handle.
    HGLRC _oglContext; ///< OGL context handle.
    HDC   _hdc;        ///< HDC.
};

#endif // VOLUME_DEMO_WINDOW_H__
