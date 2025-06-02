#ifndef __MESH_HLSL__
#define __MESH_HLSL__

# define MAX_BONE_COUNT 256

struct VS_Input
{
    float4 Position  : POSITION;
    float2 Uv        : UV;
    float4 Color     : COLOR;
    float3 Normal    : NORMAL;
    float3 Tangent   : TANGENT;
    float4 Indices   : BLENDINDICES;
    float4 Weight    : BLENDWEIGHTS;

    matrix InstanceTF: INSTANCE;
    uint InstanceID  : SV_InstanceID;
};

struct VS_Output
{
    float4 Position             : SV_Position;
    float3 WorldPosition        : WPOSITION;
    float4 ProjectorNDCPosition : PROJ_Position;
    float4 ShadowPosition       : SPOSITION;

    float2 Uv            : UV;
    float3 Normal        : NORMAL;
    float3 Tangent       : TANGENT;
};

struct DepthOutput
{
    float4 ShadowPosition : SV_Position;
};

cbuffer CB_Matrix : register(b0) // VS PS
{
    matrix World;
    matrix View;
    matrix Projection;
    matrix ViewInverse;
}

cbuffer CB_Light : register(b1)
{
    float4 LightColor;
    float3 LightDirection;
    float  LightPadding;
}

cbuffer CB_LocalMatrix : register(b2)
{
    matrix LocalTf;
}

cbuffer CB_OffsetMatrix : register(b3)
{
    matrix OffsetMatrix[MAX_BONE_COUNT];
}

static const int DiffuseMap = 0;
static const int SpecularMap = 1;
static const int NormalMap = 2;
Texture2D MaterialMaps[3] : register(t0);
SamplerState LinearSampler : register(s0);


VS_Output VSMain(VS_Input Input)
{
    VS_Output Output;

    const matrix MeshWorldTf = mul(LocalTf, World);

    Output.Position = mul(Input.Position, MeshWorldTf);
    Output.WorldPosition = Output.Position.xyz;

    Output.Position = mul(Output.Position, View);
    Output.Position = mul(Output.Position, Projection);

    Output.ProjectorNDCPosition = float4(0,0,0,1);
    Output.ShadowPosition = float4(0,0,0,1);

    Output.Uv = Input.Uv;
    Output.Normal = normalize(mul(Input.Normal, (float3x3)MeshWorldTf));
    Output.Tangent = normalize(mul(Input.Tangent, (float3x3)MeshWorldTf));
    return Output;
}

float4 PSMain(VS_Output Input) : SV_TARGET
{
    float LDotN = saturate(dot(Input.Normal, -LightDirection));
    float3 Color = LightColor.rgb * LDotN;
    return float4(Color, 1);
}

DepthOutput VSShadow(VS_Input Input)
{
    DepthOutput output;
    output.ShadowPosition = Input.Position;
    return output;
}

float4 PSShadow(DepthOutput Input) : SV_Target
{
    return float4(1,1,1,1);
}
#endif