#include "LIWFrameBuffer.h"

namespace LIW {
	void LIWFrameBuffer::CreateFrameBuffer(int width, int height, liw_flag_type attachmentFlag)
	{
		if (IsValid())
			throw std::runtime_error("framebuffer already created. ");
		
		m_width = width, m_height = height;
		glGenFramebuffers(1, &m_handleFrameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_handleFrameBuffer);

		// Color
		if (attachmentFlag & LIW_FRAMEBUFFER_ATTACHMENT_FLAG_COLOR_RGBA) {
			CreateAttachment_Texture2D(m_width, m_height, LIWRenderAttachmentFormat_ColorRGBA, m_handleColorAttachment);
			m_formatColorAttachment = LIWRenderAttachmentFormat_ColorRGBA;
		}
		else if (attachmentFlag & LIW_FRAMEBUFFER_ATTACHMENT_FLAG_COLOR_RGB) {
			CreateAttachment_Texture2D(m_width, m_height, LIWRenderAttachmentFormat_ColorRGB, m_handleColorAttachment);
			m_formatColorAttachment = LIWRenderAttachmentFormat_ColorRGB;
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
		if (m_handleColorAttachment != sc_invalidHandle) {
			glDeleteTextures(1, &m_handleColorAttachment);
		}
		if (m_handleDepthAttachment != sc_invalidHandle) {
			glDeleteTextures(1, &m_handleDepthAttachment);
		}
		if (m_handleStencilAttachment != sc_invalidHandle) {
			glDeleteTextures(1, &m_handleStencilAttachment);
		}
		m_formatColorAttachment = m_formatDepthAttachment = m_formatStencilAttachment = LIWRenderAttachmentFormat_Max;
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