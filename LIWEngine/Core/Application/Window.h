#pragma once
#include<iostream>
#include<string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/GL.h>

#include "Input/Keyboard.h"
#include "Input/Keyboard_GLFW.h"
#include "Input/Mouse.h"
#include "Input/Mouse_GLFW.h"

#include "Output/TextOutputDevice.h"
#include "Output/TextConsole.h"

namespace LIW {
	class OGLRenderer;
	namespace App {
		class Window
		{
		public:
			Window(const std::string& name = "Window", int width = 800, int height = 600, bool fullScreen = false);
			~Window();

			bool UpdateWindow();

			inline GLFWwindow* GetHandle() { return windowHandle; }
			inline bool Initialised() { return init; };
			inline void SetCurrent() { glfwMakeContextCurrent(windowHandle); }
			inline void UnsetCurrent() { glfwMakeContextCurrent(nullptr); }
			inline bool ShouldClose() { return glfwWindowShouldClose(windowHandle); }
			inline void SwapBuffer() { glfwSwapBuffers(windowHandle); }
			void SetRenderer(OGLRenderer* r);

			inline Keyboard* GetKeyboard() { return keyboard; }
			inline Mouse* GetMouse() { return mouse; }
			inline TextOutputDevice* GetTextOutput() { return textOutput; }

		protected:

			GLFWwindow* windowHandle = nullptr;
			OGLRenderer* renderer = nullptr;

			std::string title = "";
			int width = 0;
			int height = 0;

			bool init = false;

			/*Input*/
			Keyboard* keyboard = nullptr;
			Mouse* mouse = nullptr;

			/*Output*/
			TextOutputDevice* textOutput = nullptr;
		};
	}
}


