#pragma once
#include <stdexcept>
#include <cassert>

#include "LIWglfw.h"

#include "Maths/LIWMaths.h"
#include "LIWImage.h"

namespace LIW {
	extern const std::unordered_map<LIWImageFormat, GLint> LIWImageFormat_2_GLInternalFormat;

	class LIWTexture2D {
	public:
		static const uint32_t sc_invalidHandle = UINT32_MAX;
	public:
		void CreateTexture(int width, int height, LIWImageFormat format);
		void CreateTexture(const LIWImage& image);
		void DestroyTexture();

		void Bind(uint32_t rawHandleShader, const char* name, uint32_t imageUnit);
		void Unbind(uint32_t imageUnit);
	public:
		inline bool IsValid() const { return m_handleTexture != sc_invalidHandle; }
		inline int GetWidth() const { return m_width; }
		inline int GetHeight() const { return m_height; }
		inline LIWImageFormat GetFormat() const { return m_format; }
		inline uint32_t GetRawHandle() const { return m_handleTexture; }
	private:
		uint32_t m_handleTexture{ sc_invalidHandle };
		int m_width{ -1 };
		int m_height{ -1 };
		LIWImageFormat m_format{ LIWImageFormat_Max };
	};
};