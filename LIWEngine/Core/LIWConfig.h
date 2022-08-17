#pragma once
/*
* Configuration
*/

//
// Render
// 
// Render Pipeline
#define LIW_RENDER_OPENGL
//#define LIW_RENDER_VULKAN
// Shader
#define LIW_DEBUG_PRINT_SHADER_CONTENT

//
// Memory
//
#ifdef _DEBUG
//#define DEBUG_PRINT_MEMORY_INFO
#endif
#define LIW_MEMORY_HANDLE_DIGITS 20

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
#ifdef _DEBUG
#define OPENGL_DEBUGGING
#endif
#endif

