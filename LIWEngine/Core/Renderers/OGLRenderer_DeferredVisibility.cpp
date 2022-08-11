#include "OGLRenderer_DeferredVisibility.h"
namespace LIW {
	OGLRenderer_DeferredVisibility::OGLRenderer_DeferredVisibility(LIW::App::Window& parent):
		OGLRenderer(parent)
	{
		glGenBuffers(1, &m_uboCameraData);
		glGenBuffers(1, &m_uboPointLightPerPixelData);
		glGenBuffers(1, &m_pixelCountBuffer);
		glGenBuffers(1, &m_pixelPrefixSumBuffer);
		glGenBuffers(1, &m_indirectDispatchBuffer);
		glGenBuffers(1, &m_workListBuffer);
		glCreateBuffers(1, &m_indexOffsetLookupBuffer);
		glCreateBuffers(1, &m_meshIDLookupBuffer);
		glCreateBuffers(1, &m_modelMatricesBuffer);
		glCreateBuffers(1, &m_normalMatricesBuffer);
		glGenBuffers(1, &m_vertexBuffer);
		glGenBuffers(1, &m_indexBuffer);
		glGenBuffers(1, &m_lightBuffer);
		glGenVertexArrays(1, &m_emptyVAO);
		
		//
		// Init SSBO
		// 
		// Material counters
		const size_t sizeDataPixelCount = sizeof(uint32_t) * LIW_DEFERRED_VISIBILITY_MAX_MATERIAL_COUNT;
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_MATERIAL_COUNTERS, m_pixelCountBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeDataPixelCount, nullptr, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		// Prefix sums
		const size_t sizeDataPrefixSum = sizeof(uint32_t) * LIW_DEFERRED_VISIBILITY_MAX_MATERIAL_COUNT;
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_PREFIXSUM, m_pixelPrefixSumBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeDataPrefixSum, nullptr, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		// Indirect dispatch
		const size_t sizeIndirectDispatch = sizeof(uint32_t) * 3 * LIW_DEFERRED_VISIBILITY_MAX_MATERIAL_COUNT;
		glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, m_indirectDispatchBuffer);
		glBufferData(GL_DISPATCH_INDIRECT_BUFFER, sizeIndirectDispatch, nullptr, GL_STATIC_DRAW);
		glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, 0);
		// Pixel worklist
		const size_t sizeWorklist = sizeof(uint32_t) * LIW_DEFERRED_VISIBILITY_MAX_RESOLUTION;
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_workListBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeWorklist, nullptr, GL_STATIC_DRAW);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		// Mesh lookup
		glNamedBufferStorage(m_meshIDLookupBuffer, sizeof(uint32_t) * LIW_DEFERRED_VISIBILITY_MAX_OBJECT, nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
		// Model Matrix Buffer
		glNamedBufferStorage(m_modelMatricesBuffer, sizeof(glm::mat4) * LIW_DEFERRED_VISIBILITY_MAX_OBJECT, nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
		// Normal Matrix Buffer
		glNamedBufferStorage(m_normalMatricesBuffer, sizeof(glm::mat3) * LIW_DEFERRED_VISIBILITY_MAX_OBJECT, nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
		// Index offset lookup buffer
		glNamedBufferStorage(m_indexOffsetLookupBuffer, sizeof(uint32_t) * LIW_DEFERRED_VISIBILITY_MAX_MESH, nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
		// Vertex Buffer
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
		size_t sizeVertexBuffer = (size_t)LIW_DEFERRED_VISIBILITY_MAX_VERTEX * (sizeof(glm::vec3)+ sizeof(glm::vec3)+ sizeof(glm::vec4)+ sizeof(glm::vec2));
		glBufferStorage(GL_ARRAY_BUFFER, sizeVertexBuffer, nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Index Buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
		size_t sizeIndexBuffer = (size_t)LIW_DEFERRED_VISIBILITY_MAX_INDEX * sizeof(uint32_t);
		glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, sizeIndexBuffer, nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		//
		// Init Ubo
		//
		size_t sizeLightData = sizeof(glm::vec4) * LIW_LIGHT_DEFERRED_VISIBILITY_MAX_PERPIXEL * 9 + sizeof(int32_t) * 3;
		glBindBuffer(GL_UNIFORM_BUFFER, m_lightBuffer);
		glBufferData(GL_UNIFORM_BUFFER, sizeLightData, nullptr, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		auto& assetManager = *LIWGlobal::GetAssetManager();

		//
		// Create Shaders
		//
		// Shaders
		liw_objhdl_type hdlLightBufferVertShader = assetManager.CreateShader("shader_vert_lightBuffer");
		LIWShader& lightBufferVertShader = assetManager.GetShader(hdlLightBufferVertShader);
		lightBufferVertShader.LoadShader(LIW_PATH_DIR_DEFAULT_SHADERS"OGL/LIW_Deferred_LightBuffer_Vert.glsl", LIWShaderType_Vertex);

		liw_objhdl_type hdlLightBufferFragShader = assetManager.CreateShader("shader_frag_lightBuffer");
		LIWShader& lightBufferFragShader = assetManager.GetShader(hdlLightBufferFragShader);
		lightBufferFragShader.LoadShader(LIW_PATH_DIR_DEFAULT_SHADERS"OGL/LIW_Deferred_LightBuffer_Frag.glsl", LIWShaderType_Fragment);
	
		liw_objhdl_type hdlScreenQuadShader = assetManager.CreateShader("shader_vert_screenQuad");
		LIWShader& screenQuadVertShader = assetManager.GetShader(hdlScreenQuadShader);
		screenQuadVertShader.LoadShader(LIW_PATH_DIR_DEFAULT_SHADERS"OGL/LIW_ScreenQuad_Vert.glsl", LIWShaderType_Vertex);

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
		worklistGenerateP0ComputeShader.LoadShader(LIW_PATH_DIR_DEFAULT_SHADERS"OGL/LIW_VisibilityDeferred_Worklist_P0_MaterialCount_Comp.glsl", LIWShaderType_Compute);
		liw_objhdl_type hdlWorklistGenerateP1ComputeShader = assetManager.CreateShader("shader_comp_worklistGenerate_p1");
		LIWShader& worklistGenerateP1ComputeShader = assetManager.GetShader(hdlWorklistGenerateP1ComputeShader);
		worklistGenerateP1ComputeShader.LoadShader(LIW_PATH_DIR_DEFAULT_SHADERS"OGL/LIW_VisibilityDeferred_Worklist_P1_PrefixSum_Comp.glsl", LIWShaderType_Compute);
		liw_objhdl_type hdlWorklistGenerateP2ComputeShader = assetManager.CreateShader("shader_comp_worklistGenerate_p2");
		LIWShader& worklistGenerateP2ComputeShader = assetManager.GetShader(hdlWorklistGenerateP2ComputeShader);
		worklistGenerateP2ComputeShader.LoadShader(LIW_PATH_DIR_DEFAULT_SHADERS"OGL/LIW_VisibilityDeferred_Worklist_P2_WorklistPopulation_Comp.glsl", LIWShaderType_Compute);

		liw_objhdl_type hdlPresentFragShader = assetManager.CreateShader("shader_frag_present");
		LIWShader& presentFragShader = assetManager.GetShader(hdlPresentFragShader);
		presentFragShader.LoadShader(LIW_PATH_DIR_DEFAULT_SHADERS"OGL/LIW_VisibilityDeferred_Present_Frag.glsl", LIWShaderType_Fragment);

		
		// Shader programs
		m_lightBufferShaderProgram = assetManager.CreateShaderProgram("shaderProgram_lightBuffer");
		LIWShaderProgram& lightBufferShaderProgram = assetManager.GetShaderProgram(m_lightBufferShaderProgram);
		lightBufferShaderProgram.CreateShader({ lightBufferVertShader , lightBufferFragShader });

		m_combineShaderProgram = assetManager.CreateShaderProgram("shaderProgram_combine");
		LIWShaderProgram& combineShaderProgram = assetManager.GetShaderProgram(m_combineShaderProgram);
		combineShaderProgram.CreateShader({ screenQuadVertShader , combineShader });
		
		m_visibilityShaderProgram = assetManager.CreateShaderProgram("shaderProgram_visibility");
		LIWShaderProgram& visibilityShaderProgram = assetManager.GetShaderProgram(m_visibilityShaderProgram);
		visibilityShaderProgram.CreateShader({ visibilityVertShader , visibilityFragShader });

		m_visibilityShowShaderProgram = assetManager.CreateShaderProgram("shaderProgram_visibilityShow");
		LIWShaderProgram& visibilityShowShaderProgram = assetManager.GetShaderProgram(m_visibilityShowShaderProgram);
		visibilityShowShaderProgram.CreateShader({ screenQuadVertShader , visibilityShowFragShader });

		m_worklistGenerateP0ShaderProgram = assetManager.CreateShaderProgram("shaderProgram_worklistGenerate_p0");
		LIWShaderProgram& worklistGenerateP0ShaderProgram = assetManager.GetShaderProgram(m_worklistGenerateP0ShaderProgram);
		worklistGenerateP0ShaderProgram.CreateShader({ worklistGenerateP0ComputeShader });

		m_worklistGenerateP1ShaderProgram = assetManager.CreateShaderProgram("shaderProgram_worklistGenerate_p1");
		LIWShaderProgram& worklistGenerateP1ShaderProgram = assetManager.GetShaderProgram(m_worklistGenerateP1ShaderProgram);
		worklistGenerateP1ShaderProgram.CreateShader({ worklistGenerateP1ComputeShader });

		m_worklistGenerateP2ShaderProgram = assetManager.CreateShaderProgram("shaderProgram_worklistGenerate_p2");
		LIWShaderProgram& worklistGenerateP2ShaderProgram = assetManager.GetShaderProgram(m_worklistGenerateP2ShaderProgram);
		worklistGenerateP2ShaderProgram.CreateShader({ worklistGenerateP2ComputeShader });

		m_presentShaderProgram = assetManager.CreateShaderProgram("shaderProgram_present");
		LIWShaderProgram& presentShaderProgram = assetManager.GetShaderProgram(m_presentShaderProgram);
		presentShaderProgram.CreateShader({ screenQuadVertShader, presentFragShader });


		lightBufferVertShader.UnloadShader();
		lightBufferFragShader.UnloadShader();
		screenQuadVertShader.UnloadShader();
		combineShader.UnloadShader();
		visibilityVertShader.UnloadShader();
		visibilityFragShader.UnloadShader();
		visibilityShowFragShader.UnloadShader();
		worklistGenerateP0ComputeShader.UnloadShader();
		worklistGenerateP1ComputeShader.UnloadShader();
		worklistGenerateP2ComputeShader.UnloadShader();
		assetManager.DestroyShader("shader_vert_lightBuffer");
		assetManager.DestroyShader("shader_frag_lightBuffer");
		assetManager.DestroyShader("shader_vert_screenQuad");
		assetManager.DestroyShader("shader_frag_combine");
		assetManager.DestroyShader("shader_vert_visibility");
		assetManager.DestroyShader("shader_frag_visibility");
		assetManager.DestroyShader("shader_frag_visibilityShow");
		assetManager.DestroyShader("shader_comp_worklistGenerate_p0");
		assetManager.DestroyShader("shader_comp_worklistGenerate_p1");
		assetManager.DestroyShader("shader_comp_worklistGenerate_p2");

		//
		// Light volumes
		//
		m_meshSphere = assetManager.GetMeshHandle(LIW_MESH_SPHERE_NAME);

		//
		// Textures
		//


		//
		// Frame buffers
		//
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
			LIW_FRAMEBUFFER_ATTACHMENT_FLAG_INDEX16 | //Material Buffer
			LIW_FRAMEBUFFER_ATTACHMENT_FLAG_INDEX_1 | //Visibility Buffer
			LIW_FRAMEBUFFER_ATTACHMENT_FLAG_DEPTH;	  //Depth Buffer
		vBuffer.CreateFrameBuffer(width, height, vBufferFlag);

		m_frameBufferPresentBuffer = assetManager.CreateFrameBuffer("framebuffer_presentBuffer");
		auto& presentBuffer = assetManager.GetFrameBuffer(m_frameBufferPresentBuffer);
		const liw_flag_type presentBufferFlag =
			LIW_FRAMEBUFFER_ATTACHMENT_FLAG_COLOR_RGBA; //Present Colour Buffer
		presentBuffer.CreateFrameBuffer(width, height, presentBufferFlag);
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

		LIWFrameBuffer& presentBuffer = assetManager.GetFrameBuffer(m_frameBufferPresentBuffer);
		presentBuffer.DestroyFrameBuffer();
		assetManager.DestroyFrameBuffer("framebuffer_presentBuffer");

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

		LIWShaderProgram& worklistGenerateP0ShaderProgram = assetManager.GetShaderProgram(m_worklistGenerateP0ShaderProgram);
		worklistGenerateP0ShaderProgram.DestroyShader();
		assetManager.DestroyShaderProgram("shaderProgram_worklistGenerate_p0");

		LIWShaderProgram& worklistGenerateP1ShaderProgram = assetManager.GetShaderProgram(m_worklistGenerateP1ShaderProgram);
		worklistGenerateP1ShaderProgram.DestroyShader();
		assetManager.DestroyShaderProgram("shaderProgram_worklistGenerate_p1");

		LIWShaderProgram& worklistGenerateP2ShaderProgram = assetManager.GetShaderProgram(m_worklistGenerateP2ShaderProgram);
		worklistGenerateP2ShaderProgram.DestroyShader();
		assetManager.DestroyShaderProgram("shaderProgram_worklistGenerate_p2");


		glDeleteVertexArrays(1, &m_emptyVAO);
		glDeleteBuffers(1, &m_lightBuffer);
		glDeleteBuffers(1, &m_indexBuffer);
		glDeleteBuffers(1, &m_vertexBuffer);
		glDeleteBuffers(1, &m_normalMatricesBuffer);
		glDeleteBuffers(1, &m_modelMatricesBuffer);
		glDeleteBuffers(1, &m_meshIDLookupBuffer);
		glDeleteBuffers(1, &m_indexOffsetLookupBuffer);
		glDeleteBuffers(1, &m_workListBuffer);
		glDeleteBuffers(1, &m_indirectDispatchBuffer);
		glDeleteBuffers(1, &m_pixelPrefixSumBuffer);
		glDeleteBuffers(1, &m_pixelCountBuffer);
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
	void OGLRenderer_DeferredVisibility::AddRenderMaterial(liw_objhdl_type hdlMaterial)
	{
		m_materialList.insert(hdlMaterial);
	}
	void OGLRenderer_DeferredVisibility::RemoveRenderMaterial(liw_objhdl_type hdlMaterial)
	{
		auto itr = m_materialList.find(hdlMaterial);
		if (itr != m_materialList.end())
			m_materialList.erase(itr);
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
		using namespace std::chrono;
		system_clock::time_point beg_free = system_clock::now();
		UploadCameraData(camera); 
		system_clock::time_point end_free = system_clock::now();
		printf("UploadCameraData %llu ns\n\n", (std::chrono::duration_cast<std::chrono::nanoseconds>(end_free - beg_free)).count());
		
		//
		// Visibility pass
		//
		beg_free = system_clock::now();
		VisibilityPass();
		end_free = system_clock::now();
		printf("VisibilityPass %llu ns\n\n", (std::chrono::duration_cast<std::chrono::nanoseconds>(end_free - beg_free)).count());

		//
		// Worklist generate pass
		//
		beg_free = system_clock::now();
		WorklistPass();
		end_free = system_clock::now();
		printf("WorklistPass %llu ns\n\n", (std::chrono::duration_cast<std::chrono::nanoseconds>(end_free - beg_free)).count());

		//
		// Shading pass
		//
		beg_free = system_clock::now();
		ShadingPass(camera);
		end_free = system_clock::now();
		printf("ShadingPass %llu ns\n\n", (std::chrono::duration_cast<std::chrono::nanoseconds>(end_free - beg_free)).count());

		//
		// Result present pass (stupid opengl cannot write straight to swapchain image)
		//
		beg_free = system_clock::now();
		PresentPass();
		end_free = system_clock::now();
		printf("PresentPass %llu ns\n\n", (std::chrono::duration_cast<std::chrono::nanoseconds>(end_free - beg_free)).count());

		////
		//// Debug: Visibility show pass
		////
		//VisibilityDebugShowPass();

		

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
		glClearDepth(1.0f);
		glClear(GL_DEPTH_BUFFER_BIT);
		glm::uvec4 idxVis{	LIW_DEFERRED_VISIBILITY_INVALID_IDX, 
							LIW_DEFERRED_VISIBILITY_INVALID_IDX, 
							LIW_DEFERRED_VISIBILITY_INVALID_IDX, 
							LIW_DEFERRED_VISIBILITY_INVALID_IDX };
		glm::uvec4 idxMaterialClear{	LIW_DEFERRED_VISIBILITY_INVALID_IDX16, 
										LIW_DEFERRED_VISIBILITY_INVALID_IDX16, 
										LIW_DEFERRED_VISIBILITY_INVALID_IDX16, 
										LIW_DEFERRED_VISIBILITY_INVALID_IDX16 };
		glClearBufferuiv(GL_COLOR, 0, glm::value_ptr(idxMaterialClear));
		glClearBufferuiv(GL_COLOR, 1, glm::value_ptr(idxVis));
		//Enable z-test
		glEnable(GL_DEPTH_TEST);

		//Enable back culling
		glFrontFace(GL_CCW);
		glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);

		//Disable blending (transparency not gonna be rendered in deferred mode anyway)
		glDisable(GL_BLEND);


		//
		// Build Scene Vertex Buffer
		//
		std::unordered_map<liw_objhdl_type, uint32_t> meshDataHandles2IdxMesh;

		auto& meshRendererBatchedManager = LIW_ECS_GetComponentManager(LIWComponent_MeshRendererBatched);
		auto& meshRendererBatcheds = meshRendererBatchedManager.m_components;
		
		uint32_t idxMesh = 0;
		uint32_t idxObject = 0;

		size_t sizePositionBuffer = 0;
		size_t sizeNormalBuffer = 0;
		size_t sizeTangentBuffer = 0;
		size_t sizeTexcoordBuffer = 0;

		size_t sizeIndexBuffer = 0;

		// Collect all MeshData to render
		for (auto itr = meshRendererBatcheds.get_beg(); itr != meshRendererBatcheds.get_end(); itr++) {
			auto& meshRendererBatched = *itr;
			const liw_objhdl_type hdlMeshData = meshRendererBatched.m_handleMeshData;
			meshDataHandles2IdxMesh.insert({ hdlMeshData, 0 });
			idxObject++;
		}

		if (idxObject > LIW_DEFERRED_VISIBILITY_MAX_OBJECT) {
			throw std::runtime_error("object count exceeds maximum support. ");
		}

		const uint32_t sizeMeshIDLookup = idxObject * sizeof(uint32_t);
		const uint32_t sizeModelMatrices = idxObject * sizeof(glm::mat4);
		const uint32_t sizeNormalMatrices = idxObject * sizeof(glm::mat3);
		
		uint32_t* idxObject2idxMesh = (uint32_t*)glMapNamedBufferRange(m_meshIDLookupBuffer, 0, sizeMeshIDLookup, GL_MAP_WRITE_BIT);
		glm::mat4* idxObject2ModelMatrix = (glm::mat4*)glMapNamedBufferRange(m_modelMatricesBuffer, 0, sizeModelMatrices, GL_MAP_WRITE_BIT);
		glm::mat3* idxObject2NormalMatrix = (glm::mat3*)glMapNamedBufferRange(m_normalMatricesBuffer, 0, sizeNormalMatrices, GL_MAP_WRITE_BIT);
		
		// Assign idx for each MeshData
		for (auto& pair : meshDataHandles2IdxMesh) {
			const liw_objhdl_type hdlMeshData = pair.first;
			auto& meshData = assetManager.GetMeshData(hdlMeshData);
			if (!meshData.IsValid())
				throw std::runtime_error("invalid meshdata cannot be renderered. ");

			const size_t positionCount = meshData.GetPositionArray().get_size();
			assert(positionCount != 0); //required for pipeline
			const size_t normalCount = meshData.GetNormalArray().get_size();
			assert(normalCount != 0); //required for pipeline
			const size_t tangentCount = meshData.GetTangentArray().get_size();
			assert(tangentCount != 0); //required for pipeline
			const size_t texcoordCount = meshData.GetTexcoordArray().get_size();
			assert(texcoordCount != 0); //required for pipeline

			assert(positionCount == normalCount && tangentCount == texcoordCount && normalCount == tangentCount);

			const size_t indexCount = meshData.GetIndexArray().get_size();
			assert(indexCount != 0);

			sizePositionBuffer += positionCount * sizeof(glm::vec3);
			sizeNormalBuffer += normalCount * sizeof(glm::vec3);
			sizeTangentBuffer += tangentCount * sizeof(glm::vec4);
			sizeTexcoordBuffer += texcoordCount * sizeof(glm::vec2);

			sizeIndexBuffer += indexCount * sizeof(uint32_t);

			pair.second = idxMesh;

			idxMesh++;
		}

		if (idxMesh > LIW_DEFERRED_VISIBILITY_MAX_MESH) {
			throw std::runtime_error("mesh count exceeds maximum support. ");
		}

		const uint32_t sizeIndexOffsetLookup = idxMesh * sizeof(uint32_t);
		uint32_t* meshIndexOffsetLookup = (uint32_t*)glMapNamedBufferRange(m_indexOffsetLookupBuffer, 0, sizeIndexOffsetLookup, GL_MAP_WRITE_BIT);
		
		//
		// Build vertex buffers
		// Build index-offset lookup
		// Build mesh-offset lookup
		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		size_t sizeVertexBuffer = sizePositionBuffer +
			sizeNormalBuffer +
			sizeTangentBuffer +
			sizeTexcoordBuffer;

		if (sizeVertexBuffer > (size_t)LIW_DEFERRED_VISIBILITY_MAX_VERTEX * (sizeof(glm::vec3) + sizeof(glm::vec3) + sizeof(glm::vec4) + sizeof(glm::vec2))) {
			throw std::runtime_error("vertex buffer overflow. ");
		}

		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
		
		void* positionBuffer = glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeVertexBuffer, GL_MAP_WRITE_BIT);
		sizeVertexBuffer = 0;
		glVertexAttribPointer(LIW_SHADER_VA_LOCATION_POSITION, 3, GL_FLOAT, GL_FALSE, 0, (void*)sizeVertexBuffer);
		glEnableVertexAttribArray(LIW_SHADER_VA_LOCATION_POSITION);
		sizeVertexBuffer += sizePositionBuffer;
		
		void* normalBuffer = (char*)positionBuffer + sizeVertexBuffer;
		glVertexAttribPointer(LIW_SHADER_VA_LOCATION_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, (void*)sizeVertexBuffer);
		glEnableVertexAttribArray(LIW_SHADER_VA_LOCATION_NORMAL);
		sizeVertexBuffer += sizeNormalBuffer;
		
		void* tangentBuffer = (char*)positionBuffer + sizeVertexBuffer;
		glVertexAttribPointer(LIW_SHADER_VA_LOCATION_TANGENT, 4, GL_FLOAT, GL_FALSE, 0, (void*)sizeVertexBuffer);
		glEnableVertexAttribArray(LIW_SHADER_VA_LOCATION_TANGENT);
		sizeVertexBuffer += sizeTangentBuffer;
		
		void* texcoordBuffer = (char*)positionBuffer + sizeVertexBuffer;
		glVertexAttribPointer(LIW_SHADER_VA_LOCATION_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, (void*)sizeVertexBuffer);
		glEnableVertexAttribArray(LIW_SHADER_VA_LOCATION_TEXCOORD);
		sizeVertexBuffer += sizeTexcoordBuffer;


		if (sizeIndexBuffer > (size_t)LIW_DEFERRED_VISIBILITY_MAX_INDEX * sizeof(uint32_t)) {
			throw std::runtime_error("index buffer overflow. ");
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
		void* indexBuffer = glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, sizeIndexBuffer, GL_MAP_WRITE_BIT);


		size_t sizePositionBufferCur = 0;
		size_t sizeNormalBufferCur = 0;
		size_t sizeTangentBufferCur = 0;
		size_t sizeTexcoordBufferCur = 0;

		size_t sizeIndexBufferCur = 0;

		size_t indexIdxOffset = 0;
		idxMesh = 0;
		m_vertexCount = 0;
		for (auto& pair : meshDataHandles2IdxMesh) {
			const liw_objhdl_type hdlMeshData = pair.first;
			auto& meshData = assetManager.GetMeshData(hdlMeshData);

			auto& positions = meshData.GetPositionArray();
			const size_t positionCount = positions.get_size();
			const size_t sizePosition = positionCount * sizeof(glm::vec3);
			auto& normals = meshData.GetNormalArray();
			const size_t sizeNormal = normals.get_size() * sizeof(glm::vec3);
			auto& tangents = meshData.GetTangentArray();
			const size_t sizeTangent = tangents.get_size() * sizeof(glm::vec4);
			auto& texcoords = meshData.GetTexcoordArray();
			const size_t sizeTexcoord = texcoords.get_size() * sizeof(glm::vec2);
			
			auto& indices = meshData.GetIndexArray();
			const size_t countIndex = indices.get_size();
			const size_t sizeIndex = countIndex * sizeof(uint32_t);

			memcpy_s((char*)positionBuffer + sizePositionBufferCur, sizePositionBuffer - sizePositionBufferCur, positions.get_data(), sizePosition); sizePositionBufferCur += sizePosition;
			memcpy_s((char*)normalBuffer + sizeNormalBufferCur, sizeNormalBuffer - sizeNormalBufferCur, normals.get_data(), sizeNormal); sizeNormalBufferCur += sizeNormal;
			memcpy_s((char*)tangentBuffer + sizeTangentBufferCur, sizeTangentBuffer - sizeTangentBufferCur, tangents.get_data(), sizeTangent); sizeTangentBufferCur += sizeTangent;
			memcpy_s((char*)texcoordBuffer + sizeTexcoordBufferCur, sizeTexcoordBuffer - sizeTexcoordBufferCur, texcoords.get_data(), sizeTexcoord); sizeTexcoordBufferCur += sizeTexcoord;
			
			void* indexBufferCur = (char*)indexBuffer + sizeIndexBufferCur;
			memcpy_s(indexBufferCur, sizeIndexBuffer - sizeIndexBufferCur, indices.get_data(), sizeIndex);
			for (size_t i = 0; i < countIndex; ++i) {
				((uint32_t*)indexBufferCur)[i] += m_vertexCount;
			}
			sizeIndexBufferCur += sizeIndex;

			meshIndexOffsetLookup[idxMesh] = indexIdxOffset;
			indexIdxOffset += indices.get_size();
			m_vertexCount += (uint32_t)positionCount;
			idxMesh++;
		}

		glUnmapBuffer(GL_ARRAY_BUFFER);
		glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
		glBindVertexArray(0);

		glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT | GL_ELEMENT_ARRAY_BARRIER_BIT);

		//
		// Render MeshRenderers to VBuffer
		// build ModelMatrices
		auto& shaderProgram = assetManager.GetShaderProgram(m_visibilityShaderProgram);
		uint32_t rawHandleshaderProgram = shaderProgram.GetRawHandle();

		glBindVertexArray(m_vao);

		// Use program
		glUseProgram(rawHandleshaderProgram);

		const uint32_t idxMaterialLocation = glGetUniformLocation(rawHandleshaderProgram, "idxMaterial");
		const uint32_t idxObjectLocation = glGetUniformLocation(rawHandleshaderProgram, "idxObject");

		idxObject = 0;
		for (auto itr = meshRendererBatcheds.get_beg(); itr != meshRendererBatcheds.get_end(); itr++) {
			auto& meshRendererBatched = *itr;
			auto entity = meshRendererBatched.GetEntity();
			liw_objhdl_type transformHandle = LIW_ECS_GetComponentFromEntity(LIWComponent_Transform, entity);
			auto& transform = LIW_ECS_GetComponent(LIWComponent_Transform, transformHandle);

			const liw_objhdl_type hdlMeshData = meshRendererBatched.m_handleMeshData;
			//auto& material = assetManager.GetMaterial(meshRenderer.m_handleMaterial);
			auto& mesh = assetManager.GetMeshData(hdlMeshData);
			uint32_t idxMesh = meshDataHandles2IdxMesh.at(hdlMeshData);
			size_t indexIdxOffset = meshIndexOffsetLookup[idxMesh];

			// Upload uniforms
			glm::mat4 matModel = transform.GetMatrix();
			glUniformMatrix4fv(glGetUniformLocation(rawHandleshaderProgram, LIW_SHADER_MODEL_MATRIX_NAME), 1, false, glm::value_ptr(matModel));
			uint32_t idxObjectAligned = idxObject << 18;
			glUniform1ui(idxObjectLocation, idxObjectAligned);
			glUniform1ui(idxMaterialLocation, meshRendererBatched.m_handleMaterial);

			glm::mat3 matNormal = glm::transpose(glm::inverse(glm::mat3(matModel)));

			// Build object lookup buffers
			idxObject2idxMesh[idxObject] = idxMesh;
			idxObject2ModelMatrix[idxObject] = matModel;
			idxObject2NormalMatrix[idxObject] = matNormal;

			// Draw
			auto& submeshes = mesh.GetSubMeshArray();
			auto mode = LIWImageFormat_2_GLPrimitive.at(mesh.GetPrimitiveType());
			for (int i = 0; i < submeshes.get_size(); ++i) {
				const size_t start = indexIdxOffset + submeshes[i].m_idxBeg;
				const size_t size = submeshes[i].m_idxEnd - submeshes[i].m_idxBeg;
				const GLvoid* offset = (const GLvoid*)(start * sizeof(uint32_t));
				glDrawElements(mode,
					(int)size,
					GL_UNSIGNED_INT,
					offset);
			}

			idxObject++;
		}

		glUnmapNamedBuffer(m_indexOffsetLookupBuffer);
		glUnmapNamedBuffer(m_meshIDLookupBuffer);
		glUnmapNamedBuffer(m_modelMatricesBuffer);
		glUnmapNamedBuffer(m_normalMatricesBuffer);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glDeleteVertexArrays(1, &m_vao);

		glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);
	}
	void OGLRenderer_DeferredVisibility::VisibilityDebugShowPass()
	{
		////
		//// Setup
		////
		//auto& assetManager = *LIWGlobal::GetAssetManager();
		//auto& vBuffer = assetManager.GetFrameBuffer(m_frameBufferVBuffer);
		//LIWShaderProgram& visibilityShowShaderProgram = assetManager.GetShaderProgram(m_visibilityShowShaderProgram);

		//BindDefaultFrameBuffer();

		//// Clear
		//glClearColor(0.5f, 0.1f, 0.2f, 1.0f);
		//glClearDepth(1.0f);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//// Disable z-test
		//glDisable(GL_DEPTH_TEST);
		//// Enable front culling
		//glFrontFace(GL_CCW);
		//glCullFace(GL_BACK);
		//glEnable(GL_CULL_FACE);

		////Disable blending (transparency not gonna be rendered in deferred mode anyway)
		//glDisable(GL_BLEND);

		////
		//// Render
		////

		//// Use Shader
		//const uint32_t rawHdlVisibilityShowShaderProgram = visibilityShowShaderProgram.GetRawHandle();
		//glUseProgram(rawHdlVisibilityShowShaderProgram);

		//// Bind buffer textures
		//// Idx texture
		////const auto rawHdl = vBuffer.GetColorAttachmentRawHandle(1);
		//auto& testTexture0 = assetManager.GetTexture2D(m_testTexture0);
		//const auto rawHdl = testTexture0.GetRawHandle();
		//LIWTexture::Bind2DTexture(rawHdl,
		//	rawHdlVisibilityShowShaderProgram, "indexTexture", 0);

		//glBindVertexArray(m_emptyVAO);
		//glDrawArrays(GL_TRIANGLES, 0, 3); //NOTE: only works with compatibility OpenGL profile
	}
	void OGLRenderer_DeferredVisibility::WorklistPass()
	{
		//
		// Setup
		//
		auto& assetManager = *LIWGlobal::GetAssetManager();
		auto& vBuffer = assetManager.GetFrameBuffer(m_frameBufferVBuffer);
		LIWShaderProgram& worklistGenerateP0Program = assetManager.GetShaderProgram(m_worklistGenerateP0ShaderProgram);
		const uint32_t rawHdlP0ShaderProgram = worklistGenerateP0Program.GetRawHandle();
		LIWShaderProgram& worklistGenerateP1Program = assetManager.GetShaderProgram(m_worklistGenerateP1ShaderProgram);
		const uint32_t rawHdlP1ShaderProgram = worklistGenerateP1Program.GetRawHandle();
		LIWShaderProgram& worklistGenerateP2Program = assetManager.GetShaderProgram(m_worklistGenerateP2ShaderProgram);
		const uint32_t rawHdlP2ShaderProgram = worklistGenerateP2Program.GetRawHandle();

		// Prepare SSBOs
		uint32_t zeroCounts[LIW_DEFERRED_VISIBILITY_MAX_MATERIAL_COUNT];
		const size_t sizeData = sizeof(uint32_t) * LIW_DEFERRED_VISIBILITY_MAX_MATERIAL_COUNT;
		memset(zeroCounts, 0, sizeData);

		// material counters
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_MATERIAL_COUNTERS, m_pixelCountBuffer);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeData, zeroCounts);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		// prefix sums
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_PREFIXSUM, m_pixelPrefixSumBuffer);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeData, zeroCounts);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		// indirect dispatch
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_INDIRECT_DISPATCH, m_indirectDispatchBuffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		// pixel worklist
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_WORKLIST, m_workListBuffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		//
		// Dispatch P0
		//
		glUseProgram(rawHdlP0ShaderProgram);

		uint32_t countGroupX = width / 8;
		uint32_t countGroupY = height / 8;

		LIWTexture::Bind2DTextureImage(vBuffer.GetColorAttachmentRawHandle(1),
			rawHdlP0ShaderProgram, "indexImg", 0, LIWImageFormat_R32U,
			LIWTextureImageAccessType_Read);
		LIWTexture::Bind2DTextureImage(vBuffer.GetColorAttachmentRawHandle(0),
			rawHdlP0ShaderProgram, "materialImg", 1, LIWImageFormat_R16U,
			LIWTextureImageAccessType_Read);

		glDispatchCompute(countGroupX, countGroupY, 1);

		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT); //Wait for update

		LIWTexture::Unbind2DTextureImage(0);
		LIWTexture::Unbind2DTextureImage(1);

		//{
		//	uint32_t pixelCounts[LIW_DEFERRED_VISIBILITY_MAX_MATERIAL_COUNT];
		//	// Readback atomic counters
		//	GLuint* userCounters;
		//	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_pixelCountBuffer);
		//	userCounters = (GLuint*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeData, GL_MAP_READ_BIT);
		//	memcpy_s(pixelCounts, sizeData, userCounters, sizeData);
		//	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		//	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		//}

		//
		// Dispatch P1
		//
		glUseProgram(rawHdlP1ShaderProgram);

		glDispatchCompute(1, 1, 1);

		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT); //Wait for update

		//
		// Dispatch P2
		//
		glUseProgram(rawHdlP2ShaderProgram);

		// clear material counters (will be populated again in shader, for this pass just used as indices)
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_MATERIAL_COUNTERS, m_pixelCountBuffer);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeData, zeroCounts);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		LIWTexture::Bind2DTextureImage(vBuffer.GetColorAttachmentRawHandle(0),
			rawHdlP2ShaderProgram, "materialImg", 0, LIWImageFormat_R16U,
			LIWTextureImageAccessType_Read);

		//{
		//	uint32_t pixelCounts[LIW_DEFERRED_VISIBILITY_MAX_MATERIAL_COUNT];
		//	// Readback atomic counters
		//	void* userCounters;
		//	userCounters = glMapNamedBufferRange(m_pixelPrefixSumBuffer, 0, sizeData, GL_MAP_READ_BIT);
		//	memcpy_s(pixelCounts, sizeData, userCounters, sizeData);
		//	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		//	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		//}

		glDispatchCompute(countGroupX, countGroupY, 1);

		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT); //Wait for update

		LIWTexture::Unbind2DTextureImage(0);

		//glGetTextureHandleARB

		//{
		//	uint32_t pixelCounts[LIW_DEFERRED_VISIBILITY_MAX_MATERIAL_COUNT];
		//	// Readback atomic counters
		//	GLuint* userCounters;
		//	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_pixelCountBuffer);
		//	userCounters = (GLuint*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeData, GL_MAP_READ_BIT);
		//	memcpy_s(pixelCounts, sizeData, userCounters, sizeData);
		//	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		//	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		//}

		//// Readback atomic counters
		//GLuint* userCounters;
		//glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_pixelCountBuffer);
		//userCounters = (GLuint*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeData, GL_MAP_READ_BIT);
		//memcpy_s(pixelCounts, sizeData, userCounters, sizeData);
		//glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		//glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		//GLuint* userCounters1;
		//uint32_t pixelCounts1[LIW_DEFERRED_VISIBILITY_MAX_MATERIAL_COUNT];
		//glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_pixelPrefixSumBuffer);
		//userCounters1 = (GLuint*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeData, GL_MAP_READ_BIT);
		//memcpy_s(pixelCounts1, sizeData, userCounters1, sizeData);
		//glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		//glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}
	void OGLRenderer_DeferredVisibility::ShadingPass(LIWComponent_Camera& camera)
	{
		//
		// Setup
		//
		auto& assetManager = *LIWGlobal::GetAssetManager();
		auto& vBuffer = assetManager.GetFrameBuffer(m_frameBufferVBuffer);
		auto& presentBuffer = assetManager.GetFrameBuffer(m_frameBufferPresentBuffer);

		BindFrameBuffer(presentBuffer);
		// Clear
		glClearColor(0.1f, 0.2f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Upload buffers
		//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_MATERIAL_COUNTERS, m_pixelCountBuffer);
		//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_PREFIXSUM, m_pixelPrefixSumBuffer);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_VERTEX_BUFFER, m_vertexBuffer);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_INDEX_BUFFER, m_indexBuffer);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_MESH_ID_LOOKUP, m_meshIDLookupBuffer);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_INDEX_OFFSET_LOOKUP, m_indexOffsetLookupBuffer);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_MODEL_MATRICES, m_modelMatricesBuffer);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_NORMAL_MATRICES, m_normalMatricesBuffer);
		glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, m_indirectDispatchBuffer);

		//{
		//	const size_t size = sizeof(uint32_t) * LIW_DEFERRED_VISIBILITY_MAX_RESOLUTION;
		//	uint16_t* data = new uint16_t[LIW_DEFERRED_VISIBILITY_MAX_RESOLUTION * 2];
		//	void* userCounters;
		//	userCounters = glMapNamedBufferRange(m_workListBuffer, 0, size, GL_MAP_READ_BIT);
		//	memcpy_s(data, size, userCounters, size);
		//	glUnmapNamedBuffer(m_workListBuffer);
		//	delete[] data;
		//}

		UploadLightData();

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

		//{
		//	const size_t size = sizeof(uint32_t) * 3 * LIW_DEFERRED_VISIBILITY_MAX_MATERIAL_COUNT;
		//	uint32_t data[LIW_DEFERRED_VISIBILITY_MAX_MATERIAL_COUNT * 3];
		//	void* userCounters;
		//	userCounters = glMapNamedBufferRange(m_indirectDispatchBuffer, 0, size, GL_MAP_READ_BIT);
		//	memcpy_s(data, size, userCounters, size);
		//	glUnmapNamedBuffer(m_indirectDispatchBuffer);
		//}

		for (auto itrMaterial = m_materialList.begin(); itrMaterial != m_materialList.end(); ++itrMaterial) {
			const uint32_t idxMaterial = *itrMaterial;
			LIWMaterial& material = assetManager.GetMaterial(idxMaterial);
			auto& shaderProgram = assetManager.GetShaderProgram(material.m_handleShaderProgram);
			uint32_t rawHandleshaderProgram = shaderProgram.GetRawHandle();

			// Use program
			glUseProgram(rawHandleshaderProgram);

			// Bind uniform data
			material.BindData(1);

			LIWTexture::Bind2DTextureImage(vBuffer.GetColorAttachmentRawHandle(1),
				rawHandleshaderProgram, "indexImg", 0, LIWImageFormat_R32U,
				LIWTextureImageAccessType_Read, 0, -1);
			LIWTexture::Bind2DTextureImage(presentBuffer.GetColorAttachmentRawHandle(0),
				rawHandleshaderProgram, "outputImg", 1, LIWImageFormat_RGBA8,
				LIWTextureImageAccessType_Write, 0, -1);
			LIWTexture::Bind2DTexture(vBuffer.GetDepthAttachmentRawHandle(),
				rawHandleshaderProgram, "depthTexture", 0);
			//LIWTexture::Bind2DTexture(vBuffer.GetColorAttachmentRawHandle(1),
			//	rawHandleshaderProgram, "indexTexture", 1);
			const uint32_t indexImgLoc = glGetUniformLocation(rawHandleshaderProgram, "indexImg");
			const uint32_t depthTextureLoc = glGetUniformLocation(rawHandleshaderProgram, "depthTexture");
			const uint32_t dispatchIdxLoc = glGetUniformLocation(rawHandleshaderProgram, "dispatchIdx");
			const uint32_t pixelSizeLoc = glGetUniformLocation(rawHandleshaderProgram, "pixelSize");
			const uint32_t invProjViewLoc = glGetUniformLocation(rawHandleshaderProgram, "invProjView");
			const uint32_t vertexCountLoc = glGetUniformLocation(rawHandleshaderProgram, "vertexCount");

			glUniform1ui(dispatchIdxLoc, idxMaterial);
			glUniform2f(pixelSizeLoc, 1.0f / width, 1.0f / height);
			glUniformMatrix4fv(invProjViewLoc, 1, false, glm::value_ptr(inverseProjView));
			glUniform1ui(vertexCountLoc, m_vertexCount);


			glDispatchComputeIndirect((size_t)idxMaterial * 3 * sizeof(uint32_t));


			LIWTexture::Unbind2DTextureImage(0);
			LIWTexture::Unbind2DTextureImage(1);
			LIWTexture::Unbind2DTexture(0);

			material.UnbindData();
		}

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_VERTEX_BUFFER, 0);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_INDEX_BUFFER, 0);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_MESH_ID_LOOKUP, 0);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_INDEX_OFFSET_LOOKUP, 0);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_MODEL_MATRICES, 0);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_NORMAL_MATRICES, 0);
		glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, 0);
	}
	void OGLRenderer_DeferredVisibility::PresentPass()
	{
		//
		// Setup
		//
		auto& assetManager = *LIWGlobal::GetAssetManager();
		auto& presentBuffer = assetManager.GetFrameBuffer(m_frameBufferPresentBuffer);

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
		glDisable(GL_BLEND);

		// 
		// Present
		//
		auto& shaderProgram = assetManager.GetShaderProgram(m_presentShaderProgram);
		uint32_t rawHandleshaderProgram = shaderProgram.GetRawHandle();

		glUseProgram(rawHandleshaderProgram);

		LIWTexture::Bind2DTexture(presentBuffer.GetColorAttachmentRawHandle(0),
			rawHandleshaderProgram, "mainTex", 0);
		
		glBindVertexArray(m_emptyVAO);
		glDrawArrays(GL_TRIANGLES, 0, 3); //NOTE: only works with compatibility OpenGL profile

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
	void OGLRenderer_DeferredVisibility::UploadLightData()
	{
		auto& lightManager = LIW_ECS_GetComponentManager(LIWComponent_Light);
		auto& lights = lightManager.m_components;

		if (lights.get_size() > LIW_LIGHT_DEFERRED_VISIBILITY_MAX_PERPIXEL)
			throw liwexcept_not_implemented("For now can only support 8 perpixel lights. ");

		LIWDArray<glm::vec4, LIWMem_Frame> lightColours_Directional		{ LIW_LIGHT_DEFERRED_VISIBILITY_MAX_PERPIXEL, LIW_LIGHT_DEFERRED_VISIBILITY_MAX_PERPIXEL };
		LIWDArray<glm::vec4, LIWMem_Frame> lightColours_Point			{ LIW_LIGHT_DEFERRED_VISIBILITY_MAX_PERPIXEL, LIW_LIGHT_DEFERRED_VISIBILITY_MAX_PERPIXEL };
		LIWDArray<glm::vec4, LIWMem_Frame> lightColours_Spot			{ LIW_LIGHT_DEFERRED_VISIBILITY_MAX_PERPIXEL, LIW_LIGHT_DEFERRED_VISIBILITY_MAX_PERPIXEL };
		LIWDArray<glm::vec4, LIWMem_Frame> lightPositions_Directional	{ LIW_LIGHT_DEFERRED_VISIBILITY_MAX_PERPIXEL, LIW_LIGHT_DEFERRED_VISIBILITY_MAX_PERPIXEL };
		LIWDArray<glm::vec4, LIWMem_Frame> lightPositions_Point			{ LIW_LIGHT_DEFERRED_VISIBILITY_MAX_PERPIXEL, LIW_LIGHT_DEFERRED_VISIBILITY_MAX_PERPIXEL };
		LIWDArray<glm::vec4, LIWMem_Frame> lightPositions_Spot			{ LIW_LIGHT_DEFERRED_VISIBILITY_MAX_PERPIXEL, LIW_LIGHT_DEFERRED_VISIBILITY_MAX_PERPIXEL };
		LIWDArray<glm::vec4, LIWMem_Frame> lightParams_Directional		{ LIW_LIGHT_DEFERRED_VISIBILITY_MAX_PERPIXEL, LIW_LIGHT_DEFERRED_VISIBILITY_MAX_PERPIXEL };
		LIWDArray<glm::vec4, LIWMem_Frame> lightParams_Point			{ LIW_LIGHT_DEFERRED_VISIBILITY_MAX_PERPIXEL, LIW_LIGHT_DEFERRED_VISIBILITY_MAX_PERPIXEL };
		LIWDArray<glm::vec4, LIWMem_Frame> lightParams_Spot				{ LIW_LIGHT_DEFERRED_VISIBILITY_MAX_PERPIXEL, LIW_LIGHT_DEFERRED_VISIBILITY_MAX_PERPIXEL };
		int32_t lightCount_Directional = 0;
		int32_t lightCount_Point = 0;
		int32_t lightCount_Spot = 0;

		for (auto itr = lights.get_beg(); itr != lights.get_end(); itr++) {
			auto& light = *itr;
			liw_objhdl_type transLightHdl = LIW_ECS_GetComponentFromEntity(LIWComponent_Transform, light.GetEntity());
			auto& transLight = LIW_ECS_GetComponent(LIWComponent_Transform, transLightHdl);

			switch (light.m_lightType)
			{
			case LIW::LIWLightType_Directional: {
				lightColours_Directional[lightCount_Directional] = light.m_colourAndIntensity;
				lightPositions_Directional[lightCount_Directional] = glm::vec4(transLight.m_position, 1.0f);
				glm::vec3 forward = transLight.GetForward();
				forward = glm::normalize(forward);
				lightParams_Directional[lightCount_Directional] = glm::vec4(forward, 0.0f);

				lightCount_Directional++;
			}
											  break;
			case LIW::LIWLightType_Point: {
				lightColours_Point[lightCount_Point] = light.m_colourAndIntensity;
				lightPositions_Point[lightCount_Point] = glm::vec4(transLight.m_position, 1.0f);
				lightParams_Point[lightCount_Point] = glm::vec4(light.m_param.m_pointLight.m_radius, 0.0f, 0.0f, 0.0f);

				lightCount_Point++;
			}
										break;
			case LIW::LIWLightType_Spot: {
				lightColours_Spot[lightCount_Spot] = light.m_colourAndIntensity;
				lightPositions_Spot[lightCount_Spot] = glm::vec4(transLight.m_position, 1.0f);
				glm::vec3 forward = transLight.GetForward();
				forward = glm::normalize(forward);
				float cosHalfAngle = glm::cos(glm::radians(light.m_param.m_spotLight.m_angle * 0.5f));
				lightParams_Spot[lightCount_Spot] = glm::vec4(forward.x, forward.y,
					light.m_param.m_spotLight.m_radius * Maths::SignNoZero(forward.z), cosHalfAngle);

				lightCount_Spot++;
			}
									   break;
			default:
				break;
			}
		}

		const size_t sizeArray = sizeof(glm::vec4) * LIW_LIGHT_DEFERRED_VISIBILITY_MAX_PERPIXEL;
		size_t sizeData = sizeArray * 9 + sizeof(int32_t) * 3;
		glBindBuffer(GL_UNIFORM_BUFFER, m_lightBuffer);
		glBufferData(GL_UNIFORM_BUFFER, sizeData, nullptr, GL_STATIC_DRAW);

		sizeData = 0;
		glBufferSubData(GL_UNIFORM_BUFFER, sizeData, sizeArray, lightColours_Directional.get_data()); sizeData += sizeArray;
		glBufferSubData(GL_UNIFORM_BUFFER, sizeData, sizeArray, lightColours_Point.get_data()); sizeData += sizeArray;
		glBufferSubData(GL_UNIFORM_BUFFER, sizeData, sizeArray, lightColours_Spot.get_data()); sizeData += sizeArray;
		glBufferSubData(GL_UNIFORM_BUFFER, sizeData, sizeArray, lightPositions_Directional.get_data()); sizeData += sizeArray;
		glBufferSubData(GL_UNIFORM_BUFFER, sizeData, sizeArray, lightPositions_Point.get_data()); sizeData += sizeArray;
		glBufferSubData(GL_UNIFORM_BUFFER, sizeData, sizeArray, lightPositions_Spot.get_data()); sizeData += sizeArray;
		glBufferSubData(GL_UNIFORM_BUFFER, sizeData, sizeArray, lightParams_Directional.get_data()); sizeData += sizeArray;
		glBufferSubData(GL_UNIFORM_BUFFER, sizeData, sizeArray, lightParams_Point.get_data()); sizeData += sizeArray;
		glBufferSubData(GL_UNIFORM_BUFFER, sizeData, sizeArray, lightParams_Spot.get_data()); sizeData += sizeArray;
		glBufferSubData(GL_UNIFORM_BUFFER, sizeData, sizeof(int32_t), &lightCount_Directional); sizeData += sizeof(int32_t);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeData, sizeof(int32_t), &lightCount_Point); sizeData += sizeof(int32_t);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeData, sizeof(int32_t), &lightCount_Spot); sizeData += sizeof(int32_t);

		glBindBufferRange(GL_UNIFORM_BUFFER, LIW_SHADER_UBO_BIND_DEFERRED_VISIBILITY_PERPIX_LIGHTDATA, m_lightBuffer, 0, sizeData);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
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