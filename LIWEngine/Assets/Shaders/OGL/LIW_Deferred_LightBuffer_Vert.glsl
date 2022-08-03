#version 420 core
#include "Lighting/LIWLighting_Deferred.glsl"

//Camera
LIW_DEFINE_UB_CAMERADATA(cameraBlk);

//Light
LIW_DEFINE_UB_DEFERRED_PERPIX_LIGHTDATA(deferredLightBlk);

layout(location=LIW_SHADER_VA_LOCATION_POSITION) 	in vec3 position;

out Vertex{
	flat int indexID;
} OUT;

void main(){
	vec3 scale = vec3(deferredLightBlk.lightParams[gl_InstanceID].x*2.0f);
	vec3 posWorld = (position * scale) + deferredLightBlk.lightPositions[gl_InstanceID].xyz;
	OUT.indexID = gl_InstanceID;
	gl_Position = (cameraBlk.projMatrix * cameraBlk.viewMatrix) * vec4(posWorld, 1.0f);
}