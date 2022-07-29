#version 420 core

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
	//fragColour = vec4(IN.texCoord, 0, 1);
	//fragColour = vec4(0.8,0.5,0.5,1.0);
}