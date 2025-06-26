#ifndef __PNAENTRIANGLE_HLSL__
#define __PNAENTRIANGLE_HLSL__

#define WHITE (float4(1,1,1,1))

# define DOMAIN "tri"
# define HS_PARTITION "integer"
# define HS_INPUT_PATCH_SIZE 3
# define HS_OUTPUT_PATCH_SIZE 3

cbuffer WVP_DS : register(b0)
{
    matrix World : packoffset(c0);
    matrix View : packoffset(c4);
    matrix Projection : packoffset(c8);
}

cbuffer TessellationFactor_HS : register(b1)
{
    int4 EdgeTessellationFactor  : packoffset(c0);
    int2 InnerTessellationFactor : packoffset(c1); // tri에서는 x만 사용
}

struct VS_INPUT
{
    float4 Position : POSITION;
    float3 Normal : NORMAL;
};

struct VS_OUTPUT // HS_INPUT
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
};

struct HS_CONSTANT_OUTPUT
{
    float Edge[3] : SV_TessFactor;
    float Inside : SV_InsideTessFactor;
};

struct HS_POINT_OUTPUT // DS_INPUT
{
    float3 Position[3] : POSITION;
    float3 Normal : NORMAL;
};

struct DS_OUTPUT // PS_INPUT
{
    float4 Position : SV_Position;
};

VS_OUTPUT VSMain(VS_INPUT input)
{
    VS_OUTPUT output;
    output.Position = input.Position.xyz;
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

    output.Edge[0] = max(1, EdgeTessellationFactor.x);
    output.Edge[1] = max(1, EdgeTessellationFactor.x);
    output.Edge[2] = max(1, EdgeTessellationFactor.x);
    output.Inside = max(1, EdgeTessellationFactor.x);

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

    output.Position[0] = patch[id].Position;
    output.Normal = patch[id].Normal;

    output.Position[1] = ComputeControlPoint(
        patch[id].Position,
        patch[NextId].Position,
        patch[id].Normal
    );

    output.Position[2] = ComputeControlPoint(
        patch[NextId].Position,
        patch[id].Position,
        patch[NextId].Normal
    );

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

    float3 N0 = patch[0].Normal;
    float3 N1 = patch[1].Normal;
    float3 N2 = patch[2].Normal;

    float3 b300 = patch[0].Position[0];
    float3 b030 = patch[1].Position[0];
    float3 b003 = patch[2].Position[0];

    float3 b210 = patch[0].Position[1];
    float3 b120 = patch[0].Position[2];

    float3 b021 = patch[1].Position[1];
    float3 b012 = patch[1].Position[2];

    float3 b102 = patch[2].Position[1];
    float3 b201 = patch[2].Position[2];

    float3 E = (b210 + b120 + b021 + b012 + b102 + b201) / 6.0f;
    float3 V = (b300 + b030 + b003) / 3.0f;
    float3 b111 = E + (E - V) * 0.5f;

// Evaluate Bezier Triangle
    float u = barycentric.x;
    float v = barycentric.y;
    float w = barycentric.z;
    float3 position =
        b300 * (u * u * u) +
        b030 * (v * v * v) +
        b003 * (w * w * w) +
        b210 * (3 * u * u * v) +
        b120 * (3 * u * v * v) +
        b021 * (3 * v * v * w) +
        b012 * (3 * v * w * w) +
        b102 * (3 * u * w * w) +
        b201 * (3 * u * u * w) +
        b111 * (6 * u * v * w);

    output.Position = mul(float4(position, 1.0f), World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    return output;
}

float4 PSMain(VS_OUTPUT input) : SV_TARGET
{
    return WHITE;
}
#endif


/*=============================================================================================*/

float3 ComputeControlPoint(float3 Position, float3 NextPosition, float3 Normal)
{
    Normal = normalize(Normal);
    return ((2 * Position + NextPosition) - Normal * dot(NextPosition - Position, Normal)) / 3;
}