#version 420 core
#include "Lighting/LIWLighting_Deferred.glsl"

//Per camera
uniform sampler2D LIW_SHADER_DEFERRED_DEPTH_TEXURE;
uniform sampler2D LIW_SHADER_DEFERRED_NORMAL_TEXURE;

uniform vec2 LIW_SHADER_DEFERRED_PIXEL_SIZE;
uniform mat4 LIW_SHADER_DEFERRED_INV_PROJVIEW_MATRIX;

//Per light
uniform vec3 LIW_SHADER_DEFERRED_LIGHT_POS;
uniform vec4 LIW_SHADER_DEFERRED_LIGHT_COLOUR;
uniform vec4 LIW_SHADER_DEFERRED_LIGHT_PARAM;

//Camera
LIW_DEFINE_UB_CAMERADATA(cameraBlk);

layout(location = 0) out vec3 diffuseOut;
layout(location = 1) out vec3 specularOut;

void main(){
	const float specGloss = 60.0f;
	const float diffuseComponent = 0.4f;
	const float specularComponent = 0.58f;

	vec2 texCoord = vec2(gl_FragCoord.xy*LIW_SHADER_DEFERRED_PIXEL_SIZE);
	float depth = texture(LIW_SHADER_DEFERRED_DEPTH_TEXURE, texCoord.xy).r;
	vec3 posNDC = vec3(texCoord, depth)*2.0f-1.0f;
	vec4 posInvClip = LIW_SHADER_DEFERRED_INV_PROJVIEW_MATRIX*vec4(posNDC, 1.0f);
	vec3 posWorld = posInvClip.xyz/posInvClip.w;
	vec3 normal = normalize(texture(LIW_SHADER_DEFERRED_NORMAL_TEXURE, texCoord.xy).xyz*2.0f-1.0f);
	
	vec3 toEye = cameraBlk.posCamera - posWorld;
	
	CalculatePointLight( toEye, posWorld, normal, specGloss,
							LIW_SHADER_DEFERRED_LIGHT_COLOUR, 
							LIW_SHADER_DEFERRED_LIGHT_POS, 
							LIW_SHADER_DEFERRED_LIGHT_PARAM, 
							diffuseOut, specularOut);
							
	//diffuseOut = posWorld;
}