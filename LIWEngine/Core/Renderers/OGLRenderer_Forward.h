#pragma once

#include "OGLRenderer.h"
#include "Maths/LIWMaths.h"
#include "Framework/LIWECSFunctional.h"
#include "Framework/Components/LIWComponent_Transform.h"
#include "Framework/Components/LIWComponent_MeshRenderer.h"
#include "Framework/Components/LIWComponent_Camera.h"
#include "Framework/Components/LIWComponent_Light.h"

namespace LIW{
	class OGLRendererForward final:
		public OGLRenderer
	{
	public:
		OGLRendererForward(LIW::App::Window& parent);
		~OGLRendererForward();
		void RenderScene() override;
	private:
		void RenderCamera(LIWComponent_Camera& camera);
		void RenderScreenQuad(); //Test

		void UploadLightData();
		void UploadCameraData(LIWComponent_Camera& camera);
	private:
		uint32_t m_uboCameraData;
		uint32_t m_uboLightPerPixelData;
		uint32_t m_emptyVAO;

		liw_objhdl_type m_frameBuffer0{ liw_c_nullobjhdl };

		liw_objhdl_type m_screenQuadShader{ liw_c_nullobjhdl };
		liw_objhdl_type m_screenQuadTestShader{ liw_c_nullobjhdl };
		liw_objhdl_type m_screenQuadTestShaderProgram{ liw_c_nullobjhdl };
	};

	class LIW_TT_OGLForwardRender final :
		public LIWThreadWorkerTask
	{
	public:
		void Execute() override;
	public:
		LIWPointer<OGLRendererForward, LIWMem_Static> m_renderer;
	};
}