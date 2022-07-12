/*
* Class: Keyboard_GLFW
* Implements: Keyboard
* Description: Implementation of keyboard using GLFW lib as raw input.
*/

#include "Keyboard.h"
#include <map>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/GL.h>


namespace LIW {
	namespace App {
		class Keyboard_GLFW :
			public Keyboard
		{
		public:
			static const std::map<int, KeyboardKeys> keycodeMap_GLFW_Keycode;
			static const std::map<int, KeyboardActions> keyactMap_GLFW_KeyAction;
		public:
			friend class Window;
		protected:
			Keyboard_GLFW(GLFWwindow* window);

			static KeyboardKeys ToKeyCode(int keyGLFW);
			static KeyboardActions ToKeyAction(int keyActGLFW);

			static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
			static std::map<GLFWwindow*, Keyboard*> keyboards;
		};
	}
}
