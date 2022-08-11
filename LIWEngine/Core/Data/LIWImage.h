#pragma once
#include <unordered_map>
#include <stdexcept>

namespace LIW {
	enum LIWImageFormat {
		LIWImageFormat_R8,
		LIWImageFormat_RG8,
		LIWImageFormat_RGB8,
		LIWImageFormat_RGBA8,
		LIWImageFormat_Loadable_Max,		//All above are unsigned normalized format
		LIWImageFormat_R16U,
		LIWImageFormat_RG16U,
		LIWImageFormat_RGB16U,
		LIWImageFormat_RGBA16U,
		LIWImageFormat_R32F,
		LIWImageFormat_RG32F,
		LIWImageFormat_RGB32F,
		LIWImageFormat_RGBA32F,
		LIWImageFormat_R32I,
		LIWImageFormat_RG32I,
		LIWImageFormat_RGB32I,
		LIWImageFormat_RGBA32I,
		LIWImageFormat_R32U,
		LIWImageFormat_RG32U,
		LIWImageFormat_RGB32U,
		LIWImageFormat_RGBA32U,
		LIWImageFormat_Max
	};

	extern const std::unordered_map<LIWImageFormat, unsigned int>	LIWImageFormat_2_STBImageFormat;
	extern const std::unordered_map<LIWImageFormat, uint32_t>		LIWImageFormat_2_channels;

	class LIWImage {
	public:
		void LoadImg(const char* imagePath, LIWImageFormat format); //LoadImage is taken by windows
		void UnloadImg();
	public:
		inline bool IsValid() const { return m_rawData; }
		inline int GetWidth() const { return m_width; }
		inline int GetHeight() const { return m_height; }
		inline LIWImageFormat GetFormat() const { return m_format; }
		inline void* GetRawData() const { return m_rawData; }
	private:
		void* m_rawData{ nullptr };
		int m_width{ -1 };
		int m_height{ -1 };
		LIWImageFormat m_format{ LIWImageFormat_Max };
	};
}