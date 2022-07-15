#pragma once
#include "Renderers/OGLRenderer.h"
#include "Maths/Vector4.h"

class TestRenderer :
    public LIW::OGLRenderer
{
public:
    TestRenderer(LIW::App::Window& parent) :OGLRenderer(parent) {};
    virtual void Update(float msec) override;
    virtual void RenderScene() override;
};

#include "Fiber/LIWFiber.h"
#include "Fiber/LIWThreadWorkerTask.h"
#include "Memory/LIWMemory.h"
#include "LIWFrame.h"

struct TestRenderData
{
    LIWPointer<LIWFrameData, LIWMem_Frame> m_hdlFrameData;
    TestRenderer* m_renderer;
};

void FT_TestRenderUpdate(LIW_FIBER_RUNNER_PARAM);

void FT_TestRenderRender(LIW_FIBER_RUNNER_PARAM);

void TT_TestRenderRender(LIW_THREADWORKER_RUNNER_PARAM);