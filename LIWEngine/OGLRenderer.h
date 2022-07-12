#pragma once
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
#include "Framework/LIWStandardSystem.h"

#include "Common.h"

#include <string>
#include <fstream>
#include <vector>
#include <memory>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "GL/GL.h"

#include <SOIL/SOIL.h>

#include "Vector4.h"
#include "Vector3.h"
#include "Vector2.h"
#include "Quaternion.h"
#include "Matrix4.h"
#include "Window.h"

#include "Macros.h"

using namespace std;

#ifdef _DEBUG
#define OPENGL_DEBUGGING
#endif

extern const Matrix4 biasMatrix;

namespace LIW {
	class Shader;
	class Light;
	class Camera;

	class OGLRenderer:
		public LIWStandardSystem
	{
	public:
		friend class App::Window;
		OGLRenderer(App::Window& parent);
		virtual ~OGLRenderer(void);

		virtual void RenderUpdate(float dt) override {
			RenderScene();
		}

		bool			HasInitialised() const;

	protected:
		virtual void	RenderScene() = 0;
		//void			SwapBuffers();
		
		virtual void	Resize(int x, int y);
		//void			UpdateShaderMatrices();
		//void			UpdateShaderViewProjFromCamera(shared_ptr<Shader> shaderPrePixelBump, Camera* camera);
		//void			BindShader(shared_ptr<Shader>s);

		void StartDebugGroup(const std::string& s) {
			glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, (GLsizei)s.length(), s.c_str());
		}

		void EndDebugGroup() {
			glPopDebugGroup();
		}

		Matrix4 projMatrix;		//Projection matrix
		Matrix4 modelMatrix;	//Model matrix. NOT MODELVIEW
		Matrix4 viewMatrix;		//View matrix
		Matrix4 textureMatrix;	//Texture matrix
		Matrix4 shadowMatrix;

		int		width;			//Render area width (not quite the same as window width)
		int		height;			//Render area height (not quite the same as window height)
		bool	init;			//Did the renderer initialise properly?

	protected:
		shared_ptr<Shader> currentShader;
		App::Window* currentWindow = nullptr;
		GLFWwindow* currentWindowHandle = nullptr;	//GLFW window handle, which also represent its context?
#ifdef _DEBUG
		static void __stdcall DebugCallback(GLenum source, GLenum renderOrder, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
#endif
	};
}
