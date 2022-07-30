#pragma once
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace LIW {
	namespace Editor {
		class LIWIEditorDraw {
		public:
			virtual void EditorDrawUI() {};
		};
	}
}