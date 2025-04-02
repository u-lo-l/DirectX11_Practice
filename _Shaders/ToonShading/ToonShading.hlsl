#include "ToonShading.WVP.hlsl"
#include "ToonShading.LightingFunctions.hlsl"

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
    uint InstanceID  : SV_InstanceID;
};

struct VertexOutput
{
    float4 Position      : SV_Position;
    float3 WorldPosition : WPOSITION;
    float2 Uv            : UV;
    float3 Normal        : NORMAL;
    float3 Tangent       : TANGENT;
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
    input.Uv.x *= Tiling.x;
    input.Uv.y *= Tiling.y;
    // float4 NomalMapTexel = MaterialMaps[MATERIAL_TEXTURE_NORMAL].Sample(AnisotropicSampler, input.Uv);
    // input.Normal = NormalMapping(input.Uv, input.Normal, input.Tangent, NomalMapTexel.xyz);
    // return NomalMapTexel;
    float4 A = Ambient;
    float4 D = MaterialMaps[MATERIAL_TEXTURE_DIFFUSE].Sample(LinearSampler, input.Uv) * Diffuse;
    float4 S = MaterialMaps[MATERIAL_TEXTURE_SPECULAR].Sample(LinearSampler, input.Uv) * Specular;

    float3 level = 0.4f + 0.6f * dot(normalize(input.Normal), -LightDirection_PS);
    level = saturate(level);
    
    float4 toon = MaterialMaps[MATERIAL_TEXTURE_NORMAL].Sample(PointSampler, float2(level.x, 0));
    D += toon;

    float4 GlobalAmbient = float4(0.1f,0.1f,0.1f,1.f);

    ColorDesc DirectionalLightColor = ApplyGlobalDirectionalLights(
        LightDirection_PS,
        input.WorldPosition,
        ViewInv_PS._41_42_43,
        input.Normal
    );
    ColorDesc PointLightColor = ApplyPointLights(
        input.WorldPosition,
        ViewInv_PS._41_42_43,
        input.Normal
    );
    ColorDesc SpotLightColor = ApplySpotLights(
        input.WorldPosition,
        ViewInv_PS._41_42_43,
        input.Normal
    );

    ColorDesc OutPut;
    OutPut.Ambient  = GlobalAmbient + DirectionalLightColor.Ambient  + PointLightColor.Ambient  + SpotLightColor.Ambient;
    OutPut.Diffuse  =                 DirectionalLightColor.Diffuse  + PointLightColor.Diffuse  + SpotLightColor.Diffuse;
    OutPut.Specular =                 DirectionalLightColor.Specular + PointLightColor.Specular + SpotLightColor.Specular;
    OutPut.Ambient  *= A;
    OutPut.Diffuse  *= D;
    OutPut.Specular *= S;
    return OutPut.Ambient + OutPut.Diffuse + OutPut.Specular;
}

