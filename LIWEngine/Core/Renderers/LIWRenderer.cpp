#include "LIWRenderer.h"

LIW::LIWRenderer::LIWRenderer(App::Window& window)
{
	//Assume that if we have a GLFW window, we have a current working context
	if (!window.Initialised()) {
		std::cout << "OGLRenderer::OGLRenderer(): Failed to get a working window!\n";
		return;
	}
	currentWindow = &window;
}

LIW::LIWRenderer::~LIWRenderer()
{
	currentWindow = nullptr;
}
