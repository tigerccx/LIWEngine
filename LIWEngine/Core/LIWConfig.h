#pragma once
/*
* Configuration
*/

//
// Render
// 
// Render Pipeline
//#define LIW_RENDER_OPENGL
#define LIW_RENDER_VULKAN

//
// Memory
//
#ifdef _DEBUG
#define DEBUG_PRINT_MEMORY_INFO
#endif



/*
* Auto config
*/
#ifdef LIW_RENDER_OPENGL
#define LIW_USE_GLEW
#endif