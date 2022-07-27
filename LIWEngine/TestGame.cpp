#include "TestGame.h"
#include "Editor/LIWEditorTasks.h"

#include "Framework/LIWECSFunctional.h"

int TestGame::Initialise()
{
	TestGlobal::s_renderer = liw_new_static<OGLRendererForward>(*(m_currentEnvironment->m_window));

	auto& assetManager = *LIWGlobal::GetAssetManager();

	// Texture
	m_image = assetManager.CreateImage("image0");
	auto& image = assetManager.GetImage(m_image);
	image.LoadImg("../../Resources/Textures/viking_room.png", LIWImageFormat_RGBA);
	m_tex2D = assetManager.CreateTexture2D("tex0");
	auto& tex2D = assetManager.GetTexture2D(m_tex2D);
	tex2D.CreateTexture(image);
	image.UnloadImg();
	assetManager.DestroyImage("image0");

	// Mesh
	m_meshData = assetManager.CreateMeshData("meshData0");
	auto& meshData = assetManager.GetMeshData(m_meshData);
	meshData.LoadMeshData_Obj("../../Resources/Meshes/viking_room.obj");
	m_mesh = assetManager.CreateMesh("mesh0");
	auto& mesh = assetManager.GetMesh(m_mesh);
	mesh.CreateMesh(meshData);
	meshData.UnloadMeshData();
	assetManager.DestroyMeshData("meshData0");

	// Shader
	m_shader_vert = assetManager.CreateShader("shaderVert0");
	auto& shaderVert = assetManager.GetShader(m_shader_vert);
	shaderVert.LoadShader("./Shaders/OGL/Test_Vert.glsl", LIWShaderType_Vertex);
	m_shader_frag = assetManager.CreateShader("shaderFrag0");
	auto& shaderFrag = assetManager.GetShader(m_shader_frag);
	shaderFrag.LoadShader("./Shaders/OGL/Test_Frag.glsl", LIWShaderType_Fragment);
	m_shaderProgram = assetManager.CreateShaderProgram("shaderProgram0");
	auto& shaderProgram = assetManager.GetShaderProgram(m_shaderProgram);
	shaderProgram.CreateShader({ shaderVert ,shaderFrag });
	assetManager.DestroyShader("shaderVert0");
	assetManager.DestroyShader("shaderFrag0");

	// Material
	m_material = assetManager.CreateMaterial("material0");
	auto& material = assetManager.GetMaterial(m_material);
	material.AddParam_Tex2D("mainTex", m_tex2D);
	material.m_handleShaderProgram = m_shaderProgram;

	LIW_ECS_FetchEntities(m_entities, 2);
	LIW_ECS_CreateComponents(LIWComponent_Transform, m_transforms, 2);
	LIW_ECS_CreateComponents(LIWComponent_MeshRenderer, m_meshRenderers, 1);
	m_camera = LIW_ECS_CreateComponent(LIWComponent_Camera);

	// Camera
	LIW_ECS_AttachComponentToEntity(LIWComponent_Transform, m_transforms[0], m_entities[0]);
	LIW_ECS_AttachComponentToEntity(LIWComponent_Camera, m_camera, m_entities[0]);
	auto& transCam = LIW_ECS_GetComponent(LIWComponent_Transform, m_transforms[0]);
	transCam.m_location = glm::vec3(0, 0, 5);
	auto& cam = LIW_ECS_GetComponent(LIWComponent_Camera, m_camera);
	float w = TestGlobal::s_renderer->GetWidth();
	float h = TestGlobal::s_renderer->GetHeight();
	LIWCameraParam_Perspective camParam{ 30.0f, w / h };
	cam.SetPerspective(camParam);

	// Object
	LIW_ECS_AttachComponentToEntity(LIWComponent_Transform, m_transforms[1], m_entities[1]);
	LIW_ECS_AttachComponentToEntity(LIWComponent_MeshRenderer, m_meshRenderers[0], m_entities[1]);
	auto& transObj = LIW_ECS_GetComponent(LIWComponent_Transform, m_transforms[1]);
	transObj.m_location = glm::vec3(0, 0, -5);
	auto& meshRenderer = LIW_ECS_GetComponent(LIWComponent_MeshRenderer, m_meshRenderers[0]);
	meshRenderer.m_handleMaterial = m_material;
	meshRenderer.m_handleMesh = m_mesh;

	// Apply changes
	LIW_ECS_ApplyChangeOnComponentManager(LIWComponent_Transform);
	LIW_ECS_ApplyChangeOnComponentManager(LIWComponent_MeshRenderer);
	LIW_ECS_ApplyChangeOnComponentManager(LIWComponent_Camera);

	return 0;
}

int TestGame::CleanUp()
{
	auto& assetManager = *LIWGlobal::GetAssetManager();
	assetManager.DestroyMaterial("material0");
	assetManager.DestroyShaderProgram("shaderProgram0");
	assetManager.DestroyMesh("mesh0");
	assetManager.DestroyTexture2D("tex0");

	liw_delete(TestGlobal::s_renderer);
	return 0;
}

#include "LIWCore.h"
#include "TestFiberTaskIds.h"
#include "TestSystem0.h"

void FT_TestGameUpdate::Execute(LIWFiberWorkerPointer thisFiber)
{
	using namespace LIW;

	LIWFiberExecutor::m_executor.IncreaseSyncCounter(TEST_SYNC_COUNTER_SYSTEM, 1);
	auto taskUpdateTestSys0 = liw_new_def<FT_TestSystem0Update>();
	taskUpdateTestSys0->m_ptrFrameData = m_ptrFrameData;
	LIWFiberExecutor::m_executor.Submit(taskUpdateTestSys0);
	LIWFiberExecutor::m_executor.WaitOnSyncCounter(TEST_SYNC_COUNTER_SYSTEM, thisFiber);

	LIWFiberExecutor::m_executor.IncreaseSyncCounter(LIW_SYNC_COUNTER_RESERVE_RENDER, 1);
	auto ptrTT_OGLForwardRender = new LIW_TT_OGLForwardRender();
	ptrTT_OGLForwardRender->m_renderer = TestGlobal::s_renderer;
	LIWMainThreadExecutor::m_executor.Submit(ptrTT_OGLForwardRender);
	LIWFiberExecutor::m_executor.WaitOnSyncCounter(LIW_SYNC_COUNTER_RESERVE_RENDER, thisFiber);

	LIW_ECS_ApplyChangeOnComponentManager(LIWComponent_Transform);
	LIW_ECS_ApplyChangeOnComponentManager(LIWComponent_MeshRenderer);
	LIW_ECS_ApplyChangeOnComponentManager(LIWComponent_Camera);
	LIW_ECS_ApplyChangeOnComponentManager(TestComponent0);

	auto ptrFT_EdtrUIDrawBeg = liw_new_def<Editor::LIW_FT_EDTR_UIDrawBeg>();
	ptrFT_EdtrUIDrawBeg->ptrFrameData = m_ptrFrameData;
	LIWFiberExecutor::m_executor.Submit(ptrFT_EdtrUIDrawBeg);

	//auto ptrFT_FrameEnd = liw_new_def<LIW_FT_FrameEnd>();
	//ptrFT_FrameEnd->ptrFrameData = m_ptrFrameData;
	//LIWFiberExecutor::m_executor.Submit(ptrFT_FrameEnd);
}
