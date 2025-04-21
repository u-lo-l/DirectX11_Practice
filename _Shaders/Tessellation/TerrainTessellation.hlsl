#ifndef __TERRAIN_TESSELLATION_HLSL__
#define __TERRAIN_TESSELLATION_HLSL__

# define DOMAIN "quad"
# define HS_PARTITION "integer"
# define HS_INPUT_PATCH_SIZE 4
# define HS_OUTPUT_PATCH_SIZE 4
# define SCALER 2

cbuffer WVP : register(b0)  // VS DS HS PS
{
    matrix World : packoffset(c0);
    matrix View : packoffset(c4);
    matrix Projection : packoffset(c8);
}

cbuffer HeightScaler : register(b1)
{
    float HeightScaler = 30.f;
    float TriSize = 4.f;
    float ScreenDistance;
    float ScreenDiagonal;

    float3 CameraPosition;
    float Padding;
}

SamplerState LinearSampler : register(s0); // VS DS PS
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

// VS
VS_OUTPUT VSMain(VS_INPUT input)
{
    VS_OUTPUT output;
    output.Position = input.Position.xyz;
    output.UV = input.UV;
    output.Normal = input.Normal;

    float Height = HeightMap.SampleLevel(LinearSampler, output.UV, 0).r;
    output.Position.y = 0;
    output.Position += output.Normal * Height  * HeightScaler * (SCALER);

    return output;
}

float CalculateTessellationFactor(float4 Point1, float4 Point2, out float4 Color);
// HS
HS_CONSTANT_OUTPUT HSConstant
(
    InputPatch<VS_OUTPUT, HS_INPUT_PATCH_SIZE> patch,
    uint PatchID : SV_PrimitiveID
)
{
    HS_CONSTANT_OUTPUT output;
    // matrix WorldView = mul(World, View);
    // float4 Point1 = mul(float4(patch[0].Position, 1), WorldView);
    // float4 Point2 = mul(float4(patch[1].Position, 1), WorldView);
    // float4 Point3 = mul(float4(patch[2].Position, 1), WorldView);
    // float4 Point4 = mul(float4(patch[3].Position, 1), WorldView);

    // float4 Point1 = float4(patch[0].Position, 1);
    // float4 Point2 = float4(patch[1].Position, 1);
    // float4 Point3 = float4(patch[2].Position, 1);
    // float4 Point4 = float4(patch[3].Position, 1);

    // output.Edge[0] = CalculateTessellationFactor(Point1, Point2);
    // output.Edge[1] = CalculateTessellationFactor(Point2, Point3);
    // output.Edge[2] = CalculateTessellationFactor(Point3, Point4);
    // output.Edge[3] = CalculateTessellationFactor(Point4, Point1);
    output.Edge[0] = 1;
    output.Edge[1] = 1;
    output.Edge[2] = 1;
    output.Edge[3] = 1;

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
    matrix WorldView = mul(World, View);
    float4 Point1 = mul(float4(patch[0].Position, 1), WorldView);
    float4 Point2 = mul(float4(patch[1].Position, 1), WorldView);
    float4 Point3 = mul(float4(patch[2].Position, 1), WorldView);
    float4 Point4 = mul(float4(patch[3].Position, 1), WorldView);

    float4 Color1;
    float4 Color2;
    float4 Color3;
    float4 Color4;
    float foo;
    foo = CalculateTessellationFactor(Point1, Point2, Color1);
    foo = CalculateTessellationFactor(Point2, Point3, Color2);
    foo = CalculateTessellationFactor(Point3, Point4, Color3);
    foo = CalculateTessellationFactor(Point4, Point1, Color4);

    float4 c1 = lerp(Color1, Color2, UV.x);
    float4 c2 = lerp(Color4, Color3, UV.x);
    output.Color = float4(lerp(c1, c2, UV.y));

    float3 v1 = lerp(patch[0].Position, patch[1].Position, UV.x);
    float3 v2 = lerp(patch[3].Position, patch[2].Position, UV.x);
    output.Position = float4(lerp(v1, v2, UV.y), 1);

    float3 n1 = lerp(patch[0].Normal, patch[1].Normal, UV.x);
    float3 n2 = lerp(patch[3].Normal, patch[2].Normal, UV.x);
    output.Normal = lerp(n1, n2, UV.y);

    float2 u1 = lerp(patch[0].UV, patch[1].UV, UV.x);
    float2 u2 = lerp(patch[3].UV, patch[2].UV, UV.x);
    output.UV = lerp(u1, u2, UV.y);

    float Height = HeightMap.SampleLevel(LinearSampler, output.UV, 0).r;
    output.Position.y = 0;
    output.Position += float4(output.Normal * Height  * HeightScaler * (SCALER), 1);

    output.Position = mul(output.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = mul(output.Normal, (float3x3)World);



    return output;
}

// PS
float4 PSMain(DS_OUTPUT input) : SV_TARGET
{
    return input.Color;
}

/*======================================================================================*/

const static float MinTessFactor = 1;
const static float MaxTessFactor = 16;

float CalculateTessellationFactor(float4 Point1, float4 Point2, out float4 Color)
{
    // Point1 = mul(Point1, mul(World, View));
    // Point2 = mul(Point2, mul(World, View));
    Point1 /= Point1.w;
    Point2 /= Point2.w;
    float L = ScreenDistance;
    float3 Center = ((Point1 + Point2) * 0.5).xyz; // WorldSpace에선 맞음
    float3 ToCenter = Center;
    float R = length(Point2.xyz - Point1.xyz) * 0.5;
    float Dy = sqrt(ToCenter.y * ToCenter.y + ToCenter.z * ToCenter.z);
    float Dx = sqrt(ToCenter.x * ToCenter.x + ToCenter.z * ToCenter.z);

    float Phiy = asin(R/Dy);
    float Phix = asin(R/Dx);
    float ThetaY = atan2(ToCenter.y , ToCenter.z);
    float ThetaX = atan2(ToCenter.x , ToCenter.z);
    float SsdY = tan(ThetaY + Phiy) - tan(ThetaY - Phiy);
    float SsdX = tan(ThetaX + Phix) - tan(ThetaX - Phix);
    float Ssd = sqrt(pow(SsdX, 2) + pow(SsdY, 2));

    Color = float4(abs(ToCenter) / 512, 1);
    float TessFactor = lerp(MinTessFactor, MaxTessFactor, 1 - saturate(Ssd / (ScreenDiagonal / L)));
    float LODDebug = (TessFactor / MaxTessFactor);
    float CloseToCenter = 1 - sqrt(ToCenter.x * ToCenter.x + ToCenter.y * ToCenter.y) / 256;
    Color = float4(CloseToCenter, CloseToCenter, abs(0) / 256, 1);
    // Color = float4(abs(Dx) / 2048, abs(Dx) / 2048, abs(Dx) / 2048, 1);
    return TessFactor;
}
#endif