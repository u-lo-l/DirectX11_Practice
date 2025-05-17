#ifndef __Ocean_HLSL__
#define __Ocean_HLSL__

# define DOMAIN "quad"
# define HS_PARTITION "integer"
# define HS_INPUT_PATCH_SIZE 4
# define HS_OUTPUT_PATCH_SIZE 4

# define FAR_DISTANCE (2500)

const static float MinTessFactor = 1;
const static float MaxTessFactor = 64;
const static float RDRatio = 4;

const static float MIPMIN = 0;
const static float MIPMAX = 5;

#define GET_MIP_LEVEL(x) (lerp(MIPMIN, MIPMAX, (x)))

const static float WaterRefractionIndex = 1.33f; // 굴절률
const static float WaterR0 = 0.02f;              // 수직 입사 반사 계수

cbuffer CB_WVP : register(b0)  // VS DS HS PS
{
    matrix World      : packoffset(c0);
    matrix View       : packoffset(c4);
    matrix Projection : packoffset(c8);
    matrix ViewInverse : packoffset(c12);
}

cbuffer CB_HeightScaler : register(b1)
{
    float3 CameraPosition;
    float  HeightScaler = 30.f;

    float2 LODRange;
    float2 TexelSize;

    float  ScreenDistance;
    float  ScreenDiagonal;
    float  NoiseScaler = 1.f;
    float  NoisePower = 1.f;

    float2 HeightMapTiling;
    float2 NoiseMapTiling;

    float4 LightColor;

    float3 LightDirection;
    float  TerrainHeightScaler;

    float2 TerrainWorldPosition;
    float2 TerrainDimension;
}


SamplerState LinearSampler_Border : register(s0);    // VS DS PS
SamplerState AnisotropicSampler_Wrap : register(s1); // VS DS PS
Texture2D<float4>   WaterHeightMap  : register(t0);  // VS DS PS
TextureCube SkyTexture      : register(t1);          // PS
Texture2D<float> FoamGrid   : register(t2);          // PS
Texture2D<float> PerlinNoise : register(t3);         // VS DS PS

const float GetPerlinRandom(float2 uv, uint LOD = 0)
{
    uv /= HeightMapTiling * 4;
    
    float Random = PerlinNoise.SampleLevel(AnisotropicSampler_Wrap, uv, LOD); // 0 ~ 1
    return Random;
}
struct VS_INPUT
{
    float4 Position : POSITION;
    float2 UV : UV;
    float3 Normal : NORMAL;
};

struct VS_OUTPUT // HS_INPUT
{
    float3 Position : POSITION0;
    float2 UV       : UV;
    float3 Normal   : NORMAL;
};

struct HS_CONSTANT_OUTPUT
{
    float Edge[4] : SV_TessFactor;
    float Inside[2] : SV_InsideTessFactor;
};

struct HS_POINT_OUTPUT // DS_INPUT
{
    float3 Position : POSITION;
    float2 UV       : UV;
    float3 Normal : NORMAL;
};

struct DS_OUTPUT // PS_INPUT
{
    float4 Position : SV_Position;
    float2 UV       : UV;
    float3 Normal : NORMAL;

    float3 WorldPosition : POSITION;
    float  LOD : LOD;
    float  PerlinBlending : BLENDING;
};

float3 CalculateNormal(float2 UV, uint LOD);

// VS
VS_OUTPUT VSMain(VS_INPUT input)
{
    VS_OUTPUT output;

    output.Position = input.Position.xyz;
    output.UV = input.UV;
    output.Normal = input.Normal;

    // const float Random = GetPerlinRandom(input.UV);
    // const float3 Displacement = Random * WaterHeightMap.SampleLevel(LinearSampler_Border, input.UV, 0).xyz;
    // output.Position.y += Displacement.y;
    const float2 HeightMapUV = output.UV * HeightMapTiling;
    const float3 Displacement = WaterHeightMap.SampleLevel(AnisotropicSampler_Wrap, HeightMapUV, 0).rgb ;

    output.Position.y += Displacement.y;
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
    float4 Points[4];
    [unroll]
    for (int i = 0 ; i < 4 ; i++)
    {
        Points[i] = mul(float4(patch[i].Position, 1), WorldView);
    }

    [unroll]
    for (int j = 0 ; j < 4 ; j++)
    {
        float4 Point1 = Points[((j - 1) + 4) % 4];
        float4 Point2 = Points[j];
        float TessRatio = CalculateTessellationFactor(Point1, Point2);
        output.Edge[j] = lerp(MinTessFactor, MaxTessFactor, TessRatio);
    }

    // float Density = lerp(MinTessFactor, MaxTessFactor, CalculateDensity(patch));
    float TessFactor = (output.Edge[0] + output.Edge[2]) * 0.5f;
    // output.Inside[0] = Density * DensityWeight + TessFactor * SSDWeight;
    output.Inside[0] = TessFactor;
    TessFactor = (output.Edge[1] + output.Edge[3]) * 0.5f;
    // output.Inside[1] = Density * DensityWeight + TessFactor * SSDWeight;
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

    const float3 CameraPosition = ViewInverse._41_42_43;
    const float Dist = length(CameraPosition - mul(output.Position, World).xyz);
    float DetailAmount = saturate((FAR_DISTANCE - Dist) / FAR_DISTANCE);
    output.LOD = GET_MIP_LEVEL(1 - DetailAmount);

    float2 u1 = lerp(patch[0].UV, patch[1].UV, UV.x);
    float2 u2 = lerp(patch[3].UV, patch[2].UV, UV.x);
    output.UV = lerp(u1, u2, UV.y);

    output.Normal = float3(0,1,0);

    const float2 HeightMapUV = output.UV * HeightMapTiling;
    const float2 NoiseMapUV = output.UV * NoiseMapTiling;
    float Noise = PerlinNoise.SampleLevel(AnisotropicSampler_Wrap, NoiseMapUV, 0).r;
    Noise = lerp(0.5, 1, Noise);
    Noise = saturate(pow(abs(Noise * NoiseScaler) , NoisePower));
    output.PerlinBlending = lerp(Noise, 1.f, DetailAmount);
    
    const float3 Displacement = WaterHeightMap.SampleLevel(AnisotropicSampler_Wrap, HeightMapUV, 0).rgb * output.PerlinBlending;
    const float Folding = abs(FoamGrid.SampleLevel(AnisotropicSampler_Wrap, HeightMapUV, 0)).r;

    output.Position.y = Displacement.y * HeightScaler;
    output.Position.xz += Displacement.xz * HeightScaler * (1 - Folding);

    output.Position = mul(output.Position, World);
    output.WorldPosition = output.Position.xyz;
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = mul(output.Normal, (float3x3)World);


    return output;
}

// PS
float GetSpecularCoef(float VDotL);
float4 PSMain(DS_OUTPUT input) : SV_TARGET
{
    const float3 CameraPosition = ViewInverse._41_42_43;
    float3 ViewRay = (input.WorldPosition - CameraPosition); // WorldSpace
    float Distance = saturate(length(ViewRay) / FAR_DISTANCE);
    ViewRay = normalize(ViewRay);


    // float LOD = 1 - input.LOD / MIPMAX;
    // return float4(LOD, LOD, LOD, 1);
    const float2 HeightMapUV = input.UV * HeightMapTiling;
    const float2 NoiseUV = input.UV * NoiseMapTiling;

    const float3 Normal = CalculateNormal(HeightMapUV, input.LOD);
    const float3 Light = normalize(LightDirection);
    float LDotN = saturate(dot(-Light, Normal));

    float3 ShallowWaterColor = float3(0.7f, 0.85f, 0.8f);
    float3 DeepWaterColor = float3(0.0f, 0.2f, 0.3f);

    
    // Asume Distance to Sky : Infinity
    float3 ReflectedRay = reflect(ViewRay, Normal); // WorldSpace
    float RDotN = dot(ReflectedRay, Normal);
    float3 EnvColor = SkyTexture.Sample(LinearSampler_Border, ReflectedRay).rgb;

    const float Ambient = 0.3f;
    const float Specular = lerp(0, 0.8f, GetSpecularCoef(RDotN)) * lerp(0.5f, 1.f, 1 - Distance);
    const float Refraction = (1 - Specular);

    // float3 WaterColor = lerp(ShallowWaterColor, DeepWaterColor, 0.8f);
    float3 WaterColor = DeepWaterColor;
    float  Foam = (FoamGrid.SampleLevel(AnisotropicSampler_Wrap, HeightMapUV, input.LOD));
    WaterColor = 
                   WaterColor * Ambient
                 + EnvColor * Specular * LightColor.rgb
                 + WaterColor * LDotN * Refraction * LightColor.rgb
                ;
    float3 FoamColor = Foam * LightColor.rgb * (Ambient + LDotN) * GetPerlinRandom(NoiseUV, input.LOD);
    return float4(input.PerlinBlending, input.PerlinBlending, input.PerlinBlending, 1);
    return float4((WaterColor + FoamColor) * input.PerlinBlending, 1);
}

/*======================================================================================*/

// PS에서 해도 되고 DS에서 해도 된다.
// PS에서 하는 것이 더 정밀한 Normal을 얻을 수 있다.
float3 CalculateNormal(float2 UV, uint LOD)
{
    float2 dUV[4] = {
        float2(-TexelSize.x, 0), 
        float2(+TexelSize.x, 0),
        float2(0, -TexelSize.y),
        float2(0, +TexelSize.y)
    };
    float height[4] = {0, 0, 0, 0};

    const float Random = GetPerlinRandom(UV, LOD);
    [unroll]
    for(int i = 0 ; i < 4 ; i++)
    {
        height[i] = WaterHeightMap.SampleLevel(AnisotropicSampler_Wrap, UV + dUV[i], LOD).g * Random;
    }

    float HeightDiffX = (height[1] - height[0]) / 2 * HeightScaler;
    float HeightDiffZ = (height[3] - height[2]) / 2 * HeightScaler;

    float3 tangent = normalize(float3(2, HeightDiffX, 0));
    float3 bitangent = normalize(float3(0, HeightDiffZ, 2));
    return normalize(cross(bitangent, tangent));
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

# if defined (TYPE02)
    // SSD-Based
    const float MaxScreenDiagonal = sqrt((ScreenDiagonal)) / LODFar;
    float ThetaY = atan2(Center.y , Center.z);
    float ThetaX = atan2(Center.x , Center.z);
    float SsdY = tan(ThetaY + PhiY) - tan(ThetaY - PhiY);
    float SsdX = tan(ThetaX + PhiX) - tan(ThetaX - PhiX);
    Ssd = pow(L * SsdX, 2) + pow(L * SsdY, 2);
    TessRatio = pow(saturate((sqrt(Ssd) / (MaxScreenDiagonal))), LODNear);

# elif defined (TYPE01) // Better
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

/*======================================================================================*/

// Fresnel
float GetSpecularCoef(float VDotN)
{
    VDotN = max(0, VDotN);
    return (WaterR0 + (1 - WaterR0) * pow(1 - VDotN, 5));
}

#endif