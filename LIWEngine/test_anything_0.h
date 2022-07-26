#pragma once
#include <cstdio>

#include "Memory/LIWMemory.h"
#include "Threading/LIWThread.h"
#include "Utility/LIWFile.h"

#include "TestRenderer.h"
#include "Application/Window.h"

#include "Data/LIWImage.h"
#include "Data/LIWTexture.h"

#include "Data/LIWMeshData.h"
#include "Data/LIWMesh.h"

#include "Data/LIWShader.h"
#include "Data/LIWShaderProgram.h"


using namespace LIW;
using namespace LIW::App;

void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void test() {

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
		// Init GLFW
		glfwSetErrorCallback(glfw_error_callback);
		if (!glfwInit()) {
			std::cout << "GLFW init failed!" << std::endl;
			glfwTerminate();
			return;
		}

		Window window("TEST_0", 320, 320, false);
		if (!window.Initialised()) {
			return;
		}
		window.SetCurrent();

#ifdef LIW_USE_GLEW
		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			/* Problem: glewInit failed, something is seriously wrong. */
			std::cout << "GLEW init failed!" << std::endl;
			fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
			glfwTerminate();
			return;
		}
#endif // LIW_USE_GLEW

		TestRenderer renderer(window);
		renderer.Init();


		// Shader
		LIWShader shaderVert;
		LIWShader shaderFrag;
		LIWShaderProgram shaderProgram;

		shaderVert.LoadShader("./Shaders/OGL/PerPixel_AllInOne_Vertex.glsl", LIWShaderType_Vertex);
		shaderFrag.LoadShader("./Shaders/OGL/PerPixel_AllInOne_Fragment.glsl", LIWShaderType_Fragment);

		shaderProgram.CreateShader({ shaderVert ,shaderFrag });
		
		shaderVert.UnloadShader();
		shaderFrag.UnloadShader();
		
		shaderProgram.DestroyShader();

		// Texture
		LIWImage image;
		LIWTexture2D tex2D;

		image.LoadImg("../../Resources/Textures/LIS.jpg", LIWImageFormat_RGB);
		tex2D.CreateTexture(image);

		image.UnloadImg();
		tex2D.DestroyTexture();

		// Mesh
		LIWMeshData meshdata;
		LIWMesh mesh;

		meshdata.LoadMeshData_Obj("../../Resources/Meshes/viking_room.obj");
		mesh.CreateMesh(meshdata);

		meshdata.UnloadMeshData();
		mesh.DestroyMesh();


		renderer.Cleanup();

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