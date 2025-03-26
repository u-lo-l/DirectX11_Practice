#include "Slot.hlsl"

static const int DiffuseMap = 0;
static const int SpecularMap = 1;
static const int NormalMap = 2;

Texture2D MaterialMaps[MAX_MATERIAL_TEXTURE_COUNT] : register(Texture_PS_Texture);

cbuffer CB_Material : register(Const_PS_Material)
{ 
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float4 Emissive;
}

SamplerState LinearSampler : register(Sampler_PS_Linear);