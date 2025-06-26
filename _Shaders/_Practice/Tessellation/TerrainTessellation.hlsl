#ifndef __TERRAIN_TESSELLATION_HLSL__
#define __TERRAIN_TESSELLATION_HLSL__

# define DOMAIN "quad"
// # define HS_PARTITION "integer"
// # define HS_PARTITION "fractional_even"
// # define HS_PARTITION "fractional_odd"
# define HS_PARTITION "pow2"
# define HS_INPUT_PATCH_SIZE 4
# define HS_OUTPUT_PATCH_SIZE 4
# define SCALER 2

const static float MinTessFactor = 1;
const static float MaxTessFactor = 64;
const static float RDRatio = 4;

const static float handed = -1; // left-handed
cbuffer CB_WVP : register(b0)  // VS DS HS PS
{
    matrix World : packoffset(c0);
    matrix View : packoffset(c4);
    matrix Projection : packoffset(c8);
}

cbuffer CB_HeightScaler : register(b1)
{
    float3 CameraPosition;
    float HeightScaler = 30.f;

    float2 LODRange;
    float2 TexelSize;

    float ScreenDistance;
    float ScreenDiagonal;
}

cbuffer CB_DirectionalLight : register(b2) // PS
{
    float3 LightDirection;
}

SamplerState LinearSampler : register(s0); // VS DS PS
SamplerState AnisotropicSampler : register(s1); // VS DS PS
Texture2D HeightMap : register(t0);        // VS DS

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

    float4 Color : COLOR;
};

float3 CalculateNormal(float2 UV);

// VS
VS_OUTPUT VSMain(VS_INPUT input)
{
    VS_OUTPUT output;
    output.Position = input.Position.xyz;
    output.UV = input.UV;
    output.Normal = input.Normal;

    float Height = HeightMap.SampleLevel(LinearSampler, output.UV, 0).r;
    output.Position.y = Height  * HeightScaler * (SCALER);

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

    // // BackFace Culling을 시도해보았지만 성능이 더 안 좋음.
    float3 Diag1 = (Points[2] - Points[0]).xyz;
    float3 Diag2 = (Points[3] - Points[1]).xyz;
    float3 PatchNormal = normalize(cross(Diag1, Diag2));
    [flatten] if (PatchNormal.z > 0.5f)
    {
        [unroll]
        for (int i = 0; i < 4; ++i)
            output.Edge[i] = 0;

        [unroll]
        for (int j = 0; j < 2; ++j)
            output.Inside[j] = 0;

        return output;
    }

    float Density = CalculateDensity(patch) * DensityWeight;
    [unroll]
    for (int j = 0 ; j < 4 ; j++)
    {
        float4 Point1 = Points[((j - 1) + 4) % 4];
        float4 Point2 = Points[j];
        float TessRatio = Density + CalculateTessellationFactor(Point1, Point2) * SSDWeight;
        output.Edge[j] = lerp(MinTessFactor, MaxTessFactor, TessRatio);
        // output.Edge[j] = lerp(MinTessFactor, MaxTessFactor, CalculateTessellationFactor(Point1, Point2));
    }

    output.Inside[0] = (output.Edge[0] + output.Edge[2]) * 0.5f;
    output.Inside[1] = (output.Edge[1] + output.Edge[3]) * 0.5f;
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


const static float MAXGRIDSIZE = 16;
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

    output.Color = lerp(float4(0.5f, 0, 0, 1), float4(0, 0, 1, 1), input.Inside[0] / 64);
    [flatten]
    if (input.Inside[0] > 63)
        output.Color = float4(0,1,1,1); // MAX

    output.Normal = float3(0,1,0);

    float Height = HeightMap.SampleLevel(AnisotropicSampler, output.UV, 0).r;
    output.Position.y = Height * HeightScaler * (SCALER);

    output.Position = mul(output.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = mul(output.Normal, (float3x3)World);

    return output;
}

// PS
float4 PSMain(DS_OUTPUT input) : SV_TARGET
{
    float LDotN = dot(-normalize(LightDirection), CalculateNormal(input.UV));
    // return lerp(float4(0.5, 1, 0.5, 1) * saturate(LDotN), input.Color, 0.5f);
    return float4(0.5, 1, 0.5, 1) * saturate(LDotN);
}

/*======================================================================================*/

// PS에서 해도 되고 DS에서 해도 된다.
// PS에서 하는 것이 더 정밀한 Normal을 얻을 수 있다.
float3 CalculateNormal(float2 UV)
{
    float2 dUV[4] = {
        float2(-TexelSize.x, 0), 
        float2(+TexelSize.x, 0),
        float2(0, -TexelSize.y),
        float2(0, +TexelSize.y)
    };
    float height[4] = {0, 0, 0, 0};

    [unroll]
    for(int i = 0 ; i < 4 ; i++)
    {
        height[i] = HeightMap.SampleLevel(AnisotropicSampler, UV + dUV[i], 0).r;
    }

    float StrideX = (UV.x - TexelSize.x < 0) || (UV.x + TexelSize.x > 1) ? 1.f : 2.f;
    float StrideZ = (UV.y - TexelSize.y < 0) || (UV.y + TexelSize.y > 1) ? 1.f : 2.f;
    float HeightDiffX = (height[1] - height[0]) * HeightScaler;
    float HeightDiffZ = (height[3] - height[2]) * HeightScaler;

    float3 tangent = normalize(float3(StrideX, HeightDiffX, 0));
    float3 bitangent = normalize(float3(0, HeightDiffZ, StrideZ));
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

    // float TessFactor = lerp(MinTessFactor, MaxTessFactor, TessRatio);
    return TessRatio;
}

float CalculateDensity(InputPatch<VS_OUTPUT, HS_INPUT_PATCH_SIZE> patch)
{
    float3 PatchNormal = normalize(cross(patch[2].Position - patch[0].Position, patch[3].Position - patch[1].Position));
    return 1 - saturate(PatchNormal.y);
}
#endif