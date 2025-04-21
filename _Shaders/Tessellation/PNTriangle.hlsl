#ifndef __PNTRIANGLE_HLSL__
#define __PNTRIANGLE_HLSL__

#define WHITE (float4(1,1,1,1))

# define DOMAIN "tri"
# define HS_PARTITION "integer"
# define HS_INPUT_PATCH_SIZE 3
# define HS_OUTPUT_PATCH_SIZE 10


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
    float3 Position : POSITION;
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

    output.Edge[0] = 3;
    output.Edge[1] = 3;
    output.Edge[2] = 3;
    output.Inside = 1;

    return output;
}

[domain(DOMAIN)]
[partitioning(HS_PARTITION)]
[outputtopology("triangle_cw")]
[outputcontrolpoints(HS_OUTPUT_PATCH_SIZE)]
[patchconstantfunc("HSConstant")]
HS_POINT_OUTPUT HSMain(
    InputPatch<VS_OUTPUT, HS_INPUT_PATCH_SIZE> patch,
    uint id : SV_OutputControlPointID,
    uint PatchID : SV_PrimitiveID)
{
    HS_POINT_OUTPUT output;
    output.Position = patch[id].Position;
    float3 P0 = patch[0].Position;
    float3 P1 = patch[1].Position;
    float3 P2 = patch[2].Position;
    float3 N0 = patch[0].Normal;
    float3 N1 = patch[1].Normal;
    float3 N2 = patch[2].Normal;

    if (id == 0)
    {
        output.Position = P0;
        output.Normal = N0;
    }
    else if (id == 1)
    {
        output.Position = P1;
        output.Normal = N1;
    }
    else if (id == 2)
    {
        output.Position = P2;
        output.Normal = N2;
    }
    else if (id == 3)
    {
        output.Position = lerp(P0, P1, 1.0/3.0);
        output.Normal = lerp(N0, N1, 1.0/3.0);
    }
    else if (id == 4)
    {
        output.Position = lerp(P0, P1, 2.0/3.0);
        output.Normal = lerp(N0, N1, 2.0/3.0);
    }
    else if (id == 5)
    {
        output.Position = lerp(P1, P2, 1.0/3.0);
        output.Normal = lerp(N1, N2, 1.0/3.0);
    }
    else if (id == 6)
    {
        output.Position = lerp(P1, P2, 2.0/3.0);
        output.Normal = lerp(N1, N2, 2.0/3.0);
    }
    else if (id == 7)
    {
        output.Position = lerp(P2, P0, 1.0/3.0);
        output.Normal = lerp(N2, N0, 1.0/3.0);
    }
    else if (id == 8)
    {
        output.Position = lerp(P2, P0, 2.0/3.0);
        output.Normal = lerp(N2, N0, 2.0/3.0);
    }
    else if (id == 9)
    {
        output.Position = (P0 + P1 + P2) / 3.0;
        output.Normal = (N0 + N1 + N2) / 3.0;;
    }
    
    return output;
}

[domain(DOMAIN)]
DS_OUTPUT DSMain(
    HS_CONSTANT_OUTPUT input,
    float3 barycentric : SV_DomainLocation,
    const OutputPatch<HS_POINT_OUTPUT, HS_OUTPUT_PATCH_SIZE> patch)
{
    DS_OUTPUT output;
    float3 P0 = patch[0].Position;
    float3 P1 = patch[1].Position;
    float3 P2 = patch[2].Position;

    float3 N0 = patch[0].Normal;
    float3 N1 = patch[1].Normal;
    float3 N2 = patch[2].Normal;

    float3 b300 = P0;
    float3 b030 = P1;
    float3 b003 = P2;

    float3 b210 = patch[3].Position.xyz - dot(patch[3].Position.xyz - P0, N0) * N0;
    float3 b120 = patch[4].Position.xyz - dot(patch[4].Position.xyz - P1, N1) * N1;

    float3 b012 = patch[5].Position.xyz - dot(patch[5].Position.xyz - P1, N1) * N1;
    float3 b021 = patch[6].Position.xyz - dot(patch[6].Position.xyz - P2, N2) * N2;

    float3 b102 = patch[7].Position.xyz - dot(patch[7].Position.xyz - P2, N2) * N2;
    float3 b201 = patch[8].Position.xyz - dot(patch[8].Position.xyz - P0, N0) * N0;

    float3 E = (b210 + b120 + b021 + b012 + b102 + b201) / 6.0;
    float3 V = patch[9].Position.xyz;
    float3 b111 = E + (E - V) * 0.5;

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
        b012 * (3 * v * v * w) +
        b021 * (3 * v * w * w) +
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
