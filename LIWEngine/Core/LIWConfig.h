#pragma once
/*
* Configuration
*/

#ifdef _DEBUG
#define LIW_DEBUG
#else
#define LIW_RELEASE
#endif

//
// Hardware
//
#define LIW_HARDWARE_USE_HIGH_PERF_GRAPHICS_CARD

//
// Render
// 
// Render Pipeline
#define LIW_RENDER_OPENGL
//#define LIW_RENDER_VULKAN
// Shader
//#define LIW_DEBUG_PRINT_SHADER_CONTENT
//#define LIW_DEBUG_THROW_AT_GL_ERROR

//
// Memory
//
#ifdef LIW_DEBUG
//#define DEBUG_PRINT_MEMORY_INFO
#endif

// Enable memset for debugging
//#define LIW_DEBUG_DEFAULT_ALLOCATOR_MEMSET

//
// Editor
//
#define LIW_ENABLE_EDITOR


/*
* Auto config
*/
#ifdef LIW_RENDER_OPENGL
#define LIW_USE_GLEW
#ifdef LIW_DEBUG
#define OPENGL_DEBUGGING
#endif
#endif

