#pragma once

#include "OGLRenderer.h"
#include "Maths/LIWMaths.h"
#include "Framework/LIWECSFunctional.h"
#include "Framework/Components/LIWComponent_Transform.h"
#include "Framework/Components/LIWComponent_MeshRenderer.h"
#include "Framework/Components/LIWComponent_Camera.h"

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
	private:
		uint32_t m_uboCameraData;
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