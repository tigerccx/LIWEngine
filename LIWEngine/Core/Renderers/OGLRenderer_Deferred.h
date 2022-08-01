#pragma once

#include "LIWMacros.h"
#include "OGLRenderer.h"
#include "Maths/LIWMaths.h"
#include "Framework/LIWECSFunctional.h"
#include "Threading/LIWThreadWorkerTask.h"
#include "Threading/LIWFiberTask.h"
#include "Threading/LIWFiberExecutor.h"
#include "Framework/Components/LIWComponent_Transform.h"
#include "Framework/Components/LIWComponent_MeshRenderer.h"
#include "Framework/Components/LIWComponent_Camera.h"
#include "Framework/Components/LIWComponent_Light.h"

namespace LIW{
	class OGLRenderer_Deferred final:
		public OGLRenderer
	{
	public:
		OGLRenderer_Deferred(LIW::App::Window& parent);
		~OGLRenderer_Deferred();
		void RenderScene() override;
	private:
		void RenderCamera(LIWComponent_Camera& camera);
		void GBufferPass();
		void LightBufferPass(LIWComponent_Camera& camera);
		void CombinePass();

		void UploadLightPassData(uint32_t rawHandleLightBufferShader, LIWComponent_Camera& camera);
		void UploadCameraData(LIWComponent_Camera& camera);
	private:
		uint32_t m_uboCameraData;
		uint32_t m_uboLightPerPixelData;
		uint32_t m_emptyVAO;

		liw_objhdl_type m_meshSphere;

		liw_objhdl_type m_frameBufferGBuffer{ liw_c_nullobjhdl };
		liw_objhdl_type m_frameBufferLightBuffer{ liw_c_nullobjhdl };

		liw_objhdl_type m_lightBufferShaderProgram{ liw_c_nullobjhdl };
		liw_objhdl_type m_combineShaderProgram{ liw_c_nullobjhdl };
	};

	class LIW_TT_OGLDeferredRender final :
		public LIWThreadWorkerTask
	{
	public:
		void Execute() override;
	public:
		LIWPointer<OGLRenderer_Deferred, LIWMem_Static> m_renderer;
	};
}