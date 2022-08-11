#pragma once
#include "Memory/LIWMemory.h"
#include "TestRenderer.h"
#include "Renderers/OGLRenderer_Forward.h"
#include "Renderers/OGLRenderer_Deferred.h"
#include "Renderers/OGLRenderer_DeferredVisibility.h"

using namespace LIW;

class TestGlobal
{
public:
	static LIWPointer<TestRenderer, LIWMem_Static> s_rendererTest;
	static LIWPointer<OGLRenderer_Forward, LIWMem_Static> s_rendererForward;
	static LIWPointer<OGLRenderer_Deferred, LIWMem_Static> s_rendererDeferred;
	static LIWPointer<OGLRenderer_DeferredVisibility, LIWMem_Static> s_rendererVisibility;
	static liw_objhdl_type cam;
};

