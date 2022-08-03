#ifndef LIW_LIGHTING_FORWARD_H
#define LIW_LIGHTING_FORWARD_H

#include "LIWLighting.glsl"

//Separate Light Types to reduce branching
#define LIW_DEFINE_UB_FORWARD_PERPIX_LIGHTDATA(name) \
layout(std140, binding=LIW_SHADER_UBO_BIND_FORWARD_PERPIX_LIGHTDATA) uniform LIW_SHADER_UB_FORWARD_PERPIX_LIGHTDATA{ \
	vec4 lightColours_Directional[LIW_LIGHT_FORWARD_MAX_PERPIXEL]; \
	vec4 lightColours_Point[LIW_LIGHT_FORWARD_MAX_PERPIXEL]; \
	vec4 lightColours_Spot[LIW_LIGHT_FORWARD_MAX_PERPIXEL]; \
	vec4 lightPositions_Directional[LIW_LIGHT_FORWARD_MAX_PERPIXEL]; \
	vec4 lightPositions_Point[LIW_LIGHT_FORWARD_MAX_PERPIXEL]; \
	vec4 lightPositions_Spot[LIW_LIGHT_FORWARD_MAX_PERPIXEL]; \
	vec4 lightParams_Directional[LIW_LIGHT_FORWARD_MAX_PERPIXEL]; \
	vec4 lightParams_Point[LIW_LIGHT_FORWARD_MAX_PERPIXEL]; \
	vec4 lightParams_Spot[LIW_LIGHT_FORWARD_MAX_PERPIXEL]; \
	int lightCount_Directional; \
	int lightCount_Point; \
	int lightCount_Spot; \
} name

#endif //LIW_LIGHTING_FORWARD_H