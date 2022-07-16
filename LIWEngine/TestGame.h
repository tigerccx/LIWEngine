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

    TestRenderer* m_renderer{ nullptr };
    TestSystem0* m_sys0{ nullptr };
};

class FT_TestGameUpdate final:
    public LIWFiberTask 
{
public:
    void Execute(LIWFiberWorker* thisFiber) override;
public:
    LIWPointer<GameData, LIWMem_Frame> m_ptrFrameData{};
    //LIWPointer<TestGame, LIWMem_Static> m_ptrGame{};
    TestGame* m_ptrGame{ nullptr };
};