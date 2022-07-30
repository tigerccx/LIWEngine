#version 420 core
#include "Common/LIWShaderCommon.glsl"

uniform mat4 LIW_SHADER_MODEL_MATRIX;
layout(std140, binding=LIW_SHADER_UBO_BIND_CAMERADATA) uniform CameraBlock{
	mat4 viewMatrix;
	mat4 projMatrix;
	vec3 posCamera;
} cameraBlk;

layout(location=LIW_SHADER_VA_LOCATION_POSITION) 	in vec3 position;
layout(location=LIW_SHADER_VA_LOCATION_NORMAL) 		in vec3 normal;
layout(location=LIW_SHADER_VA_LOCATION_TEXCOORD) 	in vec2 texCoord;
layout(location=LIW_SHADER_VA_LOCATION_COLOUR) 		in vec4 colour;

out Vertex{
	vec3 posWorld;
	vec3 normalWorld;
	vec2 texCoord;
	vec4 colour;
} OUT;

void main() {
	OUT.colour = colour;
	OUT.texCoord = texCoord;
	
	vec4 posWorld = LIW_SHADER_MODEL_MATRIX * vec4(position, 1);
	OUT.posWorld = posWorld.xyz;
	mat3 normalMatrix = transpose(inverse(mat3(LIW_SHADER_MODEL_MATRIX)));
	OUT.normalWorld = normalize(normalMatrix * normalize(normal));
	gl_Position = (cameraBlk.projMatrix * cameraBlk.viewMatrix) * posWorld;
}