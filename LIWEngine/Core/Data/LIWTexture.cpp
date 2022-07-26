#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "LIWTexture.h"

void LIW::LIWTexture::LoadImage(const char* imagePath)
{
	assert(!m_rawData); //TODO: support reloading
	//TODO: replace stb_image/add new lib. Too few options.
	//TODO: add LIWTextureFormat
	stbi_uc* pixels = stbi_load(imagePath, &m_width, &m_height, &m_channels, STBI_rgb_alpha);
	if (!pixels) {
		throw std::runtime_error("failed to load texture image!");
	}
	m_rawData = pixels;
}

void LIW::LIWTexture::UnloadImage()
{
	stbi_image_free(m_rawData);
	m_rawData = nullptr;
	m_width = m_height = m_channels = -1;
}
