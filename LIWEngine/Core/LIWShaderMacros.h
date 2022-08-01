#ifndef LIW_SHADER_MACROS_H
#define LIW_SHADER_MACROS_H

//
//////
//Shader variables
//////
// 
//Vertex Assembly
#define LIW_SHADER_VA_LOCATION_POSITION		0
#define LIW_SHADER_VA_LOCATION_NORMAL		1
#define LIW_SHADER_VA_LOCATION_TANGENT		2
#define LIW_SHADER_VA_LOCATION_TEXCOORD		3
#define LIW_SHADER_VA_LOCATION_COLOUR		4
//Uniform Buffers
#define LIW_SHADER_UB_CAMERADATA				CameraBlock
#define LIW_SHADER_UB_FORWARD_PERPIX_LIGHTDATA	ForwardLightBlockPerPixel
//UBO Bind Point
#define LIW_SHADER_UBO_BIND_CAMERADATA					0
#define LIW_SHADER_UBO_BIND_FORWARD_PERPIX_LIGHTDATA	1
//Common Matrix
#define LIW_SHADER_MODEL_MATRIX			modelMatrix
#define LIW_SHADER_NORMAL_MATRIX		normalMatrix
#define LIW_SHADER_TEXTURE_MATRIX		textureMatrix
#define LIW_SHADER_SHADOW_MATRIX		shadowMatrix
//Texture
#define LIW_SHADER_TEXTURE_TOUSE_APPENDIX	_use
#define LIW_SHADER_TEXTURE_SCALE_APPENDIX	_texScale
#define LIW_SHADER_TEXTURE_MATRIX_APPENDIX	_texMatrix

////
//Forward Rendering
////
//Light Data
#define LIW_LIGHT_FORWARD_MAX_PERPIXEL 256
#define LIW_LIGHT_FORWARD_MAX_PERVERTEX 16

////
//Deferred Rendering
////
//Light Data
#define LIW_LIGHT_DEFERRED_MAX_PERPIXEL 256
#define LIW_LIGHT_DEFERRED_MAX_PERVERTEX 1024
//FrameBuffer Textures
#define LIW_SHADER_DEFERRED_DEPTH_TEXURE			deferredDepthTexture
#define LIW_SHADER_DEFERRED_NORMAL_TEXURE			deferredNormalTexture
#define LIW_SHADER_DEFERRED_DIFFUSE_TEXURE			deferredDiffuseTexture
#define LIW_SHADER_DEFERRED_DIFFUSE_LIGHT_TEXURE	deferredDiffuseLightTexture
#define LIW_SHADER_DEFERRED_SPECULAR_LIGHT_TEXURE	deferredSpecularLightTexture
//Shader Variables
#define LIW_SHADER_DEFERRED_PIXEL_SIZE				deferredPixelSize
#define LIW_SHADER_DEFERRED_INV_PROJVIEW_MATRIX		deferredInvProjView
#define LIW_SHADER_DEFERRED_LIGHT_POS				deferredLightPos
#define LIW_SHADER_DEFERRED_LIGHT_COLOUR			deferredLightColour
#define LIW_SHADER_DEFERRED_LIGHT_PARAM				deferredLightParam


////Common Light Data
//#define LIW_SHADER_LIGHT_COUNT "lightCount"
//#define LIW_SHADER_LIGHT_TYPE "lights"
//#define LIW_SHADER_LIGHT_COLOUR "lightColours"
//#define LIW_SHADER_LIGHT_POSITIONS "lightPositions"
//#define LIW_SHADER_LIGHT_POSITION "lightPos"
//#define LIW_SHADER_LIGHT_PARAMETERS "lightParams"
//#define LIW_SHADER_CAMERA_POSITION "cameraPos"
////Shadow data
//#define LIW_SHADER_LIGHT_SHADOW_TO_CAST "castShadow"
//#define LIW_SHADER_LIGHT_SHADOW_MATRIX "shadowMatrix"
//#define LIW_SHADER_LIGHT_SHADOW_MATRICES "shadowMatrices"
//#define LIW_SHADER_LIGHT_SHADOW_MAP "shadowTex"
//#define LIW_SHADER_LIGHT_SHADOW_MAPS "shadowTexs"
//#define LIW_SHADER_LIGHT_SHADOW_CUBEMAP "shadowCubeTex"
//#define LIW_SHADER_LIGHT_SHADOW_CUBEMAPS "shadowCubeTexs"

#endif //LIW_SHADER_MACROS_H