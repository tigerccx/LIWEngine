#version 330 core

out Vertex{
	vec2 texCoord;
} OUT;

void main(){
	vec2 position = vec2(gl_VertexID % 2, gl_VertexID / 2) * 4.0 - 1;
	vec2 texCoord = (position + 1) * 0.5;
	OUT.texCoord = texCoord;
	gl_Position = vec4(position, 0.5f, 1.0f);
}