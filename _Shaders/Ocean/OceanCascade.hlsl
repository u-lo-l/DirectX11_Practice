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
#include "./Ocean.Common.hlsli"

# define DOMAIN "quad"
# define HS_PARTITION "integer"
# define HS_INPUT_PATCH_SIZE 4
# define HS_OUTPUT_PATCH_SIZE 4

struct CascadeDesc{
	float Weight;
	float UVScale;
	float DisplacementScale;
	float FoamScale;
};
cbuffer CB_PerMaterial : register(b1) // DS PS
{
	float  DisplacementMapTiling = 1.f;
	float  NoiseTiling = 1.f;
	float2 TextureSize;

	CascadeDesc CascadeData[3];

	OceanShadingDesc ShadingParam;
};
cbuffer CB_PerRenderable : register(b2) // DS HS
{
	float  HeightScaler = 100.f;
	float  GridSize;
	float2 LODRange;
}

SamplerState			Linear_Wrap				: register(s0); // VS DS PS
Texture2DArray<float4>	WaterDisplacementMap	: register(t0); // DS -> ArraySize : 3
Texture2DArray<float4>	WaterNormalMap			: register(t1); // PS -> ArraySize : 3
Texture2DArray<float>	FoamGrid				: register(t2); // DS PS -> ArraySize : 3
TextureCube<float4>		SkyTexture				: register(t10); // PS

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
	output.LOD = 0;

	const float HorizontalScaler = GetHorizontalScaler(DisplacementMapTiling);
	const float VerticalScaler = GetVerticalScaler(HeightScaler, DisplacementMapTiling);
	const float2 DisplacementMapUV = output.UV * DisplacementMapTiling;

	const float3x3 TBN = float3x3(1, 0, 0, 0, 0, -1 , 0, 1, 0);
	float3 FinalDisplacement = 0;
	[unroll] for(int i = 0 ; i < 3 ; i++)
	{
		float3 Texcoord = float3(DisplacementMapUV * CascadeData[i].UVScale, i);

		float Foam = FoamGrid.SampleLevel(Linear_Wrap, Texcoord, 0).r * CascadeData[i].FoamScale;
		Foam *= CascadeData[i].Weight;

		float3 Displacement = WaterDisplacementMap.SampleLevel(Linear_Wrap, Texcoord, 0).rgb;
		Displacement = TangentSpaceToLocalSpace(Displacement, TBN) * CascadeData[i].DisplacementScale;
		Displacement *= CascadeData[i].Weight;

		FinalDisplacement.y += Displacement.y;
		FinalDisplacement.xz += Displacement.xz * (1 - Foam);
	}
	// [flatten]
	// if ((CascadeData[0].Weight + CascadeData[1].Weight + CascadeData[2].Weight) != 0)
	// 	FinalDisplacement /= (CascadeData[0].Weight + CascadeData[1].Weight + CascadeData[2].Weight);

	output.Position.y = FinalDisplacement.y * VerticalScaler;
	output.Position.xz += FinalDisplacement.xz * HorizontalScaler;

	output.WorldPosition = output.Position.xyz;
	output.Position = mul(output.Position, View);
	output.CameraDistance = length(output.Position);
	output.Position = mul(output.Position, Projection);

	return output;
}

// PS
float4 PSMain(DS_OUTPUT input) : SV_TARGET
{
	// Set Constants
	const float DispScaler = 1 / DisplacementMapTiling * 2;
	const float2 DisplacementMapUV = input.UV * DisplacementMapTiling;
	const float2 NoiseUV = input.UV * NoiseTiling;

	// Read Texture
	float  FoamAmount = 0.f;
	float3 WorldSpaceNormal = 0;
	float3 FinalDisplacement = 0;
	const float3x3 TBN = float3x3(1, 0, 0, 0, 0, -1 , 0, 1, 0);
	[unroll] for(int i = 0 ; i < 3 ; i++)
	{
		float3 Texcoord = float3(DisplacementMapUV * CascadeData[i].UVScale, i);
		float3 Normal = WaterNormalMap.SampleLevel(Linear_Wrap, Texcoord, 0).rgb;
		Normal = TangentSpaceToLocalSpace(Normal * 2.f - 1.f, TBN).rgb;
		Normal = lerp(float3(0, 1, 0), Normal, CascadeData[i].DisplacementScale);
		WorldSpaceNormal += Normal * CascadeData[i].Weight;

		float Foam = FoamGrid.SampleLevel(Linear_Wrap, Texcoord, 0) * CascadeData[i].FoamScale;
		FoamAmount += Foam * CascadeData[i].Weight;
	}
	// [flatten]
	// if ((CascadeData[0].Weight + CascadeData[1].Weight + CascadeData[2].Weight) != 0)
	// {
	// 	WorldSpaceNormal /= (CascadeData[0].Weight + CascadeData[1].Weight + CascadeData[2].Weight);
	// 	FoamAmount /= (CascadeData[0].Weight + CascadeData[1].Weight + CascadeData[2].Weight);
	// }

	WorldSpaceNormal = normalize(WorldSpaceNormal);
	// Shading Constants
	const float3 P = input.WorldPosition;
	const float3 E = CameraWorldPosition;
	const float3 dPE = P - E;
	const float  Dist = length(dPE) / 1000; // unit : km
	const float3 nN = normalize(WorldSpaceNormal);
	const float3 nL = normalize(-LightDirection);
	const float3 nV = normalize(dPE);
	const float3 nR = reflect(nV, nN);
	const float  NDotL = saturate(dot(nL, nN));
	// Asume Distance to Sky : Infinity
	float3 EnvColor = SkyTexture.Sample(Linear_Wrap, reflect(nV, nN)).rgb;
	const float OceanHeight = input.WorldPosition.y;
	float3 La, Ls, Lss, Lr;
	float F, S;
	float3 Color = OceanShading(
		La, Ls, Lss, Lr, F, S,
 		nN, nL, -nV, nR,
		LightColor.rgb, EnvColor, OceanHeight,
		ShadingParam
	);
	float3 FoamColor = FoamAmount.xxx;// * NDotL * LightColor.rgb;
	return FoamAmount.x > 0.75 ? float4(FoamColor.xxx, 1) : float4(Color + FoamColor.xxx, 1);
}

/*======================================================================================*/

#endif