#version 420 core
#include "Common/LIWShaderCommon.glsl"

//Material
uniform sampler2D mainTex;
uniform sampler2D normalTex;

in Vertex {
	vec3 posWorld;
	vec3 normalWorld;
	vec3 tangentWorld;
	vec3 binormalWorld;
	vec2 texCoord;
	vec4 colour;
} IN ;

layout(location = 0) out vec3 diffuseOut;
layout(location = 1) out vec3 normalOut;

void main() {
	mat3 TBN = mat3(normalize(IN.tangentWorld), normalize(IN.binormalWorld), normalize(IN.normalWorld));
	vec3 normal = texture(normalTex, IN.texCoord).rgb;
	normal = normalize(TBN*normalize(normal*2.0f-1.0f));

	diffuseOut = texture(mainTex, IN.texCoord).rgb;
	normalOut = vec3(normal.xyz*0.5f+0.5f);
}
