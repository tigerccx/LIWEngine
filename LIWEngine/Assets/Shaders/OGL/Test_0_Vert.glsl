#version 330 core
uniform mat4 modelMatrix;
layout(std140, binding=0) uniform CameraBlock{
	mat4 viewMatrix;
	mat4 projMatrix;
	vec4 posCamera;
} cameraBlk;

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 texCoord;
layout(location=3) in vec4 colour;

out Vertex{
	vec2 texCoord;
} OUT;

void main() {
	OUT.colour = colour;
	OUT.texCoord = texCoord;
	
	vec4 worldPos = modelMatrix * vec4(position, 1);
	gl_Position = (cameraBlk.projMatrix * cameraBlk.viewMatrix) * worldPos;
}