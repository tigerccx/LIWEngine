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
//Shader variable names
////
//Common Matrix
#define LIW_SHADER_MODEL_MATRIX_NAME	"modelMatrix"
#define LIW_SHADER_TEXTURE_MATRIX_NAME	"textureMatrix"
#define LIW_SHADER_SHADOW_MATRIX_NAME	"shadowMatrix"

////
//Texture slots
////
#define LIW_TEXTURESLOT_SHADOWMAP 30
#define LIW_TEXTURESLOT_SHADOWMAPS_START (30 - LIW_LIGHT_MAX_PERPIXEL)
#define LIW_TEXTURESLOT_SHADOWCUBEMAPS_START (30 - 2 * LIW_LIGHT_MAX_PERPIXEL)

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