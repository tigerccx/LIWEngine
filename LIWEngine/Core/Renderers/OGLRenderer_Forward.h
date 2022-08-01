#pragma once

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
	class OGLRenderer_Forward final:
		public OGLRenderer
	{
	public:
		OGLRenderer_Forward(LIW::App::Window& parent);
		~OGLRenderer_Forward();
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
		LIWPointer<OGLRenderer_Forward, LIWMem_Static> m_renderer;
	};
}