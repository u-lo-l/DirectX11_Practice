#ifndef __FOLIAGE_HLSL__
# define __FOLIAGE_HLSL__

#include "../../PerFrame.hlsli"
#include "../../Normal.Func.hlsli"
#include "../../Shading.Func.hlsli"
#include "./Foliage.Func.hlsli"
#include "./Foliage.Param.hlsli"

cbuffer CB_PerMaterial : register(b1)
{
	// None;
}

cbuffer CB_PerRenderable : register(b2) // VGP
{
	matrix World;

	float HeightScaler;
	float3 DistanceRange;

	float2 AltitudeRange;
	float2 TerrainSize; // Texture가 아닌 실제 지형의 크기
}

SamplerState LinearSampler_Clamp : register(s0); // VS DS PS

Texture2D<float>	TerrainHeightMap	: register(t0); // VS PS
Texture2D<float4>	TerrainNormalMap	: register(t1); // PS
Texture2D			FoliageDensityMap	: register(t2); // GS
Texture2DArray		FoliageTextures		: register(t3); // PS

VS_OUTPUT VSMain(VS_INPUT Input)
{
	VS_OUTPUT output;

	output.UV = float2(Input.Position.x / TerrainSize.x, Input.Position.z / TerrainSize.y);
	float Height = TerrainHeightMap.SampleLevel(LinearSampler_Clamp, output.UV, 0).r;
	Input.Position.y = Height * HeightScaler;

	output.WPosition = mul(Input.Position, World);
	output.Scale = Input.Scale;
	output.MapIndex = Input.MapIndex;
	output.Random = Input.Random;
	return output;
}

[maxvertexcount(12)] // 4(VertexForQuad) * 3(Triangle)
void GSMain(point VS_OUTPUT input[1], inout TriangleStream<GS_OUTPUT> stream)
{
	float3 ViewSpacePosition = mul(input[0].WPosition, View).xyz;
	float FoliageDensity = FoliageDensityMap.SampleLevel(LinearSampler_Clamp, input[0].UV, 0).r;
	float3 TerrainNormal = TerrainNormalMap.SampleLevel(LinearSampler_Clamp, input[0].UV, 0).rgb * 2.f - 1.f;
	float RandomValue = input[0].Random;

	bool bVisible = CheckVisibility(
									 ViewSpacePosition,
									 input[0].WPosition,
									 FoliageDensity,
									 TerrainNormal,
									 AltitudeRange,
									 RandomValue
									);
	[flatten]
	if (bVisible == false)
		return ;

	int LOD = 0;
	const float RatioStep[4] = {1.f, 0.5f, 0.2f, 0.f};
	const float N = DistanceRange.x;
	const float M = DistanceRange.y;
	const float F = DistanceRange.z;
	const float DistInf = F * 2.f;
	const float D = distance(input[0].WPosition.xyz, CameraWorldPosition);

	if (D <= N)
	{
		LOD = 3;
	}
	else if (D <= M)
	{
		float RenderProbability = lerp(RatioStep[0], RatioStep[1], (D - N) / (M - N));
		float Ratio = saturate((M - D) / (M - N)); // Cross 확률
		if (RandomValue <= RenderProbability)
			LOD = (RandomValue <= Ratio) ? 3 : 2;
	}
	else if (D <= F)
	{
		float RenderProbability = lerp(RatioStep[1], RatioStep[2], (D - M) / (F - M));
		float Ratio = saturate((F - D) / (F - M)); // Billboard 확률
		if (RandomValue <= RenderProbability)
			LOD = (RandomValue <= Ratio) ? 2 : 1;
	}
	else if (D <= DistInf)
	{
		float RenderProbability = lerp(RatioStep[2], RatioStep[3], (D - F) / (DistInf - F));
		float Ratio = saturate((DistInf - D) / (DistInf - F)); // 안 그릴 확률
		if (RandomValue <= RenderProbability)
			LOD = (RandomValue <= Ratio) ? 1 : 0;
	}
	else
	{
		return ;
	}
	float MipLevel = lerp(0, 9, saturate(D / DistInf));

	if (LOD == 3)
		Foliage_Triangle(input[0], stream, CameraWorldPosition, mul(View, Projection), MipLevel);
	else if (LOD == 2)
		Foliage_CrossQuad(input[0], stream, CameraWorldPosition, mul(View, Projection), MipLevel);
	else if (LOD == 1)
		Foliage_Billboard(input[0], stream, CameraWorldPosition, mul(View, Projection), MipLevel);
}

// RasterizerState : Cull_NONE
// BlendState : AlphaBlend_Coverage
const static float4 ColorMultiplier = float4(1.75f,1.75f,1.75f,1.f);
float4 PSMain(GS_OUTPUT input) : SV_Target
{
    const float Specular = 0.1f;
    const float Ambient = 0.2f;
    const float Diffuse = (1 - Specular);

    float4 FoliageColor = FoliageTextures.SampleLevel(LinearSampler_Clamp, float3(input.TexCord, input.MapIndex), input.MipLevel);
	[flatten] if (FoliageColor.a < 0.5f)
	{
		discard;
	}

	float3 TerrainNormal = TerrainNormalMap.Sample(LinearSampler_Clamp, input.UV).rgb * 2.f + 1.f;
	TerrainNormal = ApplyNormalMap(TerrainNormal);

    float LDotN = dot(-normalize(LightDirection), TerrainNormal);

	BlinnPhongInput Param;
	Param.Ambient = float4(0.1f, 0.1f, 0.1f, 1.f);
	Param.Diffuse = FoliageColor;
	Param.Specular = float4(0.f, 0.f, 0.f, 1.f);
	Param.LightColor = LightColor;
	Param.LightDirection = LightDirection;
	Param.Normal = TerrainNormal;
	Param.WorldPosition = input.WPosition;
	Param.WorldSpaceCameraPosition = CameraWorldPosition;

	float C = lerp(0.7f, 1.75f, (9.f - input.MipLevel) / 9.f);
	float4 Color = BlinnPhong(Param) * float4(C,C,C,1.f);
	return Color;
}
#endif