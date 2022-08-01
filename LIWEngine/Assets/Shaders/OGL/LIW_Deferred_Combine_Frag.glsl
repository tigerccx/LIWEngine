#version 420 core
#include "Lighting/LIWLighting_Deferred.glsl"

uniform sampler2D LIW_SHADER_DEFERRED_DIFFUSE_TEXURE;
uniform sampler2D LIW_SHADER_DEFERRED_DIFFUSE_LIGHT_TEXURE;
uniform sampler2D LIW_SHADER_DEFERRED_SPECULAR_LIGHT_TEXURE;
uniform sampler2D LIW_SHADER_DEFERRED_DEPTH_TEXURE;

in Vertex{
	vec2 texCoord;
} IN;

out vec4 fragColour;

void main() {
	vec3 diffuse = texture(LIW_SHADER_DEFERRED_DIFFUSE_TEXURE, IN.texCoord).rgb;
	vec3 diffuseLight = texture(LIW_SHADER_DEFERRED_DIFFUSE_LIGHT_TEXURE, IN.texCoord).rgb;
	vec3 specular = texture(LIW_SHADER_DEFERRED_SPECULAR_LIGHT_TEXURE, IN.texCoord).rgb;
	float depth = texture(LIW_SHADER_DEFERRED_DEPTH_TEXURE, IN.texCoord).r;
	if(depth>0.99999f){
		fragColour = vec4(diffuse ,1.0);
	}
	else{
		fragColour = vec4(diffuse*diffuseLight + specular, 0);
		vec4 ambientLightingClr = vec4(liw_c_ambientColour*diffuse*liw_c_ambientComponent, 0);
		fragColour += ambientLightingClr;
		fragColour.a = 1.0;
	}
}