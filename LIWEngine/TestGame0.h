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

class TestGame0 final :
    public LIWGame
{
public:
    TestGame0()
    {

    }
    ~TestGame0() {

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

    LIWPointer<LIWImage, LIWMem_Default> m_image;

    LIWPointer<LIWTexture2D, LIWMem_Default> m_tex2D;
    LIWPointer<LIWTexture2D, LIWMem_Default> m_tex2D1;
    LIWPointer<LIWTexture2D, LIWMem_Default> m_tex2D2;
    LIWPointer<LIWTexture2D, LIWMem_Default> m_tex2DTests[TEST_TEXTURE_COUNT];

    LIWPointer<LIWShader, LIWMem_Default> m_shader_vert;
    LIWPointer<LIWShader, LIWMem_Default> m_shader_frag;

    LIWPointer<LIWShaderProgram, LIWMem_Default> m_shaderProgram;
    LIWPointer<LIWShaderProgram, LIWMem_Default> m_shaderProgramForward;
    LIWPointer<LIWShaderProgram, LIWMem_Default> m_shaderProgramDeferred;

    LIWPointer<LIWMaterial, LIWMem_Default> m_materialTest;
    LIWPointer<LIWMaterial, LIWMem_Default> m_materialForward;
    LIWPointer<LIWMaterial, LIWMem_Default> m_materialDeferred;
    LIWPointer<LIWMaterial, LIWMem_Default> m_materialTests[TEST_TEXTURE_COUNT];
};