#pragma once
#include "Framework/LIWSystem.h"
#include "TestComponent0.h"
#include "Framework/LIWComponentManager.h"
#include "TestFiberTaskIds.h"
#include "Memory/LIWMemory.h"
#include "Threading/LIWFiberExecutor.h"
#include "LIWFrame.h"

using namespace LIW;

class TestSystem0:
	public LIW::LIWSystem
{
};

#include "Threading/LIWFiberTask.h"

class FT_TestSystem0Update final :
	public LIWFiberTask
{
public:
	void Execute(LIWFiberWorkerPointer thisFiber) override;
public:
	LIWPointer<LIWFrameData, LIWMem_Frame> m_ptrFrameData{};
};

class FT_TestSystem0Update_TestComponent0 final :
	public LIWFiberTask
{
public:
	void Execute(LIWFiberWorkerPointer thisFiber) override;
public:
	int m_idxBeg;
	int m_idxEnd;
	float m_dt;
};