#pragma once

#include "LIWShader.h"
#include "Containers/LIWDArray.h"

namespace LIW {
	class LIWShaderProgram {
	public:
		static const uint32_t sc_invalidHandle = UINT32_MAX;
	public:
		void CreateShader(const std::vector<LIWShader>& shaders);
		void DestroyShader();
	public:
		inline bool IsValid() const { return m_handleShaderProgram != sc_invalidHandle; }
		inline uint32_t GetRawHandle() const { return m_handleShaderProgram; }
	private:
		void PrintLinkLog();
	private:
		uint32_t m_handleShaderProgram;
		//uint32_t m_handleShaders[5]; //5 Max: Vert, Hull, Domain, Geo, Frag (or one Compute)
	};
}