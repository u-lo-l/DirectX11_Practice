#ifndef __NORMAL_FUNC_HLSLI__
#define __NORMAL_FUNC_HLSLI__

static const uint bUseNormalTexture = 1 << 0;
bool HasNormalTexture(uint Flag) { return (Flag & bUseNormalTexture) != 0; }

#define RED float3(1, 0, 0);
#define GREEN float3(0, 1, 0);
#define BLUE float3(0, 0, 1);
#define CYAN float3(0, 1, 1);
#define MAGENTA float3(1, 0, 1);
#define YELLOW float3(1, 1, 0);

static const float3 Color_PosX = RED;
static const float3 Color_PosY = GREEN;
static const float3 Color_PosZ = BLUE;
static const float3 Color_NegX = CYAN;
static const float3 Color_NegY = MAGENTA;
static const float3 Color_NegZ = YELLOW;

const static float handed = -1; // for LeftHanded Coordinate

/*
* TangentSpaceNormal : Texture에서 읽을 경우, (-1 ~ 1 로 매핑 후 전달)
*/
float3 ApplyNormalMap(in float3 TangentSpaceNormal, in float3 WorldSpaceNormal, in float3 WorldSpaceTangent)
{
	float3 N = normalize(WorldSpaceNormal); // Z
	float3 T = normalize(WorldSpaceTangent); // X
	float3 B = cross(N, T); // Y

	float3x3 TBN =  float3x3(T, B, N);
	float3 WorldNormal = mul(TangentSpaceNormal, TBN);
	return WorldNormal;
}

float3 ApplyNormalMap(in float3 TangentSpaceNormal)
{
	return ApplyNormalMap(TangentSpaceNormal, float3(0, 1, 0), float3(1, 0 ,0));
}

float4 VisualizeNormal(in float3 Normal, float Alpha)
{
	float3 N = Normal;
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