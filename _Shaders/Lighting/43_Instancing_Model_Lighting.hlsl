#include "43_WVP.hlsl"
#include "43_LightingMaterial.hlsl"
#include "43_Lighting.hlsl"

const static int g_BoneCountToFindWeight = 4;
const static int g_MipMapLevel = 0;

cbuffer CB_BoneMatrix : register(Const_VS_BoneMatrix)
{
    matrix OffsetMatrix[MAX_MODEL_TRANSFORM];   // (Bone의 Root-Coordinate Transform)의 역행렬
    matrix BoneTransforms[MAX_MODEL_TRANSFORM]; //
}

cbuffer CB_BoneIndex : register(Const_VS_BoneIndex)
{
    uint BaseBoneIndex;
    float3 Padding;
}

struct VertexInput
{
    float4 Position  : POSITION;
    float2 Uv        : UV;
    float4 Color     : COLOR;
    float3 Normal    : NORMAL;
    float3 Tangent   : TANGENT;
    float4 Indices   : BLENDINDICES;
    float4 Weight    : BLENDWEIGHTS;
    matrix Transform : INSTANCE;

    // InstanceID : GPU가 자동으로 생성하며, 쉐이더로 전달됩니다. C++ 코드에서 직접 설정할 필요가 없음
    uint InstanceID              : SV_InstanceID;
};

struct VertexOutput
{
    float4 Position              : SV_Position;
    float3 WorldPosition         : Position1;
    float2 Uv                    : UV;
    float3 Normal                : NORMAL;
    float3 Tangent   : TANGENT;
};

/*======================================================================================================*/

VertexOutput VSMain(VertexInput input)
{    
    matrix TF = input.Transform;
    matrix ModelWorldTF = mul(BoneTransforms[BaseBoneIndex], TF);

    VertexOutput output;
    output.Uv = input.Uv;
    output.Normal = mul(input.Normal, (float3x3) ModelWorldTF);
    output.Tangent = mul(input.Tangent, (float3x3) ModelWorldTF);
    
    output.Position = mul(input.Position, ModelWorldTF);
    output.WorldPosition = output.Position.xyz;

    output.Position = mul(output.Position, View_VS);
    output.Position = mul(output.Position, Projection_VS);
    
    return output;
}

float4 PSMain(VertexOutput input) : SV_Target
{
    float4 NomalMapTexel = MaterialMaps[MATERIAL_TEXTURE_NORMAL].Sample(AnisotropicSampler, input.Uv);
    input.Normal = NormalMapping(input.Uv, input.Normal, input.Tangent, NomalMapTexel.xyz);

    float4 A = float4(1,1,1,1);
    float4 D = MaterialMaps[MATERIAL_TEXTURE_DIFFUSE].Sample(LinearSampler, input.Uv);
    float4 S = MaterialMaps[MATERIAL_TEXTURE_SPECULAR].Sample(LinearSampler, input.Uv);

    float4 GlobalAmbient = float4(0.01f,0.01f,0.01f,0.01f);
    PhongLighting MaterialBaseColor = {A,D,S};
    PhongLightingCoefficent PhongCoeff = {1,1,1};
    float4 Phong = ComputePhongLight(
        LightDirection_PS,
        ViewInv_PS._41_42_43,
        input.Normal,
        input.WorldPosition,
        GlobalAmbient,
        MaterialBaseColor,
        PhongCoeff
    );
    float4 Rim = ComputeRimLight(
        LightDirection_PS,
        ViewInv_PS._41_42_43,
        input.Normal,
        input.WorldPosition,
        0.3f
    );

    return Phong + Rim;
}
