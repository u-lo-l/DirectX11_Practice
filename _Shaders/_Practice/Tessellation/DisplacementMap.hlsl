#ifndef __DISPLACEMENT_HLSL__
#define __DISPLACEMENT_HLSL__

#define WHITE (float4(1,1,1,1))

# define DOMAIN "tri"
# define HS_PARTITION "integer"
# define HS_INPUT_PATCH_SIZE 3
# define HS_OUTPUT_PATCH_SIZE 3

cbuffer WVP : register(b0)
{
    matrix World : packoffset(c0);
    matrix View : packoffset(c4);
    matrix Projection : packoffset(c8);
}

cbuffer DisplacementFactor : register(b1)
{
    float DisplacementFactor : packoffset(c0); // tri에서는 x만 사용
}

SamplerState LinearSampler : register(s0);
Texture2D HeightMap : register(t0);

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
    float Edge[3] : SV_TessFactor;
    float Inside : SV_InsideTessFactor;
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
};

VS_OUTPUT VSMain(VS_INPUT input)
{
    VS_OUTPUT output;
    output.Position = input.Position.xyz;
    output.UV = input.UV;
    output.Normal = input.Normal;
    return output;
}

HS_CONSTANT_OUTPUT HSConstant
(
    InputPatch<VS_OUTPUT, HS_INPUT_PATCH_SIZE> input,
    uint PatchID : SV_PrimitiveID
)
{
    HS_CONSTANT_OUTPUT output;

    // output.Edge[0] = max(1, EdgeTessellationFactor.x);
    // output.Edge[1] = max(1, EdgeTessellationFactor.x);
    // output.Edge[2] = max(1, EdgeTessellationFactor.x);
    // output.Inside = max(1, EdgeTessellationFactor.x);
    
    output.Edge[0] = 1;
    output.Edge[1] = 1;
    output.Edge[2] = 1;
    output.Inside = 1;

    return output;
}

float3 ComputeControlPoint(float3 Position, float3 NextPosition, float3 Normal);

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
    // 한 번의 호출출에 삼각형의 한 변에 대한 ControlPoint를 처리한다.
    HS_POINT_OUTPUT output;

    uint NextId = id < 2 ? id + 1 : 0; // better than (id + 1) % 3;

    output.Position = patch[id].Position;
    output.Normal = patch[id].Normal;
    output.UV = patch[id].UV;

    return output;
}

[domain(DOMAIN)]
DS_OUTPUT DSMain
(
    HS_CONSTANT_OUTPUT input,
    float3 barycentric : SV_DomainLocation,
    const OutputPatch<HS_POINT_OUTPUT, HS_OUTPUT_PATCH_SIZE> patch
)
{
    DS_OUTPUT output;
    output.Position.xyz = 
        barycentric.x * patch[0].Position 
        + barycentric.y * patch[1].Position 
        + barycentric.z * patch[2].Position;
    output.Position.w = 1;

    output.UV = 
        barycentric.x * patch[0].UV 
        + barycentric.y * patch[1].UV 
        + barycentric.z * patch[2].UV;

    output.Normal = 
        barycentric.x * patch[0].Normal 
        + barycentric.y * patch[1].Normal 
        + barycentric.z * patch[2].Normal;

    float Height = HeightMap.SampleLevel(LinearSampler, output.UV, 0).r;
    output.Position += float4(output.Normal * Height  * 10 * DisplacementFactor, 1);
    output.Position = mul(output.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = mul(output.Normal, (float3x3)World);

    return output;
}

float4 PSMain(DS_OUTPUT input) : SV_TARGET
{
    return WHITE * 0.5f;
}
#endif
