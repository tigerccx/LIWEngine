#version 330 core

out Vertex{
	vec2 texCoord;
} OUT;

void main(){
	vec2 position = vec2(gl_VertexID % 2, gl_VertexID / 2) * 4.0f - 1.0f;
	vec2 texCoord = (position + 1.0f) * 0.5f;
	OUT.texCoord = texCoord;
	gl_Position = vec4(position, 0.5f, 1.0f);
}