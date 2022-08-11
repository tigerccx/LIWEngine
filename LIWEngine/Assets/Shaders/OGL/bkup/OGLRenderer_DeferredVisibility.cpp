#include "OGLRenderer_DeferredVisibility.h"
namespace LIW {
	OGLRenderer_DeferredVisibility::OGLRenderer_DeferredVisibility(LIW::App::Window& parent):
		OGLRenderer(parent)
	{
		glGenBuffers(1, &m_uboCameraData);
		glGenBuffers(1, &m_uboPointLightPerPixelData);
		glGenBuffers(1, &m_ssboTileMaterialList);
		glGenBuffers(1, &m_atomicTileMaterialListCount);
		glGenVertexArrays(1, &m_emptyVAO);

		// Init SSBO
		const size_t sizeData = sizeof(uint32_t) * 2 * LIW_DEFERRED_VISIBILITY_TILE_MATERIAL_LIST_CAPACITY;
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_TILE_MATERIAL_LIST, m_ssboTileMaterialList);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeData, nullptr, GL_STATIC_DRAW);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		// Init Atomic
		uint32_t counter = 0;
		glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, LIW_SHADER_ATOMIC_BIND_DEFERRED_VISIBILITY_TILE_MATERIAL_LIST_COUNT, m_atomicTileMaterialListCount);
		glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(uint32_t), &counter, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);


		auto& assetManager = *LIWGlobal::GetAssetManager();

		liw_objhdl_type hdlLightBufferVertShader = assetManager.CreateShader("shader_vert_lightBuffer");
		LIWShader& lightBufferVertShader = assetManager.GetShader(hdlLightBufferVertShader);
		lightBufferVertShader.LoadShader(LIW_PATH_DIR_DEFAULT_SHADERS"OGL/LIW_Deferred_LightBuffer_Vert.glsl", LIWShaderType_Vertex);

		liw_objhdl_type hdlLightBufferFragShader = assetManager.CreateShader("shader_frag_lightBuffer");
		LIWShader& lightBufferFragShader = assetManager.GetShader(hdlLightBufferFragShader);
		lightBufferFragShader.LoadShader(LIW_PATH_DIR_DEFAULT_SHADERS"OGL/LIW_Deferred_LightBuffer_Frag.glsl", LIWShaderType_Fragment);
	
		liw_objhdl_type hdlScreenQuadShader = assetManager.CreateShader("shader_vert_screenQuad");
		LIWShader& screenQuadShader = assetManager.GetShader(hdlScreenQuadShader);
		screenQuadShader.LoadShader(LIW_PATH_DIR_DEFAULT_SHADERS"OGL/LIW_ScreenQuad_Vert.glsl", LIWShaderType_Vertex);

		liw_objhdl_type hdlCombineShader = assetManager.CreateShader("shader_frag_combine");
		LIWShader& combineShader = assetManager.GetShader(hdlCombineShader);
		combineShader.LoadShader(LIW_PATH_DIR_DEFAULT_SHADERS"OGL/LIW_Deferred_Combine_Frag.glsl", LIWShaderType_Fragment);

		liw_objhdl_type hdlVisibilityVertShader = assetManager.CreateShader("shader_vert_visibility");
		LIWShader& visibilityVertShader = assetManager.GetShader(hdlVisibilityVertShader);
		visibilityVertShader.LoadShader(LIW_PATH_DIR_DEFAULT_SHADERS"OGL/LIW_VisibilityDeferred_Visibility_Vert.glsl", LIWShaderType_Vertex);

		liw_objhdl_type hdlVisibilityFragShader = assetManager.CreateShader("shader_frag_visibility");
		LIWShader& visibilityFragShader = assetManager.GetShader(hdlVisibilityFragShader);
		visibilityFragShader.LoadShader(LIW_PATH_DIR_DEFAULT_SHADERS"OGL/LIW_VisibilityDeferred_Visibility_Frag.glsl", LIWShaderType_Fragment);

		liw_objhdl_type hdlVisibilityShowFragShader = assetManager.CreateShader("shader_frag_visibilityShow");
		LIWShader& visibilityShowFragShader = assetManager.GetShader(hdlVisibilityShowFragShader);
		visibilityShowFragShader.LoadShader(LIW_PATH_DIR_DEFAULT_SHADERS"OGL/LIW_VisibilityDeferred_VisibilityShow_Frag.glsl", LIWShaderType_Fragment);

		liw_objhdl_type hdlWorklistGenerateP0ComputeShader = assetManager.CreateShader("shader_comp_worklistGenerate_p0");
		LIWShader& worklistGenerateP0ComputeShader = assetManager.GetShader(hdlWorklistGenerateP0ComputeShader);
		worklistGenerateP0ComputeShader.LoadShader(LIW_PATH_DIR_DEFAULT_SHADERS"OGL/LIW_VisibilityDeferred_Worklist_P0_TileMaterialList_Comp.glsl", LIWShaderType_Compute);
		liw_objhdl_type hdlWorklistGenerateP1ComputeShader = assetManager.CreateShader("shader_comp_worklistGenerate_p1");
		LIWShader& worklistGenerateP1ComputeShader = assetManager.GetShader(hdlWorklistGenerateP1ComputeShader);
		worklistGenerateP1ComputeShader.LoadShader(LIW_PATH_DIR_DEFAULT_SHADERS"OGL/LIW_VisibilityDeferred_Worklist_P1_Sort_Comp.glsl", LIWShaderType_Compute);


		m_lightBufferShaderProgram = assetManager.CreateShaderProgram("shaderProgram_lightBuffer");
		LIWShaderProgram& lightBufferShaderProgram = assetManager.GetShaderProgram(m_lightBufferShaderProgram);
		lightBufferShaderProgram.CreateShader({ lightBufferVertShader , lightBufferFragShader });

		m_combineShaderProgram = assetManager.CreateShaderProgram("shaderProgram_combine");
		LIWShaderProgram& combineShaderProgram = assetManager.GetShaderProgram(m_combineShaderProgram);
		combineShaderProgram.CreateShader({ screenQuadShader , combineShader });
		
		m_visibilityShaderProgram = assetManager.CreateShaderProgram("shaderProgram_visibility");
		LIWShaderProgram& visibilityShaderProgram = assetManager.GetShaderProgram(m_visibilityShaderProgram);
		visibilityShaderProgram.CreateShader({ visibilityVertShader , visibilityFragShader });

		m_visibilityShowShaderProgram = assetManager.CreateShaderProgram("shaderProgram_visibilityShow");
		LIWShaderProgram& visibilityShowShaderProgram = assetManager.GetShaderProgram(m_visibilityShowShaderProgram);
		visibilityShowShaderProgram.CreateShader({ screenQuadShader , visibilityShowFragShader });

		m_worklistGenerateShaderProgram = assetManager.CreateShaderProgram("shaderProgram_worklistGenerate");
		LIWShaderProgram& worklistGenerateShaderProgram = assetManager.GetShaderProgram(m_worklistGenerateShaderProgram);
		worklistGenerateShaderProgram.CreateShader({ worklistGenerateP0ComputeShader });


		lightBufferVertShader.UnloadShader();
		lightBufferFragShader.UnloadShader();
		screenQuadShader.UnloadShader();
		combineShader.UnloadShader();
		visibilityVertShader.UnloadShader();
		visibilityFragShader.UnloadShader();
		visibilityShowFragShader.UnloadShader();
		worklistGenerateP0ComputeShader.UnloadShader();
		worklistGenerateP1ComputeShader.UnloadShader();
		assetManager.DestroyShader("shader_vert_lightBuffer");
		assetManager.DestroyShader("shader_frag_lightBuffer");
		assetManager.DestroyShader("shader_vert_screenQuad");
		assetManager.DestroyShader("shader_frag_combine");
		assetManager.DestroyShader("shader_vert_visibility");
		assetManager.DestroyShader("shader_frag_visibility");
		assetManager.DestroyShader("shader_frag_visibilityShow");
		assetManager.DestroyShader("shader_comp_worklistGenerate_p0");
		assetManager.DestroyShader("shader_comp_worklistGenerate_p1");


		m_meshSphere = assetManager.GetMeshHandle(LIW_MESH_SPHERE_NAME);


		m_testTexture0 = assetManager.CreateTexture2D("texture_test0");
		auto& testTexture0 = assetManager.GetTexture2D(m_testTexture0);
		testTexture0.CreateTexture(width, height, LIWImageFormat_R32U,
			LIWTextureWrapType_Clamp, LIWTextureWrapType_Clamp,
			LIWTextureFilterType_Nearest, LIWTextureFilterType_Nearest, false);


		m_frameBufferGBuffer = assetManager.CreateFrameBuffer("framebuffer_gbuffer");
		auto& gBuffer = assetManager.GetFrameBuffer(m_frameBufferGBuffer);
		const liw_flag_type gBufferFlag =
			LIW_FRAMEBUFFER_ATTACHMENT_FLAG_COLOR_RGB |
			LIW_FRAMEBUFFER_ATTACHMENT_FLAG_COLOR_RGB_1 |
			LIW_FRAMEBUFFER_ATTACHMENT_FLAG_DEPTH;
		gBuffer.CreateFrameBuffer(width, height, gBufferFlag);

		m_frameBufferLightBuffer = assetManager.CreateFrameBuffer("framebuffer_lightbuffer");
		auto& lightBuffer = assetManager.GetFrameBuffer(m_frameBufferLightBuffer);
		const liw_flag_type lightBufferFlag =
			LIW_FRAMEBUFFER_ATTACHMENT_FLAG_COLOR_RGB |
			LIW_FRAMEBUFFER_ATTACHMENT_FLAG_COLOR_RGB_1;
		lightBuffer.CreateFrameBuffer(width, height, lightBufferFlag);

		m_frameBufferVBuffer = assetManager.CreateFrameBuffer("framebuffer_vbuffer");
		auto& vBuffer = assetManager.GetFrameBuffer(m_frameBufferVBuffer);
		const liw_flag_type vBufferFlag =
			LIW_FRAMEBUFFER_ATTACHMENT_FLAG_INDEX |
			LIW_FRAMEBUFFER_ATTACHMENT_FLAG_DEPTH;
		vBuffer.CreateFrameBuffer(width, height, vBufferFlag);
	}
	OGLRenderer_DeferredVisibility::~OGLRenderer_DeferredVisibility()
	{
		auto& assetManager = *LIWGlobal::GetAssetManager();

		LIWFrameBuffer& lightBuffer = assetManager.GetFrameBuffer(m_frameBufferLightBuffer);
		lightBuffer.DestroyFrameBuffer();
		assetManager.DestroyFrameBuffer("framebuffer_lightbuffer");

		LIWFrameBuffer& gBuffer = assetManager.GetFrameBuffer(m_frameBufferGBuffer);
		gBuffer.DestroyFrameBuffer();
		assetManager.DestroyFrameBuffer("framebuffer_gbuffer");

		LIWFrameBuffer& vBuffer = assetManager.GetFrameBuffer(m_frameBufferVBuffer);
		vBuffer.DestroyFrameBuffer();
		assetManager.DestroyFrameBuffer("framebuffer_vbuffer");


		LIWShaderProgram& combineShaderProgram = assetManager.GetShaderProgram(m_combineShaderProgram);
		combineShaderProgram.DestroyShader();
		assetManager.DestroyShaderProgram("shaderProgram_combine");

		LIWShaderProgram& lightBufferShaderProgram = assetManager.GetShaderProgram(m_lightBufferShaderProgram);
		lightBufferShaderProgram.DestroyShader();
		assetManager.DestroyShaderProgram("shaderProgram_lightBuffer");

		LIWShaderProgram& visibilityShaderProgram = assetManager.GetShaderProgram(m_visibilityShaderProgram);
		visibilityShaderProgram.DestroyShader();
		assetManager.DestroyShaderProgram("shaderProgram_visibility");

		LIWShaderProgram& visibilityShowShaderProgram = assetManager.GetShaderProgram(m_visibilityShowShaderProgram);
		visibilityShowShaderProgram.DestroyShader();
		assetManager.DestroyShaderProgram("shaderProgram_visibilityShow");

		LIWShaderProgram& worklistGenerateShaderProgram = assetManager.GetShaderProgram(m_worklistGenerateShaderProgram);
		worklistGenerateShaderProgram.DestroyShader();
		assetManager.DestroyShaderProgram("shaderProgram_worklistGenerate");


		auto& testTexture0 = assetManager.GetTexture2D(m_testTexture0);
		testTexture0.DestroyTexture();
		assetManager.DestroyTexture2D("texture_test0");


		glDeleteVertexArrays(1, &m_emptyVAO);
		glDeleteBuffers(1, &m_atomicTileMaterialListCount);
		glDeleteBuffers(1, &m_ssboTileMaterialList);
		glDeleteBuffers(1, &m_uboPointLightPerPixelData);
		glDeleteBuffers(1, &m_uboCameraData);
	}
	void OGLRenderer_DeferredVisibility::RenderScene()
	{
		// Render each camera
		auto& cameraManager = LIW_ECS_GetComponentManager(LIWComponent_Camera);
		auto& cameras = cameraManager.m_components;
		
		for (auto itr = cameras.get_beg(); itr != cameras.get_end(); itr++) {
			RenderCamera(*itr);
		}
	}
	void OGLRenderer_DeferredVisibility::RenderCamera(LIWComponent_Camera& camera)
	{
		//
		// Set render states
		//
		// Set Framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// Clear
		glClearColor(0.5f, 0.1f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//Enable z-test
		glEnable(GL_DEPTH_TEST);

		//Enable back culling
		glFrontFace(GL_CCW);
		glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);
		//glDisable(GL_CULL_FACE);

		//Enable blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		auto& assetManager = *LIWGlobal::GetAssetManager();

		//
		// Calculate and upload camera data
		//
		UploadCameraData(camera); 

		//
		// Visibility pass
		//
		VisibilityPass();

		WorklistPass();

		//
		// Debug: Visibility show pass
		//
		VisibilityDebugShowPass();

		

		////
		//// GBuffer pass
		////
		//GBufferPass();

		////
		//// LightBuffer pass
		////
		//LightBufferPass(camera);

		//// Combine pass
		//CombinePass();
	}
	void OGLRenderer_DeferredVisibility::VisibilityPass()
	{
		//
		// Setup
		//
		auto& assetManager = *LIWGlobal::GetAssetManager();
		auto& vBuffer = assetManager.GetFrameBuffer(m_frameBufferVBuffer);

		BindFrameBuffer(vBuffer);

		// Clear
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//Enable z-test
		glEnable(GL_DEPTH_TEST);

		//Enable back culling
		glFrontFace(GL_CCW);
		glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);

		//Disable blending (transparency not gonna be rendered in deferred mode anyway)
		glDisable(GL_BLEND);


		//
		// Render MeshRenderers to GBuffer
		//
		auto& shaderProgram = assetManager.GetShaderProgram(m_visibilityShaderProgram);
		uint32_t rawHandleshaderProgram = shaderProgram.GetRawHandle();

		// Use program
		glUseProgram(rawHandleshaderProgram);

		auto& meshRendererManager = LIW_ECS_GetComponentManager(LIWComponent_MeshRenderer);
		auto& meshRenderers = meshRendererManager.m_components;
		uint32_t idxObject = 0;
		for (auto itr = meshRenderers.get_beg(); itr != meshRenderers.get_end(); itr++) {
			auto& meshRenderer = *itr;
			auto entity = meshRenderer.GetEntity();
			liw_objhdl_type transformHandle = LIW_ECS_GetComponentFromEntity(LIWComponent_Transform, entity);
			auto& transform = LIW_ECS_GetComponent(LIWComponent_Transform, transformHandle);

			//auto& material = assetManager.GetMaterial(meshRenderer.m_handleMaterial);
			auto& mesh = assetManager.GetMesh(meshRenderer.m_handleMesh);

			//// Bind material data
			//material.BindData();

			// Upload uniforms
			glm::mat4 matModel = transform.GetMatrix();
			glUniformMatrix4fv(glGetUniformLocation(rawHandleshaderProgram, LIW_SHADER_MODEL_MATRIX_NAME), 1, false, glm::value_ptr(matModel));
			uint32_t idxObjectAligned = idxObject << 18;
			glUniform1ui(glGetUniformLocation(rawHandleshaderProgram, "idxObject"), idxObjectAligned);

			// Draw
			auto& submeshes = mesh.GetSubmeshes();
			auto mode = LIWImageFormat_2_GLPrimitive.at(mesh.GetPrimitiveType());
			glBindVertexArray(mesh.GetHandleVAO());
			for (int i = 0; i < submeshes.get_size(); ++i) {
				size_t size = submeshes[i].m_idxEnd - submeshes[i].m_idxBeg;
				const GLvoid* offset = (const GLvoid*)(submeshes[i].m_idxBeg * sizeof(unsigned int));
				glDrawElements(mode,
					(int)size,
					GL_UNSIGNED_INT,
					offset);
			}

			idxObject++;

			//// Unbind material data
			//material.UnbindData();
		}
	}
	void OGLRenderer_DeferredVisibility::VisibilityDebugShowPass()
	{
		//
		// Setup
		//
		auto& assetManager = *LIWGlobal::GetAssetManager();
		auto& vBuffer = assetManager.GetFrameBuffer(m_frameBufferVBuffer);
		LIWShaderProgram& visibilityShowShaderProgram = assetManager.GetShaderProgram(m_visibilityShowShaderProgram);

		BindDefaultFrameBuffer();

		// Clear
		glClearColor(0.5f, 0.1f, 0.2f, 1.0f);
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Disable z-test
		glDisable(GL_DEPTH_TEST);
		// Enable front culling
		glFrontFace(GL_CCW);
		glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);

		//Disable blending (transparency not gonna be rendered in deferred mode anyway)
		glDisable(GL_BLEND);

		//
		// Render
		//

		// Use Shader
		const uint32_t rawHdlVisibilityShowShaderProgram = visibilityShowShaderProgram.GetRawHandle();
		glUseProgram(rawHdlVisibilityShowShaderProgram);

		// Bind buffer textures
		// Idx texture
		//const auto rawHdl = vBuffer.GetColorAttachmentRawHandle(0);
		auto& testTexture0 = assetManager.GetTexture2D(m_testTexture0);
		const auto rawHdl = testTexture0.GetRawHandle();
		LIWTexture::Bind2DTexture(rawHdl,
			rawHdlVisibilityShowShaderProgram, "indexTexture", 0);

		glBindVertexArray(m_emptyVAO);
		glDrawArrays(GL_TRIANGLES, 0, 3); //NOTE: only works with compatibility OpenGL profile
	}
	void OGLRenderer_DeferredVisibility::WorklistPass()
	{
		//
		// Setup
		//
		auto& assetManager = *LIWGlobal::GetAssetManager();
		auto& vBuffer = assetManager.GetFrameBuffer(m_frameBufferVBuffer);
		LIWShaderProgram& worklistGenerateProgram = assetManager.GetShaderProgram(m_worklistGenerateShaderProgram);
		const uint32_t rawHdlShaderProgram = worklistGenerateProgram.GetRawHandle();
		auto& testTexture0 = assetManager.GetTexture2D(m_testTexture0);

		//
		// Dispatch
		//
		glUseProgram(rawHdlShaderProgram);

		const uint32_t countGroupX = width / LIW_DEFERRED_VISIBILITY_TILE_X;
		const uint32_t countGroupY = height / LIW_DEFERRED_VISIBILITY_TILE_Y;

		LIWTexture::Bind2DTextureImage(vBuffer.GetColorAttachmentRawHandle(0),
			rawHdlShaderProgram, "indexImg", 0, LIWImageFormat_R32U,
			LIWTextureImageAccessType_Read);
		LIWTexture::Bind2DTextureImage(testTexture0.GetRawHandle(),
			rawHdlShaderProgram, "testImgOut", 1, LIWImageFormat_R32U,
			LIWTextureImageAccessType_Write);


		//// Bind SSBO
		//const size_t sizeData = sizeof(uint32_t) * 2 * LIW_DEFERRED_VISIBILITY_TILE_MATERIAL_LIST_CAPACITY;

		//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_TILE_MATERIAL_LIST, m_ssboTileMaterialList);
		//glBufferData(GL_SHADER_STORAGE_BUFFER, sizeData, nullptr, GL_STATIC_DRAW);
		//glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		
		// Update Atomic
		uint32_t counter = 0;
		glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, LIW_SHADER_ATOMIC_BIND_DEFERRED_VISIBILITY_TILE_MATERIAL_LIST_COUNT, m_atomicTileMaterialListCount);
		glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(uint32_t), &counter);
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

		// Dispatch
		glDispatchCompute(countGroupX, countGroupY, 1);

		glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT| GL_SHADER_STORAGE_BARRIER_BIT); //Wait for update

		//// Readback atomic counters
		//const size_t sizeData = sizeof(uint32_t) * 2 * LIW_DEFERRED_VISIBILITY_TILE_MATERIAL_LIST_CAPACITY;
		//uint32_t* data = new uint32_t[2 * LIW_DEFERRED_VISIBILITY_TILE_MATERIAL_LIST_CAPACITY];
		//GLuint* userCounters;
		//glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssboTileMaterialList);
		//userCounters = (GLuint*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeData, GL_MAP_READ_BIT);
		//memcpy_s(data, sizeData, userCounters, sizeData);
		//glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		//glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		//uint32_t atomicCounter;
		//GLuint* userCounters1;
		//glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, m_atomicTileMaterialListCount);
		//userCounters1 = (GLuint*)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(uint32_t), GL_MAP_READ_BIT);
		//memcpy_s(&atomicCounter, sizeof(uint32_t), userCounters1, sizeof(uint32_t));
		//glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
		//glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

		//for (uint32_t i = 0; i < atomicCounter; i++) {
		//	uint32_t idxTile = 2 * i;
		//	uint32_t idxMaterial = 2 * i + 1;
		//	const uint32_t tile0 = data[idxTile];
		//	const uint32_t mat0 = data[idxMaterial];
		//	for (uint32_t j = i + 1; j < atomicCounter; j++) {
		//		uint32_t idxTile1 = 2 * j;
		//		uint32_t idxMaterial1 = 2 * j + 1;
		//		const uint32_t tile1 = data[idxTile1];
		//		const uint32_t mat1 = data[idxMaterial1];
		//		if (tile0 == tile1 && mat0 == mat1)
		//			throw "WHY";
		//	}
		//}


		//glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT); //Wait for update
	}
	void OGLRenderer_DeferredVisibility::GBufferPass()
	{
		//
		// Setup
		//
		auto& assetManager = *LIWGlobal::GetAssetManager();
		auto& gBuffer = assetManager.GetFrameBuffer(m_frameBufferGBuffer);

		BindFrameBuffer(gBuffer);
		
		// Clear
		glClearColor(0.5f, 0.1f, 0.2f, 1.0f);
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//Enable z-test
		glEnable(GL_DEPTH_TEST);

		//Enable back culling
		glFrontFace(GL_CCW);
		glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);

		//Disable blending (transparency not gonna be rendered in deferred mode anyway)
		glDisable(GL_BLEND);


		//
		// Render MeshRenderers to GBuffer
		//
		auto& meshRendererManager = LIW_ECS_GetComponentManager(LIWComponent_MeshRenderer);
		auto& meshRenderers = meshRendererManager.m_components;
		for (auto itr = meshRenderers.get_beg(); itr != meshRenderers.get_end(); itr++) {
			auto& meshRenderer = *itr;
			auto entity = meshRenderer.GetEntity();
			liw_objhdl_type transformHandle = LIW_ECS_GetComponentFromEntity(LIWComponent_Transform, entity);
			auto& transform = LIW_ECS_GetComponent(LIWComponent_Transform, transformHandle);

			auto& material = assetManager.GetMaterial(meshRenderer.m_handleMaterial);
			auto& mesh = assetManager.GetMesh(meshRenderer.m_handleMesh);

			auto& shaderProgram = assetManager.GetShaderProgram(material.m_handleShaderProgram);
			uint32_t rawHandleshaderProgram = shaderProgram.GetRawHandle();

			// Use program
			glUseProgram(rawHandleshaderProgram);

			// Bind material data
			material.BindData();

			// Upload matrices
			glm::mat4 matModel = transform.GetMatrix();
			glUniformMatrix4fv(glGetUniformLocation(rawHandleshaderProgram, LIW_SHADER_MODEL_MATRIX_NAME), 1, false, glm::value_ptr(matModel));
			uint32_t locMatNormal = glGetUniformLocation(rawHandleshaderProgram, LIW_SHADER_NORMAL_MATRIX_NAME);
			if (locMatNormal != GL_INVALID_VALUE) {
				glm::mat3 matNormal = glm::transpose(glm::inverse(glm::mat3(matModel)));
				glUniformMatrix3fv(locMatNormal, 1, false, glm::value_ptr(matNormal));
			}


			// Draw
			auto& submeshes = mesh.GetSubmeshes();
			auto mode = LIWImageFormat_2_GLPrimitive.at(mesh.GetPrimitiveType());
			glBindVertexArray(mesh.GetHandleVAO());
			for (int i = 0; i < submeshes.get_size(); ++i) {
				size_t size = submeshes[i].m_idxEnd - submeshes[i].m_idxBeg;
				const GLvoid* offset = (const GLvoid*)(submeshes[i].m_idxBeg * sizeof(unsigned int));
				glDrawElements(mode,
					(int)size,
					GL_UNSIGNED_INT,
					offset);
			}


			// Unbind material data
			material.UnbindData();
		}
	}
	void OGLRenderer_DeferredVisibility::LightBufferPass(LIWComponent_Camera& camera)
	{
		//
		// Setup
		//
		auto& assetManager = *LIWGlobal::GetAssetManager();
		auto& lightBuffer = assetManager.GetFrameBuffer(m_frameBufferLightBuffer);
		auto& gBuffer = assetManager.GetFrameBuffer(m_frameBufferGBuffer);
		auto& lightBufferShaderProgram = assetManager.GetShaderProgram(m_lightBufferShaderProgram);
		const uint32_t rawHandleLightBufferShaderProgram = lightBufferShaderProgram.GetRawHandle();

		BindFrameBuffer(lightBuffer);

		// Clear
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Disable z-test
		glDisable(GL_DEPTH_TEST);
		//glDepthFunc(GL_ALWAYS);
		//glDepthMask(GL_FALSE);
		// Enable front culling
		glFrontFace(GL_CCW);
		glCullFace(GL_FRONT);
		glEnable(GL_CULL_FACE);

		// Enable blending (accumulate light info)
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);

		// Use program
		glUseProgram(rawHandleLightBufferShaderProgram);

		// Bind Gbuffer Textures
		// Normal
		LIWTexture::Bind2DTexture(gBuffer.GetColorAttachmentRawHandle(1),
			rawHandleLightBufferShaderProgram, LIW_SHADER_DEFERRED_NORMAL_TEXURE_NAME, 0);
		// Depth
		LIWTexture::Bind2DTexture(gBuffer.GetDepthAttachmentRawHandle(),
			rawHandleLightBufferShaderProgram, LIW_SHADER_DEFERRED_DEPTH_TEXURE_NAME, 1);

		// Bind variables
		UploadLightPassData(rawHandleLightBufferShaderProgram, camera);
		uint32_t lightPosLocation = glGetUniformLocation(rawHandleLightBufferShaderProgram, LIW_SHADER_DEFERRED_LIGHT_POS_NAME);
		uint32_t lightColourLocation = glGetUniformLocation(rawHandleLightBufferShaderProgram, LIW_SHADER_DEFERRED_LIGHT_COLOUR_NAME);
		uint32_t lightParamLocation = glGetUniformLocation(rawHandleLightBufferShaderProgram, LIW_SHADER_DEFERRED_LIGHT_PARAM_NAME);

		//
		// Render light volumes to Lightbuffer
		//

		// Point light
		auto& meshSphere = assetManager.GetMesh(LIW_MESH_SPHERE_NAME);
		auto& submeshSphere = meshSphere.GetSubmeshes();
		const int drawMode = LIWImageFormat_2_GLPrimitive.at(meshSphere.GetPrimitiveType());

		auto& lightManager = LIW_ECS_GetComponentManager(LIWComponent_Light);
		auto& lights = lightManager.m_components;
		for (auto itr = lights.get_beg(); itr != lights.get_end(); itr++) {
			auto& light = *itr;
			auto entity = light.GetEntity();
			liw_objhdl_type transformHandle = LIW_ECS_GetComponentFromEntity(LIWComponent_Transform, entity);
			auto& transform = LIW_ECS_GetComponent(LIWComponent_Transform, transformHandle);

			// Upload light data
			glm::vec3 lightPos = transform.m_position;
			glm::vec4 lightColour = light.m_colourAndIntensity;
			glm::vec4 lightParam = glm::vec4(light.m_param.m_pointLight.m_radius, 0.0f, 0.0f, 0.0f);

			//switch (light.m_lightType)
			//{
			//case LIW::LIWLightType_Directional: {
			//	glm::vec3 forward = transform.GetForward();
			//	forward = glm::normalize(forward);
			//	lightParam = glm::vec4(forward, 0.0f);
			//}
			//	break;
			//case LIW::LIWLightType_Point: {
			//	lightParam = glm::vec4(light.m_param.m_pointLight.m_radius, 0.0f, 0.0f, 0.0f);
			//}
			//	break;
			//case LIW::LIWLightType_Spot: {
			//	glm::vec3 forward = transform.GetForward();
			//	forward = glm::normalize(forward);
			//	float cosHalfAngle = glm::cos(glm::radians(light.m_param.m_spotLight.m_angle * 0.5f));
			//	lightParam = glm::vec4(forward.x, forward.y,
			//		light.m_param.m_spotLight.m_radius * Maths::SignNoZero(forward.z), cosHalfAngle);
			//}
			//	break;
			//default:
			//	break;
			//}

			glUniform3f(lightPosLocation, lightPos.x, lightPos.y, lightPos.z);
			glUniform4f(lightColourLocation, lightColour.x, lightColour.y, lightColour.z, lightColour.w);
			glUniform4f(lightParamLocation, lightParam.x, lightParam.y, lightParam.z, lightParam.w);

			glBindVertexArray(meshSphere.GetHandleVAO());

			// Draw light
			for (int i = 0; i < submeshSphere.get_size(); ++i) {
				size_t size = submeshSphere[i].m_idxEnd - submeshSphere[i].m_idxBeg;
				const GLvoid* offset = (const GLvoid*)(submeshSphere[i].m_idxBeg * sizeof(unsigned int));
				glDrawElements(drawMode,
					(int)size,
					GL_UNSIGNED_INT,
					offset);
			}	
		}

		//TODO: Spot light
		//TODO: Directional light
	}
	void OGLRenderer_DeferredVisibility::CombinePass()
	{
		//
		// Setup
		//
		auto& assetManager = *LIWGlobal::GetAssetManager();
		auto& gBuffer = assetManager.GetFrameBuffer(m_frameBufferGBuffer);
		auto& lightBuffer = assetManager.GetFrameBuffer(m_frameBufferLightBuffer);
		auto& combineShaderProgram = assetManager.GetShaderProgram(m_combineShaderProgram);
		const uint32_t rawHandleCombineShaderProgram = combineShaderProgram.GetRawHandle();

		BindDefaultFrameBuffer();

		// Clear
		glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		// Disable z-test
		glDisable(GL_DEPTH_TEST);
		// Enable front culling
		glFrontFace(GL_CCW);
		glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);

		// Enable blending (accumulate light info)
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glUseProgram(rawHandleCombineShaderProgram);

		// Bind buffer textures
		// Diffuse
		LIWTexture::Bind2DTexture(gBuffer.GetColorAttachmentRawHandle(0),
			rawHandleCombineShaderProgram, LIW_SHADER_DEFERRED_DIFFUSE_TEXURE_NAME, 0);
		// Diffuse light
		LIWTexture::Bind2DTexture(lightBuffer.GetColorAttachmentRawHandle(0),
			rawHandleCombineShaderProgram, LIW_SHADER_DEFERRED_DIFFUSE_LIGHT_TEXURE_NAME, 1);
		// Specular light
		LIWTexture::Bind2DTexture(lightBuffer.GetColorAttachmentRawHandle(1),
			rawHandleCombineShaderProgram, LIW_SHADER_DEFERRED_SPECULAR_LIGHT_TEXURE_NAME, 2);
		// Depth
		LIWTexture::Bind2DTexture(gBuffer.GetDepthAttachmentRawHandle(),
			rawHandleCombineShaderProgram, LIW_SHADER_DEFERRED_DEPTH_TEXURE_NAME, 3);

		glBindVertexArray(m_emptyVAO);
		glDrawArrays(GL_TRIANGLES, 0, 3); //NOTE: only works with compatibility OpenGL profile
	}
	
	void OGLRenderer_DeferredVisibility::UploadLightPassData(uint32_t rawHandleLightBufferShader, LIWComponent_Camera& camera)
	{
		auto transCamHdl = LIW_ECS_GetComponentFromEntity(LIWComponent_Transform, camera.GetEntity());
		LIWComponent_Transform& transCam = LIW_ECS_GetComponent(LIWComponent_Transform, transCamHdl);
		const glm::vec3 camForward = transCam.GetForward();
		const glm::vec3 camUp = transCam.GetUp();
		glm::mat4 matView = glm::lookAt(transCam.m_position, transCam.m_position + camForward, camUp);
		glm::mat4 matProj = glm::perspective(
			glm::radians(camera.m_param.m_perspective.fovY),
			//camera.m_param.m_perspective.aspectRatio,
			float(width) / float(height),
			camera.m_near, camera.m_far);
		glm::mat4 inverseProjView = glm::inverse(matProj * matView);

		uint32_t pixelSizeLocation = glGetUniformLocation(rawHandleLightBufferShader, LIW_SHADER_DEFERRED_PIXEL_SIZE_NAME);
		glUniform2f(pixelSizeLocation, 1.0f / width, 1.0f / height);
		uint32_t invProjViewLocation = glGetUniformLocation(rawHandleLightBufferShader, LIW_SHADER_DEFERRED_INV_PROJVIEW_MATRIX_NAME);
		glUniformMatrix4fv(invProjViewLocation, 1, false, glm::value_ptr(inverseProjView));
	}
	void OGLRenderer_DeferredVisibility::UploadCameraData(LIWComponent_Camera& camera)
	{
		auto transCamHdl = LIW_ECS_GetComponentFromEntity(LIWComponent_Transform, camera.GetEntity());
		LIWComponent_Transform& transCam = LIW_ECS_GetComponent(LIWComponent_Transform, transCamHdl);
		const glm::vec3 camForward = transCam.GetForward();
		const glm::vec3 camUp = transCam.GetUp();
		glm::mat4 matView = glm::lookAt(transCam.m_position, transCam.m_position + camForward, camUp);
		glm::mat4 matProj = glm::perspective(
			glm::radians(camera.m_param.m_perspective.fovY),
			//camera.m_param.m_perspective.aspectRatio,
			float(width)/float(height),
			camera.m_near, camera.m_far);

		size_t sizeData = 2 * sizeof(glm::mat4) + sizeof(glm::vec4);
		glBindBuffer(GL_UNIFORM_BUFFER, m_uboCameraData);
		glBufferData(GL_UNIFORM_BUFFER, sizeData, nullptr, GL_STATIC_DRAW);

		sizeData = 0;
		glm::vec4 pos = glm::vec4(transCam.m_position, 0.0f);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeData, sizeof(glm::mat4), glm::value_ptr(matView)); sizeData += sizeof(glm::mat4);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeData, sizeof(glm::mat4), glm::value_ptr(matProj)); sizeData += sizeof(glm::mat4);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeData, sizeof(glm::vec4), glm::value_ptr(pos)); sizeData += sizeof(glm::vec4);
		
		glBindBufferRange(GL_UNIFORM_BUFFER, LIW_SHADER_UBO_BIND_CAMERADATA, m_uboCameraData, 0, sizeData);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
	void LIW_TT_OGLDeferredVisibilityRender::Execute()
	{
		m_renderer->RenderScene();
		LIWFiberExecutor::m_executor.DecreaseSyncCounter(LIW_SYNC_COUNTER_RESERVE_RENDER, 1);
	}
}