#include "42_ParticleSystem.Resource.hlsl"

void VS_GS_CalculateAge(in float InElapsedTime, in float Random, out float Age, out float NormalizedAge);
float4 VS_CalculatePosition(in float3 InPosition, in float3 InStartVelocity, in float3 InEndVelocity, in float Age, in float NormalizedAge);
float2 GS_CalculateSize(in float Random, in float NormalizedAge);
float2x2 GS_CalculateRotation(in float Random, in float Age);
float4 GS_CalculateColor(in float Random, in float NormalizedAge);

void VS_CalculateAge
(
    in float InParticleSpawndTime, 
    in float Random, 
    out float Age,
    out float NormalizedAge
)
{
    Age = VS_CurrentTime - InParticleSpawndTime;
    NormalizedAge = saturate(Age / VS_Duration);
}

void GS_CalculateAge
(
    in float InParticleSpawndTime, 
    in float Random,
    out float Age,
    out float NormalizedAge
)
{
    Age = GS_CurrentTime - InParticleSpawndTime;
    NormalizedAge = saturate(Age / GS_Duration);
}

float4 VS_CalculatePosition
(
    in float3 InPosition,
    in float3 InStartVelocity,
    in float3 InEndVelocity,
    in float Age, 
    in float NormalizedAge
)
{
    float3 Velocity = lerp(InStartVelocity, InEndVelocity, NormalizedAge);
    float3 Position = InPosition + 0.5f * NormalizedAge * (Velocity + InStartVelocity);

    // InPosition += VS_Gravity * Age * NormalizedAge; // ?

    float4 result = float4(Position , 1.f);
    result = mul(result, WorldTF_VS);
    result = mul(result, View_VS);
    result = mul(result, Projection_VS);
    return result;
}

float2 GS_CalculateSize
(
    in float Random,
    in float NormalizedAge
)
{
    float start = lerp(GS_StartSize.x, GS_StartSize.y, Random);
    float end   = lerp(GS_EndSize.x, GS_EndSize.y, Random);
    
    return lerp(start, end, NormalizedAge);
}


float2x2 GS_CalculateRotation
(
    in float Random,
    in float Age
)
{
    float angle = lerp(GS_RotateSpeed.x, GS_RotateSpeed.y, Random);
    float radian = angle * Age;
    
    float c = cos(radian);
    float s = sin(radian);
    
    return float2x2(c, -s, s, c);
}


float4 GS_CalculateColor
(
    in float Random,
    in float NormalizedAge
)
{
    float4 color = lerp(GS_MinColor, GS_MaxColor, Random) * NormalizedAge;
    color.a = (1 - NormalizedAge);
    return color * GS_ColorAmount;
}

