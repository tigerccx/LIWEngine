#version 330 core

//Material
uniform sampler2D mainTex;

in Vertex{
	vec4 colour;
	vec2 texCoord;
} IN;

out vec4 fragColour;

void main() {
	vec4 texColour = vec4(texture(mainTex, IN.texCoord).rgb, 1.0);
	
	fragColour = texColour;
}