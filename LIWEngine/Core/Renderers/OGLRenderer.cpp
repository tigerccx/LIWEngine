/*
Class:OGLRenderer
Author:Rich Davison	 <richard-gordon.davison@newcastle.ac.uk>
Description:Abstract base class for the graphics tutorials. Creates an OpenGL 
3.2 CORE PROFILE rendering context. Each lesson will create a renderer that 
inherits from this class - so all context creation is handled automatically,
but students still get to see HOW such a context is created.

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*/
#include "OGLRenderer.h"
//#include "Shader.h"
#include <algorithm>

using std::string;


namespace LIW {
	static const float biasValues[16] = {
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 0.5, 0.0,
	0.5, 0.5, 0.5, 1.0
	};
	static const glm::mat4 biasMatrix = glm::make_mat4(biasValues);
}


/*
Creates an OpenGL 3.2 CORE PROFILE rendering context. Sets itself
as the current renderer of the passed 'parent' Window. Not the best
way to do it - but it kept the Tutorial code down to a minimum!
*/
LIW::OGLRenderer::OGLRenderer(LIW::App::Window &window):
	LIW::LIWRenderer::LIWRenderer(window)
{
	init					= false;

	//// Did We Get A Device Context?
	//if (!(deviceContext=GetDC(windowHandle)))		{					
	//	std::cout << "OGLRenderer::OGLRenderer(): Failed to create window!\n";
	//	return;
	//}
	//
	////A pixel format descriptor is a struct that tells the Windows OS what renderOrder of front / back buffers we want to create etc
	//PIXELFORMATDESCRIPTOR pfd;
	//memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	//
	//pfd.nSize			= sizeof(PIXELFORMATDESCRIPTOR);
	//pfd.nVersion		= 1; 
 //  	pfd.dwFlags			= PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;   //It must be double buffered, it must support OGL(!), and it must allow us to draw to it...
 //  	pfd.iPixelType		= PFD_TYPE_RGBA;	//We want our front / back buffer to have 4 channels!
 //  	pfd.cColorBits		= 32;				//4 channels of 8 bits each!
 //  	pfd.cDepthBits		= 24;				//24 bit depth buffer
	//pfd.cStencilBits	= 8;				//plus an 8 bit stencil buffer
 //  	pfd.iLayerType		= PFD_MAIN_PLANE;
	//
	//GLuint		PixelFormat;
	//if (!(PixelFormat=ChoosePixelFormat(deviceContext,&pfd)))		{	// Did Windows Find A Matching Pixel Format for our PFD?
	//	std::cout << "OGLRenderer::OGLRenderer(): Failed to choose a pixel format!\n";
	//	return;
	//}
	//
	//if(!SetPixelFormat(deviceContext,PixelFormat,&pfd))			{		// Are We Able To Set The Pixel Format?
	//	std::cout << "OGLRenderer::OGLRenderer(): Failed to set a pixel format!\n";
	//	return;
	//}
	//
	//HGLRC		tempContext;		//We need a temporary OpenGL context to check for OpenGL 3.2 compatibility...stupid!!!
	//if (!(tempContext=wglCreateContext(deviceContext)))				{	// Are We Able To get the temporary context?
	//	std::cout << "OGLRenderer::OGLRenderer(): Cannot create a temporary context!\n";
	//	wglDeleteContext(tempContext);
	//	return;
	//}
	
	GLFWwindow* windowHandle = window.GetHandle();
	currentWindowHandle = windowHandle;

	//Try to activate current window (context)
	window.SetCurrent();
	//if(!wglMakeCurrent(deviceContext,tempContext))					{	// Try To Activate The Rendering Context
	//	std::cout << "OGLRenderer::OGLRenderer(): Cannot set temporary context!\n";
	//	wglDeleteContext(tempContext);
	//	return;
	//}
	// 
	//if (!gladLoadGL()) {
	//	std::cout << "OGLRenderer::OGLRenderer(): Cannot initialise GLAD!\n";	//It's all gone wrong!
	//	return;
	//}

	//Check GL Version >= 4.5
	char* ver = (char*)glGetString(GL_VERSION); 
	int major = ver[0] - '0';		//casts the 'correct' major version integer from our version string
	int minor = ver[2] - '0';		//casts the 'correct' minor version integer from our version string

	std::cout << "OGLRenderer::OGLRenderer(): Maximum OGL version supported is " << major << "." << minor << "\n";

	if(major < 4) {
		std::cout << "OGLRenderer::OGLRenderer(): Device does not support OpenGL 4.x!\n";
		return;
	}

	if(major == 4 && minor < 5) {
		std::cout << "OGLRenderer::OGLRenderer(): Device does not support OpenGL 4.5!\n";
		return;
	}
	//We do support OGL 3! Let's set it up...

	/*This part I actually have no idea what to do*/
//	int attribs[] = {
//        WGL_CONTEXT_MAJOR_VERSION_ARB, major,	//TODO: Maybe lock this to 3? We might actually get an OpenGL 4.x context...
//        WGL_CONTEXT_MINOR_VERSION_ARB, minor, 
//		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 
//#ifdef OPENGL_DEBUGGING 
//		| WGL_CONTEXT_DEBUG_BIT_ARB
//#endif		//No deprecated stuff!! DIE DIE DIE glBegin!!!!
//		,WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,		//We want everything OpenGL 3.2 provides...
//		0					//That's enough attributes...
//    };
//
//	//Everywhere else in the Renderers, we use function pointers provided by GLEW...but we can't initialise GLEW yet! So we have to use the 'Wiggle' API
//	//to get a pointer to the function that will create our OpenGL 3.2 context...
//	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC) wglGetProcAddress("wglCreateContextAttribsARB");
//	renderContext = wglCreateContextAttribsARB(deviceContext,0, attribs);
//
//	// Check for the context, and try to make it the current rendering context
//	if(!renderContext || !wglMakeCurrent(deviceContext,renderContext))		{			
//		std::cout << "OGLRenderer::OGLRenderer(): Cannot set OpenGL 3 context!\n";	//It's all gone wrong!
//		wglDeleteContext(renderContext);
//		wglDeleteContext(tempContext);
//		return;
//	}
//
//	wglDeleteContext(tempContext);	//We don't need the temporary context any more!

	//If we get this far, everything's going well!

#ifdef OPENGL_DEBUGGING
	GLint flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(&OGLRenderer::DebugCallback, NULL);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
		puts("Debug");
	}
	else
		puts("Debug for OpenGL not supported by your system!");
#endif

	glClearColor(0.2f,0.2f,0.2f,1.0f);			//When we clear the screen, we want it to be dark grey

	currentShader = 0;							//0 is the 'null' object name for shaderPrePixelBump programs...

	window.SetRenderer(this);					//Tell our window about the new renderer! (Which will in turn resize the renderer window to fit...)
}

/*
Destructor. Deletes the default shaderPrePixelBump, and the OpenGL rendering context.
*/
LIW::OGLRenderer::~OGLRenderer(void)	{
	currentWindowHandle = nullptr;
}

void LIW::OGLRenderer::BindFrameBuffer(LIWFrameBuffer& frameBuffer)
{
	bool completeness = frameBuffer.IsComplete();
	if (!frameBuffer.IsValid())
		throw std::runtime_error("framebuffer invlid. ");
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer.GetRawHandle());
	glViewport(0, 0, frameBuffer.GetWidth(), frameBuffer.GetHeight());
	if (!frameBuffer.IsComplete())
		throw std::runtime_error("framebuffer invlid. ");
}

void LIW::OGLRenderer::BindDefaultFrameBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width, height);
}

/*
Resizes the rendering area. Should only be called by the Window class!
Does lower bounds checking on input values, so should be reasonably safe
to call.
*/
void LIW::OGLRenderer::Resize(int x, int y)	{
	width	= max(x,1);	
	height	= max(y,1);
	glViewport(0,0,width,height);
}

///*
//Swaps the buffers, ready for the next frame's rendering. Should be called
//every frame, at the end of RenderScene(), or whereever appropriate for
//your application.
//*/
//void LIW::OGLRenderer::SwapBuffers() {
//	if (currentWindow)
//		currentWindow->SwapBuffer();
//	////We call the windows OS SwapBuffers on win32. Wrapping it in this 
//	////function keeps all the tutorial code 100% cross-platform (kinda).
//	//::SwapBuffers(deviceContext);
//}

///*
//Used by some later tutorials when we want to have framerate-independent
//updates on certain datatypes. Really, OGLRenderer should have its own
//timer, so it can just sit and look after itself (potentially even in
//another thread...), but it's fine for the tutorials.
//
//STUDENTS: Don't put your entity update routine in this, or anything like
//that - it's just asking for trouble! Strictly speaking, even the camera
//shouldn't be in here...(I'm lazy)
//*/
//void LIW::OGLRenderer::UpdateScene(float msec)	{
//
//}

/*
Updates the uniform matrices of the current shaderPrePixelBump. Assumes that
the shaderPrePixelBump has uniform matrices called modelMatrix, viewMatrix,
projMatrix, and textureMatrix. Updates them with the relevant
matrix data. Sanity checks currentShader, so is always safe to
call.
*/
//void OGLRenderer::UpdateShaderMatrices()	{
//	if(currentShader) {
//		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), NCLGL_SHADER_MODEL_MATRIX)   ,	1,false, modelMatrix.values);
//		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), NCLGL_SHADER_VIEW_MATRIX)    ,	1,false, viewMatrix.values);
//		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), NCLGL_SHADER_PROJECTION_MATRIX)    ,	1,false, projMatrix.values);
//		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), NCLGL_SHADER_TEXTURE_MATRIX) , 1,false, textureMatrix.values);
//		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), NCLGL_SHADER_SHADOW_MATRIX)  , 1,false, shadowMatrix.values);
//	}
//}

//void OGLRenderer::UpdateShaderViewProjFromCamera(shared_ptr<Shader> shaderPrePixelBump, Camera* camera)
//{
//	if (shaderPrePixelBump) {
//		glUniformMatrix4fv(glGetUniformLocation(shaderPrePixelBump->GetProgram(), NCLGL_SHADER_VIEW_MATRIX), 1, false, camera->BuildViewMatrix().values);
//		glUniformMatrix4fv(glGetUniformLocation(shaderPrePixelBump->GetProgram(), NCLGL_SHADER_PROJECTION_MATRIX), 1, false, camera->projMatrix.values);
//	}
//}

//void OGLRenderer::BindShader(shared_ptr<Shader>s) {
//	currentShader = s;
//	glUseProgram(s->GetProgram());
//}

#ifdef OPENGL_DEBUGGING
void LIW::OGLRenderer::DebugCallback(GLenum source, GLenum renderOrder, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)	{
		string sourceName;
		string typeName;
		string severityName;

		if (renderOrder == GL_DEBUG_TYPE_OTHER_ARB) {
			return;
		}

		switch(source) {
			case GL_DEBUG_SOURCE_API_ARB			: sourceName = "Source(OpenGL)"			;break;
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB	: sourceName = "Source(Window System)"	;break;
			case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB: sourceName = "Source(Shader Compiler)";break;
			case GL_DEBUG_SOURCE_THIRD_PARTY_ARB	: sourceName = "Source(Third Party)"	;break;
			case GL_DEBUG_SOURCE_APPLICATION_ARB	: sourceName = "Source(Application)"	;break;
			case GL_DEBUG_SOURCE_OTHER_ARB			: sourceName = "Source(Other)"			;break;
		}

		switch(renderOrder) {
			case GL_DEBUG_TYPE_ERROR_ARB				: typeName = "Type(Error)"					;break;
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB	: typeName = "Type(Deprecated Behaviour)"	;break;
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB	: typeName = "Type(Undefined Behaviour)"	;break;
			case GL_DEBUG_TYPE_PORTABILITY_ARB			: typeName = "Type(Portability)"			;break;
			case GL_DEBUG_TYPE_PERFORMANCE_ARB			: typeName = "Type(Performance)"			;break;
			case GL_DEBUG_TYPE_OTHER_ARB				: typeName = "Type(Other)"					;break;
			default: {
				return;
			}
		}

		switch(severity) {
			case GL_DEBUG_SEVERITY_HIGH_ARB		: severityName = "Priority(High)"		;break;
			case GL_DEBUG_SEVERITY_MEDIUM_ARB	: severityName = "Priority(Medium)"		;break;
			case GL_DEBUG_SEVERITY_LOW_ARB		: severityName = "Priority(Low)"		;break;
		}

		std::cout << "OpenGL Debug Output: " + sourceName + ", " + typeName + ", " + severityName + ", " + string(message) << "\n";
#ifdef LIW_DEBUG_THROW_AT_GL_ERROR
		if (renderOrder == GL_DEBUG_TYPE_ERROR_ARB ||
			renderOrder == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB ||
			renderOrder == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB)
		{
			throw std::runtime_error("OpenGL Fatal Error! ");
		}
#endif //LIW_DEBUG_THROW_AT_GL_ERROR
}
#endif




