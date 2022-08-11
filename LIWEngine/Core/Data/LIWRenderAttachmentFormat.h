#pragma once
#include <unordered_map>

#include "LIWglfw.h"

namespace LIW {
	enum LIWRenderAttachmentFormat {
		LIWRenderAttachmentFormat_ColorRGB,
		LIWRenderAttachmentFormat_ColorRGBA,
		LIWRenderAttachmentFormat_Index,
		LIWRenderAttachmentFormat_Index16,
		LIWRenderAttachmentFormat_Depth,
		LIWRenderAttachmentFormat_Depth24,
		LIWRenderAttachmentFormat_Stencil,
		LIWRenderAttachmentFormat_Stencil8,
		LIWRenderAttachmentFormat_DepthStencil,
		LIWRenderAttachmentFormat_Max
	};
	
	extern const std::unordered_map<LIWRenderAttachmentFormat, GLenum> LIWRenderAttachmentFormat_2_GLAttachment;
	extern const std::unordered_map<LIWRenderAttachmentFormat, GLint> LIWRenderAttachmentFormat_2_GLFormat;
}