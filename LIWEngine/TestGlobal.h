#pragma once
#include "Memory/LIWMemory.h"
#include "TestRenderer.h"

using namespace LIW;

class TestGlobal
{
public:
	static LIWPointer<TestRenderer, LIWMem_Static> s_renderer;
};

