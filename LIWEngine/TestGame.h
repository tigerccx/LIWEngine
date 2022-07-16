#pragma once

#include "Framework/LIWGame.h"

#include "TestRenderer.h"

//#include "Framework/LIWEntityManager.h"
#include "Framework/LIWComponentManager.h"
#include "TestComponent0.h"
#include "TestSystem0.h"
#include "Fiber/LIWFiberCommon.h"

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

void FT_TestGameUpdate(LIW_FIBER_RUNNER_PARAM);