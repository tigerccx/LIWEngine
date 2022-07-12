#pragma once

#include "Framework/LIWGame.h"

#include "TestRenderer.h"

#include "LIWEntityManager.h"
#include "LIWComponentManager.h"
#include "TestComponent0.h"

using namespace LIW;

class TestGame :
    public LIWGame
{
public:
    TestGame(App::Environment* environment):
        LIWGame(environment)
    {

    }
    ~TestGame() {

    }
    virtual int Initialise() override;
};

