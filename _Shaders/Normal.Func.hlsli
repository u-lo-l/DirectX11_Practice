#ifndef __NORMAL_FUNC_HLSLI__
#define __NORMAL_FUNC_HLSLI__

static const float3 Color_PosX = float3(1, 0, 0);
static const float3 Color_PosY = float3(0, 1, 0);
static const float3 Color_PosZ = float3(0, 0, 1);
static const float3 Color_NegX = float3(0, 1, 1);
static const float3 Color_NegY = float3(1, 0, 1);
static const float3 Color_NegZ = float3(1, 1, 0);

const static float handed = -1; // for LeftHanded Coordinate
float3 ApplyNormalMap(in float3 TangentSpaceNormal, in float3 WorldSpaceNormal, in float3 WorldSpaceTangent)
{
	float3 Normal = TangentSpaceNormal * 2.f - 1.f;

    float3 N = normalize(WorldSpaceNormal); // Z
    float3 T = normalize(WorldSpaceTangent); // X
    float3 B = cross(N, T); // Y

	float3x3 TBN =  float3x3(T, B, N);
    float3 WorldNormal = mul(Normal, TBN);
    return WorldNormal;
}

float4 VisualizeNormal(in float3 Normal, float Alpha)
{
	float3 N = normalize(Normal);
	float4 Color = float4(0, 0, 0, Alpha);
	{
		float3 Temp = N.x >= 0 ? Color_PosX : Color_NegX;
		Color.rgb += abs(N.x) * Temp;
	}

	{
		float3 Temp = N.y >= 0 ? Color_PosY : Color_NegY;
		Color.rgb += abs(N.y) * Temp;
	}

	{
		float3 Temp = N.z >= 0 ? Color_PosZ : Color_NegZ;
		Color.rgb += abs(N.z) * Temp;
	}
	return Color;
}

#endif