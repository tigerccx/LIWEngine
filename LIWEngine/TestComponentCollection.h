#pragma once
#include "Framework/LIWComponentCollection.h"

#include "TestComponent0.h"

using namespace LIW;

struct TestComponentCollection:
	public LIWComponentCollection
{
	LIW_INJECT_COMPONENT_MANAGER(TestComponent0, 1<<15, 1<<15);
};

