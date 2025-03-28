#include "42_ParticleSystem.Slot.hlsl"

Texture2D ParticleMap : register(Texture_PS_Particle);
SamplerState LinearSampler : register(Sampler_PS_Linear);

cbuffer CB_World : register(Const_VS_World)
{ 
    matrix WorldTF_VS;
}

cbuffer CB_ViewProjection : register(Const_VS_VieProjection)
{ 
    matrix View_VS;
    matrix ViewInv_VS;
    matrix Projection_VS;
}

cbuffer CB_ParticleSetting_VS : register(Const_VS_ParticleData)
{
	float4 VS_MinColor;
	float4 VS_MaxColor;

	float3 VS_Gravity;

	float2 VS_StartSize;
	float2 VS_EndSize;

	float2 VS_RotateSpeed;

	float VS_Duration;
	float VS_DurationModifier;

	float VS_ColorAmount;
	float VS_CurrentTime;
}

cbuffer CB_ParticleSetting_GS : register(Const_GS_ParticleData)
{
	float4 GS_MinColor;
	float4 GS_MaxColor;

	float3 GS_Gravity;
	float  Padding;

	float2 GS_StartSize;
	float2 GS_EndSize;

	float2 GS_RotateSpeed;

	float  GS_Duration;
	float  GS_DurationModifier;

	float  GS_ColorAmount;
	float  GS_CurrentTime;
}
