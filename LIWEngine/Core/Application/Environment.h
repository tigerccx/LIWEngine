#pragma once

#include "Window.h"
#include "Memory/LIWMemory.h"

namespace LIW {
	namespace App {
		class Environment {
		public:
			LIWPointer<Window, LIWMem_Static> m_window;
		};
	}
}