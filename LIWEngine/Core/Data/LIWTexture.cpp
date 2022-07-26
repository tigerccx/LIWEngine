#include "LIWTexture.h"

namespace LIW {
	const std::unordered_map<LIWImageFormat, GLint> LIWImageFormat_2_GLInternalFormat =
	{
		{LIWImageFormat_R,		GL_R	},
		{LIWImageFormat_RG,		GL_RG	},
		{LIWImageFormat_RGB,	GL_RGB	},
		{LIWImageFormat_RGBA,	GL_RGBA	}
	};

	void LIWTexture2D::CreateTexture(int width, int height, LIWImageFormat format)
	{
		if (m_handleTexture != sc_invalidHandle)
			throw std::runtime_error("texture already created. ");

		m_width = width, m_height = height;
		m_format = format;

		glGenTextures(1, &m_handleTexture);
		glBindTexture(GL_TEXTURE_2D, m_handleTexture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		GLint glFormat = LIWImageFormat_2_GLInternalFormat.at(format);

		glTexImage2D(GL_TEXTURE_2D, 0, glFormat, m_width, m_height, 0, glFormat, GL_UNSIGNED_BYTE, nullptr);
	
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void LIWTexture2D::CreateTexture(const LIWImage& image)
	{
		if (m_handleTexture != sc_invalidHandle)
			throw std::runtime_error("texture already created. ");
		if (!image.IsValid())
			throw std::runtime_error("invalid image. ");

		m_width = image.GetWidth();
		m_height = image.GetHeight();
		m_format = image.GetFormat();

		glGenTextures(1, &m_handleTexture);
		glBindTexture(GL_TEXTURE_2D, m_handleTexture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		GLint glFormat = LIWImageFormat_2_GLInternalFormat.at(m_format);

		glTexImage2D(GL_TEXTURE_2D, 0, glFormat, m_width, m_height, 0, glFormat, GL_UNSIGNED_BYTE, nullptr);
		glGenerateMipmap(m_handleTexture);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void LIWTexture2D::DestroyTexture()
	{
		if(!IsValid())
			throw std::runtime_error("texture not created. ");
		glDeleteTextures(1, &m_handleTexture);
		m_handleTexture = sc_invalidHandle;
		m_width = m_height = -1;
		m_format = LIWImageFormat_Max;
	}
}
