#pragma once
#include "LIWConfig.h"

/*
Class:OGLRenderer
Author:Rich Davison	 <richard-gordon.davison@newcastle.ac.uk>
Description:Abstract base class for the graphics tutorials. Creates an OpenGL 
3.2+ CORE PROFILE rendering context. Each lesson will create a renderer that 
inherits from this class - so all context creation is handled automatically,
but students still get to see HOW such a context is created.

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*/
/*
* Revision
* Author: tigerccx
* Description: This class is modified to integrate with GLFW context instead of Windows context. 
*/

#include "LIWRenderer.h"

#include "common.h"

#include <string>
#include <fstream>
#include <vector>
#include <memory>

#include "LIWglfw.h"

#include <SOIL/SOIL.h>

#include "Maths/LIWMaths.h"
#include "Application/Window.h"

#include "Core/Macros.h"

using namespace std;

#ifdef _DEBUG
#define OPENGL_DEBUGGING
#endif

extern const glm::mat4 biasMatrix;

namespace LIW {
	class Shader;
	class Light;
	class Camera;

	class OGLRenderer:
		public LIWRenderer
	{
	public:
		friend class App::Window;
		OGLRenderer(App::Window& window);
		virtual ~OGLRenderer(void);

		virtual void RenderUpdate(float dt) override {
			RenderScene();
		}
	
	protected:
		virtual void RenderScene() = 0;
		//void			SwapBuffers();
		
		void Resize(int x, int y) override;
		//void			UpdateShaderMatrices();
		//void			UpdateShaderViewProjFromCamera(shared_ptr<Shader> shaderPrePixelBump, Camera* camera);
		//void			BindShader(shared_ptr<Shader>s);

		void StartDebugGroup(const std::string& s) {
			glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, (GLsizei)s.length(), s.c_str());
		}

		void EndDebugGroup() {
			glPopDebugGroup();
		}

	protected:
		shared_ptr<Shader> currentShader;
		GLFWwindow* currentWindowHandle = nullptr;	//GLFW window handle, which also represent its context?
#ifdef _DEBUG
		static void __stdcall DebugCallback(GLenum source, GLenum renderOrder, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
#endif
	};
}
