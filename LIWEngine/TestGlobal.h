#pragma once
#include "Memory/LIWMemory.h"
//#include "TestRenderer.h"
#include "Renderers/OGLRenderer_Forward.h"

using namespace LIW;

class TestGlobal
{
public:
	//static LIWPointer<TestRenderer, LIWMem_Static> s_renderer;
	static LIWPointer<OGLRendererForward, LIWMem_Static> s_renderer;
	static liw_objhdl_type cam;
};

