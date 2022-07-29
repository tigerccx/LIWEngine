#pragma once
#include <unordered_map>

#include "LIWMacros.h"
#include "Maths/LIWMaths.h"
#include "Containers/LIWDArray.h"

namespace LIW {
	enum LIWPreimitiveType {
		//LIWPreimitive_Points,
		//LIWPreimitive_Lines,
		LIWPreimitiveType_Triangles,
		//LIWPreimitive_TriangleFan,
		//LIWPreimitive_TriangleStrip,
		//LIWPreimitive_Patches,
		LIWPreimitiveType_Max
	};

	struct LIWSubMesh {
		uint32_t m_idxBeg;
		uint32_t m_idxEnd;
	};

	//NOTE: Assumes there will be index
	class LIWMeshData {
	public:
		void LoadMeshData_Obj(const char* objPath, bool flipTexcoordV = true);
		void LoadMeshData_Msh(const char* objPath, bool flipTexcoordV = false);
		void UnloadMeshData();

		//NOTE: Can only be used for LIWPreimitiveType_Triangles
		bool GenerateNormals();
		//NOTE: Can only be used for LIWPreimitiveType_Triangles
		bool GenerateTangents();
		//NOTE: Can only be used for LIWPreimitiveType_Triangles
		inline uint32_t GetTriangleCount() const { return (uint32_t)m_indices.get_size() / 3; }
	public:
		inline bool IsValid() const { return m_primitiveType != LIWPreimitiveType_Max; }
		const LIWDArray<glm::vec3>& GetPositionArray() const { return m_positions; }
		const LIWDArray<glm::vec3>& GetNormalArray() const { return m_normals; }
		const LIWDArray<glm::vec2>& GetTexcoordArray() const { return m_texcoords; }
		const LIWDArray<glm::vec3>& GetColourArray() const { return m_colours; }
		const LIWDArray<uint32_t>& GetIndexArray() const { return m_indices; }
		const LIWDArray<LIWSubMesh>& GetSubMeshArray() const { return m_submeshes; }
		LIWPreimitiveType GetPrimitiveType() const { return m_primitiveType; }

	public:
		static void CreateCube(LIWMeshData& meshData);
		static void CreateSphere(LIWMeshData& meshData);
		static void CreatePlane(LIWMeshData& meshData);

	private:
		//NOTE: Can only be used for LIWPreimitiveType_Triangles
		inline void GetVertexIndicesFromTriangle(uint32_t triIdx, uint32_t& vertIdxOutA, uint32_t& vertIdxOutB, uint32_t& vertIdxOutC) {
			vertIdxOutA = m_indices[((size_t)triIdx * 3)];
			vertIdxOutB = m_indices[((size_t)triIdx * 3) + 1];
			vertIdxOutC = m_indices[((size_t)triIdx * 3) + 2];
		}
		glm::vec4 GernerateTangent(uint32_t vertIdxA, uint32_t vertIdxB, uint32_t vertIdxC);

	private:
		LIWDArray<glm::vec3>	m_positions{ 0 };
		LIWDArray<glm::vec3>	m_normals{ 0 };
		LIWDArray<glm::vec4>	m_tangents{ 0 };
		LIWDArray<glm::vec2>	m_texcoords{ 0 };
		LIWDArray<glm::vec3>	m_colours{ 0 };
		LIWDArray<uint32_t>		m_indices{ 0 };
		LIWDArray<LIWSubMesh>	m_submeshes{ 0 };
		LIWPreimitiveType m_primitiveType{ LIWPreimitiveType_Max };
	};
}