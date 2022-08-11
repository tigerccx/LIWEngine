#version 450 core

#include "Lighting/LIWLighting_DeferredVisibility.glsl"

layout (local_size_x = 64) in;

layout(r32ui) uniform uimage2D indexImg;
layout(r32ui) uniform uimage2D testImgOut;

layout(std430, binding=LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_TILE_MATERIAL_LIST) buffer LIW_SHADER_SSB_DEFERRED_VISIBILITY_TILE_MATERIAL_LIST
{
    uint tileMaterialList[LIW_DEFERRED_VISIBILITY_TILE_MATERIAL_LIST_CAPACITY*2]; //Tile: 4byte; Material: 4byte
};

layout(binding=LIW_SHADER_ATOMIC_BIND_DEFERRED_VISIBILITY_TILE_MATERIAL_LIST_COUNT) uniform atomic_uint LIW_SHADER_ATOMIC_DEFERRED_VISIBILITY_TILE_MATERIAL_LIST_COUNT;
uniform int compareToLeft;

void main(void)
{
	ivec2 p = ivec2(gl_GlobalInvocationID.xy);
	const uint idxCompare = p.x*gl_NumWorkGroups.y+p.y;
	uint idxL = 0;
	uint idxR = 0;
	if(compareToLeft>0) //Should be fine. Whole dispatch should have the same value. 
	{
		idxL = idxCompare*2;
		idxR = idxCompare*2-1;
		if(idxL<0) return; //Should be fine. Only happens to the first dispatched group
	}
	else
	{
		idxL = idxCompare*2;
		idxR = idxCompare*2+1;
		const uint maxIdx = atomicCounter(LIW_SHADER_ATOMIC_DEFERRED_VISIBILITY_TILE_MATERIAL_LIST_COUNT);
		if(idxR>=maxIdx) return; //Should be fine. Only happens to the last dispatched group
	}
	
	const uint idxLTile = idxL*2;
	const uint idxLMaterial = idxL*2+1;
	const uint idxRTile = idxR*2;
	const uint idxRMaterial = idxR*2+1;
	
	//if()
	
	
	const uint idxObjectTriangle = imageLoad(indexImg, p).r;
	if(idxObjectTriangle!=0){
		uint idxObject = (liw_c_maskIdxObject&idxObjectTriangle)>>18;
		const uint idxList = atomicCounterIncrement(LIW_SHADER_ATOMIC_DEFERRED_VISIBILITY_TILE_MATERIAL_LIST_COUNT);
		const uint idxListTile = 2*idxList;
		const uint idxListMaterial = 2*idxList+1;
		tileMaterialList[idxListTile] = gl_WorkGroupID.x*gl_NumWorkGroups.y + gl_WorkGroupID.y;
		tileMaterialList[idxListMaterial] = idxObject;
	}
	
	imageStore(testImgOut, p, uvec4(idxObjectTriangle,0,0,0));
}