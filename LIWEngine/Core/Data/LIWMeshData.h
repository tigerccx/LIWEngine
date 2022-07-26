#pragma once
#include <unordered_map>

#include "Maths/LIWMaths.h"
#include "Containers/LIWDArray.h"

namespace LIW {
	enum LIWPreimitiveType {
		//LIWPreimitiveType_Points,
		//LIWPreimitiveType_Lines,
		LIWPreimitiveType_Triangles,
		//LIWPreimitiveType_TriangleFan,
		//LIWPreimitiveType_TriangleStrip,
		//LIWPreimitiveType_Patches,
		LIWPreimitiveType_Max
	};

	struct LIWSubMesh {
		uint32_t m_idxBeg;
		uint32_t m_idxEnd;
	};

	//NOTE: Assumes there will be index
	class LIWMeshData {
	public:
		void LoadMeshData_Obj(const char* objPath);
		void UnloadMeshData();
	public:
		inline bool IsValid() const { return m_primitiveType != LIWPreimitiveType_Max; }
		const LIWDArray<glm::vec3>& GetPositionArray() const { return m_positions; }
		const LIWDArray<glm::vec3>& GetNormalArray() const { return m_normals; }
		const LIWDArray<glm::vec2>& GetTexcoordArray() const { return m_texcoords; }
		const LIWDArray<glm::vec3>& GetColourArray() const { return m_colours; }
		const LIWDArray<uint32_t>& GetIndexArray() const { return m_indices; }
		const LIWDArray<LIWSubMesh>& GetSubMeshArray() const { return m_submeshes; }
		LIWPreimitiveType GetPrimitiveType() const { return m_primitiveType; }

	private:
		LIWDArray<glm::vec3>	m_positions{ 0 };
		LIWDArray<glm::vec3>	m_normals{ 0 };
		LIWDArray<glm::vec2>	m_texcoords{ 0 };
		LIWDArray<glm::vec3>	m_colours{ 0 };
		LIWDArray<uint32_t>		m_indices{ 0 };
		LIWDArray<LIWSubMesh>	m_submeshes{ 0 };
		LIWPreimitiveType m_primitiveType{ LIWPreimitiveType_Max };
	};
}