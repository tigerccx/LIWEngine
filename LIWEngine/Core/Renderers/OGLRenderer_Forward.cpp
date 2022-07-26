#include "OGLRenderer_Forward.h"
namespace LIW {
	OGLRendererForward::OGLRendererForward(LIW::App::Window& parent):
		OGLRenderer(parent)
	{
		glGenBuffers(1, &m_uboCameraData);

	}
	OGLRendererForward::~OGLRendererForward()
	{
		glDeleteBuffers(1, &m_uboCameraData);
	}
	void OGLRendererForward::RenderScene()
	{
		auto& cameraManager = LIW_ECS_GetComponentManager(LIWComponent_Camera);
		auto& cameras = cameraManager.m_components;
		
		for (auto itr = cameras.get_beg(); itr != cameras.get_end(); itr++) {
			RenderCamera(*itr);
		}
	}
	void OGLRendererForward::RenderCamera(LIWComponent_Camera& camera)
	{
		//
		// Set render states
		//
		// Set Framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// Set clear
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.1f, 0.2f, 1.0f);
		//Enable z-test
		glEnable(GL_DEPTH_TEST);

		//Enable back culling
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		//Enable blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//
		// Calculate and upload camera data
		//
		auto transCamHdl = LIW_ECS_GetComponentFromEntity(LIWComponent_Transform, camera.GetEntity());
		LIWComponent_Transform& transCam = LIW_ECS_GetComponent(LIWComponent_Transform, transCamHdl);
		const glm::vec3 camForward = transCam.GetForward();
		const glm::vec3 camUp = transCam.GetUp();
		glm::mat4 matView = glm::lookAt(transCam.m_location, transCam.m_location + camForward, camUp);
		glm::mat4 matProj = glm::perspective(
			glm::radians(camera.m_param.m_perspective.fovY),
			camera.m_param.m_perspective.aspectRatio,
			camera.m_near, camera.m_far);
		const size_t sizeCameraData = 2 * sizeof(glm::mat4);
		glBindBuffer(GL_UNIFORM_BUFFER, m_uboCameraData);
		glBufferData(GL_UNIFORM_BUFFER, sizeCameraData, nullptr, GL_STATIC_DRAW);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(matView));
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(matProj));
		glBindBufferRange(GL_UNIFORM_BUFFER, LIW_SHADER_UBO_BIND_CAMERADATA, m_uboCameraData, 0, sizeCameraData);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

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
			
			auto& material = LIWGlobal::GetAssetManager()->GetMaterial(meshRenderer.m_handleMaterial);
			auto& mesh = LIWGlobal::GetAssetManager()->GetMesh(meshRenderer.m_handleMesh);

			auto& shaderProgram = LIWGlobal::GetAssetManager()->GetShaderProgram(material.m_handleShaderProgram);
			uint32_t rawHandleshaderProgram = shaderProgram.GetRawHandle();
			
			// Use program
			glUseProgram(rawHandleshaderProgram);

			// Bind camera data
			uint32_t indexUBCameraData = glGetUniformBlockIndex(rawHandleshaderProgram, LIW_SHADER_UB_NAME_CAMERADATA);
			glUniformBlockBinding(rawHandleshaderProgram, indexUBCameraData, LIW_SHADER_UBO_BIND_CAMERADATA);

			// Bind material data
			material.BindData();

			// Upload transform
			glm::mat4 matModel = transform.GetMatrix();
			glUniformMatrix4fv(glGetUniformLocation(rawHandleshaderProgram, LIW_SHADER_MODEL_MATRIX), 1, false, glm::value_ptr(matModel));

			// Draw
			auto& submeshes = mesh.GetSubmeshes();
			auto mode = LIWImageFormat_2_GLPrimitive.at(mesh.GetPrimitiveType());
			glBindVertexArray(mesh.GetHandleVAO());
			for (int i = 0; i < submeshes.get_size(); ++i) {
				size_t size = submeshes[i].m_idxEnd - submeshes[i].m_idxBeg;
				const GLvoid* offset = (const GLvoid*)(submeshes[i].m_idxBeg * sizeof(unsigned int));
				glDrawElements(mode,
					size,
					GL_UNSIGNED_INT,
					offset);
			}

			// Unbind material data
			material.UnbindData();
		}
	}
	void LIW_TT_OGLForwardRender::Execute()
	{
		m_renderer->RenderScene();
		LIWFiberExecutor::m_executor.DecreaseSyncCounter(LIW_SYNC_COUNTER_RESERVE_RENDER, 1);
	}
}