#pragma once
#include <unordered_map>
#include <stdexcept>

namespace LIW {
	enum LIWImageFormat {
		LIWImageFormat_R,
		LIWImageFormat_RG,
		LIWImageFormat_RGB,
		LIWImageFormat_RGBA,
		LIWImageFormat_Max
	};

	extern const std::unordered_map<LIWImageFormat, unsigned int> LIWImageFormat_2_STBImageFormat;
	extern const std::unordered_map<LIWImageFormat, uint32_t> LIWImageFormat_2_channels;

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