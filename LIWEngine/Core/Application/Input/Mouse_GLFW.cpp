#include "Mouse_GLFW.h"

const std::map<int, LIW::MouseButtons> LIW::App::Mouse_GLFW::mousecodeMap_GLFW_MouseCode = {
		{GLFW_MOUSE_BUTTON_1		, MOUSE_BUTTON_1		},
		{GLFW_MOUSE_BUTTON_2		, MOUSE_BUTTON_2		},
		{GLFW_MOUSE_BUTTON_3		, MOUSE_BUTTON_3		},
		{GLFW_MOUSE_BUTTON_4		, MOUSE_BUTTON_4		},
		{GLFW_MOUSE_BUTTON_5		, MOUSE_BUTTON_5		},
		{GLFW_MOUSE_BUTTON_6		, MOUSE_BUTTON_6		},
		{GLFW_MOUSE_BUTTON_7		, MOUSE_BUTTON_7		},
		{GLFW_MOUSE_BUTTON_8		, MOUSE_BUTTON_8		},
		{GLFW_MOUSE_BUTTON_LAST		, MOUSE_BUTTON_MAX		},
		{GLFW_MOUSE_BUTTON_LEFT		, MOUSE_BUTTON_LEFT		},
		{GLFW_MOUSE_BUTTON_RIGHT	, MOUSE_BUTTON_RIGHT	},
		{GLFW_MOUSE_BUTTON_MIDDLE	, MOUSE_BUTTON_MIDDLE	},
};

const std::map<int, LIW::App::MouseButtonActions> LIW::App::Mouse_GLFW::mousebtnactMap_GLFW_MouseButtonAction = {
	{GLFW_REPEAT	, MOUSE_BUTTON_ACTION_UNKNOWN	}, //Not support repeat
	{GLFW_PRESS		, MOUSE_BUTTON_ACTION_PRESS		},
	{GLFW_RELEASE	, MOUSE_BUTTON_ACTION_RELEASE	}
};

std::map<GLFWwindow*, LIW::App::Mouse*> LIW::App::Mouse_GLFW::mouses = std::map<GLFWwindow*, Mouse*>();

LIW::App::Mouse_GLFW::Mouse_GLFW(GLFWwindow* window):
	Mouse()
{
	//Set callback
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	glfwSetCursorPosCallback(window, MouseMovementCallback);
	glfwSetScrollCallback(window, MouseScrollCallback);
	mouses[window] = this;

	Wake();
}

LIW::MouseButtons LIW::App::Mouse_GLFW::ToMouseCode(int mousebtnGLFW)
{
	return mousecodeMap_GLFW_MouseCode.at(mousebtnGLFW);
}

LIW::App::MouseButtonActions LIW::App::Mouse_GLFW::ToMouseButtonAction(int mousebtnActGLFW)
{
	return mousebtnactMap_GLFW_MouseButtonAction.at(mousebtnActGLFW);
}

void LIW::App::Mouse_GLFW::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	MouseButtons mouseBtn = ToMouseCode(button);
	MouseButtonActions mouseBtnAct = ToMouseButtonAction(action);
	((Mouse_GLFW*)mouses[window])->OnUpdateButton(mouseBtn, mouseBtnAct);
}

void LIW::App::Mouse_GLFW::MouseMovementCallback(GLFWwindow* window, double xpos, double ypos)
{
	((Mouse_GLFW*)mouses[window])->OnUpdateMovement(xpos, ypos);
}

void LIW::App::Mouse_GLFW::MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	((Mouse_GLFW*)mouses[window])->OnUpdateScroll(xoffset, yoffset);
}
