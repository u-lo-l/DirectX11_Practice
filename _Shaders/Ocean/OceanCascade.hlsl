#ifndef __OCEAN_CASCADE_HLSL__
#define __OCEAN_CASCADE_HLSL__

#define TYPE01

#include "../PerFrame.hlsli"
#include "../Normal.Func.hlsli"
#include "../Shading.Func.hlsli"
#include "../Texture.Func.hlsli"
#include "../Terrain/Terrain.Parameter.hlsli"
#include "../Terrain/Terrain.Func.Tesellation.hlsli"
#include "../Terrain/Terrain.Func.Texturing.hlsli"
#include "./OceanShading.hlsli"

# define DOMAIN "quad"
# define HS_PARTITION "integer"
# define HS_INPUT_PATCH_SIZE 4
# define HS_OUTPUT_PATCH_SIZE 4

# define USE_DISTANCE_BASED_BLENDING

# define NEAR_DISTANCE (500)
# define FAR_DISTANCE (2500)

static const float		CascadeLengthScaler[3] = { 0.5f, 3.f, 3.f};

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

Texture2DArray<float4>	WaterDisplacementMap	: register(t0); // DS -> ArraySize : 3
Texture2DArray<float4>	WaterNormalMap			: register(t1); // PS -> ArraySize : 3
Texture2DArray<float>	FoamGrid				: register(t2); // DS PS -> ArraySize : 3

TextureCube<float4>	SkyTexture				: register(t10); // PS

// Texture2D<float> PerlinNoise : register(t5);         // VS DS PS

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

	// Process Input
	float2 u1 = lerp(patch[0].UV, patch[1].UV, UV.x);
	float2 u2 = lerp(patch[3].UV, patch[2].UV, UV.x);
	output.UV = lerp(u1, u2, UV.y);

	float4 v1 = lerp(patch[0].WorldPosition, patch[1].WorldPosition, UV.x);
	float4 v2 = lerp(patch[3].WorldPosition, patch[2].WorldPosition, UV.x);
	output.Position = lerp(v1, v2, UV.y);

	float MeanTessFactor = (input.Inside[0] + input.Inside[1]) * 0.5f;
	output.LOD = (uint)(lerp(5, 0, MeanTessFactor / MaxTessFactor));

	//Set Constants
	const float DispScaler = 1 / DisplacementMapTiling;
	const float Scaler = HeightScaler * DispScaler;
	const float2 DisplacementMapUV = output.UV * DisplacementMapTiling;

	float3 CascadingUV[3] = {
		float3(DisplacementMapUV * CascadeLengthScaler[0], 0),
		float3(DisplacementMapUV * CascadeLengthScaler[1], 1),
		float3(DisplacementMapUV * CascadeLengthScaler[2], 2),
	};
	//Read Texture
	const float3 TangentSpaceDisplacement[3] = {
		WaterDisplacementMap.SampleLevel(Linear_Wrap, CascadingUV[0], output.LOD).rgb,
		WaterDisplacementMap.SampleLevel(Linear_Wrap, CascadingUV[1], output.LOD).rgb,
		WaterDisplacementMap.SampleLevel(Linear_Wrap, CascadingUV[2], output.LOD).rgb
	};
	const float Folding[3] = {
		saturate(1 - FoamGrid.SampleLevel(Linear_Wrap, CascadingUV[0], output.LOD).r),
		saturate(1 - FoamGrid.SampleLevel(Linear_Wrap, CascadingUV[1], output.LOD).r),
		saturate(1 - FoamGrid.SampleLevel(Linear_Wrap, CascadingUV[2], output.LOD).r)
	};

	const float3x3 TBN = float3x3(1, 0, 0, 0, 0, -1 , 0, 1, 0);
	float3 WorldSpaceDisplacement[3] = {
		TangentSpaceToLocalSpace(TangentSpaceDisplacement[0],TBN),
		TangentSpaceToLocalSpace(TangentSpaceDisplacement[1],TBN),
		TangentSpaceToLocalSpace(TangentSpaceDisplacement[2],TBN)
	};

	output.Position.y = 0;
	[unroll] for(int i = 0 ; i < 3 ; i++)
	{
		output.Position.y += WorldSpaceDisplacement[i].y * Scaler;
		output.Position.xz += WorldSpaceDisplacement[i].xz * Folding[i] * 10 * DispScaler;
	}

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

float4 PSMain(DS_OUTPUT input) : SV_TARGET
{
	// Set Constants
	const float DispScaler = 1 / DisplacementMapTiling;
	const float Scaler = HeightScaler * DispScaler;
	const float2 DisplacementMapUV = input.UV * DisplacementMapTiling;
	const float2 NoiseUV = input.UV * NoiseTiling;

	float3 CascadingUV[3] = {
		float3(DisplacementMapUV * CascadeLengthScaler[0], 0),
		float3(DisplacementMapUV * CascadeLengthScaler[1], 1),
		float3(DisplacementMapUV * CascadeLengthScaler[2], 2),
	};

	float  FoamAmount = 0.f;
	float3 WorldSpaceNormal = 0;
	const float3x3 TBN = float3x3(1, 0, 0, 0, 0, -1 , 0, 1, 0);
	[unroll] for(int i = 0 ; i < 3 ; i++)
	{
		float3 TangentSpaceNormal = WaterNormalMap.SampleLevel(Linear_Wrap, CascadingUV[i], input.LOD).rgb;
		WorldSpaceNormal += TangentSpaceToLocalSpace(TangentSpaceNormal * 2.f - 1.f, TBN).rgb;
		FoamAmount += FoamGrid.SampleLevel(Linear_Wrap, CascadingUV[i], 0);
	}
	FoamAmount /= 3;
	WorldSpaceNormal = normalize(WorldSpaceNormal);
	// Shading Constants
	const float3 P = input.WorldPosition;
	const float3 E = CameraWorldPosition;
	const float3 dPE = P - E;
	const float  Dist = length(dPE) / 1000;
	const float3 nL = normalize(-LightDirection);
	const float3 nN = normalize(WorldSpaceNormal);
	const float3 nV = normalize(dPE);
	const float NDotL = saturate(dot(nL, nN));

    float3 ShallowWaterColor = float3(0.7f, 0.85f, 0.8f);
    float3 DeepWaterColor = float3(0.0f, 0.2f, 0.3f);


    // Asume Distance to Sky : Infinity
    float3 nR = reflect(-nL, nN); // WorldSpace
    // float RDotN = dot(nR, nN);
    float3 EnvColor = SkyTexture.Sample(Linear_Wrap, reflect(nV, nN)).rgb;

	const float3 AirBubbleColor = float3(0.35, 0.51, 0.69);
	const float3 SpecularColor = float3(1,1,1);
	const float3 WaterScatteringColor = DeepWaterColor;
	const float OceanHeight = input.WorldPosition.y;
	const float BubbleDensity = 0.2f;
	const float AmbientFactor = 0.2f;
	const float ScatterFactor1 = 0.05f;
	const float ScatterFactor2 = 0.5f;
	const float ReflectFactor = 1.f;
	float Fresnel;

	float3 L_a, L_ss, L_s, L_r;
	float3 Color = OceanShading(
		L_a, L_ss, L_s, L_r, Fresnel,
 		nN, nL, -nV, nR,
		LightColor.rgb, AirBubbleColor, SpecularColor, WaterScatteringColor, EnvColor,
		OceanHeight, BubbleDensity,
		AmbientFactor,
		ScatterFactor1,
		ScatterFactor2,
		ReflectFactor
	);
	return float4(FoamAmount.xxx, 1);
	return float4(Color + FoamAmount.xxx, 1);
}

/*======================================================================================*/

// Fresnel
float GetSpecularCoef(float VDotN)
{
    VDotN = max(0, VDotN);
    return (WaterR0 + (1 - WaterR0) * pow(1 - VDotN, 5));
}

#endif