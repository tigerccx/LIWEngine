#include "OGLRenderer_Forward.h"
namespace LIW {
	OGLRenderer_Forward::OGLRenderer_Forward(LIW::App::Window& parent):
		OGLRenderer(parent)
	{
		glGenBuffers(1, &m_uboCameraData);
		glGenBuffers(1, &m_uboLightPerPixelData);
		glGenVertexArrays(1, &m_emptyVAO);

		auto& assetManager = *LIWGlobal::GetAssetManager();
		
		m_screenQuadShader = assetManager.CreateShader("shader_vert_screenQuad");
		LIWShader& shader = assetManager.GetShader(m_screenQuadShader);
		shader.LoadShader(LIW_PATH_DIR_DEFAULT_SHADERS"OGL/LIW_ScreenQuad_Vert.glsl", LIWShaderType_Vertex);

		m_screenQuadTestShader = assetManager.CreateShader("shader_frag_screenQuadTest");
		LIWShader& shaderFrag = assetManager.GetShader(m_screenQuadTestShader);
		shaderFrag.LoadShader(LIW_PATH_DIR_DEFAULT_SHADERS"OGL/ScreenQuadTest_Frag.glsl", LIWShaderType_Fragment);
	
		m_screenQuadTestShaderProgram = assetManager.CreateShaderProgram("shaderProgram_screenQuadTest");
		LIWShaderProgram& shaderProgram = assetManager.GetShaderProgram(m_screenQuadTestShaderProgram);
		shaderProgram.CreateShader({ shader , shaderFrag });


		m_frameBuffer0 = assetManager.CreateFrameBuffer("framebuffer_0");
		auto& frameBuffer = assetManager.GetFrameBuffer(m_frameBuffer0);
		frameBuffer.CreateFrameBuffer(256, 256, LIW_FRAMEBUFFER_ATTACHMENT_FLAG_COLOR_RGB);
	}
	OGLRenderer_Forward::~OGLRenderer_Forward()
	{
		auto& assetManager = *LIWGlobal::GetAssetManager();

		LIWFrameBuffer& frameBuffer = assetManager.GetFrameBuffer(m_frameBuffer0);
		frameBuffer.DestroyFrameBuffer();
		assetManager.DestroyFrameBuffer("framebuffer_0");

		LIWShaderProgram& shaderProgram = assetManager.GetShaderProgram(m_screenQuadTestShaderProgram);
		shaderProgram.DestroyShader();
		assetManager.DestroyShaderProgram("shaderProgram_screenQuadTest");

		LIWShader& shaderFrag = assetManager.GetShader(m_screenQuadTestShader);
		shaderFrag.UnloadShader();
		assetManager.DestroyShader("shader_frag_screenQuadTest");
		
		LIWShader& shader = assetManager.GetShader(m_screenQuadShader);
		shader.UnloadShader();
		assetManager.DestroyShader("shader_vert_screenQuad");

		glDeleteVertexArrays(1, &m_emptyVAO);
		glDeleteBuffers(1, &m_uboLightPerPixelData);
		glDeleteBuffers(1, &m_uboCameraData);
	}
	void OGLRenderer_Forward::RenderScene()
	{
		// Calculate and upload light data
		UploadLightData();

		// Render each camera
		auto& cameraManager = LIW_ECS_GetComponentManager(LIWComponent_Camera);
		auto& cameras = cameraManager.m_components;
		
		for (auto itr = cameras.get_beg(); itr != cameras.get_end(); itr++) {
			RenderCamera(*itr);
		}
	}
	void OGLRenderer_Forward::RenderCamera(LIWComponent_Camera& camera)
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

		//TestDraw FrameBuffer
		auto& frameBuffer = assetManager.GetFrameBuffer(m_frameBuffer0);
		BindFrameBuffer(frameBuffer);
		RenderScreenQuad();
		BindDefaultFrameBuffer();

		//
		// Calculate and upload camera data
		//
		UploadCameraData(camera); 

		//
		// Render MeshRenderers
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

			//NOTE: the commented code is replaced by shader-side binding (supported by GLSL 420)
			//// Bind camera data
			//uint32_t cameraDataIdx = glGetUniformBlockIndex(rawHandleshaderProgram, LIW_SHADER_UB_CAMERADATA_NAME);
			//glUniformBlockBinding(rawHandleshaderProgram, cameraDataIdx, LIW_SHADER_UBO_BIND_CAMERADATA);

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
	void OGLRenderer_Forward::RenderScreenQuad()
	{
		auto& assetManager = *LIWGlobal::GetAssetManager();
		auto& shaderProgram = assetManager.GetShaderProgram(m_screenQuadTestShaderProgram);
		
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		GLboolean isDepTestOn = false;
		glGetBooleanv(GL_DEPTH_TEST, &isDepTestOn);
		glDisable(GL_DEPTH_TEST);

		glUseProgram(shaderProgram.GetRawHandle());

		glBindVertexArray(m_emptyVAO);
		glDrawArrays(GL_TRIANGLES, 0, 3); //NOTE: only works with compatibility OpenGL profile

		if (isDepTestOn)
			glEnable(GL_DEPTH_TEST);

	}
	void OGLRenderer_Forward::UploadLightData()
	{
		auto& lightManager = LIW_ECS_GetComponentManager(LIWComponent_Light);
		auto& lights = lightManager.m_components;

		if (lights.get_size() > LIW_LIGHT_FORWARD_MAX_PERPIXEL)
			throw liwexcept_not_implemented("For now can only support 8 perpixel lights. ");

		glm::vec4 lightColours_Directional[LIW_LIGHT_FORWARD_MAX_PERPIXEL];
		glm::vec4 lightColours_Point[LIW_LIGHT_FORWARD_MAX_PERPIXEL];
		glm::vec4 lightColours_Spot[LIW_LIGHT_FORWARD_MAX_PERPIXEL];
		glm::vec4 lightPositions_Directional[LIW_LIGHT_FORWARD_MAX_PERPIXEL];
		glm::vec4 lightPositions_Point[LIW_LIGHT_FORWARD_MAX_PERPIXEL];
		glm::vec4 lightPositions_Spot[LIW_LIGHT_FORWARD_MAX_PERPIXEL];
		glm::vec4 lightParams_Directional[LIW_LIGHT_FORWARD_MAX_PERPIXEL];
		glm::vec4 lightParams_Point[LIW_LIGHT_FORWARD_MAX_PERPIXEL];
		glm::vec4 lightParams_Spot[LIW_LIGHT_FORWARD_MAX_PERPIXEL];
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

			const size_t sizeArray = sizeof(glm::vec4) * LIW_LIGHT_FORWARD_MAX_PERPIXEL;
			size_t sizeData = sizeArray * 9 + sizeof(int32_t) * 3;
			glBindBuffer(GL_UNIFORM_BUFFER, m_uboLightPerPixelData);
			glBufferData(GL_UNIFORM_BUFFER, sizeData, nullptr, GL_STATIC_DRAW);

			sizeData = 0;
			glBufferSubData(GL_UNIFORM_BUFFER, sizeData, sizeArray, lightColours_Directional); sizeData += sizeArray;
			glBufferSubData(GL_UNIFORM_BUFFER, sizeData, sizeArray, lightColours_Point); sizeData += sizeArray;
			glBufferSubData(GL_UNIFORM_BUFFER, sizeData, sizeArray, lightColours_Spot); sizeData += sizeArray;
			glBufferSubData(GL_UNIFORM_BUFFER, sizeData, sizeArray, lightPositions_Directional); sizeData += sizeArray;
			glBufferSubData(GL_UNIFORM_BUFFER, sizeData, sizeArray, lightPositions_Point); sizeData += sizeArray;
			glBufferSubData(GL_UNIFORM_BUFFER, sizeData, sizeArray, lightPositions_Spot); sizeData += sizeArray;
			glBufferSubData(GL_UNIFORM_BUFFER, sizeData, sizeArray, lightParams_Directional); sizeData += sizeArray;
			glBufferSubData(GL_UNIFORM_BUFFER, sizeData, sizeArray, lightParams_Point); sizeData += sizeArray;
			glBufferSubData(GL_UNIFORM_BUFFER, sizeData, sizeArray, lightParams_Spot); sizeData += sizeArray;
			glBufferSubData(GL_UNIFORM_BUFFER, sizeData, sizeof(int32_t), &lightCount_Directional); sizeData += sizeof(int32_t);
			glBufferSubData(GL_UNIFORM_BUFFER, sizeData, sizeof(int32_t), &lightCount_Point); sizeData += sizeof(int32_t);
			glBufferSubData(GL_UNIFORM_BUFFER, sizeData, sizeof(int32_t), &lightCount_Spot); sizeData += sizeof(int32_t);
			
			glBindBufferRange(GL_UNIFORM_BUFFER, LIW_SHADER_UBO_BIND_FORWARD_PERPIX_LIGHTDATA, m_uboLightPerPixelData, 0, sizeData);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}
	}
	void OGLRenderer_Forward::UploadCameraData(LIWComponent_Camera& camera)
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

		size_t sizeData = 2 * sizeof(glm::mat4) + sizeof(glm::vec3);
		glBindBuffer(GL_UNIFORM_BUFFER, m_uboCameraData);
		glBufferData(GL_UNIFORM_BUFFER, sizeData, nullptr, GL_STATIC_DRAW);

		sizeData = 0;
		glBufferSubData(GL_UNIFORM_BUFFER, sizeData, sizeof(glm::mat4), glm::value_ptr(matView)); sizeData += sizeof(glm::mat4);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeData, sizeof(glm::mat4), glm::value_ptr(matProj)); sizeData += sizeof(glm::mat4);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeData, sizeof(glm::vec3), glm::value_ptr(transCam.m_position)); sizeData += sizeof(glm::vec3);
		
		glBindBufferRange(GL_UNIFORM_BUFFER, LIW_SHADER_UBO_BIND_CAMERADATA, m_uboCameraData, 0, sizeData);
		//glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
	void LIW_TT_OGLForwardRender::Execute()
	{
		m_renderer->RenderScene();
		LIWFiberExecutor::m_executor.DecreaseSyncCounter(LIW_SYNC_COUNTER_RESERVE_RENDER, 1);
	}
}