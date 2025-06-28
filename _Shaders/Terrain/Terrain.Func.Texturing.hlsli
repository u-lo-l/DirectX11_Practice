#ifndef __TERRAIN_FUNC_TEXTURING_HLSLI__
#define __TERRAIN_FUNC_TEXTURING_HLSLI__

const static float GRASS = 0.f;
const static float DIRT  = 1.f;
const static float ROCK  = 2.f;
const static float SAND  = 3.f;

float3 DistanceBasedColor(uint TextureIndex, float Tiling, float2 UV, float LOD, float Weight);
float3 DistanceBasedNormal(uint TextureIndex, float Tiling, float2 UV, float LOD, float Weight);
float4 CalculateWorldAlignedWeight(float3 Normal, float Height);
float3 CalculateDetailedNormal(float3x3 TNBMatrix, float3 DetailNormal);
void ApplyMacroVaration(inout float3 TerrainColor, float2 UV, float LOD);

struct DistanceBasedParams
{
	float NearSize;
	float FarSize;
	uint TextureIndex;
	float2 Tiling;
	float2 UV;
	float LOD;
	float Weight;
};

float3 DistanceBased
(
	Texture2DArray<float4> Texture,
	SamplerState Sampler,
	DistanceBasedParams Param
)
{
	const float2 NearUV = (Param.UV * Param.Tiling.x * Param.NearSize);
	const float2 FarUV = (Param.UV * Param.Tiling.y * Param.FarSize);
    float3 NearValue = Texture.SampleLevel(Sampler, float3(NearUV, Param.TextureIndex), Param.LOD).rgb;
    float3 FarValue  = Texture.SampleLevel(Sampler, float3(FarUV,  Param.TextureIndex), Param.LOD).rgb;
    return lerp(NearValue, FarValue, Param.Weight);
}

float4 GetWorldAlignedWeight
(
	float  SlopBias,
	float  SlopSharpness,
	float  LowHeight,
	float  HighHeight,
	float  HeightSharpness,
	float3 Normal,
	float  Height
)
{
    // SlopBased;
    float Slop = dot(Normal, float3(0, 1, 0));
    float SlopAmount = saturate((Slop - cos(radians(SlopBias))) / (1 - cos(radians(SlopBias))));
    float SlopeBasedWeight = pow(SlopAmount, SlopSharpness);
    // AltitudeBased;

    float AltitudeAmount = saturate((Height - LowHeight) / (HighHeight - LowHeight));
    float AltitudeWeight = pow(AltitudeAmount, HeightSharpness);

    float4 Weight;
    Weight.x = 0; // Sand;
    Weight.y = 0; // Ground;
    Weight.z = 0; // Rock;
    Weight.w = 1; //None;

    Weight.z = 1 - SlopeBasedWeight;
    Weight.x = (1 - Weight.z) * (1 - AltitudeWeight);
    Weight.y = (1 - Weight.z) * (AltitudeWeight);
    return Weight;
}

float3 MacroVariation
(
	in Texture2D<float> InVariationTexture,
	in SamplerState Sampler,
	float3 TerrainColor,
	float2 UV,
	float LOD
)
{
    float Tiling[3] = {0.2134, 0.0534, 0.002};
    float Variation[3];
    [unroll]
    for (uint i = 0 ; i < 3 ; i++)
    {
        Variation[i] = (InVariationTexture.SampleLevel(Sampler, UV * Tiling[i], LOD));
    }
    float MacroViration;
    MacroViration = Variation[0] + (Variation[1] * Variation[2]);
    TerrainColor *= lerp(0.5f, 0.9f, (MacroViration));
	return TerrainColor;
}

float3x3 GetTerrainColorMatrix
(
	Texture2DArray DiffuseTextures,
	SamplerState Sampler,
	float Noise,
	DistanceBasedParams Param
)
{
	Param.TextureIndex = GRASS;
	float3 GrassColor = DistanceBased(DiffuseTextures, Sampler, Param);

	Param.TextureIndex = DIRT;
    float3 DirtColor =  DistanceBased(DiffuseTextures, Sampler, Param);

	Param.TextureIndex = ROCK;
    float3 RockColor = DistanceBased(DiffuseTextures, Sampler, Param);

	Param.TextureIndex = GRASS;
    float3 SandColor = DistanceBased(DiffuseTextures, Sampler, Param);

	float3 GroundColor = lerp(GrassColor, DirtColor, clamp(0, 1, Noise));

	return float3x3(SandColor, GroundColor, RockColor);
}

float3x3 GetTerrainNormalMatrix
(
	in Texture2DArray NormalTextures,
	in SamplerState Sampler,
	float Noise,
	DistanceBasedParams Param
)
{
	Param.TextureIndex = GRASS;
	float3 GrassNormal = DistanceBased(NormalTextures, Sampler, Param) * 2.f - 1.f;
	Param.TextureIndex = DIRT;
	float3 DirtNormal = DistanceBased(NormalTextures, Sampler, Param) * 2.f - 1.f;

	Param.TextureIndex = ROCK;
	float3 RockNormal = DistanceBased(NormalTextures, Sampler, Param) * 2.f - 1.f;

	Param.TextureIndex = GRASS;
	float3 SandNormal = DistanceBased(NormalTextures, Sampler, Param) * 2.f - 1.f;

	float3 GroundNormal = lerp(GrassNormal, DirtNormal, clamp(0, 1, Noise));
	return float3x3(SandNormal, GroundNormal, RockNormal);
}

#endif