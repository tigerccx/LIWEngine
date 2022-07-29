#pragma once
#include <stdexcept>

#include "LIWglfw.h"

#include "LIWRenderAttachmentFormat.h"

namespace LIW {
	class LIWRenderBuffer {
	public:
		static const uint32_t sc_invalidHandle = UINT32_MAX;
	public:
		void CreateRenderBuffer(int width, int height, LIWRenderAttachmentFormat format);
		void DestroyRenderBuffer();

	public:
		inline bool IsValid() const { return m_handleBuffer != sc_invalidHandle; }
		inline int GetWidth() const { return m_width; }
		inline int GetHeight() const { return m_height; }
		inline LIWRenderAttachmentFormat GetFormat() const { return m_format; }
		inline uint32_t GetRawHandle() const { return m_handleBuffer; }
	private:
		uint32_t m_handleBuffer{ sc_invalidHandle };
		int m_width{ -1 };
		int m_height{ -1 };
		LIWRenderAttachmentFormat m_format{ LIWRenderAttachmentFormat_Max };
	};
}