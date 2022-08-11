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

#include <unordered_set>
#include <unordered_map>
#include <set>
#include <cassert>
#include <chrono>

namespace LIW{
	class OGLRenderer_DeferredVisibility final:
		public OGLRenderer
	{
	public:
		OGLRenderer_DeferredVisibility(LIW::App::Window& parent);
		~OGLRenderer_DeferredVisibility();
		void RenderScene() override;

	public:
		void AddRenderMaterial(liw_objhdl_type hdlMaterial);
		void RemoveRenderMaterial(liw_objhdl_type hdlMaterial);
	private:
		void RenderCamera(LIWComponent_Camera& camera);
		void VisibilityPass();
		void VisibilityDebugShowPass();
		void WorklistPass();
		void ShadingPass(LIWComponent_Camera& camera);
		void PresentPass();
		
		void UploadLightData();
		void UploadCameraData(LIWComponent_Camera& camera);

		/*Deprecated*/
		void GBufferPass();
		void LightBufferPass(LIWComponent_Camera& camera);
		void CombinePass();

		void UploadLightPassData(uint32_t rawHandleLightBufferShader, LIWComponent_Camera& camera);
	private:
		uint32_t m_vertexCount;

		uint32_t m_uboCameraData;
		uint32_t m_uboPointLightPerPixelData;
		//uint32_t m_ssboTileMaterialList;
		//uint32_t m_atomicTileMaterialListCount;
		uint32_t m_pixelCountBuffer;
		uint32_t m_pixelPrefixSumBuffer;
		uint32_t m_indirectDispatchBuffer;
		uint32_t m_workListBuffer;
		uint32_t m_indexOffsetLookupBuffer;
		uint32_t m_meshIDLookupBuffer;
		uint32_t m_modelMatricesBuffer;
		uint32_t m_normalMatricesBuffer;
		uint32_t m_vertexBuffer;
		uint32_t m_indexBuffer;
		uint32_t m_lightBuffer;

		uint32_t m_vao;
		uint32_t m_emptyVAO;

		liw_objhdl_type m_meshSphere;

		liw_objhdl_type m_frameBufferVBuffer{ liw_c_nullobjhdl };
		liw_objhdl_type m_frameBufferPresentBuffer{ liw_c_nullobjhdl };
		liw_objhdl_type m_frameBufferGBuffer{ liw_c_nullobjhdl };
		liw_objhdl_type m_frameBufferLightBuffer{ liw_c_nullobjhdl };

		liw_objhdl_type m_lightBufferShaderProgram{ liw_c_nullobjhdl };
		liw_objhdl_type m_combineShaderProgram{ liw_c_nullobjhdl };
		liw_objhdl_type m_visibilityShaderProgram{ liw_c_nullobjhdl };
		liw_objhdl_type m_visibilityShowShaderProgram{ liw_c_nullobjhdl };
		liw_objhdl_type m_worklistGenerateP0ShaderProgram{ liw_c_nullobjhdl };
		liw_objhdl_type m_worklistGenerateP1ShaderProgram{ liw_c_nullobjhdl };
		liw_objhdl_type m_worklistGenerateP2ShaderProgram{ liw_c_nullobjhdl };
		liw_objhdl_type m_presentShaderProgram{ liw_c_nullobjhdl };

		std::set<liw_objhdl_type> m_materialList{ };
	};

	class LIW_TT_OGLDeferredVisibilityRender final :
		public LIWThreadWorkerTask
	{
	public:
		void Execute() override;
	public:
		LIWPointer<OGLRenderer_DeferredVisibility, LIWMem_Static> m_renderer;
	};
}