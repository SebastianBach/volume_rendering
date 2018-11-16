
#include <iostream>

#include "source/window.h"
#include "source/eventloop.h"
#include "source/renderengine.h"
#include "source/log.h"

#define EXIT_ON_FAILURE(v, msg) if (v == false) { ERROR_MSG(msg); return EXIT_FAILURE;}

int main()
{
	INFO_MSG("Program Start");

	EXIT_ON_FAILURE(UnitTestLogSystem(), "Error System failed.");

	OSWindow window;

	EXIT_ON_FAILURE(window.Init(1280, 720), "Could not create window.");
	EXIT_ON_FAILURE(window.Show(), "Could not open window.");
	EXIT_ON_FAILURE(window.CreateOglContext(), "Could not create OGL context.");
	EXIT_ON_FAILURE(window.MakeCurrentContext(), "Could not enable OGL context.");

	RenderEngine engine;

	EXIT_ON_FAILURE(engine.Init(), "Could not start render engine.");
	// run unit tests here
	EXIT_ON_FAILURE(engine.CreateScene(), "Could not create scene.");

	INFO_MSG("Start Event Loop");

	RunLoop(engine, window);

	INFO_MSG("End Event Loop");

	engine.Close();
	window.Close();

	INFO_MSG("Program End");

	return EXIT_SUCCESS;
}

