#pragma once
#include "Renderers/OGLRenderer.h"
#include "Maths/Vector4.h"
#include "Threading/LIWFiber.h"
#include "Memory/LIWMemory.h"
#include "LIWFrame.h"
#include "Threading/LIWFiberTask.h"
#include "Threading/LIWFiberExecutor.h"
#include "Threading/LIWThreadWorkerTask.h"
#include "Threading/LIWMainThreadExecutor.h"

using namespace LIW;

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
    void Execute(LIWFiberWorker* thisFiber) override;
public:
    LIWPointer<LIWFrameData, LIWMem_Frame> m_ptrFrameData;
    TestRenderer* m_renderer;
};

class FT_TestRenderRender final :
    public LIWFiberTask
{
public:
    void Execute(LIWFiberWorker* thisFiber) override;
public:
    TestRenderer* m_renderer;
};

class TT_TestRenderRender final :
    public LIWThreadWorkerTask
{
public:
    void Execute() override;
public:
    TestRenderer* m_renderer;
};
