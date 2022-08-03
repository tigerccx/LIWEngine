#version 420 core
#include "Lighting/LIWLighting_DeferredVisibility.glsl"

uniform usampler2D indexTexture;

in Vertex{
	vec2 texCoord;
} IN;

const int colourSteps = 5;

const vec3 colours[] = 
{
	vec3(0.25f, 0.0f, 0.0f),
	vec3(0.0f, 0.25f, 0.0f),
	vec3(0.0f, 0.0f, 0.25f),
	vec3(0.25f, 0.25f, 0.0f),
	vec3(0.25f, 0.0f, 0.25f),
	vec3(0.0f, 0.25f, 0.25f),
	vec3(0.25f, 0.25f, 0.25f),
	vec3(0.5f, 0.25f, 0.25f),
	vec3(0.25f, 0.5f, 0.25f),
	vec3(0.25f, 0.25f, 0.5f),
	vec3(0.5f, 0.5f, 0.25f),
	vec3(0.5f, 0.25f, 0.5f),
	vec3(0.25f, 0.5f, 0.5f),
	vec3(0.5f, 0.5f, 0.5f),
	vec3(0.75f, 0.5f, 0.5f),
	vec3(0.5f, 0.75f, 0.5f),
	vec3(0.5f, 0.5f, 0.75f),
	vec3(0.75f, 0.75f, 0.5f),
	vec3(0.75f, 0.5f, 0.75f),
	vec3(0.5f, 0.75f, 0.75f),
	vec3(0.75f, 0.75f, 0.75f),
	vec3(1.0f, 0.75f, 0.75f),
	vec3(0.75f, 1.0f, 0.75f),
	vec3(0.75f, 0.75f, 1.0f),
	vec3(1.0f, 1.0f, 0.75f),
	vec3(1.0f, 0.75f, 1.0f),
	vec3(0.75f, 1.0f, 1.0f),
	vec3(1.0f, 1.0f, 1.0f)
}; //28 colours

out vec4 fragColour;

void main() {
	uint idxObjectTriangle = texture(indexTexture, IN.texCoord).r;
	if(idxObjectTriangle==0)
		discard;
//	uint idxObject = liw_c_maskIdxObject & idxObjectTriangle;
//	idxObject = idxObject>>18;
//	fragColour = vec4(colours[idxObject%28], 1.0f);
	uint idxTriangle = liw_c_maskIdxTriangle & idxObjectTriangle;
	
	const float r = 1.0f / colourSteps * ((idxTriangle / (colourSteps*colourSteps))%colourSteps);
	const float g = 1.0f / colourSteps * ((idxTriangle / (colourSteps))%colourSteps);
	const float b = 1.0f / colourSteps * (idxTriangle % (colourSteps));
	
	//fragColour = vec4(colours[idxTriangle%28], 1.0f);
	fragColour = vec4(r, g, b, 1.0f);
}