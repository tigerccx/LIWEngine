#include "Window.h"
#include "Environment.h"
#include "TestGame.h"
#include "TestRenderer.h"
#include "Editor/LIWEditorUIHelper.h"
//#define GLFW_INCLUDE_NONE
//#include "GLFW/glfw3.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

using namespace LIW;
using namespace LIW::App;
using namespace LIW::Editor;

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main(void)
{
	// Init GLFW
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit()) {
		std::cout << "GLFW init failed!" << std::endl;
		glfwTerminate();
		return -1;
	}

	// Init Environment
	/* Window */
	Window window("My First GLFW Window", 1280, 720, false);
	if (!window.Initialised()) {
		return -1;
	}
	/* Make the window's context current */
	window.SetCurrent();
	/* Create environment */
	Environment environment;
	environment.m_window = &window;


	// Init GLEW
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		std::cout << "GLEW init failed!" << std::endl;
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		glfwTerminate();
		return -1;
	}

	// Init renderer
	TestGame game(&environment);	
	int codeGameInit = game.Initialise();
	if (codeGameInit)
		return codeGameInit;

	//
	// Setup ImGUI
	//
	// Setup ImGUI context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	//Setup ImGUI Style
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window.GetHandle(), true);
	ImGui_ImplOpenGL3_Init("#version 130");
	//State
	bool show_demo_window = true;
	bool show_another_window = true;

	/* Loop until the user closes the window */
	while (window.UpdateWindow())
	{
		/* Poll for and process events */
		glfwPollEvents();

		/* Update Game */
		game.UpdateMainLoop();


		// Start new ImGUI frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//// Show example window
		//if (show_demo_window) {
		//	ImGui::ShowDemoWindow(&show_demo_window);
		//}
		//// Show created window
		//{
		//	static float f = 0.0f;
		//	static int counter = 0;

		//	ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		//	ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		//	ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		//	ImGui::Checkbox("Another Window", &show_another_window);

		//	//ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		//	ImGui::DragFloat("float", &f);            // Edit 1 float using a slider from 0.0f to 1.0f
		//	//ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		//	if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
		//		counter++;
		//	ImGui::SameLine();
		//	ImGui::Text("counter = %d", counter);

		//	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		//	ImGui::End();
		//}
		//// Show another window
		//if (show_another_window)
		//{
		//	ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		//	ImGui::Text("Hello from another window!");
		//	if (ImGui::Button("Close Me"))
		//		show_another_window = false;
		//	ImGui::End();
		//}

		ImGui::Begin("Entities");
		auto entities = LIWEntityManager::instance.GetEntityList();
		for (auto& itr : entities) {
			LIWEditorUIHelper::ShowEntity(itr);
		}
		//LIWEditorUIHelper::TestShow();
		ImGui::End();

		// Prerender ImGUI
		ImGui::Render();

		// Render UI
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


		/* Swap front and back buffers */
		window.SwapBuffer();
	}

	game.CleanUp();

	glfwTerminate();

	return 0;
}