#ifndef __Ocean_HLSL__
#define __Ocean_HLSL__

#define TYPE01

#include "../PerFrame.hlsli"
#include "../Normal.Func.hlsli"
#include "../Shading.Func.hlsli"
#include "../Terrain/Terrain.Parameter.hlsli"
#include "../Terrain/Terrain.Func.Tesellation.hlsli"
#include "../Terrain/Terrain.Func.Texturing.hlsli"

# define DOMAIN "quad"
# define HS_PARTITION "integer"
# define HS_INPUT_PATCH_SIZE 4
# define HS_OUTPUT_PATCH_SIZE 4

# define USE_DISTANCE_BASED_BLENDING

# define NEAR_DISTANCE (500)
# define FAR_DISTANCE (2500)

const static float MIPMIN = 0;
const static float MIPMAX = 5;
const static float WaterRefractionIndex = 1.33f; // 굴절률
const static float WaterR0 = 0.02f;              // 수직 입사 반사 계수

#define GET_MIP_LEVEL(x) (lerp(MIPMIN, MIPMAX, (x)))


cbuffer CB_PerMaterial : register(b1) // PS
{
	float  DisplacementMapTiling = 1.f;
	float  NoiseTiling = 1.f;
	float2 TextureSize;
}

cbuffer CB_PerRenderable : register(b2) // DS HS
{
	float  HeightScaler = 100.f;
	float  GridSize;
	float2 LODRange;
}

SamplerState		Linear_Wrap				: register(s0); // VS DS PS
Texture2D<float4>	WaterDisplacementMap	: register(t0); // DS
Texture2D<float4>	WaterNormalMap			: register(t1); // PS
Texture2D<float>	FoamGrid				: register(t2); // PS

// TextureCube		SkyTexture			: register(t3); // PS
// Texture2D<float> PerlinNoise : register(t5);         // VS DS PS

// const float GetPerlinRandom(float2 uv, uint LOD = 0)
// {
//     uv /= HeightMapTiling * 4;

//     float Random = PerlinNoise.SampleLevel(Linear_Wrap, uv, LOD); // 0 ~ 1
//     return Random;
// }

// struct DS_OUTPUT // PS_INPUT
// {
//     float4 Position : SV_Position;
//     float2 UV       : UV;
//     float3 Normal : NORMAL;

//     float3 WorldPosition : POSITION;
//     float  LOD : LOD;
//     float  PerlinBlending : BLENDING;
// };

float3 CalculateNormal(float2 UV, uint LOD, float DistanceBlend);

// VS
VS_OUTPUT VSMain(VS_INPUT input)
{
	VS_OUTPUT output;

	output.UV = input.CellTexCoord + input.UV;
	output.Transform = input.Transform;
	output.Position = mul(input.Position, input.Transform);

	return output;
}

// HS
HS_CONSTANT_OUTPUT HSConstant
(
    InputPatch<VS_OUTPUT, HS_INPUT_PATCH_SIZE> patch,
    uint PatchID : SV_PrimitiveID
)
{
    HS_CONSTANT_OUTPUT output;

	float4 Points[4]; // Camera-Space Positions
	bool bVisible = false;
	[unroll]
	for (int i = 0 ; i < 4 ; i++)
	{
		Points[i] = mul(patch[i].Position, View);
		[flatten]
		if(Points[i].z >= 0)
			bVisible = true;
	}
	[flatten]
	if (bVisible == false)
	{
		output.Edge[0] = output.Edge[1] = output.Edge[2] = output.Edge[3] = output.Inside[0] = output.Inside[1] = 0;
		return output;
	}


	[unroll]
	for (int j = 0 ; j < 4 ; j++)
	{
		float4 Point1 = Points[((j - 1) + 4) % 4];
		float4 Point2 = Points[j];
		float TessRatio = CalculateTessellationFactor(
			Point1, Point2,
			ScreenDistance, ScreenDiagonal, RDRatio,
			LODRange
		);
		output.Edge[j] = lerp(MinTessFactor, MaxTessFactor, TessRatio);
	}

    float TessFactor = (output.Edge[0] + output.Edge[2]) * 0.5f;
    output.Inside[0] = TessFactor;
    TessFactor = (output.Edge[1] + output.Edge[3]) * 0.5f;
    output.Inside[1] = TessFactor;
    return output;
}

[domain(DOMAIN)]
[partitioning(HS_PARTITION)]
[outputtopology("triangle_cw")]
[outputcontrolpoints(HS_OUTPUT_PATCH_SIZE)]
[patchconstantfunc("HSConstant")]
HS_POINT_OUTPUT HSMain
(
    InputPatch<VS_OUTPUT, HS_INPUT_PATCH_SIZE> patch,
    uint id : SV_OutputControlPointID
)
{
	// Pass Through
	HS_POINT_OUTPUT output;
	output.WorldPosition = patch[id].Position;
	output.UV = patch[id].UV;
	return output;
}

// DS
[domain(DOMAIN)]
DS_OUTPUT DSMain
(
    HS_CONSTANT_OUTPUT input,
    float2 UV : SV_DomainLocation,
    const OutputPatch<HS_POINT_OUTPUT, HS_OUTPUT_PATCH_SIZE> patch
)
{
	DS_OUTPUT output;
	float MeanTessFactor = (input.Inside[0] + input.Inside[1]) * 0.5f;
	output.LOD = (uint)(lerp(5, 0, MeanTessFactor / MaxTessFactor));

	float4 v1 = lerp(patch[0].WorldPosition, patch[1].WorldPosition, UV.x);
	float4 v2 = lerp(patch[3].WorldPosition, patch[2].WorldPosition, UV.x);
	output.Position = lerp(v1, v2, UV.y);

	float2 u1 = lerp(patch[0].UV, patch[1].UV, UV.x);
	float2 u2 = lerp(patch[3].UV, patch[2].UV, UV.x);
	output.UV = lerp(u1, u2, UV.y);

	const float2 DisplacementMapUV = output.UV * DisplacementMapTiling;
	const float Scaler = HeightScaler / DisplacementMapTiling * 10;

	float3 Displacement = WaterDisplacementMap.SampleLevel(Linear_Wrap, DisplacementMapUV, 0).rgb * 2.f - 1.f;
	const float Folding = abs(FoamGrid.SampleLevel(Linear_Wrap, DisplacementMapUV, 0)).r;
	output.Position.y = Displacement.y * Scaler;
	output.Position.xz += Displacement.xz * Scaler * (1 - Folding);

	output.WorldPosition = output.Position.xyz;
	output.Position = mul(output.Position, View);
	output.CameraDistance = length(output.Position);
	output.Position = mul(output.Position, Projection);


	float T = (input.Inside[0] + input.Inside[1]) * 0.5f;
	output.DebugColor = float4(T, T, T, 1);
	output.DebugColor /= MaxTessFactor;
	return output;
}

// PS
float GetSpecularCoef(float VDotL);
float3 FogBlending(float3 Color, float Dist)
{
    float blend = saturate((Dist - 1000) / (5000 - 1000));
    const float3 FogColor = 0.8f;
    return lerp(Color, FogColor, blend);
}

float4 PSMain(DS_OUTPUT input) : SV_TARGET
{
	const float2 DisplacementMapUV = input.UV * DisplacementMapTiling;
	const float2 NoiseUV = input.UV * NoiseTiling;
	const float3 TangentSpaceNormal = WaterNormalMap.Sample(Linear_Wrap, DisplacementMapUV).rgb * 2.f - 1.f;
	const float3 FoamColor = FoamGrid.Sample(Linear_Wrap, DisplacementMapUV).rrr;

	float3 ViewRay = (input.WorldPosition - CameraWorldPosition); // WorldSpace
	const float Distance = length(ViewRay);
	float DistanceBasedBlending = saturate((Distance - NEAR_DISTANCE) / (FAR_DISTANCE - NEAR_DISTANCE));
	// float DistanceBasedBlending = 0;
	ViewRay = normalize(ViewRay);


	float3 Normal = ApplyNormalMap(TangentSpaceNormal, float3(0, 1, 0), float3(1, 0 ,0));
	Normal = lerp(Normal, float3(0, 1, 0), 0);

	const float3 ShallowWaterColor = float3(0.7f, 0.85f, 0.8f);
	const float3 DeepWaterColor = float3(0.0f, 0.2f, 0.3f);
	float3 WaterColor = lerp(ShallowWaterColor, DeepWaterColor, DistanceBasedBlending);

	WaterColor += FoamColor;

	const float3 EnvColor = float3(0.5f, 0.5f, 1.f);
	float3 ReflectedRay = reflect(ViewRay, Normal); // WorldSpace
	float RDotN = dot(ReflectedRay, Normal);
	float Specular = lerp(0, 0.8f, GetSpecularCoef(RDotN));

	BlinnPhongInput	BlinnPhongParam;
	BlinnPhongParam.Ambient = float4(0.2f, 0.2f, 0.2f, 1.f);
	BlinnPhongParam.Diffuse = float4((1 - Specular) * WaterColor, 1);
	BlinnPhongParam.Specular = float4(EnvColor * Specular, 1);
	BlinnPhongParam.Shininess = 0.f;

	BlinnPhongParam.LightColor = LightColor;
	BlinnPhongParam.LightDirection = LightDirection;

	BlinnPhongParam.Normal = Normal;
	BlinnPhongParam.WorldPosition = input.WorldPosition;
	BlinnPhongParam.WorldSpaceCameraPosition = CameraWorldPosition;

	float4 Result = BlinnPhong(BlinnPhongParam);
	return Result;
}

/*======================================================================================*/

// Fresnel
float GetSpecularCoef(float VDotN)
{
    VDotN = max(0, VDotN);
    return (WaterR0 + (1 - WaterR0) * pow(1 - VDotN, 5));
}

#endif