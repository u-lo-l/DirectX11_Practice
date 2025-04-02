
#include "43_LightingSlot.hlsl"

#define MAX_POINT_LIGHTS 256
#define MAX_SPOT_LIGHTS 256

static const int DiffuseMap = 0;
static const int SpecularMap = 1;
static const int NormalMap = 2;

Texture2D MaterialMaps[MAX_MATERIAL_TEXTURE_COUNT] : register(Texture_PS_Texture);

SamplerState LinearSampler		: register(Sampler_PS_Linear);
SamplerState AnisotropicSampler	: register(Sampler_PS_Anisotropic);
SamplerState PointSampler		: register(Sampler_PS_Point);

cbuffer CB_Material : register(Const_PS_Material)
{ 
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float  RimWidth;
	float  RimPower;
	float2 MaterialPadding;
}

struct ColorDesc
{
	float4 Ambient;
    float4 Diffuse;
    float4 Specular;
};

struct PointLightDesc
{
	ColorDesc Color;
    float4 Emissive;
    
    float3 Position;
    float Range;
    
    float Intensity;
    float3 Padding;
};


struct SpotLightDesc
{
	ColorDesc Color;
    float4 Emissive;
    
    float3 Position;
    float Range;
    
    float3 Direction;
    float Intensity;

    float Theta;
	float Phi;
    float2 Padding;
};

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
