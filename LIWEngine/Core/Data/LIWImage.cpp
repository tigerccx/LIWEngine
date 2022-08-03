#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "LIWImage.h"

namespace LIW {
	const std::unordered_map<LIWImageFormat, unsigned int> LIWImageFormat_2_STBImageFormat =
	{
		{LIWImageFormat_R8,			STBI_grey		},
		{LIWImageFormat_RG8,		STBI_grey_alpha	},
		{LIWImageFormat_RGB8,		STBI_rgb		},
		{LIWImageFormat_RGBA8,		STBI_rgb_alpha	}
	};
	const std::unordered_map<LIWImageFormat, uint32_t> LIWImageFormat_2_channels =
	{
		{LIWImageFormat_R8,			1	},
		{LIWImageFormat_RG8,		2	},
		{LIWImageFormat_RGB8,		3	},
		{LIWImageFormat_RGBA8,		4	},
		{LIWImageFormat_R32F,		1	},
		{LIWImageFormat_RG32F,		2	},
		{LIWImageFormat_RGB32F,		3	},
		{LIWImageFormat_RGBA32F,	4	},
		{LIWImageFormat_R32I,		1	},
		{LIWImageFormat_RG32I,		2	},
		{LIWImageFormat_RGB32I,		3	},
		{LIWImageFormat_RGBA32I,	4	},
		{LIWImageFormat_R32U,		1	},
		{LIWImageFormat_RG32U,		2	},
		{LIWImageFormat_RGB32U,		3	},
		{LIWImageFormat_RGBA32U,	4	}
	};

	void LIWImage::LoadImg(const char* imagePath, LIWImageFormat format)
	{
		if (IsValid())
			throw std::runtime_error("image already loaded. ");
		//TODO: replace stb_image/add new lib. Too few options.
		if(format>= LIWImageFormat_Loadable_Max)
			throw std::runtime_error("image format not loadable. ");

		unsigned int stbiFormat = LIWImageFormat_2_STBImageFormat.at(format);

		stbi_uc* pixels = stbi_load(imagePath, &m_width, &m_height, nullptr, stbiFormat);
		if (!pixels) {
			throw std::runtime_error("failed to load image. ");
		}
		m_rawData = pixels;
		m_format = format;
	}

	void LIWImage::UnloadImg()
	{
		if(!IsValid())
			throw std::runtime_error("image not loaded. ");
		stbi_image_free(m_rawData);
		m_rawData = nullptr;
		m_width = m_height = -1;
		m_format = LIWImageFormat_Max;
	}
}
