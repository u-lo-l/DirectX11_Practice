#ifndef __TERRAINCELL_INSTANCE_HLSL__
#define __TERRAINCELL_INSTANCE_HLSL__

#include "../PerFrame.hlsli"
#include "../Normal.Func.hlsli"
#include "../Shading.Func.hlsli"
#include "./Terrain.Parameter.hlsli"
#include "./Terrain.Func.Tesellation.hlsli"
#include "./Terrain.Func.Texturing.hlsli"

SamplerState LinearSampler_Wrap			: register(s0); // VS DS PS
SamplerState LinearSampler_Clamp		: register(s1); // VS DS PS

Texture2D<float>  TerrainHeightMap		: register(t0);	// VS DS
Texture2D<float4> TerrainNormalMap		: register(t1);	// PS
Texture2D<float4> TerrainTangentMap		: register(t2);	// PS

Texture2D<float> MacroVariationMap		: register(t3);	// 3-Textures PS
Texture2D<float> PerlinNoise			: register(t4); // PS
Texture2DArray<float4> DetailDiffuses	: register(t5);	// 4-Textures PS
Texture2DArray<float4> DetailNormals	: register(t6);	// 4-Textures PS


cbuffer CB_PerMaterial : register(b1) // PS
{
	float NearSize = 1.0f;
	float FarSize = 0.1f;
	float StartOffset = -1000.f;
	float Range = 5000.f;

	// Perlin Noise
	float NoiseAmount = 1.f;
	float NoisePower  = 1.f;
	// Slope Based
	float SlopBias = 60;
	float SlopSharpness = 3;

	// Altitude Base
	float LowHeight = 5;
	float HighHeight = 15;
	float HeightSharpness = 1;
	// Padding
	uint TextureUsageFlags;
}

cbuffer CB_PerRenderable : register(b2) // DS HS
{
	matrix BaseWorldTF;

	float  HeightScaler = 100.f;
	float  GridSize;
	float2 Padding_PerRenderable;

	float2 TerrainSize;
	float2 LODRange;
}


VS_OUTPUT VSMain(VS_INPUT input)
{
	VS_OUTPUT output;

	output.UV = input.CellTexCoord + input.UV;
	output.Transform = input.Transform;

	output.Position = input.Position;
	float Height = TerrainHeightMap.SampleLevel(LinearSampler_Clamp, output.UV, 0).r;
	output.Position.y = Height * HeightScaler;
	output.Position = mul(output.Position, input.Transform);

	return output;
}


const static float SSDWeight = 1.f;
const static float DensityWeight = 0.f;
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


	float DensityFactor = CalculateDensity(
		patch[0].Position.xyz, patch[1].Position.xyz, patch[2].Position.xyz, patch[3].Position.xyz
	);
    float Density = lerp(MinTessFactor, MaxTessFactor, DensityFactor);


    float TessFactor = (output.Edge[0] + output.Edge[2]) * 0.5f;
    output.Inside[0] = Density * DensityWeight + TessFactor * SSDWeight;
    TessFactor = (output.Edge[1] + output.Edge[3]) * 0.5f;
    output.Inside[1] = Density * DensityWeight + TessFactor * SSDWeight;

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

	float4 v1 = lerp(patch[0].WorldPosition, patch[1].WorldPosition, UV.x);
	float4 v2 = lerp(patch[3].WorldPosition, patch[2].WorldPosition, UV.x);
	output.Position = lerp(v1, v2, UV.y);

	float2 u1 = lerp(patch[0].UV, patch[1].UV, UV.x);
	float2 u2 = lerp(patch[3].UV, patch[2].UV, UV.x);
	output.UV = lerp(u1, u2, UV.y);

	// float Height = TerrainHeightMap[output.UV].r;
	float Height = TerrainHeightMap.SampleLevel(LinearSampler_Clamp, output.UV, 0).r;
	output.Position.y = Height * HeightScaler;

	output.WorldPosition = output.Position.xyz;
	output.Position = mul(output.Position, View);
	output.CameraDistance = length(output.Position);
	output.Position = mul(output.Position, Projection);

	float MeanTessFactor = (input.Inside[0] + input.Inside[1]) * 0.5f;
	output.LOD = (uint)(lerp(5, 0, MeanTessFactor / MaxTessFactor));

	float T = (input.Inside[0] + input.Inside[1]) * 0.5f;
	output.DebugColor = float4(T, T, T, 1);
	output.DebugColor /= MaxTessFactor;
	return output;
}

float4 PSMain(DS_OUTPUT Input) : SV_TARGET
{
	float3 TangentSpaceNormal  = TerrainNormalMap.Sample(LinearSampler_Clamp, Input.UV).rgb;
	TangentSpaceNormal = TangentSpaceNormal * 2.f - 1.f;
	float3 TangentSpaceTangent = TerrainTangentMap.Sample(LinearSampler_Clamp, Input.UV).rgb;
	TangentSpaceTangent = TangentSpaceTangent * 2.f - 1.f;
	float3 Normal = ApplyNormalMap(TangentSpaceNormal, float3(0, 1, 0), float3(1, 0, 0));
	float3 Tangent = ApplyNormalMap(TangentSpaceTangent, float3(0, 1, 0), float3(1, 0, 0));

	const float RawHeight = TerrainHeightMap.Sample(LinearSampler_Wrap, Input.UV).r;
	const float Height = HeightScaler * RawHeight;
	float2 Tiling = (TerrainSize / GridSize);

	uint TexWidth[2], TexHeight[2], NumOfLevels;
	TerrainHeightMap.GetDimensions(0, TexWidth[0], TexHeight[0], NumOfLevels);
	PerlinNoise.GetDimensions(0, TexWidth[1], TexHeight[1], NumOfLevels);

	// Distance Based
	float LinearBlendFactor = saturate((Input.CameraDistance + StartOffset) / Range);
	float SmoothedBlendFactor = smoothstep(0.0f, 1.0f, LinearBlendFactor);
	DistanceBasedParams Param;
	Param.NearSize = NearSize;
	Param.FarSize = FarSize;
	Param.TextureIndex = -1;
	Param.Tiling = Tiling;
	Param.UV = Input.UV;
	Param.LOD = Input.LOD;
	Param.Weight = SmoothedBlendFactor;

	// Perlin Noise Based
	float2 NoiseTiling = float2(TexWidth[0] / TexWidth[1], TexHeight[0] / TexHeight[1]);
	float Noise = clamp(0, 1, PerlinNoise.Sample(LinearSampler_Wrap, Input.UV * NoiseTiling).r);
	Noise = pow(abs(Noise * NoiseAmount), abs(NoisePower));

	float3x3 ColorMat = GetTerrainColorMatrix(DetailDiffuses, LinearSampler_Wrap, Noise, Param);
	float3x3 NormalMat = GetTerrainNormalMatrix(DetailNormals, LinearSampler_Wrap, Noise, Param);

	// World Based
	// x : Ground, y : Sand : z : Rock
	float4 WorldAlignedWeight = GetWorldAlignedWeight(
		SlopBias, SlopSharpness,
		LowHeight, HighHeight, HeightSharpness,
		Normal, Height
	);
	float3 DetailedColor = mul(WorldAlignedWeight.xyz, ColorMat);
	float3 DetailedNormal = mul(WorldAlignedWeight.xyz, NormalMat);

	// Macro Variation
	DetailedColor = MacroVariation(
		MacroVariationMap,
		LinearSampler_Wrap,
		DetailedColor,
		Input.UV,
		Input.LOD
	);
	DetailedNormal = MacroVariation(
		MacroVariationMap,
		LinearSampler_Wrap,
		DetailedNormal,
		Input.UV,
		Input.LOD
	);

	DetailedNormal = ApplyNormalMap(normalize(DetailedNormal), Normal, Tangent);
	float LDotN = dot(-normalize(LightDirection), DetailedNormal);

	// Final Shading
	BlinnPhongInput BlinnPhongParam;
	BlinnPhongParam.Ambient = float4(0.2f, 0.2f, 0.2f, 1.f);
	BlinnPhongParam.Diffuse = float4(DetailedColor, 1);
	BlinnPhongParam.Specular = float4(0.0f, 0.0f, 0.0f, 1);
	BlinnPhongParam.Shininess = 0.f;

	BlinnPhongParam.LightColor = LightColor;
	BlinnPhongParam.LightDirection = LightDirection;

	BlinnPhongParam.Normal = DetailedNormal;
	BlinnPhongParam.WorldPosition = Input.WorldPosition;
	BlinnPhongParam.WorldSpaceCameraPosition = CameraWorldPosition;

	float4 Color = BlinnPhong(BlinnPhongParam);
	return Color;


	// float4 Ambient, Diffuse, Specular;
	// BlinnPhong(BlinnPhongParam, Ambient, Diffuse, Specular);
	// return float4(RawHeight, RawHeight, RawHeight, 1);
	// return float4((DetailedNormal + 1) * 0.5f, 1);
	// return float4(Diffuse.rgb, 1);
	// return float4(Specular.rgb, 1);
	// return float4(Ambient.rgb, 1);
	// return float4(Noise, Noise, Noise , 1);
	// return float4(ColorMat[0], 1);
	// return float4(ColorMat[1], 1);
	// return float4(ColorMat[2], 1);
}
#endif