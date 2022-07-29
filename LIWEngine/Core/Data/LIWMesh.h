#pragma once
#include <unordered_map>

#include <tiny_obj_loader.h>

#include "LIWglfw.h"

#include "LIWMacros.h"
#include "Maths/LIWMaths.h"
#include "Containers/LIWDArray.h"
#include "LIWMeshData.h"

namespace LIW {
	extern const std::unordered_map<LIWPreimitiveType, GLenum> LIWImageFormat_2_GLPrimitive;

	class LIWMesh {
	public:
		static const uint32_t sc_invalidHandle = UINT32_MAX;
	public:
		void CreateMesh(const LIWMeshData& meshData);
		void DestroyMesh();
	public:
		inline bool IsValid() const { return m_handleVAO != sc_invalidHandle; }
		inline uint32_t GetHandleVAO() const { return m_handleVAO; }
		inline uint32_t GetHandleBuffer() const { return m_handleBuffer; }
		inline LIWPreimitiveType GetPrimitiveType() const { return m_primitiveType; }
		inline const LIWDArray<LIWSubMesh>& GetSubmeshes() const { return m_submeshes; }

	private:
		uint32_t m_handleVAO{ sc_invalidHandle };
		uint32_t m_handleBuffer{ sc_invalidHandle };
		uint32_t m_handleIndexBuffer{ sc_invalidHandle };
		LIWPreimitiveType m_primitiveType{ LIWPreimitiveType_Max };
		LIWDArray<LIWSubMesh> m_submeshes{ 0 };
	};
}