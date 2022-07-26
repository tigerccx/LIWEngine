#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "LIWMeshData.h"

namespace LIW {
	void LIWMeshData::LoadMeshData_Obj(const char* objPath)
	{
		//TODO: for now the loading is taking up more space than required (repeated vertex...)

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
						attrib.texcoords[(size_t)index.texcoord_index * 2 + 1]
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
	}
	void LIWMeshData::UnloadMeshData()
	{
		if (!IsValid())
			throw std::runtime_error("meshdata not loaded. ");
		m_positions.clear();
		m_positions.set_capacity(0);
		m_normals.clear();
		m_normals.set_capacity(0);
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
}