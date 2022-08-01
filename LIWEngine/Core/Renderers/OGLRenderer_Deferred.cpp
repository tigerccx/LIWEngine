#include "OGLRenderer_Deferred.h"
namespace LIW {
	OGLRenderer_Deferred::OGLRenderer_Deferred(LIW::App::Window& parent):
		OGLRenderer(parent)
	{
		glGenBuffers(1, &m_uboCameraData);
		glGenBuffers(1, &m_uboLightPerPixelData);
		glGenVertexArrays(1, &m_emptyVAO);

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


		m_lightBufferShaderProgram = assetManager.CreateShaderProgram("shaderProgram_lightBuffer");
		LIWShaderProgram& lightBufferShaderProgram = assetManager.GetShaderProgram(m_lightBufferShaderProgram);
		lightBufferShaderProgram.CreateShader({ lightBufferVertShader , lightBufferFragShader });

		m_combineShaderProgram = assetManager.CreateShaderProgram("shaderProgram_combine");
		LIWShaderProgram& combineShaderProgram = assetManager.GetShaderProgram(m_combineShaderProgram);
		combineShaderProgram.CreateShader({ screenQuadShader , combineShader });

		lightBufferVertShader.UnloadShader();
		lightBufferFragShader.UnloadShader();
		screenQuadShader.UnloadShader();
		combineShader.UnloadShader();
		assetManager.DestroyShader("shader_vert_lightBuffer");
		assetManager.DestroyShader("shader_frag_lightBuffer");
		assetManager.DestroyShader("shader_vert_screenQuad");
		assetManager.DestroyShader("shader_frag_combine");


		m_meshSphere = assetManager.GetMeshHandle(LIW_MESH_SPHERE_NAME);


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
	}
	OGLRenderer_Deferred::~OGLRenderer_Deferred()
	{
		auto& assetManager = *LIWGlobal::GetAssetManager();

		LIWFrameBuffer& lightBuffer = assetManager.GetFrameBuffer(m_frameBufferLightBuffer);
		lightBuffer.DestroyFrameBuffer();
		assetManager.DestroyFrameBuffer("framebuffer_lightbuffer");

		LIWFrameBuffer& gBuffer = assetManager.GetFrameBuffer(m_frameBufferGBuffer);
		gBuffer.DestroyFrameBuffer();
		assetManager.DestroyFrameBuffer("framebuffer_gbuffer");


		LIWShaderProgram& combineShaderProgram = assetManager.GetShaderProgram(m_combineShaderProgram);
		combineShaderProgram.DestroyShader();
		assetManager.DestroyShaderProgram("shaderProgram_combine");

		LIWShaderProgram& lightBufferShaderProgram = assetManager.GetShaderProgram(m_lightBufferShaderProgram);
		lightBufferShaderProgram.DestroyShader();
		assetManager.DestroyShaderProgram("shaderProgram_lightBuffer");


		glDeleteVertexArrays(1, &m_emptyVAO);
		glDeleteBuffers(1, &m_uboLightPerPixelData);
		glDeleteBuffers(1, &m_uboCameraData);
	}
	void OGLRenderer_Deferred::RenderScene()
	{
		// Render each camera
		auto& cameraManager = LIW_ECS_GetComponentManager(LIWComponent_Camera);
		auto& cameras = cameraManager.m_components;
		
		for (auto itr = cameras.get_beg(); itr != cameras.get_end(); itr++) {
			RenderCamera(*itr);
		}
	}
	void OGLRenderer_Deferred::RenderCamera(LIWComponent_Camera& camera)
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
		// GBuffer pass
		//
		GBufferPass();

		//
		// LightBuffer pass
		//
		LightBufferPass(camera);

		// Combine pass
		CombinePass();
	}
	void OGLRenderer_Deferred::GBufferPass()
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
	void OGLRenderer_Deferred::LightBufferPass(LIWComponent_Camera& camera)
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
			glm::mat4 matModel = transform.GetMatrix();
			glUniformMatrix4fv(glGetUniformLocation(lightBufferShaderProgram.GetRawHandle(), LIW_SHADER_MODEL_MATRIX_NAME), 1, false, glm::value_ptr(matModel));
			
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
	void OGLRenderer_Deferred::CombinePass()
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
	
	void OGLRenderer_Deferred::UploadLightPassData(uint32_t rawHandleLightBufferShader, LIWComponent_Camera& camera)
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
	void OGLRenderer_Deferred::UploadCameraData(LIWComponent_Camera& camera)
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
	void LIW_TT_OGLDeferredRender::Execute()
	{
		m_renderer->RenderScene();
		LIWFiberExecutor::m_executor.DecreaseSyncCounter(LIW_SYNC_COUNTER_RESERVE_RENDER, 1);
	}
}