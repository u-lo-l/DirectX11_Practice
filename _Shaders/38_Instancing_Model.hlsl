#include "Slot.hlsl"
#include "00_Material.hlsl"
#include "00_WorldViewProjection.hlsl"

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
    float2 Uv                    : UV;
    float3 Normal                : NORMAL;
};

/*======================================================================================================*/

VertexOutput VSMain(VertexInput input)
{    
    matrix TF = input.Transform;
    // matrix TF = WorldTF;
    matrix ModelWorldTF = mul(BoneTransforms[BaseBoneIndex], TF);

    VertexOutput output;
    output.Uv = input.Uv;
    output.Normal = mul(input.Normal, (float3x3) ModelWorldTF);

    output.Position = mul(input.Position, ModelWorldTF);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    
    return output;
}

float4 PSMain(VertexOutput input) : SV_Target
{
    float3 normal = normalize(input.Normal);
	float Light = dot(LightDirection, normal);
    float3 color = MaterialMaps[DiffuseMap].Sample(DefaultSampler, input.Uv).rgb;
    color *= Light;
    return float4(color, 1);
}
