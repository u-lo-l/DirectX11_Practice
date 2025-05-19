#ifndef __TERRAIN_TESSELLATION_HLSL__
#define __TERRAIN_TESSELLATION_HLSL__

# define DOMAIN "quad"
# define HS_PARTITION "integer"
// # define HS_PARTITION "fractional_even"
// # define HS_PARTITION "fractional_odd"
// # define HS_PARTITION "pow2"
# define HS_INPUT_PATCH_SIZE 4
# define HS_OUTPUT_PATCH_SIZE 4
# ifndef MAX_TESS_FACTOR
#  define MAX_TESS_FACTOR 64
# endif

const static float MinTessFactor = 1;
const static float MaxTessFactor = MAX_TESS_FACTOR;
const static float RDRatio = 4;

cbuffer CB_WVP : register(b0)  // VS DS HS PS
{
    matrix World      : packoffset(c0);
    matrix View       : packoffset(c4);
    matrix Projection : packoffset(c8);
}

cbuffer CB_HeightScaler : register(b1)
{
    float3 CameraPosition;
    float  HeightScaler = 30.f;

    float4 LightColor;

    float3 LightDirection;
    float  BumpScaler;

    float2 LODRange;
    float2 HeightMapTexelSize;

    float ScreenDistance;
    float ScreenDiagonal;
    uint  DiffuseCount;
    uint  NormalCount;

    float TerrainSize;
    float GridSize;
    float TextureSize;
}

cbuffer CB_DistanceBlend : register(b2)
{
    /*
       float BlendWeight = (CameraDistance + StartOffset) / Range
    */
    float NearSize;
    float FarSize;
    float StartOffset; //(음수)
    float Range;
    
    float NoiseAmount = 1;
    float NoisePower  = 1;

    /*
      float slope = dot(normal, float3(0, 1, 0)); -> normal.y;
      float t = saturate((slope - SlopBias) / (1.0 - SlopBias));
      float BlendWeight = pow(t, SlopSharpness);
    */
    float SlopBias;
    float SlopSharpness;

    /*
      float t = saturate((Height - LowHeight) / (HighHeight - LowHeight));
      float BlendWeight = pow(t, HeightSharpness);
    */
    float LowHeight;
    float HighHeight;
    float HeightSharpness;
    
    uint Padding;
}

SamplerState LinearSampler_Clamp : register(s0); // VS DS PS
SamplerState AnisotropicSampler_Wrap : register(s1); // VS DS PS

Texture2D<float> HeightMap : register(t0);        // VS DS PS
Texture2D<float> MacroVariationMap : register(t1); // 3-Textures
const static float GRASS = 0.f;
const static float DIRT  = 1.f;
const static float ROCK  = 2.f;
const static float SAND  = 3.f;
Texture2DArray<float4> DetailDiffuses : register(t2); // 4-Textures
Texture2DArray<float4> DetailNormals : register(t3);  // 4-Textures
Texture2D<float> PerlinNoise : register(t4);

struct VS_INPUT
{
    float4 Position : POSITION;
    float2 UV : UV;
    float3 Normal : NORMAL;
};

struct VS_OUTPUT // HS_INPUT
{
    float3 Position : POSITION;
    float2 UV : UV;
    float3 Normal : NORMAL;
};

struct HS_CONSTANT_OUTPUT
{
    float Edge[4] : SV_TessFactor;
    float Inside[2] : SV_InsideTessFactor;
};

struct HS_POINT_OUTPUT // DS_INPUT
{
    float3 Position : POSITION;
    float2 UV : UV;
    float3 Normal : NORMAL;
};

struct DS_OUTPUT // PS_INPUT
{
    float4 Position : SV_Position;
    float2 UV : UV;
    float3 Normal : NORMAL;

    uint   LOD : LOD;
    float  CameraDistance : DISTANCE;
};


// VS
VS_OUTPUT VSMain(VS_INPUT input)
{
    VS_OUTPUT output;
    output.Position = input.Position.xyz;
    output.UV = input.UV;
    output.Normal = input.Normal;

    float Height = HeightMap.SampleLevel(LinearSampler_Clamp, output.UV, 0).r;
    output.Position.y = Height  * HeightScaler;

    return output;
}

float CalculateTessellationFactor(float4 Point1, float4 Point2);
float CalculateDensity(InputPatch<VS_OUTPUT, HS_INPUT_PATCH_SIZE> patch);


const static float SSDWeight = 0.7f;
const static float DensityWeight = 0.3f;
// HS
HS_CONSTANT_OUTPUT HSConstant
(
    InputPatch<VS_OUTPUT, HS_INPUT_PATCH_SIZE> patch,
    uint PatchID : SV_PrimitiveID
)
{
    HS_CONSTANT_OUTPUT output;
    matrix WorldView = mul(World, View);
    float4 Points[4]; // Camera-Space Positions
    [unroll]
    for (int i = 0 ; i < 4 ; i++)
    {
        Points[i] = mul(float4(patch[i].Position, 1), WorldView);
    }

    // BackFace Culling을 시도해보았지만 성능이 더 안 좋음.
    float3 Diag1 = (Points[2] - Points[0]).xyz;
    float3 Diag2 = (Points[3] - Points[1]).xyz;
    float3 PatchNormal = normalize(cross(Diag1, Diag2));
    [flatten] if (PatchNormal.z > 0.75f)
    {
        [unroll]
        for (int i = 0; i < 4; ++i)
            output.Edge[i] = 0;

        [unroll]
        for (int j = 0; j < 2; ++j)
            output.Inside[j] = 0;

        return output;
    }

    [unroll]
    for (int j = 0 ; j < 4 ; j++)
    {
        float4 Point1 = Points[((j - 1) + 4) % 4];
        float4 Point2 = Points[j];
        float TessRatio = CalculateTessellationFactor(Point1, Point2);
        output.Edge[j] = lerp(MinTessFactor, MaxTessFactor, TessRatio);
    }

    float Density = lerp(MinTessFactor, MaxTessFactor, CalculateDensity(patch));
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

    output.Position = patch[id].Position;
    output.Normal = patch[id].Normal;
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

    float3 v1 = lerp(patch[0].Position, patch[1].Position, UV.x);
    float3 v2 = lerp(patch[3].Position, patch[2].Position, UV.x);
    output.Position = float4(lerp(v1, v2, UV.y), 1);

    float2 u1 = lerp(patch[0].UV, patch[1].UV, UV.x);
    float2 u2 = lerp(patch[3].UV, patch[2].UV, UV.x);
    output.UV = lerp(u1, u2, UV.y);

    output.Normal = float3(0,1,0);

    float Height = HeightMap.SampleLevel(LinearSampler_Clamp, output.UV, 0).r;
    output.Position.y = Height * HeightScaler;

    output.Position = mul(output.Position, World);
    output.Position = mul(output.Position, View);
    output.CameraDistance = length(output.Position);
    output.Position = mul(output.Position, Projection);

    output.Normal = mul(output.Normal, (float3x3)World);

    float MeanTessFactor = (input.Inside[0] + input.Inside[1]) * 0.5f;
    output.LOD = (uint)(lerp(5, 0, MeanTessFactor / MaxTessFactor));
    return output;
}


float3x3 CalculateNormal(float2 UV);
float3 DistanceBasedColor(uint TextureIndex, float Tiling, float2 UV, float LOD, float Weight);
float3 DistanceBasedNormal(uint TextureIndex, float Tiling, float2 UV, float LOD, float Weight);
float4 CalculateWorldAlignedWeight(float3 Normal, float Height);
float3 CalculateDetailedNormal(float3x3 TNBMatrix, float3 DetailNormal);
void ApplyMacroVaration(inout float3 TerrainColor, float2 UV, float LOD);
// PS
float4 PSMain(DS_OUTPUT input) : SV_TARGET
{
    const float3x3 TNB = CalculateNormal(input.UV);
    const float3 Normal = TNB[1];
    const float  Height = HeightScaler * HeightMap.Sample(LinearSampler_Clamp, input.UV);
    float Tiling = (TerrainSize / GridSize);

    // Distance Based
    float LinearBlendFactor = saturate((input.CameraDistance + StartOffset) / Range);
    float SmoothedBlendFactor = smoothstep(0.0f, 1.0f, LinearBlendFactor);
    float3 GrassColor = DistanceBasedColor(GRASS, Tiling, input.UV, input.LOD, SmoothedBlendFactor);
    float3 GrassNormal = DistanceBasedNormal(GRASS, Tiling, input.UV, input.LOD, SmoothedBlendFactor);

    float3 DirtColor =  DistanceBasedColor(DIRT, Tiling, input.UV, input.LOD, SmoothedBlendFactor);
    float3 DirtNormal = DistanceBasedNormal(DIRT, Tiling, input.UV, input.LOD, SmoothedBlendFactor);;
    
    float3 RockColor = DistanceBasedColor(ROCK, Tiling, input.UV, input.LOD, SmoothedBlendFactor);
    float3 RockNormal = DistanceBasedNormal(ROCK, Tiling, input.UV, input.LOD, SmoothedBlendFactor);;
    
    float3 SandColor = DistanceBasedColor(SAND, Tiling, input.UV, input.LOD, SmoothedBlendFactor);
    float3 SandNormal = DistanceBasedNormal(SAND, Tiling, input.UV, input.LOD, SmoothedBlendFactor);;

    // Perlin Noise Based
    float Noise = clamp(0, 1, PerlinNoise.Sample(LinearSampler_Clamp, input.UV).r);
    Noise = pow(abs(Noise * NoiseAmount), abs(NoisePower));
    float3 GroundColor = lerp(GrassColor, DirtColor, clamp(0, 1, Noise));
    float3 GroundNormal = lerp(GrassNormal, DirtNormal, clamp(0, 1, Noise));
    // World Based
    // x : Ground, y : Sand : z : Rock
    float4 WorldAlignedWeight = CalculateWorldAlignedWeight(Normal, Height);
    float3x3 ColorMat = float3x3(SandColor, GroundColor, RockColor);
    float3x3 NormalMat = float3x3(SandNormal, GroundNormal, RockNormal);

    float3 TerrainColor = mul(WorldAlignedWeight.xyz, ColorMat);
    float3 TerrainDetailNormal = mul(WorldAlignedWeight.xyz, NormalMat);

    // Macro Variation
    ApplyMacroVaration(TerrainColor, input.UV, input.LOD);

    const float3 DetailedNormal = CalculateDetailedNormal(TNB, float3(0,0,1));
    float LDotN = dot(-normalize(LightDirection), DetailedNormal);
    // return float4(abs(DetailedNormal), 1);
    // return float4(abs(TNB[1]), 1);
    // return float4(abs(LDotN), abs(LDotN), abs(LDotN), 1);

    const float Specular = 0.1f;
    const float Ambient = 0.2f;
    const float Diffuse = (1 - Specular);
    
    float3 Color = TerrainColor * Ambient
                 + Specular * (LightColor).rgb
                 + TerrainColor * saturate(LDotN) * Diffuse * (LightColor).rgb
                ;
    return float4(Color, 1.f);
}

/*======================================================================================*/

// PS에서 해도 되고 DS에서 해도 된다.
// PS에서 하는 것이 더 정밀한 Normal을 얻을 수 있다.
// 전체 NormalMap을 미리 ComputeShader로 구워서 테스트 해봤는데 속도의 차이가 거의 없다.
float3x3 CalculateNormal(float2 UV)
{
// HeightMap Normal
    float2 dUV[4] = {
        float2(-HeightMapTexelSize.x, 0), 
        float2(+HeightMapTexelSize.x, 0),
        float2(0, -HeightMapTexelSize.y),
        float2(0, +HeightMapTexelSize.y)
    };
    float height[4] = {0, 0, 0, 0};

    int i;
    [unroll]
    for(i = 0 ; i < 4 ; i++)
    {
        height[i] = HeightMap.SampleLevel(LinearSampler_Clamp, UV + dUV[i], 0).r;
        height[i] *= HeightScaler;
    }

    float StrideX = (UV.x - HeightMapTexelSize.x < 0) || (UV.x + HeightMapTexelSize.x > 1) ? 1.f : 2.f;
    float StrideZ = (UV.y - HeightMapTexelSize.y < 0) || (UV.y + HeightMapTexelSize.y > 1) ? 1.f : 2.f;
    float HeightDiffX = (height[1] - height[0]);
    float HeightDiffZ = (height[3] - height[2]);

    float3 tangent = normalize(float3(StrideX, HeightDiffX, 0));    // x
    float3 bitangent = normalize(float3(0, HeightDiffZ, StrideZ));  // z
    float3 normal = normalize(cross(bitangent, tangent));           // y

    return float3x3(tangent, normal, bitangent); // TNB
}

/*======================================================================================*/

float CalculateTessellationFactor(float4 Point1, float4 Point2)
{
    float L = ScreenDistance;
    float3 Center = ((Point1 + Point2) * 0.5).xyz;
    float R = length(Point2.xyz - Point1.xyz) * 0.5;
    float Distance = length(Center);
    float Dy = sqrt(Center.y * Center.y + Center.z * Center.z);
    float Dx = sqrt(Center.x * Center.x + Center.z * Center.z);
    float Ssd = 0;
    float TessRatio = 0;

    [flatten]
    if (Distance < 10.f || Distance * RDRatio < R)
    {
        return MaxTessFactor;
    }

    float PhiY = asin(saturate(RDRatio * R / Dy) / RDRatio);
    float PhiX = asin(saturate(RDRatio * R / Dx) / RDRatio);
    float LODNear = LODRange.x;
    float LODFar = LODRange.y;

# if defined (TYPE01)
    // SSD-Based
    const float MaxScreenDiagonal = sqrt((ScreenDiagonal)) / LODFar;
    float ThetaY = atan2(Center.y , Center.z);
    float ThetaX = atan2(Center.x , Center.z);
    float SsdY = tan(ThetaY + PhiY) - tan(ThetaY - PhiY);
    float SsdX = tan(ThetaX + PhiX) - tan(ThetaX - PhiX);
    Ssd = pow(L * SsdX, 2) + pow(L * SsdY, 2);
    TessRatio = pow(saturate((sqrt(Ssd) / (MaxScreenDiagonal))), LODNear);

# elif defined (TYPE02) // Better
    // Approax SSD-Based
    const float MaxScreenDiagonal = sqrt((ScreenDiagonal)) / LODFar;
    float SsdY = 2 * tan(PhiY);
    float SsdX = 2 * tan(PhiX);
    Ssd = pow(L * SsdX, 2) + pow(L * SsdY, 2);
    TessRatio = pow(saturate((sqrt(Ssd) / (MaxScreenDiagonal))), LODNear);

# elif defined (TYPE03)

    float D = min(length(Center), LODFar);
    TessRatio = saturate((LODFar - D) / (LODFar - LODNear));

# else
#  error "Either SSD Type must be defined."
# endif

    return TessRatio;
}

float CalculateDensity(InputPatch<VS_OUTPUT, HS_INPUT_PATCH_SIZE> patch)
{
    float3 PatchNormal = normalize(cross(patch[2].Position - patch[0].Position, patch[3].Position - patch[1].Position));
    return 1 - saturate(PatchNormal.y);
}

float3 DistanceBasedColor(uint TextureIndex, float Tiling, float2 UV, float LOD, float Weight)
{
    float3 NearColor = DetailDiffuses.SampleLevel(AnisotropicSampler_Wrap, float3(UV * Tiling * NearSize, TextureIndex), LOD).rgb;
    float3 FarColor  = DetailDiffuses.SampleLevel(AnisotropicSampler_Wrap, float3(UV * Tiling * FarSize,  TextureIndex), LOD).rgb;
    return lerp(NearColor, FarColor, Weight);
}
float3 DistanceBasedNormal(uint TextureIndex, float Tiling, float2 UV, float LOD, float Weight)
{
    float3 NearNormal = DetailNormals.SampleLevel(AnisotropicSampler_Wrap, float3(UV * Tiling * NearSize, TextureIndex), LOD).rgb;
    float3 FarNormal  = DetailNormals.SampleLevel(AnisotropicSampler_Wrap, float3(UV * Tiling * FarSize,  TextureIndex), LOD).rgb;
    NearNormal = NearNormal * 2.f - 1.f;
    FarNormal = FarNormal * 2.f - 1.f;

    return lerp(NearNormal, FarNormal, Weight);
}


float4 CalculateWorldAlignedWeight(float3 Normal, float Height)
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

void ApplyMacroVaration(inout float3 TerrainColor, float2 UV, float LOD)
{
    float Tiling[3] = {0.2134, 0.0534, 0.002};
    float Variation[3];
    [unroll]
    for (uint i = 0 ; i < 3 ; i++)
    {
        Variation[i] = (MacroVariationMap.SampleLevel(AnisotropicSampler_Wrap, UV * Tiling[i], LOD));
    }
    float MacroViration;
    MacroViration = Variation[0] + (Variation[1] * Variation[2]);
    TerrainColor *= lerp(0.5f, 0.9f, (MacroViration));
}

float3 CalculateDetailedNormal(float3x3 TNB, float3 DetailNormal)
{
    float3 ZupNormal = DetailNormal;
    float3 YupNormal = float3(DetailNormal.x, DetailNormal.z, -DetailNormal.y);

    DetailNormal = mul(YupNormal, TNB);
    return lerp(DetailNormal, TNB[1], 0.7f);
}
#endif