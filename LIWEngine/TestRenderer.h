#pragma once
#include "OGLRenderer.h"
#include "Maths/Vector4.h"

class TestRenderer :
    public LIW::OGLRenderer
{
public:
    TestRenderer(LIW::App::Window& parent) :OGLRenderer(parent) {};
    virtual void Update(float msec) override;
    virtual void RenderScene() override;
};

