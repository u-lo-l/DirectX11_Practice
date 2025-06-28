#ifndef __STATIC_MESH_HLSL__
#define __STATIC_MESH_HLSL__

# include "../PerFrame.hlsli"
# include "../ComputeShader/Transform.hlsl"
# include "../Texture.Func.hlsli"
# include "../Shading.Func.hlsli"
# include "../Normal.Func.hlsli"

static const int DiffuseMap = 0;
static const int SpecularMap = 1;
static const int NormalMap = 2;
static const int GlossynessMap = 3;
static const int MetallicMap = 4;
struct VS_Input
{
    float4 Position  : POSITION; // Model의 BindPose
    float2 Uv        : UV;
    float4 Color     : COLOR;
    float3 Normal    : NORMAL;
    float3 Tangent   : TANGENT;

    matrix WorldTfPerInstance : INSTANCE;
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

cbuffer CB_PerMaterial : register(b1)
{
    float4 Ambient;
    float4 Diffuse;

    float4 Specular;
    float Metalic;
    float Roughness;
    float2 Padding_Mat;
}

Texture2D<float4> MaterialMaps[5] : register(t0);
StructuredBuffer<BoneMatrix_t> BoneMatrices : register(t6);
SamplerState LinearSampler : register(s0);

VS_Output VSMain(VS_Input Input)
{
    VS_Output Output;

    Output.Position = mul(Input.Position, Input.WorldTfPerInstance);
    Output.WorldPosition = Output.Position.xyz;

    Output.Position = mul(Output.Position, mul(View, Projection));

    Output.ProjectorNDCPosition = float4(0,0,0,1);
    Output.ShadowPosition = float4(0,0,0,1);

    Output.Uv = Input.Uv;
    Output.Normal =  normalize(mul(Input.Normal, (float3x3)Input.WorldTfPerInstance));
    Output.Tangent = normalize(mul(Input.Tangent, (float3x3)Input.WorldTfPerInstance));

    return Output;
}

float4 PSMain(VS_Output Input) : SV_TARGET
{
    BlinnPhongInput BlinnPhongDesc;
    float4 Result;

    BlinnPhongDesc.Ambient = Ambient;
    BlinnPhongDesc.Diffuse = MaterialMaps[DiffuseMap].Sample(LinearSampler, Input.Uv) * Diffuse;
    BlinnPhongDesc.Specular = MaterialMaps[SpecularMap].Sample(LinearSampler, Input.Uv) * Specular;
    BlinnPhongDesc.Shininess = Specular.a;

    BlinnPhongDesc.LightColor = LightColor;
    BlinnPhongDesc.LightDirection = LightDirection;

    BlinnPhongDesc.Normal = Input.Normal;
    BlinnPhongDesc.WorldPosition = Input.WorldPosition;
    BlinnPhongDesc.WorldSpaceCameraPosition = CameraWorldPosition;

	return BlinnPhong(BlinnPhongDesc);

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