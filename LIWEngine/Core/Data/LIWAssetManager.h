#pragma once
#include "LIWConfig.h"
#include "LIWMacros.h"

#include <string>
#include <unordered_map>

#include "LIWTypes.h"
#include "Maths/LIWMaths.h"
#include "Memory/LIWMemory.h"
//#include "Containers/LIWDObjectPool.h"
#include "Containers/LIWObjectPool.h"

#include "LIWAsset.h"

#include "LIWImage.h"
#include "LIWTexture.h"
#include "LIWFrameBuffer.h"

#include "LIWMeshData.h"
#include "LIWMesh.h"

#include "LIWShader.h"
#include "LIWShaderProgram.h"

#include "LIWMaterial.h"

namespace LIW {
	template<LIWMemAllocation AllocType = LIWMem_Default>
	class LIWAssetManager {
	public:
		void Init();

		void Cleanup();

	public:
		//
		// Image and Texture
		//
		// Image
		inline LIWPointer<LIWImage, AllocType> CreateImage(const char* name) {
			return CreateAsset_(name, m_imageBuffer, m_imageMap);
		}
		inline LIWPointer<LIWImage, AllocType> GetImage(const char* name) {
			return GetAsset_(name, m_imageMap);
		}
		inline void DestroyImage(const char* name) {
			DestroyAsset_(name, m_imageBuffer, m_imageMap);
		}
		inline std::unordered_map<std::string, LIWPointer<LIWImage, AllocType>>& GetImages() { return m_imageMap; }
		inline const std::unordered_map<std::string, LIWPointer<LIWImage, AllocType>>& GetImages() const { return m_imageMap; }
		// Texture2D
		inline LIWPointer<LIWTexture2D, AllocType> CreateTexture2D(const char* name) {
			return CreateAsset_(name, m_tex2DBuffer, m_tex2DMap);
		}
		inline LIWPointer<LIWTexture2D, AllocType> GetTexture2D(const char* name) {
			return GetAsset_(name, m_tex2DMap);
		}
		inline void DestroyTexture2D(const char* name) {
			DestroyAsset_(name, m_tex2DBuffer, m_tex2DMap);
		}
		inline std::unordered_map<std::string, LIWPointer<LIWTexture2D, AllocType>>& GetTexture2Ds() { return m_tex2DMap; }
		inline const std::unordered_map<std::string, LIWPointer<LIWTexture2D, AllocType>>& GetTexture2Ds() const { return m_tex2DMap; }
		
		//
		// FrameBuffer
		//
		inline LIWPointer<LIWFrameBuffer, AllocType> CreateFrameBuffer(const char* name) {
			return CreateAsset_(name, m_frameBufferBuffer, m_frameBufferMap);
		}
		inline LIWPointer<LIWFrameBuffer, AllocType> GetFrameBuffer(const char* name) {
			return GetAsset_(name, m_frameBufferMap);
		}
		inline void DestroyFrameBuffer(const char* name) {
			DestroyAsset_(name, m_frameBufferBuffer, m_frameBufferMap);
		}
		inline std::unordered_map<std::string, LIWPointer<LIWFrameBuffer, AllocType>>& GetFrameBuffers() { return m_frameBufferMap; }
		inline const std::unordered_map<std::string, LIWPointer<LIWFrameBuffer, AllocType>>& GetFrameBuffers() const { return m_frameBufferMap; }
		
		//
		// Mesh
		//
		// MeshData
		inline LIWPointer<LIWMeshData, AllocType> CreateMeshData(const char* name) {
			return CreateAsset_(name, m_meshdataBuffer, m_meshdataMap);
		}
		inline LIWPointer<LIWMeshData, AllocType> GetMeshData(const char* name) {
			return GetAsset_(name, m_meshdataMap);
		}
		inline void DestroyMeshData(const char* name) {
			DestroyAsset_(name, m_meshdataBuffer, m_meshdataMap);
		}
		inline std::unordered_map<std::string, LIWPointer<LIWMeshData, AllocType>>& GetMeshDatas() { return m_meshdataMap; }
		inline const std::unordered_map<std::string, LIWPointer<LIWMeshData, AllocType>>& GetMeshDatas() const { return m_meshdataMap; }
		// Mesh
		inline LIWPointer<LIWMesh, AllocType> CreateMesh(const char* name) {
			return CreateAsset_(name, m_meshBuffer, m_meshMap);
		}
		inline LIWPointer<LIWMesh, AllocType> GetMesh(const char* name) {
			return GetAsset_(name, m_meshMap);
		}
		inline void DestroyMesh(const char* name) {
			DestroyAsset_(name, m_meshBuffer, m_meshMap);
		}
		inline std::unordered_map<std::string, LIWPointer<LIWMesh, AllocType>>& GetMeshes() { return m_meshMap; }
		inline const std::unordered_map<std::string, LIWPointer<LIWMesh, AllocType>>& GetMeshes() const { return m_meshMap; }

		//
		// Shader
		//
		// Shader
		inline LIWPointer<LIWShader, AllocType> CreateShader(const char* name) {
			return CreateAsset_(name, m_shaderBuffer, m_shaderMap);
		}
		inline LIWPointer<LIWShader, AllocType> GetShader(const char* name) {
			return GetAsset_(name, m_shaderMap);
		}
		inline void DestroyShader(const char* name) {
			DestroyAsset_(name, m_shaderBuffer, m_shaderMap);
		}
		inline std::unordered_map<std::string, LIWPointer<LIWShader, AllocType>>& GetShaders() { return m_shaderMap; }
		inline const std::unordered_map<std::string, LIWPointer<LIWShader, AllocType>>& GetShaders() const { return m_shaderMap; }
		// ShaderProgram
		inline LIWPointer<LIWShaderProgram, AllocType> CreateShaderProgram(const char* name) {
			return CreateAsset_(name, m_shaderProgramBuffer, m_shaderProgramMap);
		}
		inline LIWPointer<LIWShaderProgram, AllocType> GetShaderProgram(const char* name) {
			return GetAsset_(name, m_shaderProgramMap);
		}
		inline void DestroyShaderProgram(const char* name) {
			DestroyAsset_(name, m_shaderProgramBuffer, m_shaderProgramMap);
		}
		inline std::unordered_map<std::string, LIWPointer<LIWShaderProgram, AllocType>>& GetShaderPrograms() { return m_shaderProgramMap; }
		inline const std::unordered_map<std::string, LIWPointer<LIWShaderProgram, AllocType>>& GetShaderPrograms() const { return m_shaderProgramMap; }

		//
		// Material
		//
		inline LIWPointer<LIWMaterial, AllocType> CreateMaterial(const char* name) {
			return CreateAsset_(name, m_materialBuffer, m_materialMap);
		}
		inline LIWPointer<LIWMaterial, AllocType> GetMaterial(const char* name) {
			return GetAsset_(name, m_materialMap);
		}
		inline void DestroyMaterial(const char* name) {
			DestroyAsset_(name, m_materialBuffer, m_materialMap);
		}
		inline std::unordered_map<std::string, LIWPointer<LIWMaterial, AllocType>>& GetMaterials() { return m_materialMap; }
		inline const std::unordered_map<std::string, LIWPointer<LIWMaterial, AllocType>>& GetMaterials() const { return m_materialMap; }

	private:
		template<class T, size_t Size>
		inline LIWPointer<T, AllocType> CreateAsset_(const char* name, 
			LIWObjectPool_Heap<T, Size, AllocType>& assets,
			std::unordered_map<std::string, LIWPointer<T, AllocType>>& assetMap) {
			if (assets.is_empty()) {
				throw std::runtime_error("Exceeds capacity. Limit usage/increase capacity setting in LIWMacros.h. ");
			}
			LIWPointer<T, AllocType> ptr = assets.fetch_object();
			assetMap[name] = ptr;
			return ptr;
		}
		template<class T, size_t Size>
		inline void DestroyAsset_(const char* name, 
			LIWObjectPool_Heap<T, Size, AllocType>& assets, 
			std::unordered_map<std::string, LIWPointer<T, AllocType>>& assetMap) {
			auto itr = assetMap.find(name);
			if (itr == assetMap.end())
				throw std::runtime_error("Cannot find asset of such name. ");
			assets.return_object(itr->second);
			assetMap.erase(itr);
		}
		template<class T>
		inline LIWPointer<T, AllocType> GetAsset_(const char* name, 
			std::unordered_map<std::string, LIWPointer<T, AllocType>>& assetMap) {
			auto itr = assetMap.find(name);
			if (itr == assetMap.end())
				throw std::runtime_error("Cannot find asset of such name. ");
			return itr->second;
		}

	private:
		// Image and Texture
		LIWObjectPool_Heap<LIWImage, LIW_ASSET_IMAGE_CAPACITY, AllocType>					m_imageBuffer;
		std::unordered_map<std::string, LIWPointer<LIWImage, AllocType>>					m_imageMap;
		LIWObjectPool_Heap<LIWTexture2D, LIW_ASSET_TEXTURE2D_CAPACITY, AllocType>			m_tex2DBuffer;
		std::unordered_map<std::string, LIWPointer<LIWTexture2D, AllocType>>				m_tex2DMap;

		// FrameBuffer
		LIWObjectPool_Heap<LIWFrameBuffer, LIW_ASSET_FRAMEBUFFER_CAPACITY, AllocType>		m_frameBufferBuffer;
		std::unordered_map<std::string, LIWPointer<LIWFrameBuffer, AllocType>>				m_frameBufferMap;

		// Mesh
		LIWObjectPool_Heap<LIWMeshData, LIW_ASSET_MESHDATA_CAPACITY, AllocType>				m_meshdataBuffer;
		std::unordered_map<std::string, LIWPointer<LIWMeshData, AllocType>>					m_meshdataMap;
		LIWObjectPool_Heap<LIWMesh, LIW_ASSET_MESH_CAPACITY, AllocType>						m_meshBuffer;
		std::unordered_map<std::string, LIWPointer<LIWMesh, AllocType>>						m_meshMap;

		// Shader
		LIWObjectPool_Heap<LIWShader, LIW_ASSET_SHADER_CAPACITY, AllocType>					m_shaderBuffer;
		std::unordered_map<std::string, LIWPointer<LIWShader, AllocType>>					m_shaderMap;
		LIWObjectPool_Heap<LIWShaderProgram, LIW_ASSET_SHDAERPROGRAM_CAPACITY, AllocType>	m_shaderProgramBuffer;
		std::unordered_map<std::string, LIWPointer<LIWShaderProgram, AllocType>>			m_shaderProgramMap;

		// Material
		LIWObjectPool_Heap<LIWMaterial, LIW_ASSET_MATERIAL_CAPACITY, AllocType>				m_materialBuffer;
		std::unordered_map<std::string, LIWPointer<LIWMaterial, AllocType>>					m_materialMap;
	};

	template<LIWMemAllocation AllocType>
	void LIWAssetManager<AllocType>::Init()
	{
		LIWPointer<LIWMeshData, AllocType> ptrMDSphere = CreateMeshData(LIW_MESHDATA_SPHERE_NAME);
		LIWMeshData::CreateSphere(*ptrMDSphere);
		LIWPointer<LIWMesh, AllocType> ptrSphere = CreateMesh(LIW_MESH_SPHERE_NAME);
		ptrSphere->CreateMesh(*ptrMDSphere);

		LIWPointer<LIWMeshData, AllocType> ptrMDCube = CreateMeshData(LIW_MESHDATA_CUBE_NAME);
		LIWMeshData::CreateCube(*ptrMDCube);
		LIWPointer<LIWMesh, AllocType> ptrCube = CreateMesh(LIW_MESH_CUBE_NAME);
		ptrCube->CreateMesh(*ptrMDCube);

		LIWPointer<LIWMeshData, AllocType> ptrMDPlane = CreateMeshData(LIW_MESHDATA_PLANE_NAME);
		LIWMeshData::CreatePlane(*ptrMDPlane);
		LIWPointer<LIWMesh, AllocType> ptrPlane = CreateMesh(LIW_MESH_PLANE_NAME);
		ptrPlane->CreateMesh(*ptrMDPlane);
	}

	template<LIWMemAllocation AllocType>
	void LIWAssetManager<AllocType>::Cleanup()
	{
		DestroyMeshData(LIW_MESHDATA_SPHERE_NAME);
		DestroyMesh(LIW_MESH_SPHERE_NAME);

		DestroyMeshData(LIW_MESHDATA_CUBE_NAME);
		DestroyMesh(LIW_MESH_CUBE_NAME);

		DestroyMeshData(LIW_MESHDATA_PLANE_NAME);
		DestroyMesh(LIW_MESH_PLANE_NAME);


		if (m_imageMap.size() > 0) {
			printf("Warning: Not all Images have been destroyed: \n");
			for (auto itr = m_imageMap.begin(); itr != m_imageMap.end(); itr++) {
				printf(" ImageName: %s \n", itr->first.c_str());
			}
		}

		if (m_tex2DMap.size() > 0) {
			printf("Warning: Not all Tex2Ds have been destroyed: \n");
			for (auto itr = m_tex2DMap.begin(); itr != m_tex2DMap.end(); itr++) {
				printf(" Tex2D: %s \n", itr->first.c_str());
			}
		}

		if (m_frameBufferMap.size() > 0) {
			printf("Warning: Not all FrameBuffers have been destroyed: \n");
			for (auto itr = m_frameBufferMap.begin(); itr != m_frameBufferMap.end(); itr++) {
				printf(" FrameBuffer: %s \n", itr->first.c_str());
			}
		}

		if (m_meshdataMap.size() > 0) {
			printf("Warning: Not all Meshdatas have been destroyed: \n");
			for (auto itr = m_meshdataMap.begin(); itr != m_meshdataMap.end(); itr++) {
				printf(" Meshdata: %s \n", itr->first.c_str());
			}
		}

		if (m_meshMap.size() > 0) {
			printf("Warning: Not all Meshes have been destroyed: \n");
			for (auto itr = m_meshMap.begin(); itr != m_meshMap.end(); itr++) {
				printf(" Mesh: %s \n", itr->first.c_str());
			}
		}

		if (m_shaderMap.size() > 0) {
			printf("Warning: Not all Shaders have been destroyed: \n");
			for (auto itr = m_shaderMap.begin(); itr != m_shaderMap.end(); itr++) {
				printf(" Shader: %s \n", itr->first.c_str());
			}
		}

		if (m_shaderProgramMap.size() > 0) {
			printf("Warning: Not all ShaderPrograms have been destroyed: \n");
			for (auto itr = m_shaderProgramMap.begin(); itr != m_shaderProgramMap.end(); itr++) {
				printf(" ShaderProgram: %s \n", itr->first.c_str());
			}
		}

		if (m_materialMap.size() > 0) {
			printf("Warning: Not all Materials have been destroyed: \n");
			for (auto itr = m_materialMap.begin(); itr != m_materialMap.end(); itr++) {
				printf(" Material: %s \n", itr->first.c_str());
			}
		}
	}
}