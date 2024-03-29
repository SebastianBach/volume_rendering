#include "eventloop.h"
#include "log.h"
#include "renderengine.h"
#include "window.h"

//---------------------------------------------------------------------------
/// Checks for success. If failure, the application ends.
//---------------------------------------------------------------------------
#define EXIT_ON_FAILURE(v, msg)                                                \
    if (v == false)                                                            \
    {                                                                          \
        ErrorMessage(MSG_INFO(msg));                                           \
        return EXIT_FAILURE;                                                   \
    }

int main()
{
    InfoMessage(MSG_INFO("Program Start"));

    OSWindow window;

    EXIT_ON_FAILURE(window.Init(1280, 720), "Could not create window.");
    EXIT_ON_FAILURE(window.Show(), "Could not open window.");
    EXIT_ON_FAILURE(window.CreateOglContext(), "Could not create OGL context.");
    EXIT_ON_FAILURE(window.MakeCurrentContext(),
                    "Could not enable OGL context.");

    EXIT_ON_FAILURE(RenderEngine::Init(), "Could not start render engine.");

    RenderEngine engine;
    EXIT_ON_FAILURE(engine.CreateScene(), "Could not create scene.");

    InfoMessage(MSG_INFO("Start Event Loop"));

    RunLoop(engine, window);

    InfoMessage(MSG_INFO("End Event Loop"));

    engine.Close();
    window.Close();

    InfoMessage(MSG_INFO("Program End"));

    return EXIT_SUCCESS;
}
