#include "TestGame1.h"

#ifdef LIW_ENABLE_EDITOR
#include "Editor/LIWEditorTasks.h"
#endif //LIW_ENABLE_EDITOR

#include "Framework/LIWECSFunctional.h"

int TestGame1::Initialise()
{
	//
	// Renderer creation
	//
	TestGlobal::s_rendererDeferred = liw_new_static<OGLRenderer_Deferred>(*(m_currentEnvironment->m_window));

	
	//
	// Asset creation
	//
	auto& assetManager = *LIWGlobal::GetAssetManager();

	// Texture
	m_image = assetManager.CreateImage("image0");
	auto& image = *m_image;

	image.LoadImg(LIW_PATH_DIR_RESOURCE_TEXTURES"viking_room.png", LIWImageFormat_RGBA8);
	m_tex2D = assetManager.CreateTexture2D("tex0");
	auto& tex2D = *m_tex2D;
	tex2D.CreateTexture(image);
	image.UnloadImg();

	image.LoadImg(LIW_PATH_DIR_RESOURCE_TEXTURES"chessboard512.jpg", LIWImageFormat_RGB8);
	//image.LoadImg(LIW_PATH_DIR_RESOURCE_TEXTURES"test_img0.png", LIWImageFormat_RGB8);
	m_tex2D1 = assetManager.CreateTexture2D("tex1");
	auto& tex2D1 = *m_tex2D1;
	tex2D1.CreateTexture(image);
	image.UnloadImg();

	image.LoadImg(LIW_PATH_DIR_RESOURCE_TEXTURES"test_normal.jpg", LIWImageFormat_RGB8);
	m_tex2D2 = assetManager.CreateTexture2D("tex2");
	auto& tex2D2 = *m_tex2D2;
	tex2D2.CreateTexture(image);
	image.UnloadImg();

	assetManager.DestroyImage("image0");

	// Shader
	m_shader_vert = assetManager.CreateShader("shaderVert0");
	auto& shaderVert = *m_shader_vert;
	m_shader_frag = assetManager.CreateShader("shaderFrag0");
	auto& shaderFrag = *m_shader_frag;
	
	shaderVert.LoadShader(LIW_PATH_DIR_DEFAULT_SHADERS"OGL/Test_Vert.glsl", LIWShaderType_Vertex);
	shaderFrag.LoadShader(LIW_PATH_DIR_DEFAULT_SHADERS"OGL/Test_Frag.glsl", LIWShaderType_Fragment);
	m_shaderProgram = assetManager.CreateShaderProgram("shaderProgram0");
	auto& shaderProgram = *m_shaderProgram;
	shaderProgram.CreateShader({ shaderVert ,shaderFrag });
	shaderVert.UnloadShader();
	shaderFrag.UnloadShader();

	shaderVert.LoadShader(LIW_PATH_DIR_DEFAULT_SHADERS"OGL/Test_1_Vert.glsl", LIWShaderType_Vertex);
	shaderFrag.LoadShader(LIW_PATH_DIR_DEFAULT_SHADERS"OGL/Test_1_Frag.glsl", LIWShaderType_Fragment);
	m_shaderProgramForward = assetManager.CreateShaderProgram("shaderProgramForward0");
	auto& shaderProgramForward = *m_shaderProgramForward;
	shaderProgramForward.CreateShader({ shaderVert ,shaderFrag });
	shaderFrag.UnloadShader();

	shaderFrag.LoadShader(LIW_PATH_DIR_DEFAULT_SHADERS"OGL/LIW_Deferred_GBuffer_Frag.glsl", LIWShaderType_Fragment);
	m_shaderProgramDeferred = assetManager.CreateShaderProgram("shaderProgramDeferred0");
	auto& shaderProgramDeferred = *m_shaderProgramDeferred;
	shaderProgramDeferred.CreateShader({ shaderVert ,shaderFrag });
	shaderFrag.UnloadShader();

	shaderVert.UnloadShader();

	assetManager.DestroyShader("shaderVert0");
	assetManager.DestroyShader("shaderFrag0");

	// Material
	m_materialTest = assetManager.CreateMaterial("materialTest");
	auto& materialTest = *m_materialTest;
	materialTest.AddParam_Tex2D("mainTex", m_tex2D);
	materialTest.m_handleShaderProgram = m_shaderProgram;

	m_materialForward = assetManager.CreateMaterial("materialForward");
	auto& materialForward = *m_materialForward;
	materialForward.AddParam_Tex2D("mainTex", m_tex2D1);
	materialForward.AddParam_Tex2D("normalTex", m_tex2D2);
	materialForward.m_handleShaderProgram = m_shaderProgramForward;

	m_materialDeferred = assetManager.CreateMaterial("materialDeferred");
	auto& materialDeferred = *m_materialDeferred;
	materialDeferred.AddParam_Tex2D("mainTex", m_tex2D1);
	materialDeferred.AddParam_Tex2D("normalTex", m_tex2D2);
	materialDeferred.m_handleShaderProgram = m_shaderProgramDeferred;

	//
	// Scene creation
	//

	const int countSpheres = 9;
	const int countLights = 1024;
	const int countObjs = countSpheres + countLights + 2;
	const int countRenderObjs = countSpheres + 1;

	LIW_ECS_FetchEntities(m_entities, countObjs);
	LIW_ECS_CreateComponents(LIWComponent_Transform, m_transforms, countObjs);
	LIW_ECS_CreateComponents(LIWComponent_SceneNode, m_sceneNodes, countObjs);
	LIW_ECS_CreateComponents(LIWComponent_MeshRenderer, m_meshRenderers, countRenderObjs);
	m_camera = LIW_ECS_CreateComponent(LIWComponent_Camera);
	m_cameraController = LIW_ECS_CreateComponent(LIWComponent_CameraController);
	LIW_ECS_CreateComponents(LIWComponent_Light, m_lights, countLights);


	int idxEntity = 0;
	int idxTransform = 0;
	int idxSceneNode = 0;
	int idxMeshRenderer = 0;
	int idxLight = 0;

	// Camera
	LIW_ECS_AttachComponentToEntity(LIWComponent_Transform, m_transforms[idxTransform], m_entities[idxEntity]);
	LIW_ECS_AttachComponentToEntity(LIWComponent_SceneNode, m_sceneNodes[idxSceneNode], m_entities[idxEntity]);
	LIW_ECS_AttachComponentToEntity(LIWComponent_Camera, m_camera, m_entities[idxEntity]);
	LIW_ECS_AttachComponentToEntity(LIWComponent_CameraController, m_cameraController, m_entities[idxEntity]);
	auto& transCam = LIW_ECS_GetComponent(LIWComponent_Transform, m_transforms[idxTransform]);
	transCam.m_position = glm::vec3(-13.9f, 4.6f, 16.6f);
	transCam.m_rotation = Maths::EulerToQuatXZY(glm::radians(glm::vec3(-13.3f, -36.8f, 0.0f)));
	auto& cam = LIW_ECS_GetComponent(LIWComponent_Camera, m_camera);
	float w = (float)TestGlobal::s_rendererDeferred->GetWidth();
	float h = (float)TestGlobal::s_rendererDeferred->GetHeight();
	LIWCameraParam_Perspective camParam{ 30.0f, w / h };
	cam.SetPerspective(camParam);
	auto& camCtrl = LIW_ECS_GetComponent(LIWComponent_CameraController, m_cameraController);
	camCtrl.m_isSelected = true;
	TestGlobal::cam = m_transforms[idxTransform];

	idxEntity++; idxTransform++; idxSceneNode++;

	// Lights
	for (size_t i = 0; i < countLights; i++) {
		LIW_ECS_AttachComponentToEntity(LIWComponent_Transform, m_transforms[idxTransform], m_entities[idxEntity]);
		LIW_ECS_AttachComponentToEntity(LIWComponent_SceneNode, m_sceneNodes[idxSceneNode], m_entities[idxEntity]);
		LIW_ECS_AttachComponentToEntity(LIWComponent_Light, m_lights[idxLight], m_entities[idxEntity]);
		auto& transLight = LIW_ECS_GetComponent(LIWComponent_Transform, m_transforms[idxTransform]);
		glm::vec3 xyz = glm::linearRand(glm::vec3(-75.0f, 1.0f, -75.0f), glm::vec3(75.0f, 5.0f, 75.0f));
		transLight.m_position = xyz;
		transLight.m_rotation = Maths::EulerToQuatXZY(glm::radians(glm::vec3(-45.0f, 0.0f, 0.0f)));
		auto& light0 = LIW_ECS_GetComponent(LIWComponent_Light, m_lights[idxLight]);

		//light0.SetDirectional();
		//light0.m_colourAndIntensity = glm::vec4(0.7, 0.7, 0.9, 3.0f);

		light0.SetPoint(LIWPointLightParameters{ 10.0f });
		light0.m_colourAndIntensity = glm::vec4(0.7, 0.7, 0.9, 3000.0f);

		//light0.SetSpot(LIWSpotLightParameters{ 20.0f, 60.0f });
		//light0.m_colourAndIntensity = glm::vec4(0.7, 0.7, 0.9, 3000.0f);

		idxEntity++; idxTransform++; idxSceneNode++; idxLight++;
	}

	// Object1: Plane
	LIW_ECS_AttachComponentToEntity(LIWComponent_Transform, m_transforms[idxTransform], m_entities[idxEntity]);
	LIW_ECS_AttachComponentToEntity(LIWComponent_SceneNode, m_sceneNodes[idxSceneNode], m_entities[idxEntity]);
	LIW_ECS_AttachComponentToEntity(LIWComponent_MeshRenderer, m_meshRenderers[idxMeshRenderer], m_entities[idxEntity]);
	auto& transObj1 = LIW_ECS_GetComponent(LIWComponent_Transform, m_transforms[idxTransform]);
	transObj1.m_position = glm::vec3(0.0f, -3.0f, 0.0f);
	transObj1.m_rotation = Maths::EulerToQuatXZY(glm::radians(glm::vec3(0.0f, 0.0f, 0.0f)));
	transObj1.m_scale = glm::vec3(100.0f, 1.0f, 100.0f);
	auto& meshRenderer1 = LIW_ECS_GetComponent(LIWComponent_MeshRenderer, m_meshRenderers[idxMeshRenderer]);
	//meshRenderer1.m_handleMaterial = m_materialForward;
	meshRenderer1.m_handleMaterial = m_materialDeferred;
	meshRenderer1.m_handleMesh = assetManager.GetMesh(LIW_MESH_PLANE_NAME);

	idxEntity++; idxTransform++; idxSceneNode++; idxMeshRenderer++;

	// Objects: Sphere
	LIWPointer<LIWMesh, LIWMem_Default> ptrMeshSphere = assetManager.GetMesh(LIW_MESH_SPHERE_NAME);
	for (size_t i = 0; i < countSpheres; i++) {
		LIW_ECS_AttachComponentToEntity(LIWComponent_Transform, m_transforms[idxTransform], m_entities[idxEntity]);
		LIW_ECS_AttachComponentToEntity(LIWComponent_SceneNode, m_sceneNodes[idxSceneNode], m_entities[idxEntity]);
		LIW_ECS_AttachComponentToEntity(LIWComponent_MeshRenderer, m_meshRenderers[idxMeshRenderer], m_entities[idxEntity]);
		auto& transObj = LIW_ECS_GetComponent(LIWComponent_Transform, m_transforms[idxTransform]);
		transObj.m_position = glm::vec3(-3.0f + (i/3) % 3 * 3.0f, 0.0f, -3.0f + i % 3 * 3.0f);
		transObj.m_rotation = glm::identity<glm::quat>();//glm::quat(glm::vec3(glm::radians(-90.0f), glm::radians(180.0f), glm::radians(0.0f)));
		transObj.m_scale = glm::vec3(2.0f, 2.0f, 2.0f);
		auto& meshRenderer1 = LIW_ECS_GetComponent(LIWComponent_MeshRenderer, m_meshRenderers[idxMeshRenderer]);
		//meshRenderer1.m_handleMaterial = m_materialForward;
		meshRenderer1.m_handleMaterial = m_materialDeferred;
		meshRenderer1.m_handleMesh = ptrMeshSphere;

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

int TestGame1::CleanUp()
{
	auto& assetManager = *LIWGlobal::GetAssetManager();
	
	assetManager.DestroyMaterial("materialTest");
	assetManager.DestroyMaterial("materialForward");
	assetManager.DestroyMaterial("materialDeferred");

	auto& shaderProgram = *m_shaderProgram;
	shaderProgram.DestroyShader();
	assetManager.DestroyShaderProgram("shaderProgram0");
	auto& shaderProgramForward = *m_shaderProgramForward;
	shaderProgramForward.DestroyShader();
	assetManager.DestroyShaderProgram("shaderProgramForward0");
	auto& shaderProgramDeferred = *m_shaderProgramDeferred;
	shaderProgramDeferred.DestroyShader();
	assetManager.DestroyShaderProgram("shaderProgramDeferred0");

	//auto& mesh = assetManager.GetMesh(m_mesh);
	//mesh.DestroyMesh();
	//assetManager.DestroyMesh("mesh0");

	auto& tex = *m_tex2D;
	tex.DestroyTexture();
	assetManager.DestroyTexture2D("tex0");
	auto& tex1 = *m_tex2D1;
	tex1.DestroyTexture();
	assetManager.DestroyTexture2D("tex1");
	auto& tex2 = *m_tex2D2;
	tex2.DestroyTexture();
	assetManager.DestroyTexture2D("tex2");

	liw_delete(TestGlobal::s_rendererDeferred);
	return 0;
}

#include "LIWCore.h"
#include "TestFiberTaskIds.h"
#include "TestSystem0.h"

int TestGame1::Update(LIWPointer<LIWFrameData, LIWMem_Frame> frameData, LIWFiberWorkerPointer thisFiber)
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
	taskUpdateTestSys0->m_ptrFrameData = frameData;
	LIWFiberExecutor::m_executor.Submit(taskUpdateTestSys0);
	LIWFiberExecutor::m_executor.WaitOnSyncCounter(LIW_SYNC_COUNTER_RESERVE_SYSTEM_UPDATE, thisFiber);


	//
	// Render
	//
	LIWFiberExecutor::m_executor.IncreaseSyncCounter(LIW_SYNC_COUNTER_RESERVE_RENDER, 1);
	auto ptrTT_OGLDeferredRender = new LIW_TT_OGLDeferredRender();
	ptrTT_OGLDeferredRender->m_renderer = TestGlobal::s_rendererDeferred;
	LIWMainThreadExecutor::m_executor.Submit(ptrTT_OGLDeferredRender);
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

	return 0;
}
