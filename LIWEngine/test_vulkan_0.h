#pragma once
#include "LIWConfig.h"

#include <cstdio>

#define GLFW_INCLUDE_VULKAN

#include <Maths/LIWMaths.h>

#include "Memory/LIWMemory.h"
#include "Threading/LIWThread.h"

#include "LIWglfw.h"

#include "Application/Window.h"
#include "Renderers/VulkanRenderer.h"

using namespace LIW;

void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void test() {
	printf("%llu\n", sizeof(LIWPointer<int, LIWMem_Default>));
	printf("%llu\n", sizeof(LIWPointer<float, LIWMem_DFrame>));
	//printf("%llu\n", sizeof(LIWCPointer<float, LIWMem_Default>));

	// Init mem
	liw_minit_def(1);
	liw_minit_static(1);
	liw_minit_frame(1);
	liw_minit_dframe(1);

	LIW::LIWThreadRegisterID(LIW_THREAD_IDX_MAIN);

	// Init mem for thread
	liw_minit_def_thd(LIW_THREAD_IDX_MAIN);
	liw_minit_static_thd(LIW_THREAD_IDX_MAIN);
	liw_minit_frame_thd(LIW_THREAD_IDX_MAIN);
	liw_minit_dframe_thd(LIW_THREAD_IDX_MAIN);



	{
		//glfwInit();
		//glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		//GLFWwindow* window = glfwCreateWindow(1024, 720, "Vulkan", nullptr, nullptr);
		//uint32_t extCount = 0;
		//vkEnumerateInstanceExtensionProperties(nullptr, &extCount, nullptr);
		//printf("Extension count: %i\n", extCount);
		//glm::mat4 testMatrix(1.0f);
		//glm::vec4 testVec(1.0f);
		//auto testRes = testMatrix * testVec;

		//while (!glfwWindowShouldClose(window)) {
		//	glfwPollEvents();
		//}

		//glfwDestroyWindow(window);

		//glfwTerminate();
	}

	{
		//
		// Setup
		//
		glfwSetErrorCallback(glfw_error_callback);
		if (!glfwInit()) {
			std::cout << "GLFW init failed!" << std::endl;
			glfwTerminate();
			throw "GLFW init failed. ";
		}

		/* Window */
		auto ptrWindow = liw_new_static<LIW::App::Window>("My First GLFW Window", 1280, 720, false);
		if (!ptrWindow->Initialised()) {
			glfwTerminate();
			throw "Window creation failed. ";
		}
		/* Make the window's context current */
		ptrWindow->SetCurrent();
		/* Create environment */



		/*BEG*/
		{
			VulkanRenderer test(*ptrWindow);
			bool running = true;
			while (running) {
				glfwPollEvents();
				running = !glfwWindowShouldClose(ptrWindow->GetHandle());

				test.RendererUpdate(0);
			}

			test.WaitUntilIdle();
		}
		/*END*/



		//
		// Cleanup
		//
		liw_delete(ptrWindow);

		glfwTerminate();
	}



	// Cleanup mem for thread
	liw_mclnup_dframe_thd(LIW_THREAD_IDX_MAIN);
	liw_mclnup_frame_thd(LIW_THREAD_IDX_MAIN);
	liw_mclnup_static_thd(LIW_THREAD_IDX_MAIN);
	liw_mclnup_def_thd(LIW_THREAD_IDX_MAIN);

	// Cleanup mem
	liw_mclnup_dframe();
	liw_mclnup_frame();
	liw_mclnup_static();
	liw_mclnup_def();
}

