#version 330 core

uniform sampler2D mainTex;

in Vertex{
	vec2 texCoord;
} IN;

out vec4 fragColour;

void main() {
	vec4 texColour = vec4(texture(mainTex, IN.texCoord).rgb, 1.0);
	
	fragColour = texColour;//vec4(0.5,0.5,0.8,1.0);
}