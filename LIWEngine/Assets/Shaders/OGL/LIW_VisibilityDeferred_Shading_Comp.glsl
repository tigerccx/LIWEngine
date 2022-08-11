#version 450 core

#include "Lighting/LIWLighting_DeferredVisibility.glsl"

layout (local_size_x = 64) in;

layout(r32ui) uniform uimage2D indexImg;
layout(rgba8) uniform image2D outputImg;
uniform sampler2D depthTexture;

uniform uint dispatchIdx; //Also the material idx
uniform vec2 pixelSize;
uniform mat4 invProjView;
uniform uint vertexCount;

// Camera
LIW_DEFINE_UB_CAMERADATA(cameraBlk);

// Light
LIW_DEFINE_UB_DEFERRED_VISIBILITY_PERPIX_LIGHTDATA(lights);

// counters
layout(std430, binding=LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_MATERIAL_COUNTERS) buffer LIW_SHADER_SSB_DEFERRED_VISIBILITY_MATERIAL_COUNTERS
{
    uint counters[LIW_DEFERRED_VISIBILITY_MAX_MATERIAL_COUNT];
};
// sums
layout(std430, binding=LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_PREFIXSUM) buffer LIW_SHADER_SSB_DEFERRED_VISIBILITY_PREFIXSUM
{
	uint prefixSums[LIW_DEFERRED_VISIBILITY_MAX_MATERIAL_COUNT];
};

// worklist
layout(std430, binding=LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_WORKLIST) buffer LIW_SHADER_SSB_DEFERRED_VISIBILITY_WORKLIST
{
    uint worklist[];
};

// vertex buffer
layout(std430, binding=LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_VERTEX_BUFFER) buffer LIW_SHADER_SSB_DEFERRED_VISIBILITY_VERTEX_BUFFER
{
    float vertexBuffer[];
};

// index buffer
layout(std430, binding=LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_INDEX_BUFFER) buffer LIW_SHADER_SSB_DEFERRED_VISIBILITY_INDEX_BUFFER
{
    uint indexBuffer[];
};

// mesh id lookup
layout(std430, binding=LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_MESH_ID_LOOKUP) buffer LIW_SHADER_SSB_DEFERRED_VISIBILITY_MESH_ID_LOOKUP
{
    uint meshIdLookup[];
};

// index offset lookup
layout(std430, binding=LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_INDEX_OFFSET_LOOKUP) buffer LIW_SHADER_SSB_DEFERRED_VISIBILITY_INDEX_OFFSET_LOOKUP
{
    uint indexLookup[];
};

// model matrices
layout(std430, binding=LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_MODEL_MATRICES) readonly buffer LIW_SHADER_SSB_DEFERRED_VISIBILITY_MODEL_MATRICES
{
    mat4 modelMatrices[];
};

// normal matrices
layout(std430, binding=LIW_SHADER_SSBO_DEFERRED_VISIBILITY_BIND_NORMAL_MATRICES) readonly buffer LIW_SHADER_SSB_DEFERRED_VISIBILITY_NORMAL_MATRICES
{
    float normalMatrices[];
};

const vec3 colours[] = 
{
	vec3(0.25f, 0.0f, 0.0f),
	vec3(0.0f, 0.25f, 0.0f),
	vec3(0.0f, 0.0f, 0.25f),
	vec3(0.25f, 0.25f, 0.0f),
	vec3(0.25f, 0.0f, 0.25f),
	vec3(0.0f, 0.25f, 0.25f),
	vec3(0.25f, 0.25f, 0.25f),
	vec3(0.5f, 0.25f, 0.25f),
	vec3(0.25f, 0.5f, 0.25f),
	vec3(0.25f, 0.25f, 0.5f),
	vec3(0.5f, 0.5f, 0.25f),
	vec3(0.5f, 0.25f, 0.5f),
	vec3(0.25f, 0.5f, 0.5f),
	vec3(0.5f, 0.5f, 0.5f),
	vec3(0.75f, 0.5f, 0.5f),
	vec3(0.5f, 0.75f, 0.5f),
	vec3(0.5f, 0.5f, 0.75f),
	vec3(0.75f, 0.75f, 0.5f),
	vec3(0.75f, 0.5f, 0.75f),
	vec3(0.5f, 0.75f, 0.75f),
	vec3(0.75f, 0.75f, 0.75f),
	vec3(1.0f, 0.75f, 0.75f),
	vec3(0.75f, 1.0f, 0.75f),
	vec3(0.75f, 0.75f, 1.0f),
	vec3(1.0f, 1.0f, 0.75f),
	vec3(1.0f, 0.75f, 1.0f),
	vec3(0.75f, 1.0f, 1.0f),
	vec3(1.0f, 1.0f, 1.0f)
}; //28 colours

//Material
uniform sampler2D mainTex;
uniform sampler2D normalTex;



const uint maskCoordY = (1<<16)-1;
const uint maskCoordX = ~maskCoordY;

struct Vertex{
	vec3 position;
	vec3 normal;
	vec4 tangent;
	vec2 texcoord;
};

void IntersectPlane(vec3 org, vec3 dir, vec3 p0, vec3 p1, vec3 p2, out vec3 p)
{
    vec3 D = dir;
    vec3 N = cross(p1-p0, p2-p0);
    p = org + D * dot(p0 - org, N) / dot(D, N);
}

void GetBaryCentric( 
    vec3 p,
    vec3 a, vec3 b, vec3 c, 
    out vec3 baryCentric) 
{ 
    vec3 v0 = b - a;
	vec3 v1 = c - a;
	vec3 v2 = p - a;
    float d00 = dot(v0, v0);
    float d01 = dot(v0, v1);
    float d11 = dot(v1, v1);
    float d20 = dot(v2, v0);
    float d21 = dot(v2, v1);
    float denom = d00 * d11 - d01 * d01;
    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;
	baryCentric = vec3(u,v,w);
} 

//struct BarycentricDeriv
//{
//	vec3 m_lambda;
//	vec3 m_ddx;
//	vec3 m_ddy;
//};
//
//BarycentricDeriv CalcFullBary(vec4 pt0, vec4 pt1, vec4 pt2, vec4 pixelNdc, vec4 winSize)
//{
//	BarycentricDeriv ret = (BarycentricDeriv)0;
//	
//	vec3 invW = rcp(vec3(pt0.w, pt1.w, pt2.w));
//	
//	vec2 ndc0 = pt0.xy * invW.x;
//	vec2 ndc1 = pt1.xy * invW.y;
//	vec2 ndc2 = pt2.xy * invW.z;
//	
//	float invDet = rcp(determinant(mat2(ndc2 - ndc1, ndc0 - ndc1)));
//	ret.m_ddx = vec3(ndc1.y - ndc2.y, ndc2.y - ndc0.y, ndc0.y - ndc1.y) * invDet * invW;
//	ret.m_ddy = vec3(ndc2.x - ndc1.x, ndc0.x - ndc2.x, ndc1.x - ndc0.x) * invDet * invW;
//	float ddxSum = dot(ret.m_ddx, vec3(1,1,1));
//	float ddySum = dot(ret.m_ddy, vec3(1,1,1));
//	
//	vec2 deltaVec = pixelNdc - ndc0;
//	float interpInvW = invW.x + deltaVec.x*ddxSum + deltaVec.y*ddySum;
//	float interpW = rcp(interpInvW);
//	
//	ret.m_lambda.x = interpW * (invW[0] + deltaVec.x*ret.m_ddx.x + deltaVec.y*ret.m_ddy.x);
//	ret.m_lambda.y = interpW * (0.0f    + deltaVec.x*ret.m_ddx.y + deltaVec.y*ret.m_ddy.y);
//	ret.m_lambda.z = interpW * (0.0f    + deltaVec.x*ret.m_ddx.z + deltaVec.y*ret.m_ddy.z);
//	
//	ret.m_ddx *= (2.0f/winSize.x);
//	ret.m_ddy *= (2.0f/winSize.y);
//	ddxSum    *= (2.0f/winSize.x);
//	ddySum    *= (2.0f/winSize.y);
//	
//	ret.m_ddy *= -1.0f;
//	ddySum    *= -1.0f;
//	
//	float interpW_ddx = 1.0f / (interpInvW + ddxSum);
//	float interpW_ddy = 1.0f / (interpInvW + ddySum);
//	
//	ret.m_ddx = interpW_ddx*(ret.m_lambda*interpInvW + ret.m_ddx) - ret.m_lambda;
//	ret.m_ddy = interpW_ddy*(ret.m_lambda*interpInvW + ret.m_ddy) - ret.m_lambda;  
//	
//	return ret;
//}

void GetTriangleVertices(uint idxMesh, uint idxTriangle, out Vertex v0, out Vertex v1, out Vertex v2)
{
	//TODO: For now assume same count of position/normal/tangent/texcoord 
	//const uint vertexCount = vertexBuffer.length()/(3+3+4+2);

	const uint idxOffset = indexLookup[idxMesh];
	uint idx0 = indexBuffer[idxOffset+idxTriangle*3];
	uint idx1 = indexBuffer[idxOffset+idxTriangle*3+1];
	uint idx2 = indexBuffer[idxOffset+idxTriangle*3+2];
	
	v0.position = vec3(vertexBuffer[3*idx0], vertexBuffer[3*idx0+1], vertexBuffer[3*idx0+2]);
	v1.position = vec3(vertexBuffer[3*idx1], vertexBuffer[3*idx1+1], vertexBuffer[3*idx1+2]);
	v2.position = vec3(vertexBuffer[3*idx2], vertexBuffer[3*idx2+1], vertexBuffer[3*idx2+2]);
	
	const uint offsetNormal = 3*vertexCount;
	v0.normal = vec3(vertexBuffer[offsetNormal+3*idx0], vertexBuffer[offsetNormal+3*idx0+1], vertexBuffer[offsetNormal+3*idx0+2]);
	v1.normal = vec3(vertexBuffer[offsetNormal+3*idx1], vertexBuffer[offsetNormal+3*idx1+1], vertexBuffer[offsetNormal+3*idx1+2]);
	v2.normal = vec3(vertexBuffer[offsetNormal+3*idx2], vertexBuffer[offsetNormal+3*idx2+1], vertexBuffer[offsetNormal+3*idx2+2]);
	v0.normal = normalize(v0.normal);
	v1.normal = normalize(v1.normal);
	v2.normal = normalize(v2.normal);
	
	const uint offsetTangent = offsetNormal + 3*vertexCount;
	v0.tangent = vec4(vertexBuffer[offsetTangent+4*idx0], vertexBuffer[offsetTangent+4*idx0+1], vertexBuffer[offsetTangent+4*idx0+2], vertexBuffer[offsetTangent+4*idx0+3]);
	v1.tangent = vec4(vertexBuffer[offsetTangent+4*idx1], vertexBuffer[offsetTangent+4*idx1+1], vertexBuffer[offsetTangent+4*idx1+2], vertexBuffer[offsetTangent+4*idx1+3]);
	v2.tangent = vec4(vertexBuffer[offsetTangent+4*idx2], vertexBuffer[offsetTangent+4*idx2+1], vertexBuffer[offsetTangent+4*idx2+2], vertexBuffer[offsetTangent+4*idx2+3]);
	
	const uint offsetTexcoord = offsetTangent + 4*vertexCount;
	v0.texcoord = vec2(vertexBuffer[offsetTexcoord+2*idx0], vertexBuffer[offsetTexcoord+2*idx0+1]);
	v1.texcoord = vec2(vertexBuffer[offsetTexcoord+2*idx1], vertexBuffer[offsetTexcoord+2*idx1+1]);
	v2.texcoord = vec2(vertexBuffer[offsetTexcoord+2*idx2], vertexBuffer[offsetTexcoord+2*idx2+1]);
}

void main(void)
{		
	const uint workGroupID = gl_WorkGroupID.x*gl_NumWorkGroups.y + gl_WorkGroupID.y;
	const uint invocationID = workGroupID*gl_WorkGroupSize.x*gl_WorkGroupSize.y+gl_LocalInvocationIndex;
	const uint materialID = dispatchIdx;
	
	if(invocationID>counters[materialID]) return;
	
	const uint offsetID = prefixSums[materialID];
	const uint pixelLocation = worklist[offsetID+invocationID];
	const uint pixelLocX = (pixelLocation&maskCoordX)>>16;
	const uint pixelLocY = pixelLocation&maskCoordY;
	ivec2 p = ivec2(pixelLocX, pixelLocY);
	vec2 texCoordScreen = vec2(p*pixelSize);
	
	const uint idxObjectTriangle = imageLoad(indexImg, p).r;
	Vertex v0, v1, v2;
	const uint idxObject = (liw_c_maskIdxObject&idxObjectTriangle)>>18;
	const uint idxTriangle = liw_c_maskIdxTriangle&idxObjectTriangle;
	const uint idxMesh = meshIdLookup[idxObject];
	GetTriangleVertices(idxMesh, idxTriangle, v0, v1, v2);
	const mat4 modelMatrix = modelMatrices[idxObject];
	const mat3 normalMatrix = mat3(normalMatrices[9*idxObject], normalMatrices[9*idxObject+1], normalMatrices[9*idxObject+2],
									normalMatrices[9*idxObject+3], normalMatrices[9*idxObject+4], normalMatrices[9*idxObject+5],
									normalMatrices[9*idxObject+6], normalMatrices[9*idxObject+7], normalMatrices[9*idxObject+8]);
	
	//
	// Calculate current fragment
	//
	const float depth = texture(depthTexture, texCoordScreen).r;
	const vec3 posNDC = vec3(texCoordScreen, depth)*2.0f-1.0f;
	const vec4 posInvClip = invProjView*vec4(posNDC, 1.0f);
	const vec3 posWorld = posInvClip.xyz/posInvClip.w; // Position
	
	const vec3 pos0W = (modelMatrix*vec4(v0.position, 1)).xyz;
	const vec3 pos1W = (modelMatrix*vec4(v1.position, 1)).xyz;
	const vec3 pos2W = (modelMatrix*vec4(v2.position, 1)).xyz;
	
	vec3 baryCentric;
	GetBaryCentric(posWorld, pos0W, pos1W, pos2W, baryCentric);
	
	const vec3 normalLocal = baryCentric.x*v0.normal+baryCentric.y*v1.normal+baryCentric.z*v2.normal;
	const vec3 normalWorld = normalize(normalMatrix*normalLocal); // Normal
	
	const vec3 tangent = baryCentric.x*v0.tangent.xyz+baryCentric.y*v1.tangent.xyz+baryCentric.z*v2.tangent.xyz;
	const vec3 tangentWorld = normalize(normalMatrix*tangent); // Tangent
	
	const vec3 binormalWorld = cross(normalWorld, tangentWorld) * v0.tangent.w; // Binormal
	
	const vec2 texcoord = baryCentric.x*v0.texcoord+baryCentric.y*v1.texcoord+baryCentric.z*v2.texcoord; // Texcoord
	
	
	const float specGloss = 100.0f;
	const float diffuseComponent = 0.4f;
	const float specularComponent = 0.58f;
	
	const vec3 toEye = cameraBlk.posCamera - posWorld;
	
	vec3 diffuseColour = texture(mainTex, texcoord).rgb;
	
	mat3 TBN = mat3(normalize(tangentWorld), normalize(binormalWorld), normalize(normalWorld));
	vec3 normal = texture(normalTex, texcoord).rgb;
	normal = normalize(TBN*normalize(normal*2.0f-1.0f));
	
	//
	// Lighting
	//
	
	vec3 diffuse = vec3(0,0,0);
	vec3 specular = vec3(0,0,0);
	
	//Directional
	for(int i=0;i<lights.lightCount_Directional;i++){
		vec3 diffuseOut = vec3(0,0,0);
		vec3 specularOut = vec3(0,0,0);
		CalculateDirectionalLight( toEye, normal, specGloss,
								lights.lightColours_Directional[i], 
								lights.lightPositions_Directional[i].xyz, 
								lights.lightParams_Directional[i], 
								diffuseOut, specularOut);
		diffuse+=diffuseOut;
		specular+=specularOut;
	}
	
	//Point
	for(int i=0;i<lights.lightCount_Point;i++){
		vec3 diffuseOut = vec3(0,0,0);
		vec3 specularOut = vec3(0,0,0);
		CalculatePointLight( toEye, posWorld, normal, specGloss,
							lights.lightColours_Point[i], 
							lights.lightPositions_Point[i].xyz, 
							lights.lightParams_Point[i], 
							diffuseOut, specularOut);
		diffuse+=diffuseOut;
		specular+=specularOut;
	}
	
	//Spot
	for(int i=0;i<lights.lightCount_Spot;i++){
		vec3 diffuseOut = vec3(0,0,0);
		vec3 specularOut = vec3(0,0,0);
		CalculateSpotLight( toEye, posWorld, normal, specGloss,
							lights.lightColours_Spot[i], 
							lights.lightPositions_Spot[i].xyz, 
							lights.lightParams_Spot[i], 
							diffuseOut, specularOut);
		diffuse+=diffuseOut;
		specular+=specularOut;
	}
	
	
	vec4 fragColour = vec4(diffuse*diffuseColour*diffuseComponent + specular*specularComponent, 0);
	vec4 ambientLightingClr = vec4(liw_c_ambientColour*diffuseColour.rgb*liw_c_ambientComponent, 0);
	fragColour += ambientLightingClr;
	fragColour.a = 1.0f;
	
	imageStore(outputImg, p, fragColour);
}