#ifndef __MESH_HLSL__
#define __MESH_HLSL__

# define MAX_BONE_COUNT 256
# define MAX_BLENDING_BONE_COUNT 4

# include "../ComputeShader/Transform.hlsl"

static const int DiffuseMap = 0;
static const int SpecularMap = 1;
static const int NormalMap = 2;
struct VS_Input
{
    float4 Position  : POSITION; // Model의 BindPose
    float2 Uv        : UV;
    float4 Color     : COLOR;
    float3 Normal    : NORMAL;
    float3 Tangent   : TANGENT;
    float4 Indices   : BLENDINDICES; // BoneIndices
    float4 Weights   : BLENDWEIGHTS;
    uint   VertexID  : SV_VERTEXID;
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

cbuffer CB_PerFrame : register(b0)
{
    matrix View;
    matrix Projection;
    matrix ViewProjection;
    float3 CameraWorldPosition;
    float  Padding;
    float4 LightColor;
    float3 LightDirection;
    float  Padding2;
}

cbuffer CB_PerMaterial : register(b1)
{
    float4 Ambient;
    float4 Diffuse;

    float4 Specular;
    float Metalic;
    float Roughness;
    float2 Padding_Mat;
}

cbuffer CB_PerModel : register(b2)
{
    matrix OffsetMatrices[MAX_BONE_COUNT]; // Inv(BindPose BoneMatrix)
}
cbuffer CB_PerMesh : register(b3)
{
    matrix World;
}


Texture2D MaterialMaps[3] : register(t0);
StructuredBuffer<BoneMatrix_t> BoneMatrices : register(t3);
SamplerState LinearSampler : register(s0);

float4 BlendPosition(float4 BindPoseInModelSpace, float4 Indicies, float4 Weights)
{
    float4 Result = float4(0,0,0,0);

    [unroll]
    for (int i = 0 ; i < MAX_BLENDING_BONE_COUNT ; i++)
    {
        const int TargetBoneIndex = Indicies[i];
        const float Weight = Weights[i];
        float4 VertexPoseInBoneSpace = mul(BindPoseInModelSpace, OffsetMatrices[TargetBoneIndex]);
        Result += mul(VertexPoseInBoneSpace, BoneMatrices[TargetBoneIndex].M) * Weight;
    }
    return Result;
}

VS_Output VSMain(VS_Input Input)
{
    VS_Output Output;

    Output.Position = BlendPosition(Input.Position, Input.Indices, Input.Weights);
    Output.Position = mul(Output.Position, World);
    Output.WorldPosition = Output.Position.xyz;

    Output.Position = mul(Output.Position, View);
    Output.Position = mul(Output.Position, Projection);

    Output.ProjectorNDCPosition = float4(0,0,0,1);
    Output.ShadowPosition = float4(0,0,0,1);

    Output.Uv = Input.Uv;
    Output.Normal =  normalize(mul(Input.Normal, (float3x3)World));
    Output.Tangent = normalize(mul(Input.Tangent, (float3x3)World));

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