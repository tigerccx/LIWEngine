#version 450 core

#include "Lighting/LIWLighting_DeferredVisibility.glsl"

layout (local_size_x = 8, local_size_y = 8) in;

layout(r32ui) uniform uimage2D indexImg;
layout(r16ui) uniform uimage2D materialImg;

layout(std430, binding=LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_MATERIAL_COUNTERS) buffer LIW_SHADER_SSB_DEFERRED_VISIBILITY_MATERIAL_COUNTERS
{
    uint counters[LIW_DEFERRED_VISIBILITY_MAX_MATERIAL_COUNT];
};

void main(void)
{
	ivec2 p = ivec2(gl_GlobalInvocationID.xy);
	
	const uint idxObjectTriangle = imageLoad(indexImg, p).r;
	const uint idxMaterial = imageLoad(materialImg, p).r;
	if(idxMaterial!=LIW_DEFERRED_VISIBILITY_INVALID_IDX16){
		atomicAdd(counters[idxMaterial], 1);
	}
}