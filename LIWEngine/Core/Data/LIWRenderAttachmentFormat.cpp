#include "LIWRenderAttachmentFormat.h"

namespace LIW {
	const std::unordered_map<LIWRenderAttachmentFormat, GLenum> LIWRenderAttachmentFormat_2_GLAttachment =
	{
		{LIWRenderAttachmentFormat_ColorRGB,		GL_COLOR_ATTACHMENT0		},
		{LIWRenderAttachmentFormat_ColorRGBA,		GL_COLOR_ATTACHMENT0		},
		{LIWRenderAttachmentFormat_Depth,			GL_DEPTH_ATTACHMENT			},
		{LIWRenderAttachmentFormat_Stencil,			GL_STENCIL_ATTACHMENT		},
		{LIWRenderAttachmentFormat_DepthStencil,	GL_DEPTH_STENCIL_ATTACHMENT	}
	};
	const std::unordered_map<LIWRenderAttachmentFormat, GLint> LIWRenderAttachmentFormat_2_GLInternalFormat =
	{
		{LIWRenderAttachmentFormat_ColorRGB,		GL_RGB				},
		{LIWRenderAttachmentFormat_ColorRGBA,		GL_RGBA				},
		{LIWRenderAttachmentFormat_Depth,			GL_DEPTH_COMPONENT	},
		{LIWRenderAttachmentFormat_Stencil,			GL_R8				},
		{LIWRenderAttachmentFormat_DepthStencil,	GL_DEPTH24_STENCIL8	}
	};
}