#include "LIWRenderBuffer.h"

namespace LIW {
	void LIWRenderBuffer::CreateRenderBuffer(int width, int height, LIWRenderAttachmentFormat format)
	{
		if (m_handleBuffer != sc_invalidHandle)
			throw std::runtime_error("renderbuffer already created. ");

		m_width = width, m_height = height;
		m_format = format;

		glGenRenderbuffers(1, &m_handleBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, m_handleBuffer);

		const GLint glInternalFormat = LIWRenderAttachmentFormat_2_GLInternalFormat.at(format);

		glRenderbufferStorage(GL_RENDERBUFFER, glInternalFormat, width, height);

		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}
	void LIWRenderBuffer::DestroyRenderBuffer()
	{
		if (!IsValid())
			throw std::runtime_error("renderbuffer not created. ");
		glDeleteRenderbuffers(1, &m_handleBuffer);
		m_handleBuffer = sc_invalidHandle;
		m_width = m_height = -1;
		m_format = LIWRenderAttachmentFormat_Max;
	}
}