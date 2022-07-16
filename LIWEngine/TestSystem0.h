#pragma once
#include "Framework/LIWSystem.h"
#include "TestComponent0.h"
#include "Framework/LIWComponentManager.h"
#include "TestFiberTaskIds.h"

using namespace LIW;

class TestSystem0:
	public LIW::LIWSystem
{
public:
	struct TestComponent0UpdateParam {
		int m_idxBeg;
		int m_idxEnd;
		float m_dt;
	};
	static void FT_TestComponent0Update(LIW_FIBER_RUNNER_PARAM);

	
	static void FT_Update(LIW_FIBER_RUNNER_PARAM);
};