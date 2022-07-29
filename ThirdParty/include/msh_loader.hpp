#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <vector>


/*
* Define: MSHLOADER_IMPLEMENTATION to include implementation. 
*/
namespace msh {
	enum class GeometryChunkTypes {
		VPositions = 1,
		VNormals = 2,
		VTangents = 4,
		VColors = 8,
		VTex0 = 16,
		VTex1 = 32,
		VWeightValues = 64,
		VWeightIndices = 128,
		Indices = 256,
		JointNames = 512,
		JointParents = 1024,
		BindPose = 2048,
		BindPoseInv = 4096,
		Material = 65536,
		SubMeshes = 1 << 14,
		SubMeshNames = 1 << 15
	};

	//struct Vector2 {
	//	float x;
	//	float y;
	//};

	//
	// Declarations
	//
	void read_test_float2s(std::ifstream& file, std::vector<float>& element, int numVertices);
	void read_test_float3s(std::ifstream& file, std::vector<float>& element, int numVertices);
	void read_test_float4s(std::ifstream& file, std::vector<float>& element, int numVertices);
	void read_indices(std::ifstream& file, std::vector<uint32_t>& elements, int numIndices);
	bool load_from_msh_file(const char* path,
		std::vector<float>* readPositions,
		std::vector<float>* readColours,
		std::vector<float>* readNormals,
		std::vector<float>* readTangents,
		std::vector<float>* readUVs,
		std::vector<uint32_t>* readIndices,
		std::vector<uint32_t>* readSubMeshes);

#ifdef MSHLOADER_IMPLEMENTATION
	//
	// Implementations
	//

	void read_test_float2s(std::ifstream& file, std::vector<float>& element, int numVertices) {
		for (int i = 0; i < numVertices; ++i) {
			float x, y;
			file >> x;
			file >> y;
			element.emplace_back(x);
			element.emplace_back(y);
		}
	}
	void read_test_float3s(std::ifstream& file, std::vector<float>& element, int numVertices) {
		for (int i = 0; i < numVertices; ++i) {
			float x, y, z;
			file >> x;
			file >> y;
			file >> z;
			element.emplace_back(x);
			element.emplace_back(y);
			element.emplace_back(z);
		}
	}
	void read_test_float4s(std::ifstream& file, std::vector<float>& element, int numVertices) {
		for (int i = 0; i < numVertices; ++i) {
			float x, y, z, w;
			file >> x;
			file >> y;
			file >> z;
			file >> w;
			element.emplace_back(x);
			element.emplace_back(y);
			element.emplace_back(z);
			element.emplace_back(w);
		}
	}
	void read_indices(std::ifstream& file, std::vector<uint32_t>& elements, int numIndices) {
		for (int i = 0; i < numIndices; ++i) {
			uint32_t temp;
			file >> temp;
			elements.emplace_back(temp);
		}
	}
	void read_sub_meshes(std::ifstream& file, int count, std::vector<uint32_t>& subMeshes) {
		for (int i = 0; i < count; ++i) {
			uint32_t start, end;
			file >> start;
			file >> end;
			subMeshes.emplace_back(start);
			subMeshes.emplace_back(end);
		}
	}

	bool load_from_msh_file(const char* path,
							std::vector<float>* readPositions,
							std::vector<float>* readColours,
							std::vector<float>* readNormals,
							std::vector<float>* readTangents,
							std::vector<float>* readUVs,
							std::vector<uint32_t>* readIndices,
							std::vector<uint32_t>* readSubMeshes) {
		using namespace std;

		ifstream file(path);
		if (!file) {
			cout << "File does not exist!" << endl;
			return false;
		}

		string filetype;
		int fileVersion;

		file >> filetype;

		if (filetype != "MeshGeometry") {
			cout << "File is not a MeshGeometry file!" << endl;
			return false;
		}

		file >> fileVersion;

		if (fileVersion != 1) {
			cout << "MeshGeometry file has incompatible version!" << endl;
			return false;
		}

		int numMeshes = 0; //read	//number of submeshes
		int numVertices = 0; //read	//number of vertices
		int numIndices = 0; //read	//number of indices
		int numChunks = 0; //read	//number of data chunks (for different data: vertex, index, normal, ...)

		file >> numMeshes;
		file >> numVertices;
		file >> numIndices;
		file >> numChunks;


		for (int i = 0; i < numChunks; ++i) {
			int chunkType = (int)GeometryChunkTypes::VPositions;

			file >> chunkType;

			switch ((GeometryChunkTypes)chunkType) {
			case GeometryChunkTypes::VPositions:	if (readPositions)	read_test_float3s(file, *readPositions, numVertices);  break;
			case GeometryChunkTypes::VColors:		if (readColours)	read_test_float4s(file, *readColours, numVertices);  break;
			case GeometryChunkTypes::VNormals:		if (readNormals)	read_test_float3s(file, *readNormals, numVertices);  break;
			case GeometryChunkTypes::VTangents:		if (readTangents)	read_test_float4s(file, *readTangents, numVertices);  break;
			case GeometryChunkTypes::VTex0:			if (readUVs)		read_test_float2s(file, *readUVs, numVertices);  break;
			case GeometryChunkTypes::Indices:		if (readIndices)	read_indices(file, *readIndices, numIndices); break;

			//TODO: Add these back when adding animation
			//case GeometryChunkTypes::VWeightValues:		read_test_floats(file, readWeights, numVertices);  break;
			//case GeometryChunkTypes::VWeightIndices:	ReadTextVertexIndices(file, readWeightIndices, numVertices);  break;
			//case GeometryChunkTypes::JointNames:		ReadJointNames(file, this->jointNames);  break;
			//case GeometryChunkTypes::JointParents:		ReadJointParents(file, this->jointParents);  break;
			//case GeometryChunkTypes::BindPose:			ReadRigPose(file, &this->bindPose);  break;
			//case GeometryChunkTypes::BindPoseInv:		ReadRigPose(file, &this->inverseBindPose);  break;
			case GeometryChunkTypes::SubMeshes: 	if (readSubMeshes)	read_sub_meshes(file, numMeshes, *readSubMeshes); break;
			//TODO: Add this back if ever support submesh name
			//case GeometryChunkTypes::SubMeshNames: 		ReadSubMeshNames(file, numMeshes, this->layerNames); break;
			}
		}
		//Now that the data has been read, we can shove it into the actual Mesh object

		return true;
	}
#endif
}