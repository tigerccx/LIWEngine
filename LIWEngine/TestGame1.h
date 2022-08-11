#pragma once

#include "Framework/LIWGame.h"

//#include "TestRenderer.h"
#include "Renderers/OGLRenderer_Forward.h"

//#include "Framework/LIWEntityManager.h"
#include "Framework/LIWComponentManager.h"
#include "Framework/Systems/LIWSystem_CameraControl.h"
#include "TestComponent0.h"
#include "TestSystem0.h"
#include "Threading/LIWFiberCommon.h"
#include "Threading/LIWFiberTask.h"
#include "Threading/LIWFiberExecutor.h"
#include "TestGlobal.h"

using namespace LIW;

class TestGame1 final :
    public LIWGame
{
public:
    TestGame1()
    {

    }
    ~TestGame1() {

    }
    int Initialise() override;
    int CleanUp() override;

    int Update(LIWPointer<LIWFrameData, LIWMem_Frame> frameData, LIWFiberWorkerPointer thisFiber) override;


    LIWDArray<liw_objhdl_type> m_testComponent0s;
    LIWDArray<liw_objhdl_type> m_transforms;
    LIWDArray<liw_objhdl_type> m_sceneNodes;
    LIWDArray<liw_objhdl_type> m_meshRenderers;
    LIWDArray<LIWEntity> m_entities;
    liw_objhdl_type m_camera;
    liw_objhdl_type m_cameraController;
    LIWDArray<liw_objhdl_type> m_lights;

    liw_objhdl_type m_image;

    liw_objhdl_type m_tex2D;
    liw_objhdl_type m_tex2D1;
    liw_objhdl_type m_tex2D2;

    liw_objhdl_type m_shader_vert;
    liw_objhdl_type m_shader_frag;

    liw_objhdl_type m_shaderProgram;
    liw_objhdl_type m_shaderProgramForward;
    liw_objhdl_type m_shaderProgramDeferred;

    liw_objhdl_type m_materialTest;
    liw_objhdl_type m_materialForward;
    liw_objhdl_type m_materialDeferred;
};