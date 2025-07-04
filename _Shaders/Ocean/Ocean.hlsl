#ifndef __OCEAN_HLSL__
#define __OCEAN_HLSL__

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

SamplerState			Linear_Wrap				: register(s0); // VS DS PS
Texture2DArray<float4>	WaterDisplacementMap	: register(t0); // DS
Texture2DArray<float4>	WaterNormalMap			: register(t1); // PS
Texture2DArray<float>	FoamGrid				: register(t2); // PS
TextureCube<float4>		SkyTexture				: register(t10); // PS

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
	const float3 DisplacementMapUV = float3(output.UV * DisplacementMapTiling, 0);

	//Read Texture
	const float3 TangentSpaceDisplacement =
		WaterDisplacementMap.SampleLevel(Linear_Wrap, DisplacementMapUV, output.LOD).rgb;
	const float Folding =
		saturate(1 - FoamGrid.SampleLevel(Linear_Wrap, DisplacementMapUV, output.LOD).r);


	float3 WorldSpaceDisplacement = TangentSpaceToLocalSpace(
		TangentSpaceDisplacement,
		// World좌표계 기준 TangentSpace의 Basis
		float3(1, 0, 0),  // Tanjent (Right)
		float3(0, 0, -1), // Bitanjent
		float3(0, 1, 0)   // Normal  (Outward)
	);

	output.Position.y   = WorldSpaceDisplacement.y * Scaler;
	output.Position.xz += WorldSpaceDisplacement.xz * Folding * DispScaler;

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
float4 PSMain(DS_OUTPUT input) : SV_TARGET
{
	// Set Constants
	const float DispScaler = 1 / DisplacementMapTiling;
	const float Scaler = HeightScaler * DispScaler;
	const float3 DisplacementMapUV = float3(input.UV * DisplacementMapTiling, 0);
	const float2 NoiseUV = input.UV * NoiseTiling;

	// Read Texture
	float3 TangentSpaceNormal =
		 WaterNormalMap.SampleLevel(Linear_Wrap, DisplacementMapUV, input.LOD).rgb;
	float  FoamAmount =
		 FoamGrid.SampleLevel(Linear_Wrap, DisplacementMapUV, 0);

	float3 WorldSpaceNormal = TangentSpaceToLocalSpace(
		TangentSpaceNormal * 2.f - 1.f,	float3x3(1, 0, 0, 0, 0, -1, 0, 1, 0)
	);


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
	// return float4(L_a, 1);
	// return float4(Fresnel.xxx, 1);
	// return float4(L_r, 1);
	// return float4(L_ss, 1);
	// return float4(L_s, 1);
	if (FoamAmount > 0.75f)
		return float4(FoamAmount.xxx, 1);
	else
		return float4(Color, 1);

    // const float Ambient = 0.1f;
    // const float Specular = lerp(0, 0.8f, GetSpecularCoef(RDotN));
    // const float Refraction = (1 - Specular);

    // float3 WaterColor = lerp(DeepWaterColor, ShallowWaterColor, exp(-Dist * 10));


	// float3 Diffuse = WaterColor * Refraction;
    // WaterColor = WaterColor * Ambient
    //            + EnvColor * Specular * LightColor.rgb
    //            + Diffuse * NDotL * LightColor.rgb;
	// float3 FoamColor = float3(FoamAmount.r, 0, 0) * LightColor.rgb;

	// if (FoamColor.r > 0.75f)
	// 	return float4(1, 0, 1, 1);
	// else
	// 	return float4(WaterColor, 1);
    // return float4(WaterColor + FoamColor, 1);


	// // Water Shading - PBR
	// const float3 UpWelling = float3(0.0f, 0.2f, 0.3f);
	// const float3 SkyColor = float3(0.69f, 0.84f, 1.f);
	// const float3 AirColor = float3(0.1f, 0.1f, 0.1f);
	// const float nSnell = 1.34f;
	// const float KDiffuse = 0.91f;

	// const float VDotN = abs(dot(nV, nN));
	// const float Theta_i = acos(VDotN);
	// const float SinTheta_t = sin(Theta_i) / nSnell;
	// const float Theat_t = asin(SinTheta_t);

	// float reflecity = 0.f;
	// if (Theta_i == 0.f)
	// {
	// 	reflecity = (nSnell - 1) / (nSnell + 1);
	// 	reflecity *= reflecity;
	// }
	// else
	// {
	// 	float fs = sin(Theat_t - Theta_i) / sin(Theat_t + Theta_i);
	// 	float ts = tan(Theat_t - Theta_i) / tan(Theat_t + Theta_i);
	// 	reflecity = 0.5f * (fs * fs + ts * ts);
	// }
	// float Distance = length(dPE) * KDiffuse;
	// Distance = exp(-Distance / 1000);

    // float3 WaterColor = lerp(DeepWaterColor, ShallowWaterColor, exp(-Dist * 10));
	// float3 Color = lerp(AirColor, reflecity * SkyColor + (1 - SkyColor) * WaterColor, Distance);
	// Color.r += FoamAmount.r;
	// return float4(Color, 1.f);
}

/*======================================================================================*/

#endif