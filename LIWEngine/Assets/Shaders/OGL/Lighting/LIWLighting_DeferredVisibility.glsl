#ifndef LIW_LIGHTING_DEFERRED_VISIBILITY_H
#define LIW_LIGHTING_DEFERRED_VISIBILITY_H

#include "LIWLighting_Deferred.glsl"

const uint liw_c_maskIdxTriangle = (0x1<<18)-1; //Last 18 bits
const uint liw_c_maskIdxObject = ~liw_c_maskIdxTriangle; //First 14 bits

#define LIW_DEFINE_UB_DEFERRED_VISIBILITY_PERPIX_LIGHTDATA(name) \
layout(std140, binding=LIW_SHADER_UBO_BIND_DEFERRED_VISIBILITY_PERPIX_LIGHTDATA) uniform LIW_SHADER_UB_DEFERRED_VISIBILITY_PERPIX_LIGHTDATA{ \
	vec4 lightColours_Directional[LIW_LIGHT_DEFERRED_VISIBILITY_MAX_PERPIXEL]; \
	vec4 lightColours_Point[LIW_LIGHT_DEFERRED_VISIBILITY_MAX_PERPIXEL]; \
	vec4 lightColours_Spot[LIW_LIGHT_DEFERRED_VISIBILITY_MAX_PERPIXEL]; \
	vec4 lightPositions_Directional[LIW_LIGHT_DEFERRED_VISIBILITY_MAX_PERPIXEL]; \
	vec4 lightPositions_Point[LIW_LIGHT_DEFERRED_VISIBILITY_MAX_PERPIXEL]; \
	vec4 lightPositions_Spot[LIW_LIGHT_DEFERRED_VISIBILITY_MAX_PERPIXEL]; \
	vec4 lightParams_Directional[LIW_LIGHT_DEFERRED_VISIBILITY_MAX_PERPIXEL]; \
	vec4 lightParams_Point[LIW_LIGHT_DEFERRED_VISIBILITY_MAX_PERPIXEL]; \
	vec4 lightParams_Spot[LIW_LIGHT_DEFERRED_VISIBILITY_MAX_PERPIXEL]; \
	int lightCount_Directional; \
	int lightCount_Point; \
	int lightCount_Spot; \
} name

#endif //LIW_LIGHTING_DEFERRED_VISIBILITY_H