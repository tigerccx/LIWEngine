#include "TestGame.h"

#ifdef LIW_ENABLE_EDITOR
#include "Editor/LIWEditorTasks.h"
#endif //LIW_ENABLE_EDITOR

#include "Framework/LIWECSFunctional.h"

int TestGame::Initialise()
{
	//
	// Renderer creation
	//
	TestGlobal::s_renderer = liw_new_static<OGLRendererForward>(*(m_currentEnvironment->m_window));

	
	//
	// Asset creation
	//
	auto& assetManager = *LIWGlobal::GetAssetManager();

	// Texture
	m_image = assetManager.CreateImage("image0");
	auto& image = assetManager.GetImage(m_image);

	image.LoadImg(LIW_PATH_DIR_RESOURCE_TEXTURES"viking_room.png", LIWImageFormat_RGBA);
	m_tex2D = assetManager.CreateTexture2D("tex0");
	auto& tex2D = assetManager.GetTexture2D(m_tex2D);
	tex2D.CreateTexture(image);
	image.UnloadImg();

	image.LoadImg(LIW_PATH_DIR_RESOURCE_TEXTURES"chessboard512.jpg", LIWImageFormat_RGB);
	m_tex2D1 = assetManager.CreateTexture2D("tex1");
	auto& tex2D1 = assetManager.GetTexture2D(m_tex2D1);
	tex2D1.CreateTexture(image);
	image.UnloadImg();

	assetManager.DestroyImage("image0");

	// Mesh
	m_meshData = assetManager.CreateMeshData("meshData0");
	auto& meshData = assetManager.GetMeshData(m_meshData);
	meshData.LoadMeshData_Obj(LIW_PATH_DIR_RESOURCE_MESHES"viking_room.obj");
	m_mesh = assetManager.CreateMesh("mesh0");
	auto& mesh = assetManager.GetMesh(m_mesh);
	mesh.CreateMesh(meshData);
	meshData.UnloadMeshData();
	assetManager.DestroyMeshData("meshData0");

	// Shader
	m_shader_vert = assetManager.CreateShader("shaderVert0");
	auto& shaderVert = assetManager.GetShader(m_shader_vert);
	m_shader_frag = assetManager.CreateShader("shaderFrag0");
	auto& shaderFrag = assetManager.GetShader(m_shader_frag);
	
	shaderVert.LoadShader(LIW_PATH_DIR_DEFAULT_SHADERS"OGL/Test_Vert.glsl", LIWShaderType_Vertex);
	shaderFrag.LoadShader(LIW_PATH_DIR_DEFAULT_SHADERS"OGL/Test_Frag.glsl", LIWShaderType_Fragment);
	m_shaderProgram = assetManager.CreateShaderProgram("shaderProgram0");
	auto& shaderProgram = assetManager.GetShaderProgram(m_shaderProgram);
	shaderProgram.CreateShader({ shaderVert ,shaderFrag });
	shaderVert.UnloadShader();
	shaderFrag.UnloadShader();

	shaderVert.LoadShader(LIW_PATH_DIR_DEFAULT_SHADERS"OGL/Test_1_Vert.glsl", LIWShaderType_Vertex);
	shaderFrag.LoadShader(LIW_PATH_DIR_DEFAULT_SHADERS"OGL/Test_1_Frag.glsl", LIWShaderType_Fragment);
	m_shaderProgramLight = assetManager.CreateShaderProgram("shaderProgram1");
	auto& shaderProgramLight = assetManager.GetShaderProgram(m_shaderProgramLight);
	shaderProgramLight.CreateShader({ shaderVert ,shaderFrag });
	shaderVert.UnloadShader();
	shaderFrag.UnloadShader();

	assetManager.DestroyShader("shaderVert0");
	assetManager.DestroyShader("shaderFrag0");

	// Material
	m_material = assetManager.CreateMaterial("material0");
	auto& material = assetManager.GetMaterial(m_material);
	material.AddParam_Tex2D("mainTex", m_tex2D);
	material.m_handleShaderProgram = m_shaderProgram;

	m_material1 = assetManager.CreateMaterial("material1");
	auto& material1 = assetManager.GetMaterial(m_material1);
	material1.AddParam_Tex2D("mainTex", m_tex2D1);
	material1.m_handleShaderProgram = m_shaderProgramLight;

	//
	// Scene creation
	//

	int countSpheres = 9;

	LIW_ECS_FetchEntities(m_entities, 3 + countSpheres);
	LIW_ECS_CreateComponents(LIWComponent_Transform, m_transforms, 3 + countSpheres);
	LIW_ECS_CreateComponents(LIWComponent_SceneNode, m_sceneNodes, 3 + countSpheres);
	LIW_ECS_CreateComponents(LIWComponent_MeshRenderer, m_meshRenderers, 1 + countSpheres);
	m_camera = LIW_ECS_CreateComponent(LIWComponent_Camera);
	m_cameraController = LIW_ECS_CreateComponent(LIWComponent_CameraController);
	m_light = LIW_ECS_CreateComponent(LIWComponent_Light);

	int idxEntity = 0;
	int idxTransform = 0;
	int idxSceneNode = 0;
	int idxMeshRenderer = 0;

	// Camera
	LIW_ECS_AttachComponentToEntity(LIWComponent_Transform, m_transforms[idxTransform], m_entities[idxEntity]);
	LIW_ECS_AttachComponentToEntity(LIWComponent_SceneNode, m_sceneNodes[idxSceneNode], m_entities[idxEntity]);
	LIW_ECS_AttachComponentToEntity(LIWComponent_Camera, m_camera, m_entities[idxEntity]);
	LIW_ECS_AttachComponentToEntity(LIWComponent_CameraController, m_cameraController, m_entities[idxEntity]);
	auto& transCam = LIW_ECS_GetComponent(LIWComponent_Transform, m_transforms[idxTransform]);
	transCam.m_position = glm::vec3(0.0f, 0.0f, 5.0f);
	auto& cam = LIW_ECS_GetComponent(LIWComponent_Camera, m_camera);
	float w = (float)TestGlobal::s_renderer->GetWidth();
	float h = (float)TestGlobal::s_renderer->GetHeight();
	LIWCameraParam_Perspective camParam{ 30.0f, w / h };
	cam.SetPerspective(camParam);
	auto& camCtrl = LIW_ECS_GetComponent(LIWComponent_CameraController, m_cameraController);
	camCtrl.m_isSelected = true;
	TestGlobal::cam = m_transforms[idxTransform];

	idxEntity++; idxTransform++; idxSceneNode++;


	// Object0: Model
	LIW_ECS_AttachComponentToEntity(LIWComponent_Transform, m_transforms[idxTransform], m_entities[idxEntity]);
	LIW_ECS_AttachComponentToEntity(LIWComponent_SceneNode, m_sceneNodes[idxSceneNode], m_entities[idxEntity]);
	LIW_ECS_AttachComponentToEntity(LIWComponent_MeshRenderer, m_meshRenderers[idxMeshRenderer], m_entities[idxEntity]);
	auto& transObj = LIW_ECS_GetComponent(LIWComponent_Transform, m_transforms[idxTransform]);
	transObj.m_position = glm::vec3(0.0f, -10.0f, 0.0f);
	transObj.m_rotation = glm::quat(glm::vec3(glm::radians(-90.0f), glm::radians(180.0f), glm::radians(0.0f)));
	auto& meshRenderer = LIW_ECS_GetComponent(LIWComponent_MeshRenderer, m_meshRenderers[idxMeshRenderer]);
	meshRenderer.m_handleMaterial = m_material;
	meshRenderer.m_handleMesh = m_mesh;

	idxEntity++; idxTransform++; idxSceneNode++; idxMeshRenderer++;


	// Light
	LIW_ECS_AttachComponentToEntity(LIWComponent_Transform, m_transforms[idxTransform], m_entities[idxEntity]);
	LIW_ECS_AttachComponentToEntity(LIWComponent_SceneNode, m_sceneNodes[idxSceneNode], m_entities[idxEntity]);
	LIW_ECS_AttachComponentToEntity(LIWComponent_Light, m_light, m_entities[idxEntity]);
	auto& transLight = LIW_ECS_GetComponent(LIWComponent_Transform, m_transforms[idxTransform]);
	transLight.m_position = glm::vec3(0.0f, 3.0f, 1.0f);
	transLight.m_rotation = glm::quat(glm::vec3(glm::radians(-45.0f), glm::radians(0.0f), glm::radians(0.0f)));
	auto& light0 = LIW_ECS_GetComponent(LIWComponent_Light, m_light);
	
	light0.SetDirectional();
	light0.m_colourAndIntensity = glm::vec4(0.7, 0.7, 0.9, 3.0f);

	//light0.SetPoint(LIWPointLightParameters{ 5.0f });
	//light0.m_colourAndIntensity = glm::vec4(0.7, 0.7, 0.9, 3000.0f);

	//light0.SetSpot(LIWSpotLightParameters{ 20.0f, 60.0f });
	//light0.m_colourAndIntensity = glm::vec4(0.7, 0.7, 0.9, 3000.0f);

	idxEntity++; idxTransform++; idxSceneNode++; 


	for (size_t i = 0; i < countSpheres; i++) {
		// Object1: Sphere
		LIW_ECS_AttachComponentToEntity(LIWComponent_Transform, m_transforms[idxTransform], m_entities[idxEntity]);
		LIW_ECS_AttachComponentToEntity(LIWComponent_SceneNode, m_sceneNodes[idxSceneNode], m_entities[idxEntity]);
		LIW_ECS_AttachComponentToEntity(LIWComponent_MeshRenderer, m_meshRenderers[idxMeshRenderer], m_entities[idxEntity]);
		auto& transObj1 = LIW_ECS_GetComponent(LIWComponent_Transform, m_transforms[idxTransform]);
		transObj1.m_position = glm::vec3(-3.0f + (i/3) % 3 * 3.0f, 0.0f, -3.0f + i % 3 * 3.0f);
		transObj1.m_rotation = glm::identity<glm::quat>();//glm::quat(glm::vec3(glm::radians(-90.0f), glm::radians(180.0f), glm::radians(0.0f)));
		auto& meshRenderer1 = LIW_ECS_GetComponent(LIWComponent_MeshRenderer, m_meshRenderers[idxMeshRenderer]);
		meshRenderer1.m_handleMaterial = m_material1;
		liw_objhdl_type hdlMeshSphere = assetManager.GetMeshHandle(LIW_MESH_SPHERE_NAME);
		meshRenderer1.m_handleMesh = hdlMeshSphere;

		idxEntity++; idxTransform++; idxSceneNode++; idxMeshRenderer++;
	}
	

	// Apply changes
	LIW_ECS_ApplyChangeOnComponentManager(LIWComponent_Transform);
	LIW_ECS_ApplyChangeOnComponentManager(LIWComponent_SceneNode);
	LIW_ECS_ApplyChangeOnComponentManager(LIWComponent_MeshRenderer);
	LIW_ECS_ApplyChangeOnComponentManager(LIWComponent_Camera);
	LIW_ECS_ApplyChangeOnComponentManager(LIWComponent_CameraController);
	LIW_ECS_ApplyChangeOnComponentManager(LIWComponent_Light);

	return 0;
}

int TestGame::CleanUp()
{
	auto& assetManager = *LIWGlobal::GetAssetManager();
	
	assetManager.DestroyMaterial("material0");
	assetManager.DestroyMaterial("material1");

	auto& shaderProgram = assetManager.GetShaderProgram(m_shaderProgram);
	shaderProgram.DestroyShader();
	assetManager.DestroyShaderProgram("shaderProgram0");
	auto& shaderProgramLight = assetManager.GetShaderProgram(m_shaderProgramLight);
	shaderProgramLight.DestroyShader();
	assetManager.DestroyShaderProgram("shaderProgram1");

	auto& mesh = assetManager.GetMesh(m_mesh);
	mesh.DestroyMesh();
	assetManager.DestroyMesh("mesh0");

	auto& tex = assetManager.GetTexture2D(m_tex2D);
	tex.DestroyTexture();
	assetManager.DestroyTexture2D("tex0");
	auto& tex1 = assetManager.GetTexture2D(m_tex2D1);
	tex1.DestroyTexture();
	assetManager.DestroyTexture2D("tex1");

	liw_delete(TestGlobal::s_renderer);
	return 0;
}

#include "LIWCore.h"
#include "TestFiberTaskIds.h"
#include "TestSystem0.h"

void FT_TestGameUpdate::Execute(LIWFiberWorkerPointer thisFiber)
{
	using namespace LIW;

	//
	// System updates
	//

	// Update LIWCameraControl
	LIWFiberExecutor::m_executor.IncreaseSyncCounter(LIW_SYNC_COUNTER_RESERVE_SYSTEM_UPDATE, 1);
	auto taskUpdateCamCtrl = liw_new_def<LIW_FT_LIWSystem_CameraControl_Update>();
	LIWFiberExecutor::m_executor.Submit(taskUpdateCamCtrl);
	LIWFiberExecutor::m_executor.WaitOnSyncCounter(LIW_SYNC_COUNTER_RESERVE_SYSTEM_UPDATE, thisFiber);

	// Update TestSystem0
	LIWFiberExecutor::m_executor.IncreaseSyncCounter(LIW_SYNC_COUNTER_RESERVE_SYSTEM_UPDATE, 1);
	auto taskUpdateTestSys0 = liw_new_def<FT_TestSystem0Update>();
	taskUpdateTestSys0->m_ptrFrameData = m_ptrFrameData;
	LIWFiberExecutor::m_executor.Submit(taskUpdateTestSys0);
	LIWFiberExecutor::m_executor.WaitOnSyncCounter(LIW_SYNC_COUNTER_RESERVE_SYSTEM_UPDATE, thisFiber);


	//
	// Render
	//
	LIWFiberExecutor::m_executor.IncreaseSyncCounter(LIW_SYNC_COUNTER_RESERVE_RENDER, 1);
	auto ptrTT_OGLForwardRender = new LIW_TT_OGLForwardRender();
	ptrTT_OGLForwardRender->m_renderer = TestGlobal::s_renderer;
	LIWMainThreadExecutor::m_executor.Submit(ptrTT_OGLForwardRender);
	LIWFiberExecutor::m_executor.WaitOnSyncCounter(LIW_SYNC_COUNTER_RESERVE_RENDER, thisFiber);


	//
	// Component update
	//
	LIW_ECS_ApplyChangeOnComponentManager(LIWComponent_Transform);
	LIW_ECS_ApplyChangeOnComponentManager(LIWComponent_SceneNode);
	LIW_ECS_ApplyChangeOnComponentManager(LIWComponent_MeshRenderer);
	LIW_ECS_ApplyChangeOnComponentManager(LIWComponent_Camera);
	LIW_ECS_ApplyChangeOnComponentManager(LIWComponent_CameraController);
	LIW_ECS_ApplyChangeOnComponentManager(TestComponent0);

	
#ifdef LIW_ENABLE_EDITOR
	// Kick off editor
	auto ptrFT_EdtrUIDrawBeg = liw_new_def<Editor::LIW_FT_EDTR_UIDrawBeg>();
	ptrFT_EdtrUIDrawBeg->ptrFrameData = m_ptrFrameData;
	LIWFiberExecutor::m_executor.Submit(ptrFT_EdtrUIDrawBeg);
#else
	// Straight to end of frame
	auto ptrFT_FrameEnd = liw_new_def<LIW_FT_FrameEnd>();
	ptrFT_FrameEnd->ptrFrameData = m_ptrFrameData;
	LIWFiberExecutor::m_executor.Submit(ptrFT_FrameEnd);
#endif //LIW_ENABLE_EDITOR
}
