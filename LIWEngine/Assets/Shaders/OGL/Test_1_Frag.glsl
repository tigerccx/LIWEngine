#version 420 core
#include "Lighting/LIWLighting_Forward.glsl"

//Camera
LIW_DEFINE_UB_CAMERADATA(cameraBlk);

//Light
LIW_DEFINE_UB_FORWARD_PERPIX_LIGHTDATA(forwardLightBlk);

//Material
uniform sampler2D mainTex;
uniform sampler2D normalTex;

in Vertex{
	vec3 posWorld;
	vec3 normalWorld;
	vec3 tangentWorld;
	vec3 binormalWorld;
	vec2 texCoord;
	vec4 colour;
} IN;

out vec4 fragColour;

void main() {
	const float specGloss = 100.0f;
	const float diffuseComponent = 0.4f;
	const float specularComponent = 0.58f;

	const vec3 normalWorld = normalize(IN.normalWorld);
	const vec3 tangentWorld = normalize(IN.tangentWorld);
	const vec3 binormalWorld = normalize(IN.binormalWorld);
	const vec3 posWorld = IN.posWorld;

	vec3 toEye = cameraBlk.posCamera - posWorld;
	vec3 diffuseColour = texture(mainTex, IN.texCoord).rgb;
	
	mat3 TBN = mat3(normalize(tangentWorld), normalize(binormalWorld), normalize(normalWorld));
	vec3 normal = texture(normalTex, IN.texCoord).rgb;
	normal = normalize(TBN*normalize(normal*2.0f-1.0f));
	
	vec3 diffuse = vec3(0,0,0);
	vec3 specular = vec3(0,0,0);
	
	//Directional
	for(int i=0;i<forwardLightBlk.lightCount_Directional;i++){
		vec3 diffuseOut = vec3(0,0,0);
		vec3 specularOut = vec3(0,0,0);
		CalculateDirectionalLight( toEye, normal, specGloss,
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
		CalculatePointLight( toEye, posWorld, normal, specGloss,
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
		CalculateSpotLight( toEye, posWorld, normal, specGloss,
							forwardLightBlk.lightColours_Spot[i], 
							forwardLightBlk.lightPositions_Spot[i].xyz, 
							forwardLightBlk.lightParams_Spot[i], 
							diffuseOut, specularOut);
		diffuse+=diffuseOut;
		specular+=specularOut;
	}
	
	fragColour = vec4(diffuse*diffuseColour*diffuseComponent + specular*specularComponent, 0);
	vec4 ambientLightingClr = vec4(liw_c_ambientColour*diffuseColour.rgb*liw_c_ambientComponent, 0);
	fragColour += ambientLightingClr;
	fragColour.a = 1.0f;
	
	//fragColour = vec4(normal*0.5f+0.5f, 1.0f);
	//fragColour = vec4(gl_FragCoord.x/1280.0f, gl_FragCoord.y/720.0f, 0.0f, 1.0f);
	//fragColour = vec4(gl_FragCoord.z*2.0f-1.0f, 0.0f, 0.0f, 1.0f);
	//fragColour = vec4(specular, 1.0f);
}