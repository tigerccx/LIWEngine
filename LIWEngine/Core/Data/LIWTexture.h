#pragma once

#include <stdexcept>
#include <cassert>
#include "Maths/LIWMaths.h"

namespace LIW {
	class LIWTexture {
	public:
		void LoadImage(const char* imagePath);
		void UnloadImage();
	public:
		inline bool HasImage() const { return m_rawData; }
		inline int GetWidth() const { return m_width; }
		inline int GetHeight() const { return m_height; }
		inline int GetChannels() const { return m_channels; }
		inline void* GetRawData() const { return m_rawData; }
	private:
		int m_width{ -1 };
		int m_height{ -1 };
		int m_channels{ -1 };
		void* m_rawData{ nullptr };
	};
};