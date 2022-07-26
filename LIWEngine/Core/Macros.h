#pragma once

////
//Shader variable names
////
//
//Vertex Assembly
#define LIW_SHADER_VA_LOCATION_POSITION 0
#define LIW_SHADER_VA_LOCATION_NORMAL 0
#define LIW_SHADER_VA_LOCATION_TEXCOORD 0
#define LIW_SHADER_VA_LOCATION_COLOUR 0
//Common Matrix
#define LIW_SHADER_MODEL_MATRIX "modelMatrix"
#define LIW_SHADER_VIEW_MATRIX "viewMatrix"
#define LIW_SHADER_PROJECTION_MATRIX "projMatrix"
#define LIW_SHADER_TEXTURE_MATRIX "textureMatrix"
#define LIW_SHADER_SHADOW_MATRIX "shadowMatrix"
//Texture
#define LIW_SHADER_TEXTURE_TOUSE_APPENDIX "_use"
#define LIW_SHADER_TEXTURE_SCALE_APPENDIX "_texScale"
#define LIW_SHADER_TEXTURE_MATRIX_APPENDIX "_texMatrix"
//
//Light Data
//Light Global Setting
#define LIW_LIGHT_MAX_PERPIXEL 8
#define LIW_LIGHT_MAX_PERVERTEX 16
//Common Light Data
#define LIW_SHADER_LIGHT_COUNT "lightCount"
#define LIW_SHADER_LIGHT_TYPE "lights"
#define LIW_SHADER_LIGHT_COLOUR "lightColours"
#define LIW_SHADER_LIGHT_POSITIONS "lightPositions"
#define LIW_SHADER_LIGHT_POSITION "lightPos"
#define LIW_SHADER_LIGHT_PARAMETERS "lightParams"
#define LIW_SHADER_CAMERA_POSITION "cameraPos"
//Shadow data
#define LIW_SHADER_LIGHT_SHADOW_TO_CAST "castShadow"
#define LIW_SHADER_LIGHT_SHADOW_MATRIX "shadowMatrix"
#define LIW_SHADER_LIGHT_SHADOW_MATRICES "shadowMatrices"
#define LIW_SHADER_LIGHT_SHADOW_MAP "shadowTex"
#define LIW_SHADER_LIGHT_SHADOW_MAPS "shadowTexs"
#define LIW_SHADER_LIGHT_SHADOW_CUBEMAP "shadowCubeTex"
#define LIW_SHADER_LIGHT_SHADOW_CUBEMAPS "shadowCubeTexs"

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