#version 420 core
#include "Lighting/LIWLighting_DeferredVisibility.glsl"

in Vertex{
	flat uint idxObjectTriangle;
} IN;

out uint idxObjectTriangle;

void main() {
	idxObjectTriangle = IN.idxObjectTriangle + gl_PrimitiveID;
}