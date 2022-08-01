#ifndef LIW_SHADER_COMMON_H
#define LIW_SHADER_COMMON_H

#include "../../../../Core/LIWShaderMacros.h"

#define LIW_DEFINE_UB_CAMERADATA(name) \
layout(std140, binding=LIW_SHADER_UBO_BIND_CAMERADATA) uniform CameraBlock{ \
	mat4 viewMatrix; \
	mat4 projMatrix; \
	vec3 posCamera; \
} name

#endif //LIW_SHADER_COMMON_H