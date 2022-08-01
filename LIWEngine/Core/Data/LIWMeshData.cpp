#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#define MSHLOADER_IMPLEMENTATION
#include <msh_loader.hpp>

#include "LIWMeshData.h"

namespace LIW {
	void LIWMeshData::LoadMeshData_Obj(const char* objPath, bool flipTexcoordV)
	{
		//TODO: for now the loading is taking up more space than required (repeated vertex...)
		if (IsValid())
			throw std::runtime_error("meshdata already loaded. ");

		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, objPath)) { //Auto triangulation enabled as default
			throw std::runtime_error(warn + err);
		}

		uint32_t indexCount = 0;
		uint32_t submeshCount = 0;
		for (const auto& shape : shapes) {
			indexCount += (uint32_t)shape.mesh.indices.size();
			submeshCount++;
		}
		m_indices.set_capacity(indexCount);
		m_submeshes.set_capacity(submeshCount);
		bool useNormal = false;
		bool useTexcoord = false;
		bool useColour = false;
		if ((uint32_t)attrib.normals.size() > 0) {
			useNormal = true; m_normals.set_capacity(indexCount);
		}
		if ((uint32_t)attrib.texcoords.size() > 0) {
			useTexcoord = true; m_texcoords.set_capacity(indexCount);
		}
		if ((uint32_t)attrib.colors.size() > 0) {
			useTexcoord = true; m_colours.set_capacity(indexCount);
		}

		uint32_t indexNew = 0;
		for (const auto& shape : shapes) {
			const uint32_t indexNewBeg = indexNew;
			for (const auto& index : shape.mesh.indices) {
				m_indices.push_back(indexNew);
				// Position
				m_positions.push_back(glm::vec3(
					attrib.vertices[(size_t)index.vertex_index * 3],
					attrib.vertices[(size_t)index.vertex_index * 3 + 1],
					attrib.vertices[(size_t)index.vertex_index * 3 + 2]
				));
				// Normal
				if (useNormal) {
					m_normals.push_back(glm::vec3(
						attrib.normals[(size_t)index.normal_index * 3],
						attrib.normals[(size_t)index.normal_index * 3 + 1],
						attrib.normals[(size_t)index.normal_index * 3 + 2]
					));
				}
				// Texcoord
				if (useTexcoord) {
					m_texcoords.push_back(glm::vec2(
						attrib.texcoords[(size_t)index.texcoord_index * 2],
						flipTexcoordV ? 1.0f - attrib.texcoords[(size_t)index.texcoord_index * 2 + 1] : attrib.texcoords[(size_t)index.texcoord_index * 2 + 1]
					));
				}
				// Colour
				if (useColour) { //TODO: not sure if this is correct..
					m_colours.push_back(glm::vec3(
						attrib.colors[(size_t)index.vertex_index * 3],
						attrib.colors[(size_t)index.vertex_index * 3 + 1],
						attrib.colors[(size_t)index.vertex_index * 3 + 2]
					));
				}
				indexNew++;
			}
			m_submeshes.push_back(LIWSubMesh{ indexNewBeg , indexNew });
		}

		m_primitiveType = LIWPreimitiveType_Triangles;

		if (m_indices.get_size() <= 0)
			throw std::runtime_error("No index array generated. \n");
	}
	void LIWMeshData::LoadMeshData_Msh(const char* objPath, bool flipTexcoordV)
	{
		if (IsValid())
			throw std::runtime_error("meshdata already loaded. ");

		std::vector<float> readPositions;
		std::vector<float> readColours;
		std::vector<float> readNormals;
		std::vector<float> readTangents;
		std::vector<float> readUVs;
		std::vector<uint32_t> readIndices;
		std::vector<uint32_t> readSubMeshes;
		if (!msh::load_from_msh_file(objPath,
			&readPositions,
			&readColours,
			&readNormals,
			&readTangents,
			&readUVs,
			&readIndices,
			&readSubMeshes)) {
			throw std::runtime_error("unable to load msh file. \n");
		}
		const size_t vertCount = readPositions.size() / 3;
		const size_t indexCount = readIndices.size();
		const size_t subMeshCount = readSubMeshes.size() / 2;

		if (vertCount == 0)
			throw std::runtime_error("msh file contains no verts. \n");
		if (indexCount == 0)
			throw liwexcept_not_implemented("msh file contains no index. Not supported for now. \n");

		m_positions.set_capacity(vertCount);
		for (size_t i = 0; i < vertCount; i++) {
			m_positions.push_back(glm::vec3(
				readPositions[3 * i],
				readPositions[3 * i + 1],
				readPositions[3 * i + 2]
			));
		}
		if (readNormals.size() > 0) {
			m_normals.set_capacity(vertCount);
			for (size_t i = 0; i < vertCount; i++) {
				m_normals.push_back(glm::vec3(
					readNormals[3 * i],
					readNormals[3 * i + 1],
					readNormals[3 * i + 2]
				));
			}
		}
		if (readTangents.size() > 0) {
			m_tangents.set_capacity(vertCount);
			for (size_t i = 0; i < vertCount; i++) {
				m_tangents.push_back(glm::vec4(
					readTangents[4 * i],
					readTangents[4 * i + 1],
					readTangents[4 * i + 2],
					readTangents[4 * i + 3]
				));
			}
		}
		if (readUVs.size() > 0) {
			m_texcoords.set_capacity(vertCount);
			for (size_t i = 0; i < vertCount; i++) {
				m_texcoords.push_back(glm::vec2(
					readUVs[2 * i],
					flipTexcoordV ? 1.0f - readUVs[2 * i + 1]: readUVs[2 * i + 1]
				));
			}
		}
		if (readColours.size() > 0) {
			m_colours.set_capacity(vertCount);
			for (size_t i = 0; i < vertCount; i++) {
				m_colours.push_back(glm::vec3(
					readColours[4 * i],
					readColours[4 * i + 1],
					readColours[4 * i + 2]
				));
			}
		}
		if (indexCount > 0) {
			m_indices.set_capacity(indexCount);
			for (size_t i = 0; i < indexCount; i++) {
				m_indices.push_back(readIndices[i]);
			}
		}
		if (subMeshCount > 0) {
			m_submeshes.set_capacity(subMeshCount);
			for (size_t i = 0; i < subMeshCount; i++) {
				m_submeshes.push_back(LIWSubMesh{ 
					readSubMeshes[2 * i], 
					readSubMeshes[2 * i + 1] 
				});
			}
		}
		else
		{
			m_submeshes.expand_size(1);
			m_submeshes[0].m_idxBeg = 0;
			m_submeshes[0].m_idxEnd = indexCount;
		}

		m_primitiveType = LIWPreimitiveType_Triangles;
	}

	void LIWMeshData::UnloadMeshData()
	{
		if (!IsValid())
			throw std::runtime_error("meshdata not loaded. ");
		m_positions.clear();
		m_positions.set_capacity(0);
		m_normals.clear();
		m_normals.set_capacity(0);
		m_tangents.clear();
		m_tangents.set_capacity(0);
		m_texcoords.clear();
		m_texcoords.set_capacity(0);
		m_colours.clear();
		m_colours.set_capacity(0);
		m_indices.clear();
		m_indices.set_capacity(0);
		m_submeshes.clear();
		m_submeshes.set_capacity(0);
		m_primitiveType = LIWPreimitiveType_Max;
	}
	bool LIWMeshData::GenerateNormals()
	{
		if (!IsValid())
			throw std::runtime_error("meshdata not loaded. \n");
		if (m_primitiveType != LIWPreimitiveType_Triangles)
			throw liwexcept_not_implemented("Now only support primitive type: triangles. \n");

		m_normals.clear();
		uint32_t countVertices = m_positions.get_size();
		m_normals.set_capacity((size_t)countVertices);
		
		for (size_t i = 0; i < (size_t)countVertices; i++) {
			m_normals.push_back(glm::zero<glm::vec3>());
		}

		uint32_t triCount = GetTriangleCount();

		for (uint32_t triIdx = 0; triIdx < triCount; triIdx++) {
			uint32_t vertIdxA, vertIdxB, vertIdxC;
			GetVertexIndicesFromTriangle(triIdx, vertIdxA, vertIdxB, vertIdxC);
			glm::vec3 normal = glm::cross(m_positions[vertIdxA] - m_positions[vertIdxB], m_positions[vertIdxC] - m_positions[vertIdxB]);
			
			m_normals[vertIdxA] += normal;
			m_normals[vertIdxB] += normal;
			m_normals[vertIdxC] += normal;
		}

		for (size_t i = 0; i < (size_t)countVertices; i++) {
			m_normals[i] = glm::normalize(m_normals[i]);
		}

		return true;
	}
	bool LIWMeshData::GenerateTangents()
	{
		if (!IsValid())
			throw std::runtime_error("meshdata not loaded. \n");
		if (m_primitiveType != LIWPreimitiveType_Triangles)
			throw liwexcept_not_implemented("Now only support primitive type: triangles. \n");

		if (m_texcoords.get_size() <= 0)
			return false;

		m_tangents.clear();
		uint32_t countVertices = m_positions.get_size();
		m_tangents.set_capacity((size_t)countVertices);

		for (size_t i = 0; i < (size_t)countVertices; i++) {
			m_tangents.push_back(glm::zero<glm::vec4>());
		}

		uint32_t triCount = GetTriangleCount();

		for (uint32_t triIdx = 0; triIdx < triCount; triIdx++) {
			uint32_t vertIdxA, vertIdxB, vertIdxC;
			GetVertexIndicesFromTriangle(triIdx, vertIdxA, vertIdxB, vertIdxC);
			glm::vec4 tangent = GenerateTangent(vertIdxA, vertIdxB, vertIdxC);
			m_tangents[vertIdxA] += tangent;
			m_tangents[vertIdxB] += tangent;
			m_tangents[vertIdxC] += tangent;
		}

		for (size_t i = 0; i < (size_t)countVertices; i++) {
			float handedness = m_tangents[i].w > 0.0f ? 1.0f : -1.0f;
			m_tangents[i].w = 0.0f;
			m_tangents[i] = glm::normalize(m_tangents[i]);
			m_tangents[i].w = handedness;
		}

		return true;
	}
	void LIWMeshData::CreateCube(LIWMeshData& meshData)
	{
		if (meshData.IsValid())
			throw std::runtime_error("meshdata already loaded. empty meshdata required. \n");
		
		meshData.m_positions.expand_size(24);
		meshData.m_indices.expand_size(36);
		meshData.m_texcoords.expand_size(24);
		meshData.m_colours.expand_size(24);

		uint32_t idx = 0;
		uint32_t idxIndice = 0;

		for (float z : {-0.5, 0.5}) {
			for (float y : {-0.5, 0.5}) {
				for (float x : {-0.5, 0.5}) {
					uint32_t idx_ = idx++;
					float s = Maths::Sign(z);
					meshData.m_positions[idx_] = glm::vec3(s * x, y, z);
					meshData.m_texcoords[idx_] = glm::vec2(x + 0.5, y + 0.5);
					meshData.m_colours[idx_] = glm::one<glm::vec4>();
				}
			}
			meshData.m_indices[idxIndice++] = idx - 2;
			meshData.m_indices[idxIndice++] = idx - 3;
			meshData.m_indices[idxIndice++] = idx - 1;

			meshData.m_indices[idxIndice++] = idx - 3;
			meshData.m_indices[idxIndice++] = idx - 2;
			meshData.m_indices[idxIndice++] = idx - 4;
		}

		for (float x : {-0.5, 0.5}) {
			for (float y : {-0.5, 0.5}) {
				for (float z : {-0.5, 0.5}) {
					uint32_t idx_ = idx++;
					float s = Maths::Sign(-x);
					meshData.m_positions[idx_] = glm::vec3(x, y, s * z);
					meshData.m_texcoords[idx_] = glm::vec2(z + 0.5, y + 0.5);
					meshData.m_colours[idx_] = glm::one<glm::vec4>();
				}
			}
			meshData.m_indices[idxIndice++] = idx - 2;
			meshData.m_indices[idxIndice++] = idx - 3;
			meshData.m_indices[idxIndice++] = idx - 1;
			
			meshData.m_indices[idxIndice++] = idx - 3;
			meshData.m_indices[idxIndice++] = idx - 2;
			meshData.m_indices[idxIndice++] = idx - 4;
		}

		for (float y : {-0.5, 0.5}) {
			for (float z : {0.5, -0.5}) {
				for (float x : {-0.5, 0.5}) {
					uint32_t idx_ = idx++;
					float s = Maths::Sign(y);
					meshData.m_positions[idx_] = glm::vec3(x, y, s * z);
					meshData.m_texcoords[idx_] = glm::vec2(x + 0.5, -z + 0.5);
					meshData.m_colours[idx_] = glm::one<glm::vec4>();
				}
			}
			meshData.m_indices[idxIndice++] = idx - 2;
			meshData.m_indices[idxIndice++] = idx - 3;
			meshData.m_indices[idxIndice++] = idx - 1;
			
			meshData.m_indices[idxIndice++] = idx - 3;
			meshData.m_indices[idxIndice++] = idx - 2;
			meshData.m_indices[idxIndice++] = idx - 4;
		}

		meshData.m_submeshes.push_back(LIWSubMesh{ 0, idxIndice });
		meshData.m_primitiveType = LIWPreimitiveType_Triangles;

		meshData.GenerateNormals();
		meshData.GenerateTangents();
	}
	void LIWMeshData::CreateSphere(LIWMeshData& meshData)
	{
		if (meshData.IsValid())
			throw std::runtime_error("meshdata already loaded. empty meshdata required. \n");
		//meshData.LoadMeshData_Obj(LIW_PATH_DIR_DEFAULT_MESHES"sphere.obj");
		meshData.LoadMeshData_Msh(LIW_PATH_DIR_DEFAULT_MESHES"sphere.msh"); //Model radius = 1
		for (size_t i = 0; i < meshData.m_positions.get_size(); i++) { //normalize to radius = 0.5
			meshData.m_positions[i] /= 2.0f;
		}
		meshData.GenerateTangents();
	}
	void LIWMeshData::CreatePlane(LIWMeshData& meshData)
	{
		if (meshData.IsValid())
			throw std::runtime_error("meshdata already loaded. empty meshdata required. \n");

		meshData.m_positions.expand_size(4);
		meshData.m_indices.expand_size(6);
		meshData.m_normals.expand_size(4);
		meshData.m_tangents.expand_size(4);
		meshData.m_texcoords.expand_size(4);
		meshData.m_colours.expand_size(4);

		meshData.m_positions[0] = glm::vec3(-1.0f, 0.0f, 1.0f);
		meshData.m_positions[1] = glm::vec3(-1.0f, 0.0f, -1.0f);
		meshData.m_positions[2] = glm::vec3(1.0f, 0.0f, 1.0f);
		meshData.m_positions[3] = glm::vec3(1.0f, 0.0f, -1.0f);

		meshData.m_indices[0] = 0;
		meshData.m_indices[1] = 3;
		meshData.m_indices[2] = 1;
		meshData.m_indices[3] = 2;
		meshData.m_indices[4] = 3;
		meshData.m_indices[5] = 0;

		meshData.m_texcoords[0] = glm::vec2(0.0f, 0.0f);
		meshData.m_texcoords[1] = glm::vec2(0.0f, 1.0f);
		meshData.m_texcoords[2] = glm::vec2(1.0f, 0.0f);
		meshData.m_texcoords[3] = glm::vec2(1.0f, 1.0f);

		meshData.m_colours[0] = 
		meshData.m_colours[1] = 
		meshData.m_colours[2] = 
		meshData.m_colours[3] = glm::one<glm::vec4>();

		meshData.m_normals[0] =
		meshData.m_normals[1] =
		meshData.m_normals[2] =
		meshData.m_normals[3] = glm::vec3(0.0f, 1.0f, 0.0f);

		meshData.m_tangents[0] =
		meshData.m_tangents[1] =
		meshData.m_tangents[2] =
		meshData.m_tangents[3] = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

		meshData.m_submeshes.push_back(LIWSubMesh{ 0, 6 });
		meshData.m_primitiveType = LIWPreimitiveType_Triangles;
	}
	glm::vec4 LIWMeshData::GenerateTangent(uint32_t vertIdxA, uint32_t vertIdxB, uint32_t vertIdxC)
	{
		glm::vec3 ba = m_positions[vertIdxB] - m_positions[vertIdxA];
		glm::vec3 ca = m_positions[vertIdxC] - m_positions[vertIdxA];

		glm::vec2 tba = m_texcoords[vertIdxB] - m_texcoords[vertIdxA];
		glm::vec2 tca = m_texcoords[vertIdxC] - m_texcoords[vertIdxA];

		glm::mat2 texMatrix = glm::mat2(tba.x, tca.x, tba.y, tca.y);
		texMatrix = glm::inverse(texMatrix);

		glm::vec3 tangent, binormal;
		tangent = ba * texMatrix[0][0] + ca * texMatrix[0][1];
		binormal = ba * texMatrix[1][0] + ca * texMatrix[1][1];

		glm::vec3 normal = glm::cross(ca, ba);
		glm::vec3 biCross = glm::cross(normal, tangent);
		float handedness = 1.0f;
		if (glm::dot(biCross, binormal) < 0.0f) {
			handedness = -1.0f;
		}
		return glm::vec4(tangent, handedness);
	}
}