#pragma once

#include "LIWObject.h"

namespace LIW
{
	enum LIWAssetType {
		// Image and Texture
		LIWAssetType_Image,
		LIWAssetType_Texture2D,

		// FrameBuffer
		LIWAssetType_FrameBuffer,

		// Mesh
		LIWAssetType_MeshData,
		LIWAssetType_Mesh,

		// Shader
		LIWAssetType_Shader,
		LIWAssetType_ShaderProgram,

		// Material
		LIWAssetType_Material,

		LIWAssetType_Max
	};

	class LIWAsset: public LIWObject
	{
	public:
		LIWAsset(): LIWObject() {}
	public:
		virtual LIWAssetType GetAssetType() = 0;
	};
}


