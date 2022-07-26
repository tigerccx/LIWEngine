#include "Window.h"
#include "Renderers/OGLRenderer.h"

std::map<GLFWwindow*, LIW::App::Window*> LIW::App::Window::windows = std::map<GLFWwindow*, LIW::App::Window*>();

LIW::App::Window::Window(const std::string& name, int width, int height, bool fullScreen)
{
	init = false;

	title = name;
	this->m_width = width;
	this->m_height = height;

//#ifdef LIW_RENDER_VULKAN
//	// Ask glfw to create no API
//	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
//	//TODO: MUST CHANGE LATER! Temp disable resize
//	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
//#endif
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

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
	if (textOutput.is_null()) {
		textOutput = liw_new_static<TextConsole>();
	}

	windows[windowHandle] = this;

	/*Window resize*/
	glfwSetFramebufferSizeCallback(windowHandle, ResizeCallback);

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
	if (!textOutput.is_null()) {
		liw_delete_static(textOutput);
		textOutput.set_null();
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

void LIW::App::Window::SetRenderer(LIWRenderer* r)
{
	renderer = r;
	if (r) {
		renderer->Resize((int)m_width, (int)m_height);
	}
}

void LIW::App::Window::Resize(int width, int height)
{
	m_width = width;
	m_height = height;
	if (renderer) renderer->Resize(width, height);
}

void LIW::App::Window::ResizeCallback(GLFWwindow* window, int width, int height)
{
	//Here we assume callback will always receive a GLFWwindow handle already registered.
	windows[window]->Resize(width, height);
}
