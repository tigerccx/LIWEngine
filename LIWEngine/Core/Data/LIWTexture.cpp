#include "LIWTexture.h"

namespace LIW {
	const std::unordered_map<LIWImageFormat, GLint> LIWImageFormat_2_GLInternalFormat =
	{
		{LIWImageFormat_R,		GL_R	},
		{LIWImageFormat_RG,		GL_RG	},
		{LIWImageFormat_RGB,	GL_RGB	},
		{LIWImageFormat_RGBA,	GL_RGBA	}
	};
	const std::unordered_map<LIWImageFormat, GLenum> LIWImageFormat_2_GLFormat = 
	{
		{LIWImageFormat_R,		GL_R	},
		{LIWImageFormat_RG,		GL_RG	},
		{LIWImageFormat_RGB,	GL_RGB	},
		{LIWImageFormat_RGBA,	GL_RGBA	}
	};

	const std::unordered_map<LIWRenderAttachmentFormat, GLenum> LIWRenderAttachmentFormat_2_GLFormat =
	{
		{LIWRenderAttachmentFormat_ColorRGB,		GL_RGB					},
		{LIWRenderAttachmentFormat_ColorRGBA,		GL_RGBA					},
		{LIWRenderAttachmentFormat_Depth,			GL_DEPTH_COMPONENT		},
		{LIWRenderAttachmentFormat_Depth24,			GL_DEPTH_COMPONENT24	},
		{LIWRenderAttachmentFormat_Stencil,			GL_STENCIL_INDEX		},
		{LIWRenderAttachmentFormat_Stencil8,		GL_STENCIL_INDEX8		},
		{LIWRenderAttachmentFormat_DepthStencil,	GL_DEPTH_STENCIL		}
	};
	const std::unordered_map<LIWRenderAttachmentFormat, GLenum> LIWRenderAttachmentFormat_2_GLDataType =
	{
		{LIWRenderAttachmentFormat_ColorRGB,		GL_UNSIGNED_BYTE		},
		{LIWRenderAttachmentFormat_ColorRGBA,		GL_UNSIGNED_BYTE		},
		{LIWRenderAttachmentFormat_Depth,			GL_FLOAT				},
		{LIWRenderAttachmentFormat_Depth24,			GL_UNSIGNED_BYTE		},
		{LIWRenderAttachmentFormat_Stencil,			GL_UNSIGNED_INT			},
		{LIWRenderAttachmentFormat_Stencil8,		GL_UNSIGNED_BYTE		},
		{LIWRenderAttachmentFormat_DepthStencil,	GL_UNSIGNED_INT_24_8	}
	};

	void LIWTexture2D::CreateTexture(int width, int height, LIWImageFormat format)
	{
		if (IsValid())
			throw std::runtime_error("texture already created. ");

		m_width = width, m_height = height;
		m_format = format;

		glGenTextures(1, &m_handleTexture);
		glBindTexture(GL_TEXTURE_2D, m_handleTexture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		const GLint glInternalFormat = LIWImageFormat_2_GLInternalFormat.at(format);
		const GLenum glFormat = LIWImageFormat_2_GLFormat.at(format);

		glTexImage2D(GL_TEXTURE_2D, 0, glInternalFormat, m_width, m_height, 0, glFormat, GL_UNSIGNED_BYTE, nullptr);
	
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void LIWTexture2D::CreateTexture(const LIWImage& image)
	{
		if (IsValid())
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

		const GLint glInternalFormat = LIWImageFormat_2_GLInternalFormat.at(m_format);
		const GLenum glFormat = LIWImageFormat_2_GLFormat.at(m_format);

		glTexImage2D(GL_TEXTURE_2D, 0, glInternalFormat, m_width, m_height, 0, glFormat, GL_UNSIGNED_BYTE, image.GetRawData());
		glGenerateMipmap(GL_TEXTURE_2D);

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
	void LIWTexture2D::Bind(uint32_t rawHandleShader, const char* name, uint32_t imageUnit)
	{
		if(!IsValid())
			throw std::runtime_error("texture not created. ");
		const uint32_t texUnit = imageUnit;
		glUniform1i(glGetUniformLocation(rawHandleShader, name), texUnit);	//Setting sampler in shaderPrePixelBump
		glActiveTexture(GL_TEXTURE0 + texUnit);	//Activating textureTest0 slot on GPU
		glBindTexture(GL_TEXTURE_2D, m_handleTexture);
	}
	void LIWTexture2D::Unbind(uint32_t imageUnit)
	{
		glActiveTexture(GL_TEXTURE0 + imageUnit);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	void LIWRenderTexture2D::CreateTexture(int width, int height, LIWRenderAttachmentFormat format)
	{
		if (IsValid())
			throw std::runtime_error("texture already created. ");

		m_width = width, m_height = height;
		m_format = format;

		glGenTextures(1, &m_handleTexture);
		glBindTexture(GL_TEXTURE_2D, m_handleTexture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		const GLint glInternalFormat = LIWRenderAttachmentFormat_2_GLInternalFormat.at(format);
		const GLenum glFormat = LIWRenderAttachmentFormat_2_GLFormat.at(format);
		const GLenum glDataType = LIWRenderAttachmentFormat_2_GLFormat.at(format);

		glTexImage2D(GL_TEXTURE_2D, 0, glInternalFormat, m_width, m_height, 0, glFormat, glDataType, nullptr);

		glBindTexture(GL_TEXTURE_2D, 0);
	}
	void LIWRenderTexture2D::DestroyTexture()
	{
		if (!IsValid())
			throw std::runtime_error("texture not created. ");
		glDeleteTextures(1, &m_handleTexture);
		m_handleTexture = sc_invalidHandle;
		m_width = m_height = -1;
		m_format = LIWRenderAttachmentFormat_Max;
	}
	void LIWRenderTexture2D::Bind(uint32_t rawHandleShader, const char* name, uint32_t imageUnit)
	{
		if (!IsValid())
			throw std::runtime_error("texture not created. ");
		const uint32_t texUnit = imageUnit;
		glUniform1i(glGetUniformLocation(rawHandleShader, name), texUnit);	//Setting sampler in shaderPrePixelBump
		glActiveTexture(GL_TEXTURE0 + texUnit);	//Activating textureTest0 slot on GPU
		glBindTexture(GL_TEXTURE_2D, m_handleTexture);
	}
	void LIWRenderTexture2D::Unbind(uint32_t imageUnit)
	{
		glActiveTexture(GL_TEXTURE0 + imageUnit);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void LIWTexture::Bind2DTexture(uint32_t rawHandleTexture, uint32_t rawHandleShader, const char* name, uint32_t imageUnit)
	{
		const uint32_t texUnit = imageUnit;
		uint32_t location = glGetUniformLocation(rawHandleShader, name);
		glUniform1i(location, texUnit);	//Setting sampler in shaderPrePixelBump
		glActiveTexture(GL_TEXTURE0 + texUnit);	//Activating textureTest0 slot on GPU
		glBindTexture(GL_TEXTURE_2D, rawHandleTexture);
	}
	void LIWTexture::Unbind2DTexture(uint32_t imageUnit)
	{
		glActiveTexture(GL_TEXTURE0 + imageUnit);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}
