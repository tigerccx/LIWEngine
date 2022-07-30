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

    LIWDArray<liw_objhdl_type> m_testComponent0s;
    LIWDArray<liw_objhdl_type> m_transforms;
    LIWDArray<liw_objhdl_type> m_sceneNodes;
    LIWDArray<liw_objhdl_type> m_meshRenderers;
    LIWDArray<LIWEntity> m_entities;
    liw_objhdl_type m_camera;
    liw_objhdl_type m_cameraController;
    liw_objhdl_type m_light;

    liw_objhdl_type m_meshData;

    liw_objhdl_type m_mesh;

    liw_objhdl_type m_image;

    liw_objhdl_type m_tex2D;
    liw_objhdl_type m_tex2D1;
    liw_objhdl_type m_tex2D2;

    liw_objhdl_type m_shader_vert;
    liw_objhdl_type m_shader_frag;

    liw_objhdl_type m_shaderProgram;
    liw_objhdl_type m_shaderProgramLight;

    liw_objhdl_type m_material;
    liw_objhdl_type m_material1;
};

class FT_TestGameUpdate final:
    public LIWFiberTask 
{
public:
    void Execute(LIWFiberWorkerPointer thisFiber) override;
public:
    LIWPointer<LIWFrameData, LIWMem_Frame> m_ptrFrameData{};
};