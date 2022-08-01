#pragma once
#include "Memory/LIWMemory.h"
//#include "TestRenderer.h"
#include "Renderers/OGLRenderer_Forward.h"
#include "Renderers/OGLRenderer_Deferred.h"

using namespace LIW;

class TestGlobal
{
public:
	//static LIWPointer<TestRenderer, LIWMem_Static> s_renderer;
	//static LIWPointer<OGLRenderer_Forward, LIWMem_Static> s_renderer;
	static LIWPointer<OGLRenderer_Deferred, LIWMem_Static> s_renderer;
	static liw_objhdl_type cam;
};

