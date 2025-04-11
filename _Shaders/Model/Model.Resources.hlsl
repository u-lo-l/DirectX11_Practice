#ifndef __MODEL_RESOURCES_HLSL__
#define __MODEL_RESOURCES_HLSL__
#include "Model.Slot.hlsl"
#include "Model.Struct.hlsl"

// Constant Buffer
cbuffer CB_World_VS : register(Const_VS_World)
{ 
    matrix WorldTF_VS;
}

cbuffer CB_ViewProjection_VS : register(Const_VS_ViewProjection)
{ 
    matrix View_VS;
    matrix ViewInv_VS;
    matrix Projection_VS;
}

cbuffer CB_BoneMatrix : register(Const_VS_BoneMatrix)
{
    matrix OffsetMatrix[MAX_MODEL_TRANSFORM];   // (Bone의 Root-Coordinate Transform)의 역행렬
    matrix BoneTransforms[MAX_MODEL_TRANSFORM]; //
}

cbuffer CB_BoneIndex : register(Const_VS_BoneIndex)
{
    uint BaseBoneIndex;
    float3 Padding;
}

cbuffer CB_Decal_VS : register(Const_VS_DecalProjector)
{
    matrix View_Projector_VS;
    matrix Projection_Projector_VS;
}

cbuffer CB_Shadow_VS : register (Const_VS_ShadowViewProjection)
{
    matrix ShadowView;
    matrix ShadowProjection;
};

cbuffer CB_Material : register(Const_PS_Material)
{ 
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float  RimWidth;
	float  RimPower;
	float2 MaterialPadding;
}

cbuffer CB_ViewProjection : register(Const_PS_ViewInv)
{
    matrix ViewInv_PS;
    float4 Tiling;
}

cbuffer CB_LightDirection_PS : register(Const_PS_LightDirection)
{ 
    float3 LightDirection_PS;
}

cbuffer CB_Shadow_PS : register (Const_PS_ShadowData)
{
    float2 ShadowMapSize;
    float  ShadowBias;
    uint   ShadowQuality;
};

#define MAX_POINT_LIGHTS 256
#define MAX_SPOT_LIGHTS 256
cbuffer CB_Lights : register(Const_PS_Lights)
{
// PointLight
	uint PointLightNum;
	float3 PointLightPadding;
	PointLightDesc PointLights[MAX_POINT_LIGHTS];

// SpotLight
	uint SpotLightNum;
	float3 SpotLightPadding;
	SpotLightDesc SpotLights[MAX_SPOT_LIGHTS];
}

// Sampler
SamplerState LinearSampler		            : register(Sampler_PS_Linear);
SamplerState AnisotropicSampler	            : register(Sampler_PS_Anisotropic);
SamplerState PointSampler		            : register(Sampler_PS_Point);
SamplerComparisonState ShadowSampler		: register(Sampler_PS_Shadow);

// Texture
static const int DiffuseMap = 0;
static const int SpecularMap = 1;
static const int NormalMap = 2;
Texture2D MaterialMaps[MAX_MATERIAL_TEXTURE_COUNT] : register(Texture_PS_Texture);
Texture2D ProjectorTexture : register(Texture_PS_Decal);
Texture2D ShadowMap : register (Texture_PS_Shadow);

#endif