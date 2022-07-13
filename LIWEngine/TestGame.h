#pragma once

#include "Framework/LIWGame.h"

#include "TestRenderer.h"

#include "Framework/LIWEntityManager.h"
#include "Framework/LIWComponentManager.h"
#include "TestComponent0.h"

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
    virtual int Initialise() override;
};

