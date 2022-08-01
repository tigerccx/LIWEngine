#ifndef LIW_LIGHTING_H
#define LIW_LIGHTING_H

#include "../Common/LIWShaderCommon.glsl"

//Old Lighting
const float liw_c_thresAtRadius = 0.0001f;
const vec3 liw_c_ambientColour = vec3(1,1,1);
const float liw_c_ambientComponent = 0.02f;
const float PI = 3.14159265359f;

float GetAttenuationDirectionalLight(vec4 lightColour){
	return lightColour.a;
}

float GetAttenuationPointLight(vec3 worldPos, vec3 lightPos, vec4 lightColour, vec4 lightParam){
	float distance = length(lightPos - worldPos);
	float lightRadius = lightParam.x;
	return (clamp((lightRadius * lightRadius) / (distance * distance) * liw_c_thresAtRadius - liw_c_thresAtRadius, 0.0f, 1.0f - liw_c_thresAtRadius)) * lightColour.a;
}

float GetAttenuationSpotLight(vec3 worldPos, vec3 lightPos, vec4 lightColour, vec4 lightParam){
	float dirX = lightParam.x;
	float dirY = lightParam.y;
	float dirZ =  sign(lightParam.z)*sqrt(abs(1-dirX*dirX-dirY*dirY));
	vec3 dirLight = vec3(dirX, dirY, dirZ);
	
	float cosHalfAngle = lightParam.w;
	
	vec3 toLight = normalize(lightPos - worldPos);
	float cosFragment = dot(dirLight, -toLight);
	if(cosFragment>cosHalfAngle) //If in spot light area
	{
		float lightRadius = abs(lightParam.z);
		float distance = length(lightPos - worldPos);
		float attenuation = (clamp((lightRadius * lightRadius) / (distance * distance) * liw_c_thresAtRadius - liw_c_thresAtRadius, 0.0f, 1.0f - liw_c_thresAtRadius)) * lightColour.a;
		float sqrSinFragment = abs(1-cosFragment*cosFragment);
		float sqrSinHalfAngle = abs(1-cosHalfAngle*cosHalfAngle);
		float scaleAttenu = (sqrSinHalfAngle-sqrSinFragment)/sqrSinHalfAngle;
		return attenuation *  scaleAttenu * scaleAttenu;
	}
	else{
		return 0.0f;
	}
}

void LightingCommon(vec3 toLight, vec3 toEye, vec3 normal, 
					vec3 lightColour, float specGloss, float attenuation, 
					out vec3 diffuse, out vec3 specular){
	vec3 halfDir = normalize(toLight + toEye);
	float cosAngIncidence = dot(normal, toLight);
	float lambert = max(cosAngIncidence, 0.0f);
	
	float specFactor = clamp(dot(halfDir, normal), 0.0f, 1.0f) * smoothstep(-0.5f, 0.1f, cosAngIncidence);
	specFactor = pow(specFactor, specGloss);
	diffuse = lightColour * lambert * attenuation;
	specular = (lightColour * specFactor) * attenuation * 0.33; //0.33 is optional
}

void CalculateDirectionalLight( vec3 toEye, vec3 normal, 
								float specGloss,
								vec4 lightColour, vec3 lightPos, vec4 lightParam, 
								out vec3 diffuse, out vec3 specular){
	vec3 toLight = -vec3(lightParam.x, lightParam.y, lightParam.z);
	float attenuation = lightColour.a;
	
	LightingCommon(toLight, toEye, normal, 
					lightColour.rgb, specGloss, attenuation, 
					diffuse, specular);
}

void CalculatePointLight( vec3 toEye, vec3 worldPos, vec3 normal,
							float specGloss, 
							vec4 lightColour, vec3 lightPos, vec4 lightParam, 
							out vec3 diffuse, out vec3 specular){
	float lightRadius = lightParam.x;
	vec3 toLight = normalize(lightPos - worldPos);
	float distance = length(lightPos - worldPos);
	float attenuation = (clamp((lightRadius * lightRadius) / (distance * distance) * liw_c_thresAtRadius - liw_c_thresAtRadius, 0.0f, 1.0f - liw_c_thresAtRadius)) * lightColour.a;

	LightingCommon(toLight, toEye, normal, 
					lightColour.rgb, specGloss, attenuation, 
					diffuse, specular);
}

void CalculateSpotLight( vec3 toEye, vec3 worldPos, vec3 normal,
							float specGloss,
							vec4 lightColour, vec3 lightPos, vec4 lightParam, 
							out vec3 diffuse, out vec3 specular){
	float dirX = lightParam.x;
	float dirY = lightParam.y;
	float dirZ =  sign(lightParam.z)*sqrt(abs(1-dirX*dirX-dirY*dirY));
	vec3 dirLight = vec3(dirX, dirY, dirZ);
	
	float lightRadius = abs(lightParam.z);
	float cosHalfAngle = lightParam.w;
	
	vec3 toLight = normalize(lightPos - worldPos);
	float cosFragment = dot(dirLight, -toLight);
	if(cosFragment>cosHalfAngle) //If in spot light area
	{
		vec3 halfDir = normalize(toLight + toEye);
		float distance = length(lightPos - worldPos);
		float attenuation = (clamp((lightRadius * lightRadius) / (distance * distance) * liw_c_thresAtRadius - liw_c_thresAtRadius, 0.0f, 1.0f - liw_c_thresAtRadius)) * lightColour.a;
		float sqrSinFragment = abs(1-cosFragment*cosFragment);
		float sqrSinHalfAngle = abs(1-cosHalfAngle*cosHalfAngle);
		float scaleAttenu = (sqrSinHalfAngle-sqrSinFragment)/sqrSinHalfAngle;
		attenuation = attenuation *  scaleAttenu * scaleAttenu;

		LightingCommon(toLight, toEye, normal, 
					lightColour.rgb, specGloss, attenuation, 
					diffuse, specular);
	}
	else{
		diffuse = vec3(0,0,0);
		specular = vec3(0,0,0);
	}
}

#endif //LIW_LIGHTING_H