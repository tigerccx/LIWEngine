#include "TestGame2.h"

#ifdef LIW_ENABLE_EDITOR
#include "Editor/LIWEditorTasks.h"
#endif //LIW_ENABLE_EDITOR

#include <string>
#include "Framework/LIWECSFunctional.h"

int TestGame2::Initialise()
{
	//
	// Renderer creation
	//
	TestGlobal::s_rendererVisibility = liw_new_static<OGLRenderer_DeferredVisibility>(*(m_currentEnvironment->m_window));

	
	//
	// Asset creation
	//
	auto& assetManager = *LIWGlobal::GetAssetManager();

	// Texture
	m_image = assetManager.CreateImage("image0");
	auto& image = assetManager.GetImage(m_image);

	image.LoadImg(LIW_PATH_DIR_RESOURCE_TEXTURES"viking_room.png", LIWImageFormat_RGBA8);
	m_tex2D = assetManager.CreateTexture2D("tex0");
	auto& tex2D = assetManager.GetTexture2D(m_tex2D);
	tex2D.CreateTexture(image);
	image.UnloadImg();

	image.LoadImg(LIW_PATH_DIR_RESOURCE_TEXTURES"chessboard512.jpg", LIWImageFormat_RGB8);
	//image.LoadImg(LIW_PATH_DIR_RESOURCE_TEXTURES"test_img0.png", LIWImageFormat_RGB8);
	m_tex2D1 = assetManager.CreateTexture2D("tex1");
	auto& tex2D1 = assetManager.GetTexture2D(m_tex2D1);
	tex2D1.CreateTexture(image);
	image.UnloadImg();

	image.LoadImg(LIW_PATH_DIR_RESOURCE_TEXTURES"test_normal.jpg", LIWImageFormat_RGB8);
	m_tex2D2 = assetManager.CreateTexture2D("tex2");
	auto& tex2D2 = assetManager.GetTexture2D(m_tex2D2);
	tex2D2.CreateTexture(image);
	image.UnloadImg();

	for (int i = 0; i < TEST_TEXTURE_COUNT; i++) {
		std::string file = "Test/test_" + std::to_string(i)+".jpg";
		std::string name = "texTest_" + std::to_string(i);
		image.LoadImg((LIW_PATH_DIR_RESOURCE_TEXTURES+file).c_str(), LIWImageFormat_RGB8);
		m_tex2DTests[i] = assetManager.CreateTexture2D(name.c_str());
		auto& tex2D = assetManager.GetTexture2D(m_tex2DTests[i]);
		tex2D.CreateTexture(image);
		image.UnloadImg();
	}

	assetManager.DestroyImage("image0");

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

	assetManager.DestroyShader("shaderVert0");
	assetManager.DestroyShader("shaderFrag0");

	m_shader_standardMaterial = assetManager.CreateShader("shaderStandardMaterial");
	auto& shaderStandard = assetManager.GetShader(m_shader_standardMaterial);
	shaderStandard.LoadShader(LIW_PATH_DIR_DEFAULT_SHADERS"OGL/LIW_VisibilityDeferred_Shading_Comp.glsl", LIWShaderType_Compute);
	m_shaderProgramStandard = assetManager.CreateShaderProgram("shaderProgramStandard");
	auto& shaderProgramStandard = assetManager.GetShaderProgram(m_shaderProgramStandard);
	shaderProgramStandard.CreateShader({ shaderStandard });
	shaderStandard.UnloadShader();
	assetManager.DestroyShader("shaderStandardMaterial");



	// Material
	m_materialTest = assetManager.CreateMaterial("materialTest");
	auto& materialTest = assetManager.GetMaterial(m_materialTest);
	materialTest.AddParam_Tex2D("mainTex", m_tex2D);
	materialTest.m_handleShaderProgram = m_shaderProgram;

	m_materialStandard = assetManager.CreateMaterial("materialStandard");
	auto& materialStandard = assetManager.GetMaterial(m_materialStandard);
	materialStandard.AddParam_Tex2D("mainTex", m_tex2D1);
	materialStandard.AddParam_Tex2D("normalTex", m_tex2D2);
	materialStandard.m_handleShaderProgram = m_shaderProgramStandard;

	//TestGlobal::s_rendererVisibility->AddRenderMaterial(m_materialStandard);

	for (int i = 0; i < TEST_TEXTURE_COUNT; i++) {
		std::string name = "materialTest_" + std::to_string(i);
		m_materialTests[i] = assetManager.CreateMaterial(name.c_str());
		auto& materialTest = assetManager.GetMaterial(m_materialTests[i]);
		materialTest.AddParam_Tex2D("mainTex", m_tex2DTests[i]);
		materialTest.AddParam_Tex2D("normalTex", m_tex2D2);
		materialTest.m_handleShaderProgram = m_shaderProgramStandard;
		TestGlobal::s_rendererVisibility->AddRenderMaterial(m_materialTests[i]);
	}

	//
	// Scene creation
	//

	const int countSpheres = 1024;
	const int countLights = 256;
	const int countObjs = countSpheres + countLights + 1;
	const int countRenderObjs = countSpheres + 0;

	LIW_ECS_FetchEntities(m_entities, countObjs);
	LIW_ECS_CreateComponents(LIWComponent_Transform, m_transforms, countObjs);
	LIW_ECS_CreateComponents(LIWComponent_SceneNode, m_sceneNodes, countObjs);
	LIW_ECS_CreateComponents(LIWComponent_MeshRendererBatched, m_meshRendererBatcheds, countRenderObjs);
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
	//transCam.m_position = glm::vec3(17.7f, 13.9f, 91.9f);
	//transCam.m_rotation = Maths::EulerToQuatXZY(glm::radians(glm::vec3(-9.8f, 11.0f, 0.0f)));
	transCam.m_position = glm::vec3(-9.6f, 4.3f, 41.3f);
	transCam.m_rotation = Maths::EulerToQuatXZY(glm::radians(glm::vec3(-11.2f, -9.2f, 0.0f)));
	auto& cam = LIW_ECS_GetComponent(LIWComponent_Camera, m_camera);
	float w = (float)TestGlobal::s_rendererVisibility->GetWidth();
	float h = (float)TestGlobal::s_rendererVisibility->GetHeight();
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
		//glm::vec3 xyz = glm::linearRand(glm::vec3(-75.0f, 1.0f, -75.0f), glm::vec3(75.0f, 5.0f, 75.0f));
		glm::vec3 xyz = glm::linearRand(glm::vec3(-50.0f, -50.0f, -50.0f), glm::vec3(50.0f, 50.0f, 50.0f));
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

	//// Object1: Plane
	//LIW_ECS_AttachComponentToEntity(LIWComponent_Transform, m_transforms[idxTransform], m_entities[idxEntity]);
	//LIW_ECS_AttachComponentToEntity(LIWComponent_SceneNode, m_sceneNodes[idxSceneNode], m_entities[idxEntity]);
	//LIW_ECS_AttachComponentToEntity(LIWComponent_MeshRendererBatched, m_meshRendererBatcheds[idxMeshRenderer], m_entities[idxEntity]);
	//auto& transObj1 = LIW_ECS_GetComponent(LIWComponent_Transform, m_transforms[idxTransform]);
	//transObj1.m_position = glm::vec3(0.0f, -3.0f, 0.0f);
	//transObj1.m_rotation = Maths::EulerToQuatXZY(glm::radians(glm::vec3(0.0f, 0.0f, 0.0f)));
	//transObj1.m_scale = glm::vec3(100.0f, 1.0f, 100.0f);
	//auto& meshRenderer1 = LIW_ECS_GetComponent(LIWComponent_MeshRendererBatched, m_meshRendererBatcheds[idxMeshRenderer]);
	////meshRenderer1.m_handleMaterial = m_materialForward;
	//meshRenderer1.m_handleMaterial = m_materialStandard;
	//meshRenderer1.m_handleMeshData = assetManager.GetMeshDataHandle(LIW_MESHDATA_PLANE_NAME);

	//idxEntity++; idxTransform++; idxSceneNode++; idxMeshRenderer++;

	// Objects: Sphere
	liw_objhdl_type hdlMeshSphere = assetManager.GetMeshDataHandle(LIW_MESHDATA_SPHERE_NAME);
	for (size_t i = 0; i < countSpheres; i++) {
		LIW_ECS_AttachComponentToEntity(LIWComponent_Transform, m_transforms[idxTransform], m_entities[idxEntity]);
		LIW_ECS_AttachComponentToEntity(LIWComponent_SceneNode, m_sceneNodes[idxSceneNode], m_entities[idxEntity]);
		LIW_ECS_AttachComponentToEntity(LIWComponent_MeshRendererBatched, m_meshRendererBatcheds[idxMeshRenderer], m_entities[idxEntity]);
		auto& transObj = LIW_ECS_GetComponent(LIWComponent_Transform, m_transforms[idxTransform]);
		glm::vec3 xyz = glm::linearRand(glm::vec3(-25.0f, -25.0f, -25.0f), glm::vec3(25.0f, 25.0f, 25.0f));
		transObj.m_position = xyz;// glm::vec3(-3.0f + (i / 3) % 3 * 3.0f, 0.0f, -3.0f + i % 3 * 3.0f);
		transObj.m_rotation = glm::identity<glm::quat>();//glm::quat(glm::vec3(glm::radians(-90.0f), glm::radians(180.0f), glm::radians(0.0f)));
		transObj.m_scale = glm::vec3(2.0f, 2.0f, 2.0f);
		auto& meshRenderer1 = LIW_ECS_GetComponent(LIWComponent_MeshRendererBatched, m_meshRendererBatcheds[idxMeshRenderer]);
		//meshRenderer1.m_handleMaterial = m_materialForward;
		meshRenderer1.m_handleMaterial = m_materialTests[i%TEST_TEXTURE_COUNT];
		meshRenderer1.m_handleMeshData = hdlMeshSphere;

		idxEntity++; idxTransform++; idxSceneNode++; idxMeshRenderer++;
	}
	

	// Apply changes
	LIW_ECS_ApplyChangeOnComponentManager(LIWComponent_Transform);
	LIW_ECS_ApplyChangeOnComponentManager(LIWComponent_SceneNode);
	LIW_ECS_ApplyChangeOnComponentManager(LIWComponent_MeshRenderer);
	LIW_ECS_ApplyChangeOnComponentManager(LIWComponent_MeshRendererBatched);
	LIW_ECS_ApplyChangeOnComponentManager(LIWComponent_Camera);
	LIW_ECS_ApplyChangeOnComponentManager(LIWComponent_CameraController);
	LIW_ECS_ApplyChangeOnComponentManager(LIWComponent_Light);

	return 0;
}

int TestGame2::CleanUp()
{
	auto& assetManager = *LIWGlobal::GetAssetManager();
	
	assetManager.DestroyMaterial("materialTest");
	assetManager.DestroyMaterial("materialStandard");
	for (int i = 0; i < TEST_TEXTURE_COUNT; i++) {
		std::string name = "materialTest_" + std::to_string(i);
		assetManager.DestroyMaterial(name.c_str());
	}

	auto& shaderProgram = assetManager.GetShaderProgram(m_shaderProgram);
	shaderProgram.DestroyShader();
	assetManager.DestroyShaderProgram("shaderProgram0");
	auto& shaderProgramStandard = assetManager.GetShaderProgram(m_shaderProgramStandard);
	shaderProgramStandard.DestroyShader();
	assetManager.DestroyShaderProgram("shaderProgramStandard");

	//auto& mesh = assetManager.GetMesh(m_mesh);
	//mesh.DestroyMesh();
	//assetManager.DestroyMesh("mesh0");

	auto& tex = assetManager.GetTexture2D(m_tex2D);
	tex.DestroyTexture();
	assetManager.DestroyTexture2D("tex0");
	auto& tex1 = assetManager.GetTexture2D(m_tex2D1);
	tex1.DestroyTexture();
	assetManager.DestroyTexture2D("tex1");
	auto& tex2 = assetManager.GetTexture2D(m_tex2D2);
	tex2.DestroyTexture();
	assetManager.DestroyTexture2D("tex2");

	for (int i = 0; i < TEST_TEXTURE_COUNT; i++) {
		std::string name = "texTest_" + std::to_string(i);
		auto& tex2 = assetManager.GetTexture2D(m_tex2DTests[i]);
		tex2.DestroyTexture();
		assetManager.DestroyTexture2D(name.c_str());
	}

	liw_delete(TestGlobal::s_rendererVisibility);
	return 0;
}

#include "LIWCore.h"
#include "TestFiberTaskIds.h"
#include "TestSystem0.h"

int TestGame2::Update(LIWPointer<LIWFrameData, LIWMem_Frame> frameData, LIWFiberWorkerPointer thisFiber)
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
	auto ptrTT_OGLDeferredVisibilityRender = new LIW_TT_OGLDeferredVisibilityRender();
	ptrTT_OGLDeferredVisibilityRender->m_renderer = TestGlobal::s_rendererVisibility;
	LIWMainThreadExecutor::m_executor.Submit(ptrTT_OGLDeferredVisibilityRender);
	LIWFiberExecutor::m_executor.WaitOnSyncCounter(LIW_SYNC_COUNTER_RESERVE_RENDER, thisFiber);


	//
	// Component update
	//
	LIW_ECS_ApplyChangeOnComponentManager(LIWComponent_Transform);
	LIW_ECS_ApplyChangeOnComponentManager(LIWComponent_SceneNode);
	LIW_ECS_ApplyChangeOnComponentManager(LIWComponent_MeshRenderer);
	LIW_ECS_ApplyChangeOnComponentManager(LIWComponent_MeshRendererBatched);
	LIW_ECS_ApplyChangeOnComponentManager(LIWComponent_Camera);
	LIW_ECS_ApplyChangeOnComponentManager(LIWComponent_CameraController);
	LIW_ECS_ApplyChangeOnComponentManager(TestComponent0);

	return 0;
}
