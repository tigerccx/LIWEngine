#pragma once
#include "Renderers/OGLRenderer.h"
#include "Maths/LIWMaths.h"
#include "Threading/LIWFiber.h"
#include "Memory/LIWMemory.h"
#include "LIWFrame.h"
#include "Threading/LIWFiberTask.h"
#include "Threading/LIWFiberExecutor.h"
#include "Threading/LIWThreadWorkerTask.h"
#include "Threading/LIWMainThreadExecutor.h"

using namespace LIW;
using namespace LIW::Maths;

class TestRenderer :
    public LIW::OGLRenderer
{
public:
    TestRenderer(LIW::App::Window& parent) :OGLRenderer(parent) {};
    virtual void Update(float msec) override;
    virtual void RenderScene() override;
};


class FT_TestRenderUpdate final :
    public LIWFiberTask
{
public:
    void Execute(LIWFiberWorkerPointer thisFiber) override;
public:
    LIWPointer<LIWFrameData, LIWMem_Frame> m_ptrFrameData;
};

class FT_TestRenderRender final :
    public LIWFiberTask
{
public:
    void Execute(LIWFiberWorkerPointer thisFiber) override;
};

class TT_TestRenderRender final :
    public LIWThreadWorkerTask
{
public:
    void Execute() override;
};
