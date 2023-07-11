#pragma once
#include "LIWShaderMacros.h"

////
//Paths
////
//Default Asset Paths
#define LIW_PATH_DIR_DEFAULT_SHADERS		"./Assets/Shaders/"
#define LIW_PATH_DIR_DEFAULT_MESHES			"./Assets/Meshes/"
#define LIW_PATH_DIR_DEFAULT_TEXTURES		"./Assets/Textures/"
//Resource Asset Paths
#define LIW_PATH_DIR_RESOURCE_SHADERS		"../../Resources/Shaders/"
#define LIW_PATH_DIR_RESOURCE_MESHES		"../../Resources/Meshes/"
#define LIW_PATH_DIR_RESOURCE_TEXTURES		"../../Resources/Textures/"

////
//Capacities
////
// 
//Assets
// Image and Texture
#define LIW_ASSET_IMAGE_CAPACITY			2048
#define LIW_ASSET_TEXTURE2D_CAPACITY		2048
// FrameBuffer
#define LIW_ASSET_FRAMEBUFFER_CAPACITY		32
// Mesh
#define LIW_ASSET_MESHDATA_CAPACITY			2048
#define LIW_ASSET_MESH_CAPACITY				2048
// Shader
#define LIW_ASSET_SHADER_CAPACITY			512
#define LIW_ASSET_SHDAERPROGRAM_CAPACITY	512
// Material
#define LIW_ASSET_MATERIAL_CAPACITY			2048


////
//Memory
////
//Default Allocation
#define LIW_MEMORY_DEFAULT_HANDLE_HANDLE_DIGITS 20 //NOTE: Must be <= 32 (should be well enough)

////
//Shader variable names
////
//Common Matrix
#define LIW_SHADER_MODEL_MATRIX_NAME	"modelMatrix"
#define LIW_SHADER_NORMAL_MATRIX_NAME	"normalMatrix"
#define LIW_SHADER_TEXTURE_MATRIX_NAME	"textureMatrix"
#define LIW_SHADER_SHADOW_MATRIX_NAME	"shadowMatrix"

////
//Texture slots
////
#define LIW_TEXTURESLOT_SHADOWMAP 30
#define LIW_TEXTURESLOT_SHADOWMAPS_START (30 - LIW_LIGHT_FORWARD_MAX_PERPIXEL)
#define LIW_TEXTURESLOT_SHADOWCUBEMAPS_START (30 - 2 * LIW_LIGHT_FORWARD_MAX_PERPIXEL)

////
//Const value
////
//Shadow
#define LIW_SHADOWMAP_SIZE 2048

////
//OpenGL sucks
////
//Texture
#define LIW_CUBETEX_UNIT_BEGIN 10
#define LIW_CUBETEX_UNIT_LOAD 31


////
//Mesh
////
//MeshData Names
#define LIW_MESHDATA_SPHERE_NAME	"liw_meshdata_sphere"
#define LIW_MESHDATA_CUBE_NAME		"liw_meshdata_cube"
#define LIW_MESHDATA_PLANE_NAME		"liw_meshdata_plane"
//Mesh Names
#define LIW_MESH_SPHERE_NAME		"liw_mesh_sphere"
#define LIW_MESH_CUBE_NAME			"liw_meshd_cube"
#define LIW_MESH_PLANE_NAME			"liw_mesh_plane"

////
//Forward
////

////
//Deferred
////
#define LIW_SHADER_DEFERRED_DEPTH_TEXURE_NAME			"deferredDepthTexture"
#define LIW_SHADER_DEFERRED_NORMAL_TEXURE_NAME			"deferredNormalTexture"
#define LIW_SHADER_DEFERRED_DIFFUSE_TEXURE_NAME			"deferredDiffuseTexture"
#define LIW_SHADER_DEFERRED_DIFFUSE_LIGHT_TEXURE_NAME	"deferredDiffuseLightTexture"
#define LIW_SHADER_DEFERRED_SPECULAR_LIGHT_TEXURE_NAME	"deferredSpecularLightTexture"
#define LIW_SHADER_DEFERRED_PIXEL_SIZE_NAME				"deferredPixelSize"
#define LIW_SHADER_DEFERRED_INV_PROJVIEW_MATRIX_NAME	"deferredInvProjView"
#define LIW_SHADER_DEFERRED_LIGHT_POS_NAME				"deferredLightPos"
#define LIW_SHADER_DEFERRED_LIGHT_COLOUR_NAME			"deferredLightColour"
#define LIW_SHADER_DEFERRED_LIGHT_PARAM_NAME			"deferredLightParam"