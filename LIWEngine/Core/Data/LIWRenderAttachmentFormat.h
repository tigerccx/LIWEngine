#pragma once
#include <unordered_map>

#include "LIWglfw.h"

namespace LIW {
	enum LIWRenderAttachmentFormat {
		LIWRenderAttachmentFormat_ColorRGB,
		LIWRenderAttachmentFormat_ColorRGBA,
		LIWRenderAttachmentFormat_Depth,
		LIWRenderAttachmentFormat_Stencil,
		LIWRenderAttachmentFormat_DepthStencil,
		LIWRenderAttachmentFormat_Max
	};
	
	extern const std::unordered_map<LIWRenderAttachmentFormat, GLenum> LIWRenderAttachmentFormat_2_GLAttachment;
	extern const std::unordered_map<LIWRenderAttachmentFormat, GLint> LIWRenderAttachmentFormat_2_GLInternalFormat;
}