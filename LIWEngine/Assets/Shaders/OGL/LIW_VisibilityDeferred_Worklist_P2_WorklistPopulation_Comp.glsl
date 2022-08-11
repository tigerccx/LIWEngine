#version 450 core

#include "Lighting/LIWLighting_DeferredVisibility.glsl"

layout (local_size_x = 8, local_size_y = 8) in;

layout(r16ui) uniform uimage2D materialImg;

// counters
layout(std430, binding=LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_MATERIAL_COUNTERS) buffer LIW_SHADER_SSB_DEFERRED_VISIBILITY_MATERIAL_COUNTERS
{
    uint counters[LIW_DEFERRED_VISIBILITY_MAX_MATERIAL_COUNT];
};
// sums
layout(std430, binding=LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_PREFIXSUM) buffer LIW_SHADER_SSB_DEFERRED_VISIBILITY_PREFIXSUM
{
	uint prefixSums[LIW_DEFERRED_VISIBILITY_MAX_MATERIAL_COUNT];
};
// worklist
layout(std430, binding=LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_WORKLIST) buffer LIW_SHADER_SSB_DEFERRED_VISIBILITY_WORKLIST
{
    uint worklist[];
};

const uint maskCoordY = (1<<16)-1;
const uint maskCoordX = ~maskCoordY;

void main(void)
{
	ivec2 p = ivec2(gl_GlobalInvocationID.xy);
	
	const uint idxMaterial = imageLoad(materialImg, p).r;
	if(idxMaterial!=LIW_DEFERRED_VISIBILITY_INVALID_IDX16){
		uint idx = atomicAdd(counters[idxMaterial], 1);
		
		idx = prefixSums[idxMaterial] + idx;
		
		uint pix = ((uint(p.x)<<16)&maskCoordX) + (uint(p.y)&maskCoordY);
		
		worklist[idx] = pix;
	}
}