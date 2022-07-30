#include "LIWFrameBuffer.h"

namespace LIW {
	LIWFrameBuffer::LIWFrameBuffer()
	{
		for (uint32_t i = 0; i < sc_maxColorAttachmentCount; i++) {
			m_handleColorAttachments[i] = sc_invalidHandle;
			m_formatColorAttachments[i] = LIWRenderAttachmentFormat_Max;
		}
	}
	void LIWFrameBuffer::CreateFrameBuffer(int width, int height, liw_flag_type attachmentFlag)
	{
		if (IsValid())
			throw std::runtime_error("framebuffer already created. ");
		
		m_width = width, m_height = height;
		glGenFramebuffers(1, &m_handleFrameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_handleFrameBuffer);

		// Color
		//0
		if (attachmentFlag & LIW_FRAMEBUFFER_ATTACHMENT_FLAG_COLOR_RGBA) {
			CreateAttachment_Texture2D(m_width, m_height, LIWRenderAttachmentFormat_ColorRGBA, m_handleColorAttachments[0]);
			m_formatColorAttachments[0] = LIWRenderAttachmentFormat_ColorRGBA;
		}
		else if (attachmentFlag & LIW_FRAMEBUFFER_ATTACHMENT_FLAG_COLOR_RGB) {
			CreateAttachment_Texture2D(m_width, m_height, LIWRenderAttachmentFormat_ColorRGB, m_handleColorAttachments[0]);
			m_formatColorAttachments[0] = LIWRenderAttachmentFormat_ColorRGB;
		}
		//1
		if (attachmentFlag & LIW_FRAMEBUFFER_ATTACHMENT_FLAG_COLOR_RGBA_1) {
			CreateAttachment_Texture2D(m_width, m_height, LIWRenderAttachmentFormat_ColorRGBA, m_handleColorAttachments[1]);
			m_formatColorAttachments[1] = LIWRenderAttachmentFormat_ColorRGBA;
		}
		else if (attachmentFlag & LIW_FRAMEBUFFER_ATTACHMENT_FLAG_COLOR_RGB_1) {
			CreateAttachment_Texture2D(m_width, m_height, LIWRenderAttachmentFormat_ColorRGB, m_handleColorAttachments[1]);
			m_formatColorAttachments[1] = LIWRenderAttachmentFormat_ColorRGB;
		}
		//2
		if (attachmentFlag & LIW_FRAMEBUFFER_ATTACHMENT_FLAG_COLOR_RGBA_2) {
			CreateAttachment_Texture2D(m_width, m_height, LIWRenderAttachmentFormat_ColorRGBA, m_handleColorAttachments[2]);
			m_formatColorAttachments[2] = LIWRenderAttachmentFormat_ColorRGBA;
		}
		else if (attachmentFlag & LIW_FRAMEBUFFER_ATTACHMENT_FLAG_COLOR_RGB_2) {
			CreateAttachment_Texture2D(m_width, m_height, LIWRenderAttachmentFormat_ColorRGB, m_handleColorAttachments[2]);
			m_formatColorAttachments[2] = LIWRenderAttachmentFormat_ColorRGB;
		}
		//3
		if (attachmentFlag & LIW_FRAMEBUFFER_ATTACHMENT_FLAG_COLOR_RGBA_3) {
			CreateAttachment_Texture2D(m_width, m_height, LIWRenderAttachmentFormat_ColorRGBA, m_handleColorAttachments[3]);
			m_formatColorAttachments[3] = LIWRenderAttachmentFormat_ColorRGBA;
		}
		else if (attachmentFlag & LIW_FRAMEBUFFER_ATTACHMENT_FLAG_COLOR_RGB_3) {
			CreateAttachment_Texture2D(m_width, m_height, LIWRenderAttachmentFormat_ColorRGB, m_handleColorAttachments[3]);
			m_formatColorAttachments[3] = LIWRenderAttachmentFormat_ColorRGB;
		}


		// Depth Stencil
		if (attachmentFlag & LIW_FRAMEBUFFER_ATTACHMENT_FLAG_DEPTHSTENCIL) {
			CreateAttachment_Texture2D(m_width, m_height, LIWRenderAttachmentFormat_DepthStencil, m_handleDepthAttachment);
			m_formatDepthAttachment = LIWRenderAttachmentFormat_DepthStencil;
		}
		else {
			if (attachmentFlag & LIW_FRAMEBUFFER_ATTACHMENT_FLAG_DEPTH) {
				CreateAttachment_Texture2D(m_width, m_height, LIWRenderAttachmentFormat_Depth, m_handleDepthAttachment);
				m_formatDepthAttachment = LIWRenderAttachmentFormat_Depth;
			}
			if (attachmentFlag & LIW_FRAMEBUFFER_ATTACHMENT_FLAG_STENCIL) {
				CreateAttachment_Texture2D(m_width, m_height, LIWRenderAttachmentFormat_Stencil, m_handleStencilAttachment);
				m_formatStencilAttachment = LIWRenderAttachmentFormat_Stencil;
			}
		}


		GLenum completeness = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if (completeness != GL_FRAMEBUFFER_COMPLETE) {
			throw std::runtime_error("framebuffer creation not valid. ");
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}
	void LIWFrameBuffer::DestroyFrameBuffer()
	{
		if (!IsValid())
			throw std::runtime_error("framebuffer not created. ");
		glDeleteFramebuffers(1, &m_handleFrameBuffer);
		m_handleFrameBuffer = sc_invalidHandle;
		m_width = m_height = -1;
		for (uint32_t i = 0; i < sc_maxColorAttachmentCount; i++) {
			if (m_handleColorAttachments[i] != sc_invalidHandle) {
				glDeleteTextures(1, &m_handleColorAttachments[i]);
			}
			m_formatColorAttachments[i] = LIWRenderAttachmentFormat_Max;
		}
		if (m_handleDepthAttachment != sc_invalidHandle) {
			glDeleteTextures(1, &m_handleDepthAttachment);
		}
		if (m_handleStencilAttachment != sc_invalidHandle) {
			glDeleteTextures(1, &m_handleStencilAttachment);
		}
		m_formatDepthAttachment = m_formatStencilAttachment = LIWRenderAttachmentFormat_Max;
	}
	void LIWFrameBuffer::CreateAttachment_Texture2D(int width, int height, LIWRenderAttachmentFormat format, uint32_t& handleOut)
	{
		glGenTextures(1, &handleOut);
		glBindTexture(GL_TEXTURE_2D, handleOut);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		const GLint glInternalFormat = LIWRenderAttachmentFormat_2_GLInternalFormat.at(format);
		const GLenum glFormat = LIWRenderAttachmentFormat_2_GLFormat.at(format);
		const GLenum glDataType = LIWRenderAttachmentFormat_2_GLDataType.at(format);

		glTexImage2D(GL_TEXTURE_2D, 0, glInternalFormat, width, height, 0, glFormat, glDataType, nullptr);

		glBindTexture(GL_TEXTURE_2D, 0);

		const GLenum glAttachment = LIWRenderAttachmentFormat_2_GLAttachment.at(format);
		glFramebufferTexture2D(GL_FRAMEBUFFER, glAttachment, GL_TEXTURE_2D, handleOut, 0);
	}
}