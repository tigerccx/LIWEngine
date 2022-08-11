#include "LIWTexture.h"

namespace LIW {
	const std::unordered_map<LIWTextureWrapType, GLint>	LIWTextureWrapType_2_GLWrap =
	{
		{LIWTextureWrapType_Repeat, 		GL_REPEAT	},
		{LIWTextureWrapType_Clamp, 			GL_CLAMP	}
	};
	const std::unordered_map<LIWTextureFilterType, GLint>	LIWTextureFilterType_2_GLFilter =
	{
		{LIWTextureFilterType_Nearest, 					GL_NEAREST					},
		{LIWTextureFilterType_Linear,					GL_LINEAR					},
		{LIWTextureFilterType_Nearest_MipmapNearest,	GL_NEAREST_MIPMAP_NEAREST	},
		{LIWTextureFilterType_Linear_MipmapNearest,		GL_LINEAR_MIPMAP_NEAREST	},
		{LIWTextureFilterType_Nearest_MipmapLinear,		GL_NEAREST_MIPMAP_LINEAR	},
		{LIWTextureFilterType_Linear_MipmapLinear,		GL_LINEAR_MIPMAP_LINEAR		}
	};

	const std::unordered_map<LIWTextureImageAccessType, GLenum>	LIWTextureImageAccessType_2_GLAccess =
	{
		{LIWTextureImageAccessType_ReadWrite, 	GL_READ_WRITE 	},
		{LIWTextureImageAccessType_Read,		GL_READ_ONLY	},
		{LIWTextureImageAccessType_Write,		GL_WRITE_ONLY	}
	};

	const std::unordered_map<LIWImageFormat, GLint> LIWImageFormat_2_GLFormat =
	{
		{LIWImageFormat_R8, 		GL_R8		},
		{LIWImageFormat_RG8,		GL_RG8		},
		{LIWImageFormat_RGB8,		GL_RGB8		},
		{LIWImageFormat_RGBA8,		GL_RGBA8	},
		{LIWImageFormat_R16U,		GL_R16UI	},
		{LIWImageFormat_RG16U,		GL_RG16UI	},
		{LIWImageFormat_RGB16U,		GL_RGB16UI	},
		{LIWImageFormat_RGBA16U,	GL_RGBA16UI	},
		{LIWImageFormat_R32F,		GL_R32F		},
		{LIWImageFormat_RG32F,		GL_RG32F	},
		{LIWImageFormat_RGB32F,		GL_RGB32F	},
		{LIWImageFormat_RGBA32F,	GL_RGBA32F	},
		{LIWImageFormat_R32I,		GL_R32I		},
		{LIWImageFormat_RG32I,		GL_RG32I	},
		{LIWImageFormat_RGB32I,		GL_RGB32I	},
		{LIWImageFormat_RGBA32I,	GL_RGBA32I	},
		{LIWImageFormat_R32U,		GL_R32UI	},
		{LIWImageFormat_RG32U,		GL_RG32UI	},
		{LIWImageFormat_RGB32U,		GL_RGB32UI	},
		{LIWImageFormat_RGBA32U,	GL_RGBA32UI	}
	};
	const std::unordered_map<LIWImageFormat, GLenum> LIWImageFormat_2_GLDataLayout = 
	{
		{LIWImageFormat_R8,			GL_RED	},
		{LIWImageFormat_RG8,		GL_RG	},
		{LIWImageFormat_RGB8,		GL_RGB	},
		{LIWImageFormat_RGBA8,		GL_RGBA	},
		{LIWImageFormat_R16U,		GL_RED	},
		{LIWImageFormat_RG16U,		GL_RG	},
		{LIWImageFormat_RGB16U,		GL_RGB	},
		{LIWImageFormat_RGBA16U,	GL_RGBA	},
		{LIWImageFormat_R32F,		GL_RED	},
		{LIWImageFormat_RG32F,		GL_RG	},
		{LIWImageFormat_RGB32F,		GL_RGB	},
		{LIWImageFormat_RGBA32F,	GL_RGBA	},
		{LIWImageFormat_R32I,		GL_RED_INTEGER	},
		{LIWImageFormat_RG32I,		GL_RG_INTEGER	},
		{LIWImageFormat_RGB32I,		GL_RGB_INTEGER	},
		{LIWImageFormat_RGBA32I,	GL_RGBA_INTEGER	},
		{LIWImageFormat_R32U,		GL_RED_INTEGER	},
		{LIWImageFormat_RG32U,		GL_RG_INTEGER	},
		{LIWImageFormat_RGB32U,		GL_RGB_INTEGER	},
		{LIWImageFormat_RGBA32U,	GL_RGBA_INTEGER	}
	};
	const std::unordered_map<LIWImageFormat, GLenum> LIWImageFormat_2_GLDataType =
	{
		{LIWImageFormat_R8,			GL_UNSIGNED_BYTE	},
		{LIWImageFormat_RG8,		GL_UNSIGNED_BYTE	},
		{LIWImageFormat_RGB8,		GL_UNSIGNED_BYTE	},
		{LIWImageFormat_RGBA8,		GL_UNSIGNED_BYTE	},
		{LIWImageFormat_R16U,		GL_UNSIGNED_SHORT	},
		{LIWImageFormat_RG16U,		GL_UNSIGNED_SHORT	},
		{LIWImageFormat_RGB16U,		GL_UNSIGNED_SHORT	},
		{LIWImageFormat_RGBA16U,	GL_UNSIGNED_SHORT	},
		{LIWImageFormat_R32F,		GL_FLOAT			},
		{LIWImageFormat_RG32F,		GL_FLOAT			},
		{LIWImageFormat_RGB32F,		GL_FLOAT			},
		{LIWImageFormat_RGBA32F,	GL_FLOAT			},
		{LIWImageFormat_R32I,		GL_INT				},
		{LIWImageFormat_RG32I,		GL_INT				},
		{LIWImageFormat_RGB32I,		GL_INT				},
		{LIWImageFormat_RGBA32I,	GL_INT				},
		{LIWImageFormat_R32U,		GL_UNSIGNED_INT		},
		{LIWImageFormat_RG32U,		GL_UNSIGNED_INT		},
		{LIWImageFormat_RGB32U,		GL_UNSIGNED_INT		},
		{LIWImageFormat_RGBA32U,	GL_UNSIGNED_INT		}
	};


	const std::unordered_map<LIWRenderAttachmentFormat, GLenum> LIWRenderAttachmentFormat_2_GLDataLayout =
	{
		{LIWRenderAttachmentFormat_ColorRGB,		GL_RGB					},
		{LIWRenderAttachmentFormat_ColorRGBA,		GL_RGBA					},
		{LIWRenderAttachmentFormat_Index,			GL_RED_INTEGER			},
		{LIWRenderAttachmentFormat_Index16,			GL_RED_INTEGER			},
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
		{LIWRenderAttachmentFormat_Index,			GL_UNSIGNED_INT			},
		{LIWRenderAttachmentFormat_Index16,			GL_UNSIGNED_SHORT		},
		{LIWRenderAttachmentFormat_Depth,			GL_FLOAT				},
		{LIWRenderAttachmentFormat_Depth24,			GL_UNSIGNED_BYTE		},
		{LIWRenderAttachmentFormat_Stencil,			GL_UNSIGNED_INT			},
		{LIWRenderAttachmentFormat_Stencil8,		GL_UNSIGNED_BYTE		},
		{LIWRenderAttachmentFormat_DepthStencil,	GL_UNSIGNED_INT_24_8	}
	};

	void LIWTexture2D::CreateTexture(int width, int height, LIWImageFormat format, 
									LIWTextureWrapType wrapU,
									LIWTextureWrapType wrapV,
									LIWTextureFilterType filterMin,
									LIWTextureFilterType filterMag,
									bool generateMipMap)
	{
		if (IsValid())
			throw std::runtime_error("texture already created. ");

		m_width = width, m_height = height;
		m_format = format;

		glGenTextures(1, &m_handleTexture);
		glBindTexture(GL_TEXTURE_2D, m_handleTexture);

		const GLint glWrapU = LIWTextureWrapType_2_GLWrap.at(wrapU);
		const GLint glWrapV = LIWTextureWrapType_2_GLWrap.at(wrapV);
		const GLint glMinFilter = LIWTextureFilterType_2_GLFilter.at(filterMin);
		const GLint glMagFilter = LIWTextureFilterType_2_GLFilter.at(filterMag);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glWrapU);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glWrapV);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glMinFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glMagFilter);
		if (!generateMipMap) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		}

		const GLint glInternalFormat = LIWImageFormat_2_GLFormat.at(format);
		const GLenum glDataLayout = LIWImageFormat_2_GLDataLayout.at(format);
		const GLenum glDataType = LIWImageFormat_2_GLDataType.at(format);

		glTexImage2D(GL_TEXTURE_2D, 0, glInternalFormat, m_width, m_height, 0, glDataLayout, glDataType, nullptr);
		if (generateMipMap) {
			glGenerateMipmap(GL_TEXTURE_2D);
		}

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void LIWTexture2D::CreateTexture(const LIWImage& image,
									LIWTextureWrapType wrapU,
									LIWTextureWrapType wrapV,
									LIWTextureFilterType filterMin,
									LIWTextureFilterType filterMag,
									bool generateMipMap)
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

		const GLint glWrapU = LIWTextureWrapType_2_GLWrap.at(wrapU);
		const GLint glWrapV = LIWTextureWrapType_2_GLWrap.at(wrapV);
		const GLint glMinFilter = LIWTextureFilterType_2_GLFilter.at(filterMin);
		const GLint glMagFilter = LIWTextureFilterType_2_GLFilter.at(filterMag);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glWrapU);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glWrapV);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glMinFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glMagFilter);
		if (!generateMipMap) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		}

		const GLint glInternalFormat = LIWImageFormat_2_GLFormat.at(m_format);
		const GLenum glDataLayout = LIWImageFormat_2_GLDataLayout.at(m_format);
		const GLenum glDataType = LIWImageFormat_2_GLDataType.at(m_format);

		glTexImage2D(GL_TEXTURE_2D, 0, glInternalFormat, m_width, m_height, 0, glDataLayout, glDataType, image.GetRawData());
		if (generateMipMap) {
			glGenerateMipmap(GL_TEXTURE_2D);
		}

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
	void LIWRenderTexture2D::CreateTexture(int width, int height, LIWRenderAttachmentFormat format,
										LIWTextureWrapType wrapU,
										LIWTextureWrapType wrapV,
										LIWTextureFilterType filterMin,
										LIWTextureFilterType filterMag,
										bool generateMipMap)
	{
		if (IsValid())
			throw std::runtime_error("texture already created. ");

		m_width = width, m_height = height;
		m_format = format;

		glGenTextures(1, &m_handleTexture);
		glBindTexture(GL_TEXTURE_2D, m_handleTexture);

		const GLint glWrapU = LIWTextureWrapType_2_GLWrap.at(wrapU);
		const GLint glWrapV = LIWTextureWrapType_2_GLWrap.at(wrapV);
		const GLint glMinFilter = LIWTextureFilterType_2_GLFilter.at(filterMin);
		const GLint glMagFilter = LIWTextureFilterType_2_GLFilter.at(filterMag);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glWrapU);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glWrapV);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glMinFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glMagFilter);
		if (!generateMipMap) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		}

		const GLint glInternalFormat = LIWRenderAttachmentFormat_2_GLFormat.at(format);
		const GLenum glDataLayout = LIWRenderAttachmentFormat_2_GLDataLayout.at(format);
		const GLenum glDataType = LIWRenderAttachmentFormat_2_GLDataType.at(format);

		glTexImage2D(GL_TEXTURE_2D, 0, glInternalFormat, m_width, m_height, 0, glDataLayout, glDataType, nullptr);
		if (generateMipMap) {
			glGenerateMipmap(GL_TEXTURE_2D);
		}

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

	void LIWTexture::Bind2DTexture(uint32_t rawHandleTexture, uint32_t rawHandleShader, const char* name, uint32_t textureUnit)
	{
		const uint32_t texUnit = textureUnit;
		uint32_t location = glGetUniformLocation(rawHandleShader, name);
		glUniform1i(location, texUnit);	//Setting sampler in shaderPrePixelBump
		glActiveTexture(GL_TEXTURE0 + texUnit);	//Activating textureTest0 slot on GPU
		glBindTexture(GL_TEXTURE_2D, rawHandleTexture);
	}
	void LIWTexture::Unbind2DTexture(uint32_t textureUnit)
	{
		glActiveTexture(GL_TEXTURE0 + textureUnit);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	void LIWTexture::Bind2DTextureImage(uint32_t rawHandleTexture, uint32_t rawHandleShader, const char* name, uint32_t imageUnit, 
										LIWImageFormat format,
										LIWTextureImageAccessType accessType, 
										int mipmapLevel, int arrayLayer)
	{
		const uint32_t imgUnit = imageUnit;
		uint32_t location = glGetUniformLocation(rawHandleShader, name);
		glUniform1i(location, imageUnit);
		const bool isArray = arrayLayer >= 0;
		const GLenum glAccess = LIWTextureImageAccessType_2_GLAccess.at(accessType);
		const GLint glFormat = LIWImageFormat_2_GLFormat.at(format);
		glBindImageTexture(imageUnit,
			rawHandleTexture, 
			mipmapLevel, isArray, isArray ? arrayLayer : 0,
			glAccess, glFormat);
	}
	void LIWTexture::Unbind2DTextureImage(uint32_t imageUnit)
	{
		glBindImageTexture(imageUnit, 0, 0, false, 0, GL_READ_WRITE, GL_R8);
		//glBindImageTexture(imageUnit, 0, 0, false, 0, 0, 0);
	}
}
