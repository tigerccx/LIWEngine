#pragma once
#include <stdexcept>
#include <cassert>

#include "LIWglfw.h"

#include "Maths/LIWMaths.h"
#include "LIWImage.h"
#include "LIWRenderAttachmentFormat.h"

namespace LIW {
	extern const std::unordered_map<LIWImageFormat, GLint> LIWImageFormat_2_GLInternalFormat;
	extern const std::unordered_map<LIWImageFormat, GLenum> LIWImageFormat_2_GLFormat;
	extern const std::unordered_map<LIWRenderAttachmentFormat, GLenum> LIWRenderAttachmentFormat_2_GLFormat;
	extern const std::unordered_map<LIWRenderAttachmentFormat, GLenum> LIWRenderAttachmentFormat_2_GLDataType;

	class LIWTexture {
	public:
		static void Bind2DTexture(uint32_t rawHandleTexture,
			uint32_t rawHandleShader,
			const char* name, uint32_t imageUnit);
		static void Unbind2DTexture(uint32_t imageUnit);
	};

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

	//NOTE: Not used for now. Only managed by LIWFrameBuffer
	class LIWRenderTexture2D {
	public:
		static const uint32_t sc_invalidHandle = UINT32_MAX;
	public:
		void CreateTexture(int width, int height, LIWRenderAttachmentFormat format);
		void DestroyTexture();
		
		void Bind(uint32_t rawHandleShader, const char* name, uint32_t imageUnit);
		void Unbind(uint32_t imageUnit);
	public:
		inline bool IsValid() const { return m_handleTexture != sc_invalidHandle; }
		inline int GetWidth() const { return m_width; }
		inline int GetHeight() const { return m_height; }
		inline LIWRenderAttachmentFormat GetFormat() const { return m_format; }
		inline uint32_t GetRawHandle() const { return m_handleTexture; }
	private:
		uint32_t m_handleTexture{ sc_invalidHandle };
		int m_width{ -1 };
		int m_height{ -1 };
		LIWRenderAttachmentFormat m_format{ LIWRenderAttachmentFormat_Max };
	};
};