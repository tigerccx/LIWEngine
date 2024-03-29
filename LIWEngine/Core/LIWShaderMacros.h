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
#define LIW_SHADER_UB_CAMERADATA			CameraBlock
//UBO Bind Point
#define LIW_SHADER_UBO_BIND_CAMERADATA		0
//Common Matrix
#define LIW_SHADER_MODEL_MATRIX				modelMatrix
#define LIW_SHADER_NORMAL_MATRIX			normalMatrix
#define LIW_SHADER_TEXTURE_MATRIX			textureMatrix
#define LIW_SHADER_SHADOW_MATRIX			shadowMatrix
//Texture
#define LIW_SHADER_TEXTURE_TOUSE_APPENDIX	_use
#define LIW_SHADER_TEXTURE_SCALE_APPENDIX	_texScale
#define LIW_SHADER_TEXTURE_MATRIX_APPENDIX	_texMatrix

////
//Forward Rendering
////
//Light Data
#define LIW_LIGHT_FORWARD_MAX_PERPIXEL					256
#define LIW_LIGHT_FORWARD_MAX_PERVERTEX					16
//Uniform Buffers
#define LIW_SHADER_UB_FORWARD_PERPIX_LIGHTDATA			ForwardLightBlockPerPixel
//UBO Bind Point
#define LIW_SHADER_UBO_BIND_FORWARD_PERPIX_LIGHTDATA	1

////
//Deferred Rendering
////
//Light Data
#define LIW_LIGHT_DEFERRED_MAX_PERPIXEL					256
#define LIW_LIGHT_DEFERRED_MAX_PERVERTEX				1024
#define LIW_LIGHT_DEFERRED_MAX_LIGHT_PERTYPE			1024
//FrameBuffer Textures
#define LIW_SHADER_DEFERRED_DEPTH_TEXURE				deferredDepthTexture
#define LIW_SHADER_DEFERRED_NORMAL_TEXURE				deferredNormalTexture
#define LIW_SHADER_DEFERRED_DIFFUSE_TEXURE				deferredDiffuseTexture
#define LIW_SHADER_DEFERRED_DIFFUSE_LIGHT_TEXURE		deferredDiffuseLightTexture
#define LIW_SHADER_DEFERRED_SPECULAR_LIGHT_TEXURE		deferredSpecularLightTexture
//Shader Variables
#define LIW_SHADER_DEFERRED_PIXEL_SIZE					deferredPixelSize
#define LIW_SHADER_DEFERRED_INV_PROJVIEW_MATRIX			deferredInvProjView
#define LIW_SHADER_DEFERRED_LIGHT_POS					deferredLightPos
#define LIW_SHADER_DEFERRED_LIGHT_COLOUR				deferredLightColour
#define LIW_SHADER_DEFERRED_LIGHT_PARAM					deferredLightParam
//Uniform Buffers
#define LIW_SHADER_UB_DEFERRED_PERPIX_LIGHTDATA			DeferredLightBlockPerPixel
//UBO Bind Point
#define LIW_SHADER_UBO_BIND_DEFERRED_PERPIX_LIGHTDATA	1

////
//Deferred Visibility Rendering
////
//Tile
#define LIW_DEFERRED_VISIBILITY_TILE_X											16
#define LIW_DEFERRED_VISIBILITY_TILE_Y											8
#define LIW_DEFERRED_VISIBILITY_MAX_RESOLUTION									1920*1080
#define LIW_DEFERRED_VISIBILITY_TILE_MATERIAL_LIST_CAPACITY						LIW_DEFERRED_VISIBILITY_MAX_RESOLUTION
//Limitations
#define LIW_DEFERRED_VISIBILITY_MAX_OBJECT										4096
#define LIW_DEFERRED_VISIBILITY_MAX_MESH										4096
#define LIW_DEFERRED_VISIBILITY_MAX_VERTEX										(uint32_t(1)<<20) //1M
#define LIW_DEFERRED_VISIBILITY_MAX_INDEX										(uint32_t(1)<<20) //1M
//Light Data
#define LIW_LIGHT_DEFERRED_VISIBILITY_MAX_PERPIXEL								256
#define LIW_LIGHT_DEFERRED_VISIBILITY_MAX_PERVERTEX								1024
//Constant Value
#define LIW_DEFERRED_VISIBILITY_INVALID_IDX										0xffffffff
#define LIW_DEFERRED_VISIBILITY_INVALID_IDX16									0xffff
//Material
#define LIW_DEFERRED_VISIBILITY_MAX_MATERIAL_COUNT								1024 //NOTE: MUST BE MULTIPLE OF TILE_X x TILE_Y
//Shader Storage Buffers
#define LIW_SHADER_SSB_DEFERRED_VISIBILITY_MATERIAL_COUNTERS					DeferredVisibilityMaterialCounters
#define LIW_SHADER_SSB_DEFERRED_VISIBILITY_PREFIXSUM							DeferredVisibilityPrefixSum
#define LIW_SHADER_SSB_DEFERRED_VISIBILITY_INDIRECT_DISPATCH					DeferredVisibilityIndirectDispatch
#define LIW_SHADER_SSB_DEFERRED_VISIBILITY_WORKLIST								DeferredVisibilityWorklist
#define LIW_SHADER_SSB_DEFERRED_VISIBILITY_VERTEX_BUFFER						DeferredVisibilityVertexBuffer
#define LIW_SHADER_SSB_DEFERRED_VISIBILITY_INDEX_BUFFER							DeferredVisibilityIndexBuffer
#define LIW_SHADER_SSB_DEFERRED_VISIBILITY_INDEX_OFFSET_LOOKUP					DeferredVisibilityIndexOffsetLookup
#define LIW_SHADER_SSB_DEFERRED_VISIBILITY_MESH_ID_LOOKUP						DeferredVisibilityMeshIDLookup
#define LIW_SHADER_SSB_DEFERRED_VISIBILITY_MODEL_MATRICES						DeferredVisibilityModelMatrices
#define LIW_SHADER_SSB_DEFERRED_VISIBILITY_NORMAL_MATRICES						DeferredVisibilityNormalMatrices
#define LIW_SHADER_SSB_DEFERRED_VISIBILITY_MATERIAL_LOOKUP						DeferredVisibilityLookup
#define LIW_SHADER_SSB_DEFERRED_VISIBILITY_TILE_MATERIAL_LIST					DeferredVisibilityTileMaterialList
//SSBO Bind Point
#define LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_MATERIAL_COUNTERS				0
#define LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_PREFIXSUM						1
#define LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_INDIRECT_DISPATCH				2
#define LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_WORKLIST						3
#define LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_VERTEX_BUFFER					4
#define LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_INDEX_BUFFER					5
#define LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_INDEX_OFFSET_LOOKUP			6
#define LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_MESH_ID_LOOKUP					7
#define LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_MODEL_MATRICES					8
#define LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_NORMAL_MATRICES				9
#define LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_MATERIAL_LOOKUP				10
#define LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_TILE_MATERIAL_LIST				11
//Atomic
#define LIW_SHADER_ATOMIC_DEFERRED_VISIBILITY_TILE_MATERIAL_LIST_COUNT			DeferredVisibilityTileMaterialListCount
//Uniform Buffers
#define LIW_SHADER_UB_DEFERRED_VISIBILITY_PERPIX_LIGHTDATA						DeferredVisibilityLightBlockPerPixel
//UBO Bind Point
#define LIW_SHADER_ATOMIC_BIND_DEFERRED_VISIBILITY_TILE_MATERIAL_LIST_COUNT		1
#define LIW_SHADER_UBO_BIND_DEFERRED_VISIBILITY_PERPIX_LIGHTDATA				2



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