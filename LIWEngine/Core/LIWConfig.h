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

//
// Memory
//
#ifdef _DEBUG
#define DEBUG_PRINT_MEMORY_INFO
#endif

// Enable memset for debugging
//#define LIW_DEBUG_DEFAULT_ALLOCATOR_MEMSET



/*
* Auto config
*/
#ifdef LIW_RENDER_OPENGL
#define LIW_USE_GLEW
#ifdef _DEBUG
#define OPENGL_DEBUGGING
#endif
#endif
