#include "LIWEditor.h"

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