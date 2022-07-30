#include "LIWEditor.h"

#ifdef LIW_ENABLE_EDITOR

void LIWEditor::InitEditor(GLFWwindow* window)
{
	//
	// Setup ImGUI
	//
	// Setup ImGUI context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	//Setup ImGUI Style
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");

}

#endif //LIW_ENABLE_EDITOR