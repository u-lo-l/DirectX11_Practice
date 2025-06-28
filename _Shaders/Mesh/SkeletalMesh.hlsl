#ifndef __SKELETAL_MESH_HLSL__
#define __SKELETAL_MESH_HLSL__

# define MAX_BONE_COUNT 256
# define MAX_BLENDING_BONE_COUNT 4

# include "../PerFrame.hlsli"
# include "../ComputeShader/Transform.hlsl"
# include "../Texture.Func.hlsli"
# include "../Shading.Func.hlsli"
# include "../Normal.Func.hlsli"

static const int NormalMap = 0;
static const int DiffuseMap = 1;
static const int SpecularMap = 2;
static const int Shininessmap = 3;
static const int MetallicMap = 4;
static const int RoughnessMap = 5;

struct VS_Input
{
    float4 Position  : POSITION; // Model의 BindPose
    float2 Uv        : UV;
    float4 Color     : COLOR;
    float3 Normal    : NORMAL;
    float3 Tangent   : TANGENT;
    float4 Indices   : BLENDINDICES; // BoneIndices
    float4 Weights   : BLENDWEIGHTS;
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
    uint  TextureUsageFlags;
    float Padding_Mat;
}

cbuffer CB_PerModel : register(b2)
{
    matrix OffsetMatrices[MAX_BONE_COUNT]; // Inv(BindPose BoneMatrix)
}
cbuffer CB_PerMesh : register(b3)
{
    matrix World;
}

Texture2D MaterialMaps[6] : register(t0);
StructuredBuffer<BoneMatrix_t> BoneMatrices : register(t6);
SamplerState LinearSampler : register(s0);

float4 BlendPosition(float4 BindPoseInModelSpace, float4 Indicies, float4 Weights);
float3 BlendVector(float3 NormalInModelSpace, float4 Indicies, float4 Weights);

VS_Output VSMain(VS_Input Input)
{
    VS_Output Output;

    Output.Position = BlendPosition(Input.Position, Input.Indices, Input.Weights);
	Output.Normal = BlendVector(Input.Normal, Input.Indices, Input.Weights);
	Output.Tangent = BlendVector(Input.Tangent, Input.Indices, Input.Weights);

    Output.Position = mul(Output.Position, World);
    Output.WorldPosition = Output.Position.xyz;

    Output.Position = mul(Output.Position, mul(View, Projection));

    Output.ProjectorNDCPosition = float4(0,0,0,1);
    Output.ShadowPosition = float4(0,0,0,1);

    Output.Uv = Input.Uv;
    Output.Normal = normalize(mul(Output.Normal, (float3x3)World));
    Output.Tangent = normalize(mul(Output.Tangent, (float3x3)World));
    return Output;
}

float4 PSMain(VS_Output Input) : SV_TARGET
{
	float3 TangentSpaceNormal = HasNormalTexture(TextureUsageFlags) ?
        MaterialMaps[NormalMap].Sample(LinearSampler, Input.Uv).rgb : float3(0.5f, 0.5f, 1.f);
	float3 Normal = ApplyNormalMap((TangentSpaceNormal + 1.f) * 0.5f, Input.Normal, Input.Tangent);

    BlinnPhongInput BlinnPhongDesc;

    BlinnPhongDesc.Ambient = Ambient;
    BlinnPhongDesc.Diffuse = HasDiffuseTexture(TextureUsageFlags) ?
        MaterialMaps[DiffuseMap].Sample(LinearSampler, Input.Uv) * Diffuse : Diffuse;
    BlinnPhongDesc.Specular = HasSpecularTexture(TextureUsageFlags) ?
        MaterialMaps[SpecularMap].Sample(LinearSampler, Input.Uv) * Specular : Specular;
    BlinnPhongDesc.Shininess = HasSpecularTexture(TextureUsageFlags) ? Specular.a : 0.f;
    BlinnPhongDesc.LightColor = LightColor;
    BlinnPhongDesc.LightDirection = LightDirection;

    BlinnPhongDesc.Normal = Normal;
    BlinnPhongDesc.WorldPosition = Input.WorldPosition;
    BlinnPhongDesc.WorldSpaceCameraPosition = CameraWorldPosition;

	float4 Color = BlinnPhong(BlinnPhongDesc);
	if (Color.a < 0.5f)
		discard;
	return Color;
    // return VisualizeNormal(Normal, Diffuse.a);
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

float4 BlendPosition(float4 BindPoseInModelSpace, float4 Indicies, float4 Weights)
{
	float4 Result = 0;

	if (all(Weights == 0))
		return BindPoseInModelSpace;

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

float3 BlendVector(float3 NormalInModelSpace, float4 Indicies, float4 Weights)
{
	float3 Result = 0;

	if (all(Weights == 0))
		return NormalInModelSpace;

	[unroll]
	for (int i = 0 ; i < MAX_BLENDING_BONE_COUNT ; i++)
	{
		const int TargetBoneIndex = Indicies[i];
		const float Weight = Weights[i];
		float3 NormalInBoneSpace = mul(NormalInModelSpace, (float3x3)(OffsetMatrices[TargetBoneIndex]));
		Result += mul(NormalInBoneSpace, (float3x3)(BoneMatrices[TargetBoneIndex].M)) * Weight;
	}
	return normalize(Result);
}

#endif