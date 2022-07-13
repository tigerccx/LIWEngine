#pragma once
#include "Mouse.h"
#include <map>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/GL.h>

namespace LIW {
	namespace App {
		class Mouse_GLFW :
			public Mouse
		{
		public:
			static const std::map<int, MouseButtons> mousecodeMap_GLFW_MouseCode;
			static const std::map<int, MouseButtonActions> mousebtnactMap_GLFW_MouseButtonAction;
		public:
			friend class Window;
		protected:
			Mouse_GLFW(GLFWwindow* window);

			static MouseButtons ToMouseCode(int mousebtnGLFW);
			static MouseButtonActions ToMouseButtonAction(int mousebtnActGLFW);

			static void	MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
			static void MouseMovementCallback(GLFWwindow* window, double xpos, double ypos);
			static void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
			static std::map<GLFWwindow*, Mouse*> mouses;
		};
	}
}


