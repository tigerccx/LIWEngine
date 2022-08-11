#version 420 core
#include "Lighting/LIWLighting_DeferredVisibility.glsl"

layout(early_fragment_tests) in;

in Vertex{
	flat uint idxObjectTriangle;
	flat uint idxMaterial;
} IN;

layout(location = 0) out uint idxMaterial;
layout(location = 1) out uint idxObjectTriangle;

void main() {
	idxObjectTriangle = IN.idxObjectTriangle + gl_PrimitiveID;
	idxMaterial = IN.idxMaterial;
}