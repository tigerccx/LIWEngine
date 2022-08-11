#pragma once
#include <stdexcept>
#include <cassert>

#include "LIWglfw.h"

#include "Maths/LIWMaths.h"
#include "LIWImage.h"
#include "LIWRenderAttachmentFormat.h"

namespace LIW {
	enum LIWTextureWrapType {
		LIWTextureWrapType_Repeat,
		LIWTextureWrapType_Clamp,
		LIWTextureWrapType_Max
	};

	enum LIWTextureFilterType {
		LIWTextureFilterType_Nearest,
		LIWTextureFilterType_Linear,
		LIWTextureFilterType_Nearest_MipmapNearest,
		LIWTextureFilterType_Linear_MipmapNearest,
		LIWTextureFilterType_Nearest_MipmapLinear,
		LIWTextureFilterType_Linear_MipmapLinear,
		LIWTextureFilterType_Max
	};

	enum LIWTextureImageAccessType {
		LIWTextureImageAccessType_ReadWrite,
		LIWTextureImageAccessType_Read,
		LIWTextureImageAccessType_Write,
		LIWTextureImageAccessType_Max
	};

	extern const std::unordered_map<LIWTextureWrapType, GLint>			LIWTextureWrapType_2_GLWrap;
	extern const std::unordered_map<LIWTextureFilterType, GLint>		LIWTextureFilterType_2_GLFilter;
	
	extern const std::unordered_map<LIWTextureImageAccessType, GLenum>	LIWTextureImageAccessType_2_GLAccess;

	extern const std::unordered_map<LIWImageFormat, GLint>				LIWImageFormat_2_GLFormat;
	extern const std::unordered_map<LIWImageFormat, GLenum>				LIWImageFormat_2_GLDataLayout;
	extern const std::unordered_map<LIWImageFormat, GLenum>				LIWImageFormat_2_GLDataType;

	
	extern const std::unordered_map<LIWRenderAttachmentFormat, GLenum>	LIWRenderAttachmentFormat_2_GLDataLayout;
	extern const std::unordered_map<LIWRenderAttachmentFormat, GLenum>	LIWRenderAttachmentFormat_2_GLDataType;


	class LIWTexture {
	public:
		// Texture
		static void Bind2DTexture(uint32_t rawHandleTexture,
			uint32_t rawHandleShader,
			const char* name, uint32_t textureUnit);
		static void Unbind2DTexture(uint32_t textureUnit);
		// Image (in Texture)
		static void Bind2DTextureImage(uint32_t rawHandleTexture,
			uint32_t rawHandleShader,
			const char* name, uint32_t imageUnit, 
			LIWImageFormat format, 
			LIWTextureImageAccessType accessType = LIWTextureImageAccessType_ReadWrite,
			int mipmapLevel = 0, int arrayLayer = -1);
		static void Unbind2DTextureImage(uint32_t imageUnit);
	};

	class LIWTexture2D {
	public:
		static const uint32_t sc_invalidHandle = UINT32_MAX;
	public:
		void CreateTexture(int width, int height, LIWImageFormat format, 
			LIWTextureWrapType wrapU = LIWTextureWrapType_Clamp, 
			LIWTextureWrapType wrapV = LIWTextureWrapType_Clamp,
			LIWTextureFilterType filterMin = LIWTextureFilterType_Nearest,
			LIWTextureFilterType filterMag = LIWTextureFilterType_Nearest,
			bool generateMipMap = false);
		void CreateTexture(const LIWImage& image,
			LIWTextureWrapType wrapU = LIWTextureWrapType_Repeat,
			LIWTextureWrapType wrapV = LIWTextureWrapType_Repeat,
			LIWTextureFilterType filterMin = LIWTextureFilterType_Linear_MipmapLinear,
			LIWTextureFilterType filterMag = LIWTextureFilterType_Linear, 
			bool generateMipMap = true);
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
		void CreateTexture(int width, int height, LIWRenderAttachmentFormat format,
			LIWTextureWrapType wrapU = LIWTextureWrapType_Clamp,
			LIWTextureWrapType wrapV = LIWTextureWrapType_Clamp,
			LIWTextureFilterType filterMin = LIWTextureFilterType_Nearest,
			LIWTextureFilterType filterMag = LIWTextureFilterType_Nearest,
			bool generateMipMap = false);
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