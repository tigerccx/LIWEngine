#version 420 core
#include "Lighting/LIWLighting_Deferred.glsl"

//Per camera
uniform sampler2D LIW_SHADER_DEFERRED_DEPTH_TEXURE;
uniform sampler2D LIW_SHADER_DEFERRED_NORMAL_TEXURE;

uniform vec2 LIW_SHADER_DEFERRED_PIXEL_SIZE;
uniform mat4 LIW_SHADER_DEFERRED_INV_PROJVIEW_MATRIX;

//Light
LIW_DEFINE_UB_DEFERRED_PERPIX_LIGHTDATA(deferredLightBlk);

//Camera
LIW_DEFINE_UB_CAMERADATA(cameraBlk);

layout(location = 0) out vec3 diffuseOut;
layout(location = 1) out vec3 specularOut;

in Vertex{
	flat int indexID;
} IN;

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
							deferredLightBlk.lightColours[IN.indexID], 
							deferredLightBlk.lightPositions[IN.indexID].xyz, 
							deferredLightBlk.lightParams[IN.indexID], 
							diffuseOut, specularOut);
							
	//diffuseOut = posWorld;
}