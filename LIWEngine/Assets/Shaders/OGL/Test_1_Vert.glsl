#version 420 core

uniform mat4 modelMatrix;
layout(std140, binding=0) uniform CameraBlock{
	mat4 viewMatrix;
	mat4 projMatrix;
	vec3 posCamera;
} cameraBlk;

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 texCoord;
layout(location=3) in vec4 colour;

out Vertex{
	vec3 posWorld;
	vec3 normalWorld;
	vec2 texCoord;
	vec4 colour;
} OUT;

void main() {
	OUT.colour = colour;
	OUT.texCoord = texCoord;
	
	vec4 posWorld = modelMatrix * vec4(position, 1);
	OUT.posWorld = posWorld.xyz;
	OUT.normalWorld = (modelMatrix * vec4(normal, 1)).xyz;
	gl_Position = (cameraBlk.projMatrix * cameraBlk.viewMatrix) * posWorld;
}