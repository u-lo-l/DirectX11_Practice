#ifndef __BASICTESSELLATION_HLSL__
#define __BASICTESSELLATION_HLSL__

#define WHITE (float4(1,1,1,1))

// #define QUAD 0
#ifdef TRI
# define DOMAIN "tri"
# define HS_PARTITION "integer"
# define HS_INPUT_PATCH_SIZE 3
# define HS_OUTPUT_PATCH_SIZE 3
#endif

#ifdef QUAD
# define DOMAIN "quad"
# define HS_PARTITION "integer"
# define HS_INPUT_PATCH_SIZE 4
# define HS_OUTPUT_PATCH_SIZE 4
#endif


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
};

struct VS_OUTPUT // HS_INPUT
{
    float4 Position : POSITION;
};

struct HS_CONSTANT_OUTPUT
{
#ifdef TRI
    float Edge[3] : SV_TessFactor;
    float Inside : SV_InsideTessFactor;
#endif
#ifdef QUAD
    float Edge[4] : SV_TessFactor;
    float Inside[2] : SV_InsideTessFactor;
#endif
};

struct HS_POINT_OUTPUT // DS_INPUT
{
    float4 Position : POSITION;
};

struct DS_OUTPUT // PS_INPUT
{
    float4 Position : SV_Position;
};

VS_OUTPUT VSMain(VS_INPUT input)
{
    VS_OUTPUT output;
    output.Position = input.Position;
    return output;
}

HS_CONSTANT_OUTPUT HSConstant
(
    InputPatch<VS_OUTPUT, HS_INPUT_PATCH_SIZE> input,
    uint PatchID : SV_PrimitiveID
)
{
    HS_CONSTANT_OUTPUT output;

    #ifdef TRI
    output.Edge[0] = EdgeTessellationFactor.x;
    output.Edge[1] = EdgeTessellationFactor.y;
    output.Edge[2] = EdgeTessellationFactor.z;
    output.Inside = InnerTessellationFactor.x;
    #endif

    #ifdef QUAD
    output.Edge[0] = EdgeTessellationFactor.x;
    output.Edge[1] = EdgeTessellationFactor.y;
    output.Edge[2] = EdgeTessellationFactor.z;
    output.Edge[3] = EdgeTessellationFactor.w;
    output.Inside[0] = InnerTessellationFactor.x;
    output.Inside[1] = InnerTessellationFactor.y;
    #endif

    return output;
}

[domain(DOMAIN)]
[partitioning(HS_PARTITION)]
[outputtopology("triangle_cw")]
[outputcontrolpoints(HS_OUTPUT_PATCH_SIZE)]
[patchconstantfunc("HSConstant")]
HS_POINT_OUTPUT HSMain(
    InputPatch<VS_OUTPUT, HS_INPUT_PATCH_SIZE> input,
    uint id : SV_OutputControlPointID,
    uint PatchID : SV_PrimitiveID)
{
    HS_POINT_OUTPUT output;
    output.Position = input[id].Position;
    return output;
}

[domain(DOMAIN)]
DS_OUTPUT DSMain(
    HS_CONSTANT_OUTPUT input,
#ifdef TRI
    float3 barycentric : SV_DomainLocation,
#endif
#ifdef QUAD
    float2 UV : SV_DomainLocation,
#endif
    const OutputPatch<HS_POINT_OUTPUT, HS_OUTPUT_PATCH_SIZE> patch)
{
    DS_OUTPUT output;
#if defined(TRI)
    output.Position =
        patch[0].Position * barycentric.x +
        patch[1].Position * barycentric.y +
        patch[2].Position * barycentric.z;
#elif defined(QUAD)
    float3 v1 = lerp(patch[0].Position.xyz, patch[1].Position.xyz, UV.x);
    float3 v2 = lerp(patch[3].Position.xyz, patch[2].Position.xyz, UV.x);
    float3 position = lerp(v1, v2, UV.y);
    output.Position = float4(position, 1);
#else
#error "Either TRI or QUAD must be defined."
#endif

output.Position = mul(output.Position, World);
output.Position = mul(output.Position, View);
output.Position = mul(output.Position, Projection);
return output;
}

float4 PSMain(DS_OUTPUT input) : SV_TARGET
{
    return WHITE;
}
#endif
