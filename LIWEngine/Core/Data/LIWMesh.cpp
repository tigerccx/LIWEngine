#include "LIWMesh.h"

namespace LIW {
	const std::unordered_map<LIWPreimitiveType, GLenum> LIWImageFormat_2_GLPrimitive =
	{
		{LIWPreimitiveType_Triangles, GL_TRIANGLES}
	};

	void LIWMesh::CreateMesh(const LIWMeshData& meshData)
	{
		if (m_handleVAO != sc_invalidHandle)
			throw std::runtime_error("mesh already created. ");
		if (!meshData.IsValid())
			throw std::runtime_error("invalid meshData. ");

		// Generate and bind VAO and VBOs
		glGenVertexArrays(1, &m_handleVAO);
		glBindVertexArray(m_handleVAO);
		glGenBuffers(1, &m_handleBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_handleBuffer);
		glGenBuffers(1, &m_handleIndexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_handleIndexBuffer);

		m_primitiveType = meshData.GetPrimitiveType();

		//
		// Buffer data
		//
		auto& positions = meshData.GetPositionArray();
		auto& normals = meshData.GetNormalArray();
		auto& tangents = meshData.GetTangentArray();
		auto& texcoords = meshData.GetTexcoordArray();
		auto& colours = meshData.GetColourArray();
		auto& indices = meshData.GetIndexArray();
		m_submeshes = meshData.GetSubMeshArray();
		
		bool useNormal = false;
		bool useTangent = false;
		bool useTexcoord = false;
		bool useColour = false;
		const size_t sizePositions = positions.get_size() * sizeof(glm::vec3);
		const size_t sizeNormals = normals.get_size() * sizeof(glm::vec3);
		const size_t sizeTangents = tangents.get_size() * sizeof(glm::vec4);
		const size_t sizeTexcoords = texcoords.get_size() * sizeof(glm::vec2);
		const size_t sizeColours = colours.get_size() * sizeof(glm::vec3);
		const size_t sizeIndices = indices.get_size() * sizeof(uint32_t);
		uint64_t sizeBuffer = sizePositions;
		if (normals.get_size() > 0) { 
			useNormal = true; 
			sizeBuffer += sizeNormals;
		}
		if (tangents.get_size() > 0) {
			useTangent = true;
			sizeBuffer += sizeTangents;
		}
		if (texcoords.get_size() > 0) {
			useTexcoord = true;
			sizeBuffer += sizeTexcoords;
		}
		if (colours.get_size() > 0) {
			useColour = true;
			sizeBuffer += sizeColours;
		}

		glBufferData(GL_ARRAY_BUFFER, sizeBuffer, nullptr, GL_STATIC_DRAW);
		
		sizeBuffer = 0;
		// Position
		glBufferSubData(GL_ARRAY_BUFFER, sizeBuffer, sizePositions, positions.get_data());
		size_t size = sizeof(glm::vec3);
		glVertexAttribPointer(LIW_SHADER_VA_LOCATION_POSITION, 3, GL_FLOAT, GL_FALSE, 0, (void*)sizeBuffer);
		glEnableVertexAttribArray(LIW_SHADER_VA_LOCATION_POSITION);
		sizeBuffer += sizePositions;
		// Normal
		if (useNormal) {
			glBufferSubData(GL_ARRAY_BUFFER, sizeBuffer, sizeNormals, normals.get_data());
			glVertexAttribPointer(LIW_SHADER_VA_LOCATION_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, (void*)sizeBuffer);
			glEnableVertexAttribArray(LIW_SHADER_VA_LOCATION_NORMAL);
			sizeBuffer += sizeNormals;
		}
		// Tangent
		if (useTangent) {
			glBufferSubData(GL_ARRAY_BUFFER, sizeBuffer, sizeTangents, tangents.get_data());
			glVertexAttribPointer(LIW_SHADER_VA_LOCATION_TANGENT, 4, GL_FLOAT, GL_FALSE, 0, (void*)sizeBuffer);
			glEnableVertexAttribArray(LIW_SHADER_VA_LOCATION_TANGENT);
			sizeBuffer += sizeTangents;
		}
		// Texcoord
		if (useTexcoord) {
			glBufferSubData(GL_ARRAY_BUFFER, sizeBuffer, sizeTexcoords, texcoords.get_data());
			glVertexAttribPointer(LIW_SHADER_VA_LOCATION_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, (void*)sizeBuffer);
			glEnableVertexAttribArray(LIW_SHADER_VA_LOCATION_TEXCOORD);
			sizeBuffer += sizeTexcoords;
		}
		// Colour
		if (useColour) {
			glBufferSubData(GL_ARRAY_BUFFER, sizeBuffer, sizeColours, colours.get_data());
			glVertexAttribPointer(LIW_SHADER_VA_LOCATION_COLOUR, 3, GL_FLOAT, GL_FALSE, 0, (void*)sizeBuffer);
			glEnableVertexAttribArray(LIW_SHADER_VA_LOCATION_COLOUR);
			sizeBuffer += sizeColours;
		}

		// Index
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeIndices, indices.get_data(), GL_STATIC_DRAW);


#ifdef _DEBUG
		glObjectLabel(GL_BUFFER, m_handleBuffer, -1, "MeshVertexBuffer");
		glObjectLabel(GL_BUFFER, m_handleIndexBuffer, -1, "MeshIndexBuffer");
#endif

		// Unbind buffers
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void LIWMesh::DestroyMesh()
	{
		if (!IsValid())
			throw std::runtime_error("mesh not created. ");
		uint32_t handles[] = { m_handleBuffer , m_handleIndexBuffer };
		glDeleteBuffers(2, handles);
		glDeleteVertexArrays(1, &m_handleVAO);
		m_handleVAO = m_handleBuffer = m_handleIndexBuffer = sc_invalidHandle;
		m_submeshes.clear();
		m_submeshes.set_capacity(0);
		m_primitiveType = LIWPreimitiveType_Max;
	}

}