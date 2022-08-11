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

#define TEST_TEXTURE_COUNT 256

class TestGame2 final :
    public LIWGame
{
public:
    TestGame2()
    {

    }
    ~TestGame2() {

    }
    int Initialise() override;
    int CleanUp() override;

    int Update(LIWPointer<LIWFrameData, LIWMem_Frame> frameData, LIWFiberWorkerPointer thisFiber) override;


    LIWDArray<liw_objhdl_type> m_testComponent0s;
    LIWDArray<liw_objhdl_type> m_transforms;
    LIWDArray<liw_objhdl_type> m_sceneNodes;
    LIWDArray<liw_objhdl_type> m_meshRendererBatcheds;
    LIWDArray<LIWEntity> m_entities;
    liw_objhdl_type m_camera;
    liw_objhdl_type m_cameraController;
    LIWDArray<liw_objhdl_type> m_lights;

    liw_objhdl_type m_image;

    liw_objhdl_type m_tex2D;
    liw_objhdl_type m_tex2D1;
    liw_objhdl_type m_tex2D2;
    liw_objhdl_type m_tex2DTests[TEST_TEXTURE_COUNT];

    liw_objhdl_type m_shader_vert;
    liw_objhdl_type m_shader_frag;
    liw_objhdl_type m_shader_standardMaterial;

    liw_objhdl_type m_shaderProgram;
    liw_objhdl_type m_shaderProgramStandard;

    liw_objhdl_type m_materialTest;
    liw_objhdl_type m_materialStandard;
    liw_objhdl_type m_materialTests[TEST_TEXTURE_COUNT];
};