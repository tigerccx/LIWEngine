#pragma once
#include "LIWConfig.h"

#include <vector>
#include <unordered_map>
#include <stdexcept>

#include "LIWglfw.h"

#include "Utility/LIWFile.h"

namespace LIW {
	enum LIWShaderType {
		LIWShaderType_Vertex,
		LIWShaderType_Hull,
		LIWShaderType_Domain,
		LIWShaderType_Geometry,
		LIWShaderType_Fragment,
		LIWShaderType_Compute,
		LIWShaderType_Max
	};

	extern const std::unordered_map<LIWShaderType, GLenum> LIWImageFormat_2_GLShaderType;

	class LIWShader {
	public:
		static const uint32_t sc_invalidHandle = UINT32_MAX;
	public:
		void LoadShader(const char* filePath, LIWShaderType shaderType);
		void UnloadShader();
	public:
		inline bool IsValid() const { return m_handleShader != sc_invalidHandle; }
		inline uint32_t GetRawHandle() const { return m_handleShader; }
		inline LIWShaderType GetShaderType() const { return m_shaderType; }
		inline const char* GetName() const { return m_name; }
	private:
		void PrintCompileLog();
	private:
		uint32_t m_handleShader;
		LIWShaderType m_shaderType;
		char m_name[128]; //allow name to be as long as 127 chars
	};
}