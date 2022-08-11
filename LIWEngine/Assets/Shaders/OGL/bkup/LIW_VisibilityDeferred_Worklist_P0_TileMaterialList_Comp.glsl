#version 460 core

#include "Lighting/LIWLighting_DeferredVisibility.glsl"

layout (local_size_x = LIW_DEFERRED_VISIBILITY_TILE_X, local_size_y = LIW_DEFERRED_VISIBILITY_TILE_Y) in;

layout(r32ui) uniform uimage2D indexImg;
layout(r32ui) uniform uimage2D testImgOut;

layout(std430, binding=LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_TILE_MATERIAL_LIST) buffer LIW_SHADER_SSB_DEFERRED_VISIBILITY_TILE_MATERIAL_LIST
{
    uint tileMaterialList[LIW_DEFERRED_VISIBILITY_TILE_MATERIAL_LIST_CAPACITY*2]; //Tile: 4byte; Material: 4byte
};

layout(binding=LIW_SHADER_ATOMIC_BIND_DEFERRED_VISIBILITY_TILE_MATERIAL_LIST_COUNT) uniform atomic_uint LIW_SHADER_ATOMIC_DEFERRED_VISIBILITY_TILE_MATERIAL_LIST_COUNT;

shared uint materialCounters[LIW_DEFERRED_VISIBILITY_MAX_MATERIAL_COUNT];

void main(void)
{
	// Init shared group
	const uint groupSize = gl_WorkGroupSize.x*gl_WorkGroupSize.y*gl_WorkGroupSize.z;
	const uint loopCount = LIW_DEFERRED_VISIBILITY_MAX_MATERIAL_COUNT/groupSize;
	const ivec2 pLocal = ivec2(gl_LocalInvocationID.xy);
	const uint idxLocalInvocation = pLocal.x*gl_WorkGroupSize.y+pLocal.y;
	const uint beg = idxLocalInvocation*loopCount;
	const uint end = beg+loopCount;
	
	for(uint i=beg;i<end;i++) //Loop count is the same for every invocation
	{
		materialCounters[i] = 0;
	}
	
	// Sync
	groupMemoryBarrier();
	barrier();
	
	// Check every material
	const ivec2 p = ivec2(gl_GlobalInvocationID.xy);
	
	const uint idxObjectTriangle = imageLoad(indexImg, p).r;
	if(idxObjectTriangle!=0){
		uint idxObject = (liw_c_maskIdxObject&idxObjectTriangle)>>18;
		atomicOr(materialCounters[idxObject], 1);
	}
	
	// Sync
	groupMemoryBarrier();
	barrier();
	
	// Record tile material
	const uint idxWorkGroup = gl_WorkGroupID.x*gl_NumWorkGroups.y + gl_WorkGroupID.y;
	
	for(uint i=beg;i<end;i++) //Loop count is the same for every invocation
	{
		if(materialCounters[i]>0){
			const uint idxList = atomicCounterIncrement(LIW_SHADER_ATOMIC_DEFERRED_VISIBILITY_TILE_MATERIAL_LIST_COUNT);
			const uint idxListTile = 2*idxList;
			const uint idxListMaterial = 2*idxList+1;
			tileMaterialList[idxListTile] = idxWorkGroup;
			tileMaterialList[idxListMaterial] = i;
		}
	}

	imageStore(testImgOut, p, uvec4(idxObjectTriangle,0,0,0));
}