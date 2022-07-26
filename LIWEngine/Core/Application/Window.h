#pragma once
#include "LIWConfig.h"

#include<iostream>
#include<string>
#include<map>

#include "LIWglfw.h"

#include "Memory/LIWMemory.h"

#include "Input/Keyboard.h"
#include "Input/Keyboard_GLFW.h"
#include "Input/Mouse.h"
#include "Input/Mouse_GLFW.h"

#include "Output/TextOutputDevice.h"
#include "Output/TextConsole.h"

namespace LIW {
	class LIWRenderer;
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
			void SetRenderer(LIWRenderer* r);
			void Resize(int width, int height);

			inline Keyboard* GetKeyboard() { return keyboard; }
			inline Mouse* GetMouse() { return mouse; }
			inline LIWPointer<TextOutputDevice, LIWMem_Static> GetTextOutput() { return textOutput; }

		protected:

			GLFWwindow* windowHandle = nullptr;
			LIWRenderer* renderer = nullptr;

			std::string title = "";
			int m_width = 0;
			int m_height = 0;

			bool init = false;

			/*Input*/
			Keyboard* keyboard = nullptr;
			Mouse* mouse = nullptr;

			/*Output*/
			LIWPointer<TextOutputDevice, LIWMem_Static> textOutput{ liw_c_nullhdl };

		protected:
			static void ResizeCallback(GLFWwindow* window, int width, int height);
			static std::map<GLFWwindow*, Window*> windows;
		};
	}
}


