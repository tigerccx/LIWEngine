#version 450 core

#include "Lighting/LIWLighting_DeferredVisibility.glsl"

layout (local_size_x = LIW_DEFERRED_VISIBILITY_MAX_MATERIAL_COUNT/2) in;

layout(std430, binding=LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_MATERIAL_COUNTERS) buffer LIW_SHADER_SSB_DEFERRED_VISIBILITY_MATERIAL_COUNTERS
{
    uint counters[LIW_DEFERRED_VISIBILITY_MAX_MATERIAL_COUNT];
};

layout(std430, binding=LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_PREFIXSUM) buffer LIW_SHADER_SSB_DEFERRED_VISIBILITY_PREFIXSUM
{
	uint prefixSums[LIW_DEFERRED_VISIBILITY_MAX_MATERIAL_COUNT];
};

layout(std430, binding=LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_INDIRECT_DISPATCH) buffer LIW_SHADER_SSB_DEFERRED_VISIBILITY_INDIRECT_DISPATCH
{
	uint indirectDispatch[LIW_DEFERRED_VISIBILITY_MAX_MATERIAL_COUNT*3];
};

shared uint shared_data[gl_WorkGroupSize.x * 2];

void main(void)
{
	uint id = gl_LocalInvocationID.x;
	uint rd_id;
	uint wr_id;
	uint mask;
	
	const uint steps = uint(log2(gl_WorkGroupSize.x)) + 1;
	uint step = 0;

	if(id==0){
		shared_data[id * 2] = 0;
	}
	else{
		shared_data[id * 2] = counters[id * 2 - 1];
	}
	shared_data[id * 2 + 1] = counters[id * 2];
		
	barrier();
	memoryBarrierShared();
	
	for (step = 0; step < steps; step++)
	{
		mask = (1 << step) - 1;
		rd_id = ((id >> step) << (step + 1)) + mask;
		wr_id = rd_id + 1 + (id & mask);

		shared_data[wr_id] += shared_data[rd_id];

		barrier();
		memoryBarrierShared();
	}
	
	prefixSums[id * 2] = shared_data[id * 2];
	prefixSums[id * 2 + 1] = shared_data[id * 2 + 1];

	// Populate indirect dispatch buffer
	indirectDispatch[id * 2*3] = uint(ceil(float(counters[id * 2])/64));
	indirectDispatch[id * 2*3 + 1] = 1;
	indirectDispatch[id * 2*3 + 2] = 1;
	
	indirectDispatch[id * 2*3 + 3] = uint(ceil(float(counters[id * 2 + 1])/64));
	indirectDispatch[id * 2*3 + 4] = 1;
	indirectDispatch[id * 2*3 + 5] = 1;
}