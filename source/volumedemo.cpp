#include "window.h"
#include "eventloop.h"
#include "renderengine.h"
#include "log.h"

//---------------------------------------------------------------------------
/// Checks for success. If failure, the application ends.
//---------------------------------------------------------------------------
#define EXIT_ON_FAILURE(v, msg) if (v == false)\
								{ \
								ErrorMessage(MSG_INFO(msg));\
								return EXIT_FAILURE;\
								}

int main()
{
	InfoMessage(MSG_INFO("Program Start"));

	EXIT_ON_FAILURE(UnitTestLogSystem(), "Error System failed.");

	OSWindow window;

	EXIT_ON_FAILURE(window.Init(1280, 720), "Could not create window.");
	EXIT_ON_FAILURE(window.Show(), "Could not open window.");
	EXIT_ON_FAILURE(window.CreateOglContext(), "Could not create OGL context.");
	EXIT_ON_FAILURE(window.MakeCurrentContext(), "Could not enable OGL context.");

	RenderEngine engine;

	EXIT_ON_FAILURE(engine.Init(), "Could not start render engine.");
	EXIT_ON_FAILURE(engine.CreateScene(), "Could not create scene.");

	InfoMessage(MSG_INFO("Start Event Loop"));

	RunLoop(engine, window);

	InfoMessage(MSG_INFO("End Event Loop"));

	engine.Close();
	window.Close();

	InfoMessage(MSG_INFO("Program End"));

	return EXIT_SUCCESS;
}

