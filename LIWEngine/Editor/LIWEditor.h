#pragma once
#include "LIWConfig.h"

#ifdef LIW_ENABLE_EDITOR

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

class LIWEditor
{
public:
	static void InitEditor(GLFWwindow* window);
};

#endif //LIW_ENABLE_EDITOR

