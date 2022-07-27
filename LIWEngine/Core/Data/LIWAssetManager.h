#pragma once
#include <string>
#include <unordered_map>

#include "LIWTypes.h"
#include "Maths/LIWMaths.h"
#include "Memory/LIWMemory.h"
#include "Containers/LIWDObjectPool.h"

#include "LIWImage.h"
#include "LIWTexture.h"

#include "LIWMeshData.h"
#include "LIWMesh.h"

#include "LIWShader.h"
#include "LIWShaderProgram.h"

#include "LIWMaterial.h"

namespace LIW {
	enum LIWAssetType {
		LIWAsset_Image,
		LIWAsset_Texture2D,
		LIWAsset_MeshData,
		LIWAsset_Mesh,
		LIWAsset_Shader,
		LIWAsset_ShaderProgram,
		LIWAsset_Material,
		LIWAsset_Max
	};

	class LIWAssetManager {
	private:
		struct AssetEntry {
			LIWAssetType m_assetType{ LIWAsset_Max };
			liw_objhdl_type m_handle{ liw_c_nullobjhdl };
		};
	public:
		// Image and Texture
		inline liw_objhdl_type CreateImage(const char* name) {
			return CreateAsset(name, m_images, LIWAsset_Image);
		}
		inline liw_objhdl_type GetImageHandle(const char* name) {
			return GetHandle(name, LIWAsset_Image);
		}
		inline LIWImage& GetImage(liw_objhdl_type handle) {
			return m_images.get_object(handle);
		}
		inline LIWImage& GetImage(const char* name) {
			return GetAsset(name, m_images, LIWAsset_Image);
		}
		inline void DestroyImage(const char* name) {
			DestroyAsset(name, m_images, LIWAsset_Image);
		}
		inline liw_objhdl_type CreateTexture2D(const char* name) {
			return CreateAsset(name, m_tex2Ds, LIWAsset_Texture2D);
		}
		inline liw_objhdl_type GetTexture2DHandle(const char* name) {
			return GetHandle(name, LIWAsset_Texture2D);
		}
		inline LIWTexture2D& GetTexture2D(liw_objhdl_type handle) {
			return m_tex2Ds.get_object(handle);
		}
		inline LIWTexture2D& GetTexture2D(const char* name) {
			return GetAsset(name, m_tex2Ds, LIWAsset_Texture2D);
		}
		inline void DestroyTexture2D(const char* name) {
			DestroyAsset(name, m_tex2Ds, LIWAsset_Texture2D);
		}

		// Mesh
		inline liw_objhdl_type CreateMeshData(const char* name) {
			return CreateAsset(name, m_meshdatas, LIWAsset_MeshData);
		}
		inline liw_objhdl_type GetMeshDataHandle(const char* name) {
			return GetHandle(name, LIWAsset_MeshData);
		}
		inline LIWMeshData& GetMeshData(liw_objhdl_type handle) {
			return m_meshdatas.get_object(handle);
		}
		inline LIWMeshData& GetMeshData(const char* name) {
			return GetAsset(name, m_meshdatas, LIWAsset_MeshData);
		}
		inline void DestroyMeshData(const char* name) {
			DestroyAsset(name, m_meshdatas, LIWAsset_MeshData);
		}
		liw_objhdl_type CreateMesh(const char* name) {
			return CreateAsset(name, m_meshes, LIWAsset_Mesh);
		}
		inline liw_objhdl_type GetMeshHandle(const char* name) {
			return GetHandle(name, LIWAsset_Mesh);
		}
		inline LIWMesh& GetMesh(liw_objhdl_type handle) {
			return m_meshes.get_object(handle);
		}
		inline LIWMesh& GetMesh(const char* name) {
			return GetAsset(name, m_meshes, LIWAsset_Mesh);
		}
		inline void DestroyMesh(const char* name) {
			DestroyAsset(name, m_meshes, LIWAsset_Mesh);
		}

		// Shader
		inline liw_objhdl_type CreateShader(const char* name) {
			return CreateAsset(name, m_shaders, LIWAsset_Shader);
		}
		inline liw_objhdl_type GetShaderHandle(const char* name) {
			return GetHandle(name, LIWAsset_Shader);
		}
		inline LIWShader& GetShader(liw_objhdl_type handle) {
			return m_shaders.get_object(handle);
		}
		inline LIWShader& GetShader(const char* name) {
			return GetAsset(name, m_shaders, LIWAsset_Shader);
		}
		inline void DestroyShader(const char* name) {
			DestroyAsset(name, m_shaders, LIWAsset_Shader);
		}
		inline liw_objhdl_type CreateShaderProgram(const char* name) {
			return CreateAsset(name, m_shaderPrograms, LIWAsset_ShaderProgram);
		}
		inline liw_objhdl_type GetShaderProgramHandle(const char* name) {
			return GetHandle(name, LIWAsset_ShaderProgram);
		}
		inline LIWShaderProgram& GetShaderProgram(liw_objhdl_type handle) {
			return m_shaderPrograms.get_object(handle);
		}
		inline LIWShaderProgram& GetShaderProgram(const char* name) {
			return GetAsset(name, m_shaderPrograms, LIWAsset_ShaderProgram);
		}
		inline void DestroyShaderProgram(const char* name) {
			DestroyAsset(name, m_shaderPrograms, LIWAsset_ShaderProgram);
		}

		// Material
		inline liw_objhdl_type CreateMaterial(const char* name) {
			return CreateAsset(name, m_materials, LIWAsset_Material);
		}
		inline liw_objhdl_type GetMaterialHandle(const char* name) {
			return GetHandle(name, LIWAsset_Material);
		}
		inline LIWMaterial& GetMaterial(liw_objhdl_type handle) {
			return m_materials.get_object(handle);
		}
		inline LIWMaterial& GetMaterial(const char* name) {
			return GetAsset(name, m_materials, LIWAsset_Material);
		}
		inline void DestroyMaterial(const char* name) {
			DestroyAsset(name, m_materials, LIWAsset_Material);
		}

	private:
		template<class T>
		inline liw_objhdl_type CreateAsset(const char* name, LIWDObjectPool<T, LIWMem_Default>& assets, LIWAssetType type) {
			if (assets.is_empty()) {
				uint32_t capacity = (uint32_t)assets.get_capacity();
				assets.set_capacity(uint32_t(capacity * 1.5f));
			}
			liw_objhdl_type handle = assets.fetch_object();
			m_assetMap[name] = AssetEntry{ type, handle };
			return handle;
		}
		template<class T>
		inline void DestroyAsset(const char* name, LIWDObjectPool<T, LIWMem_Default>& assets, LIWAssetType type) {
			auto itr = m_assetMap.find(name);
			if (itr == m_assetMap.end())
				throw std::runtime_error("Cannot find asset of such name. ");
			if(itr->second.m_assetType!= type)
				throw std::runtime_error("Cannot find asset of such type. ");
			assets.return_object(itr->second.m_handle);
			m_assetMap.erase(itr);
		}
		template<class T>
		inline T& GetAsset(const char* name, LIWDObjectPool<T, LIWMem_Default>& assets, LIWAssetType type) {
			auto itr = m_assetMap.find(name);
			if (itr == m_assetMap.end())
				throw std::runtime_error("Cannot find asset of such name. ");
			if (itr->second.m_assetType != type)
				throw std::runtime_error("Cannot find asset of such type. ");
			return assets.get_object(itr->second.m_handle);
		}
		inline liw_objhdl_type GetHandle(const char* name, LIWAssetType type) {
			auto itr = m_assetMap.find(name);
			if (itr == m_assetMap.end())
				throw std::runtime_error("Cannot find asset of such name. ");
			if (itr->second.m_assetType != type)
				throw std::runtime_error("Cannot find asset of such type. ");
			return itr->second.m_handle;
		}
		//template<class T>
		//inline liw_objhdl_type GetHandle(const char* name, LIWDObjectPool<T, LIWMem_Default>& assets, LIWAssetType type) {
		//	auto itr = m_assetMap.find(name);
		//	if (itr == m_assetMap.end())
		//		throw std::runtime_error("Cannot find asset of such name. ");
		//	if (itr->second.m_assetType != type)
		//		throw std::runtime_error("Cannot find asset of such type. ");
		//	return itr->second.m_handle;
		//}

	private:
		std::unordered_map<std::string, AssetEntry>	m_assetMap;

		// Image and Texture
		LIWDObjectPool<LIWImage, LIWMem_Default>			m_images{ 64 };
		LIWDObjectPool<LIWTexture2D, LIWMem_Default>		m_tex2Ds{ 64 };

		// Mesh
		LIWDObjectPool<LIWMeshData, LIWMem_Default>			m_meshdatas{ 64 };
		LIWDObjectPool<LIWMesh, LIWMem_Default>				m_meshes{ 64 };

		// Shader
		LIWDObjectPool<LIWShader, LIWMem_Default>			m_shaders{ 64 };
		LIWDObjectPool<LIWShaderProgram, LIWMem_Default>	m_shaderPrograms{ 64 };

		// Material
		LIWDObjectPool<LIWMaterial, LIWMem_Default>			m_materials{ 64 };
	};
}