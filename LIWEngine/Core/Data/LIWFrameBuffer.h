#pragma once

#include "LIWglfw.h"
#include "LIWTypes.h"
#include "LIWConstants.h"
#include "LIWTexture.h"
#include "LIWRenderBuffer.h"

#define LIW_FRAMEBUFFER_ATTACHMENT_FLAG_COLOR_RGB		uint32_t(1)<<0
#define LIW_FRAMEBUFFER_ATTACHMENT_FLAG_COLOR_RGBA		uint32_t(1)<<1
#define LIW_FRAMEBUFFER_ATTACHMENT_FLAG_DEPTH			uint32_t(1)<<2
#define LIW_FRAMEBUFFER_ATTACHMENT_FLAG_STENCIL			uint32_t(1)<<3
#define LIW_FRAMEBUFFER_ATTACHMENT_FLAG_DEPTHSTENCIL	uint32_t(1)<<4
#define LIW_FRAMEBUFFER_ATTACHMENT_FLAG_COLOR_RGB_1		uint32_t(1)<<5
#define LIW_FRAMEBUFFER_ATTACHMENT_FLAG_COLOR_RGBA_1	uint32_t(1)<<6
#define LIW_FRAMEBUFFER_ATTACHMENT_FLAG_COLOR_RGB_2		uint32_t(1)<<7
#define LIW_FRAMEBUFFER_ATTACHMENT_FLAG_COLOR_RGBA_2	uint32_t(1)<<8
#define LIW_FRAMEBUFFER_ATTACHMENT_FLAG_COLOR_RGB_3		uint32_t(1)<<9
#define LIW_FRAMEBUFFER_ATTACHMENT_FLAG_COLOR_RGBA_3	uint32_t(1)<<10


namespace LIW {
	//enum LIWFrameBufferAttachmentType {
	//	LIWFrameBufferAttachmentType_RenderTexture2D,
	//	LIWFrameBufferAttachmentType_RenderBuffer,
	//	LIWFrameBufferAttachmentType_Max
	//};



	//NOTE: Only binding render texture to framebuffer..
	//NOTE: render texture can only be managed by framebuffer

	class LIWFrameBuffer {
	public:
		static const uint32_t sc_invalidHandle = UINT32_MAX;
		static const uint32_t sc_maxColorAttachmentCount = 4;
	public:
		LIWFrameBuffer();
	public:
		void CreateFrameBuffer(int width, int height, liw_flag_type attachmentFlag);
		void DestroyFrameBuffer();
	public:
		inline bool IsValid() const { 
			return (m_handleFrameBuffer != sc_invalidHandle);
		}
		inline bool IsComplete() const {
			GLenum completeness = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			return completeness == GL_FRAMEBUFFER_COMPLETE;
		}
		inline int GetWidth() const { return m_width; }
		inline int GetHeight() const { return m_height; }
		inline uint32_t GetRawHandle() const { return m_handleFrameBuffer; }
		

		inline uint32_t GetColorAttachmentRawHandle(uint32_t idx = 0) const { return m_handleColorAttachments[idx]; }
		inline LIWRenderAttachmentFormat GetColorAttachmentFormat(uint32_t idx = 0) const { return m_formatColorAttachments[idx]; }
		
		inline uint32_t GetDepthAttachmentRawHandle() const { 
			if (m_formatDepthAttachment == LIWRenderAttachmentFormat_Depth)
				return m_handleDepthAttachment; 
			return sc_invalidHandle;
		}
		inline LIWRenderAttachmentFormat GetDepthAttachmentFormat() const { return m_formatDepthAttachment; }
		
		inline uint32_t GetStencilAttachmentRawHandle() const { return m_handleStencilAttachment; }
		inline LIWRenderAttachmentFormat GetStencilAttachmentFormat() const { return m_formatStencilAttachment; }
		
		inline uint32_t GetDepthStencilAttachmentRawHandle() const {
			if (m_formatDepthAttachment == LIWRenderAttachmentFormat_DepthStencil)
				return m_handleDepthAttachment;
			return sc_invalidHandle;
		}
		inline LIWRenderAttachmentFormat GetDepthStencilAttachmentFormat() const { return m_formatDepthAttachment; }

	private:
		void CreateAttachment_Texture2D(int width, int height, LIWRenderAttachmentFormat format, uint32_t& handleOut);
	private:
		uint32_t m_handleFrameBuffer{ sc_invalidHandle };
		int m_width{ -1 };
		int m_height{ -1 };
		uint32_t					m_handleColorAttachments[sc_maxColorAttachmentCount];
		LIWRenderAttachmentFormat	m_formatColorAttachments[sc_maxColorAttachmentCount];
		uint32_t					m_handleDepthAttachment{ sc_invalidHandle }; //Also used to store depth&stencil if LIWRenderAttachmentFormat_DepthStencil
		LIWRenderAttachmentFormat	m_formatDepthAttachment{ LIWRenderAttachmentFormat_Max };
		uint32_t					m_handleStencilAttachment{ sc_invalidHandle };
		LIWRenderAttachmentFormat	m_formatStencilAttachment{ LIWRenderAttachmentFormat_Max };
	};
}