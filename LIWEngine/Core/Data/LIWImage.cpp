#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "LIWImage.h"

namespace LIW {
	const std::unordered_map<LIWImageFormat, unsigned int> LIWImageFormat_2_STBImageFormat =
	{
		{LIWImageFormat_R,		STBI_grey		},
		{LIWImageFormat_RG,		STBI_grey_alpha	},
		{LIWImageFormat_RGB,	STBI_rgb		},
		{LIWImageFormat_RGBA,	STBI_rgb_alpha	}
	};
	const std::unordered_map<LIWImageFormat, uint32_t> LIWImageFormat_2_channels =
	{
		{LIWImageFormat_R,		0	},
		{LIWImageFormat_RG,		1	},
		{LIWImageFormat_RGB,	2	},
		{LIWImageFormat_RGBA,	3	}
	};

	void LIWImage::LoadImg(const char* imagePath, LIWImageFormat format)
	{
		if (IsValid())
			throw std::runtime_error("image already loaded. ");
		//TODO: replace stb_image/add new lib. Too few options.
		if(format>= LIWImageFormat_Max)
			throw std::runtime_error("image format invalid. ");

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
