#version 420 core
#include "Common/LIWShaderCommon.glsl"

//Old Lighting
const float thresAtRadius = 0.0001f;
const vec4 ambientColour = vec4(1,1,1,0);
const float ambientComponent = 0.02f;
const float PI = 3.14159265359f;

float GetAttenuationDirectionalLight(vec4 lightColour){
	return lightColour.a;
}

float GetAttenuationPointLight(vec3 worldPos, vec3 lightPos, vec4 lightColour, vec4 lightParam){
	float distance = length(lightPos - worldPos);
	float lightRadius = lightParam.x;
	return (clamp((lightRadius * lightRadius) / (distance * distance) * thresAtRadius - thresAtRadius, 0.0f, 1.0f - thresAtRadius)) * lightColour.a;
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
		float attenuation = (clamp((lightRadius * lightRadius) / (distance * distance) * thresAtRadius - thresAtRadius, 0.0f, 1.0f - thresAtRadius)) * lightColour.a;
		float sqrSinFragment = abs(1-cosFragment*cosFragment);
		float sqrSinHalfAngle = abs(1-cosHalfAngle*cosHalfAngle);
		float scaleAttenu = (sqrSinHalfAngle-sqrSinFragment)/sqrSinHalfAngle;
		return attenuation *  scaleAttenu * scaleAttenu;
	}
	else{
		return 0.0f;
	}
}

void SurfaceCommon(vec3 toLight, vec3 toEye,
					vec3 normal, 
					vec3 diffuseColour, vec3 lightColour, 
					float specGloss, float attenuation, 
					out vec3 diffuse, out vec3 specular){
	vec3 halfDir = normalize(toLight + toEye);
	float lambert = max(dot(toLight, normal), 0.0f);
	
	float specFactor = clamp(dot(halfDir, normal), 0.0f, 1.0f);
	specFactor = pow(specFactor, specGloss);
	vec3 surface = diffuseColour * lightColour;
	diffuse = surface * lambert * attenuation;
	specular = (lightColour * specFactor) * attenuation * 0.33; //0.33 is optional
}

void CalculateDirectionalLight( vec3 toEye, vec3 normal,
								vec3 diffuseColour, float specGloss,
								vec4 lightColour, vec3 lightPos, vec4 lightParam, 
								out vec3 diffuse, out vec3 specular){
	vec3 toLight = -vec3(lightParam.x, lightParam.y, lightParam.z);
	float attenuation = lightColour.a;
	
	SurfaceCommon(toLight, toEye, normal, 
					diffuseColour, lightColour.rgb, 
					specGloss, attenuation, diffuse, specular);
}

void CalculatePointLight( vec3 toEye, vec3 worldPos, vec3 normal,
							vec3 diffuseColour, float specGloss, 
							vec4 lightColour, vec3 lightPos, vec4 lightParam, 
							out vec3 diffuse, out vec3 specular){
	float lightRadius = lightParam.x;
	vec3 toLight = normalize(lightPos - worldPos);
	float distance = length(lightPos - worldPos);
	float attenuation = (clamp((lightRadius * lightRadius) / (distance * distance) * thresAtRadius - thresAtRadius, 0.0f, 1.0f - thresAtRadius)) * lightColour.a;

	SurfaceCommon(toLight, toEye, normal, 
					diffuseColour, lightColour.rgb, 
					specGloss, attenuation, diffuse, specular);
}

void CalculateSpotLight( vec3 toEye, vec3 worldPos, vec3 normal,
							vec3 diffuseColour, float specGloss,
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
		float attenuation = (clamp((lightRadius * lightRadius) / (distance * distance) * thresAtRadius - thresAtRadius, 0.0f, 1.0f - thresAtRadius)) * lightColour.a;
		float sqrSinFragment = abs(1-cosFragment*cosFragment);
		float sqrSinHalfAngle = abs(1-cosHalfAngle*cosHalfAngle);
		float scaleAttenu = (sqrSinHalfAngle-sqrSinFragment)/sqrSinHalfAngle;
		attenuation = attenuation *  scaleAttenu * scaleAttenu;

		SurfaceCommon(toLight, toEye, normal, 
					diffuseColour, lightColour.rgb, 
					specGloss, attenuation, diffuse, specular);
	}
	else{
		diffuse = vec3(0,0,0);
		specular = vec3(0,0,0);
	}
}

//Camera
layout(std140, binding=LIW_SHADER_UBO_BIND_CAMERADATA) uniform CameraBlock{
	mat4 viewMatrix;
	mat4 projMatrix;
	vec3 posCamera;
} cameraBlk;

//Light
layout(std140, binding=LIW_SHADER_UBO_BIND_FORWARD_PERPIX_LIGHTDATA) uniform ForwardLightBlockPerPixel{ //Separate Light Types to reduce branching
	vec4 lightColours_Directional[LIW_LIGHT_MAX_PERPIXEL];
	vec4 lightColours_Point[LIW_LIGHT_MAX_PERPIXEL];
	vec4 lightColours_Spot[LIW_LIGHT_MAX_PERPIXEL];
	vec4 lightPositions_Directional[LIW_LIGHT_MAX_PERPIXEL];
	vec4 lightPositions_Point[LIW_LIGHT_MAX_PERPIXEL];
	vec4 lightPositions_Spot[LIW_LIGHT_MAX_PERPIXEL];
	vec4 lightParams_Directional[LIW_LIGHT_MAX_PERPIXEL];
	vec4 lightParams_Point[LIW_LIGHT_MAX_PERPIXEL];
	vec4 lightParams_Spot[LIW_LIGHT_MAX_PERPIXEL];
	int lightCount_Directional;
	int lightCount_Point;
	int lightCount_Spot;
} forwardLightBlk;

//Material
uniform sampler2D mainTex;

in Vertex{
	vec3 posWorld;
	vec3 normalWorld;
	vec2 texCoord;
	vec4 colour;
} IN;

out vec4 fragColour;

void main() {
	const float specGloss = 0.8f;
	const float diffuseComponent = 0.6f;
	const float specularComponent = 0.38f;

	const vec3 normalWorld = IN.normalWorld;
	const vec3 posWorld = IN.posWorld;

	vec3 toEye = cameraBlk.posCamera - posWorld;
	vec3 diffuseColour = texture(mainTex, IN.texCoord).rgb;
	
	vec3 diffuse = vec3(0,0,0);
	vec3 specular = vec3(0,0,0);
	
	//Directional
	for(int i=0;i<forwardLightBlk.lightCount_Directional;i++){
		vec3 diffuseOut = vec3(0,0,0);
		vec3 specularOut = vec3(0,0,0);
		CalculateDirectionalLight( toEye, normalWorld, diffuseColour, specGloss,
								forwardLightBlk.lightColours_Directional[i], 
								forwardLightBlk.lightPositions_Directional[i].xyz, 
								forwardLightBlk.lightParams_Directional[i], 
								diffuseOut, specularOut);
		diffuse+=diffuseOut;
		specular+=specularOut;
	}
	
	//Point
	for(int i=0;i<forwardLightBlk.lightCount_Point;i++){
		vec3 diffuseOut = vec3(0,0,0);
		vec3 specularOut = vec3(0,0,0);
		CalculatePointLight( toEye, posWorld, normalWorld, diffuseColour, specGloss,
							forwardLightBlk.lightColours_Point[i], 
							forwardLightBlk.lightPositions_Point[i].xyz, 
							forwardLightBlk.lightParams_Point[i], 
							diffuseOut, specularOut);
		diffuse+=diffuseOut;
		specular+=specularOut;
	}
	
	//Spot
	for(int i=0;i<forwardLightBlk.lightCount_Spot;i++){
		vec3 diffuseOut = vec3(0,0,0);
		vec3 specularOut = vec3(0,0,0);
		CalculateSpotLight( toEye, posWorld, normalWorld, diffuseColour, specGloss,
							forwardLightBlk.lightColours_Point[i], 
							forwardLightBlk.lightPositions_Point[i].xyz, 
							forwardLightBlk.lightParams_Point[i], 
							diffuseOut, specularOut);
		diffuse+=diffuseOut;
		specular+=specularOut;
	}
	
	fragColour = vec4(diffuse*diffuseComponent + specular*specularComponent, 0);
	vec4 ambientLightingClr = vec4(ambientColour.xyz*diffuseColour.rgb*ambientComponent, 0);
	fragColour += ambientLightingClr;
	fragColour.a = 1.0f;
}