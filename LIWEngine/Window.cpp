#include "Window.h"
#include "OGLRenderer.h"

LIW::App::Window::Window(const std::string& name, int width, int height, bool fullScreen)
{
	init = false;

	title = name;
	this->width = width;
	this->height = height;

	windowHandle = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);

	if (!windowHandle) {
		std::cout << "GLFW window init failed!" << std::endl;
		return;
	}

	/*Init input devices*/
	if (!keyboard) {
		keyboard = new Keyboard_GLFW(windowHandle); //Use GLFW keyboard for now
	}
	if (!mouse) {
		mouse = new Mouse_GLFW(windowHandle); //Use GLFW mouse for now
	}

	/*Init output devices*/
	if (!textOutput) {
		textOutput = new TextConsole();
	}

	init = true;
}

LIW::App::Window::~Window()
{
	if (keyboard) {
		delete keyboard;
		keyboard = nullptr;
	}
	if (mouse) {
		delete mouse;
		mouse = nullptr;
	}
	if (windowHandle) {
		glfwDestroyWindow(windowHandle);
	}
}

bool LIW::App::Window::UpdateWindow()
{
	//TODO: Add timer and provide devices UpdateFrame with proper time dur
	// Update keyboard
	keyboard->UpdateFrame(0);
	// Update mouse
	mouse->UpdateFrame(0.01f);

	return !ShouldClose();
}

void LIW::App::Window::SetRenderer(OGLRenderer* r)
{
	renderer = r;
	if (r) {
		renderer->Resize((int)width, (int)height);
	}
}
