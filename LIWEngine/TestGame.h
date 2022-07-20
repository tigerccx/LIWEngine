#pragma once

#include "Framework/LIWGame.h"

#include "TestRenderer.h"

//#include "Framework/LIWEntityManager.h"
#include "Framework/LIWComponentManager.h"
#include "TestComponent0.h"
#include "TestSystem0.h"
#include "Threading/LIWFiberCommon.h"
#include "Threading/LIWFiberTask.h"
#include "Threading/LIWFiberExecutor.h"
#include "TestGlobal.h"

using namespace LIW;

class TestGame :
    public LIWGame
{
public:
    TestGame()
    {

    }
    ~TestGame() {

    }
    int Initialise() override;
    int CleanUp() override;
};

class FT_TestGameUpdate final:
    public LIWFiberTask 
{
public:
    void Execute(LIWFiberWorkerPointer thisFiber) override;
public:
    LIWPointer<LIWFrameData, LIWMem_Frame> m_ptrFrameData{};
};