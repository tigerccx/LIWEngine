#version 420 core
#include "Common/LIWShaderCommon.glsl"

uniform mat4 LIW_SHADER_MODEL_MATRIX;

//Camera
LIW_DEFINE_UB_CAMERADATA(cameraBlk);

layout(location=LIW_SHADER_VA_LOCATION_POSITION) 	in vec3 position;

uniform vec3 LIW_SHADER_DEFERRED_LIGHT_POS;
uniform vec4 LIW_SHADER_DEFERRED_LIGHT_COLOUR;
uniform vec4 LIW_SHADER_DEFERRED_LIGHT_PARAM;

void main(){
	vec3 scale = vec3(LIW_SHADER_DEFERRED_LIGHT_PARAM.x*2.0f);
	vec3 posWorld = (position * scale) + LIW_SHADER_DEFERRED_LIGHT_POS;
	gl_Position = (cameraBlk.projMatrix * cameraBlk.viewMatrix) * vec4(posWorld, 1.0f);
}