#version 420 core
#include "Lighting/LIWLighting_DeferredVisibility.glsl"

uniform mat4 LIW_SHADER_MODEL_MATRIX;
uniform uint idxObject; //Should already be aligned to the first 14 bits
uniform uint idxMaterial;

LIW_DEFINE_UB_CAMERADATA(cameraBlk);

layout(location=LIW_SHADER_VA_LOCATION_POSITION) 	in vec3 position;
layout(location=LIW_SHADER_VA_LOCATION_NORMAL) 		in vec3 normal;

out Vertex{
	flat uint idxObjectTriangle;
	flat uint idxMaterial;
} OUT;

void main() {
	gl_Position = cameraBlk.projMatrix * cameraBlk.viewMatrix * LIW_SHADER_MODEL_MATRIX * vec4(position, 1);
	OUT.idxObjectTriangle = idxObject;
	OUT.idxMaterial = idxMaterial;
}