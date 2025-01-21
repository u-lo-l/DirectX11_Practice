#include "00_Context.fx"
#include "00_Material.fx"

#define MAX_MODEL_TRANSFORM 256
cbuffer CB_ModelBones
{
    matrix BoneTransforms[MAX_MODEL_TRANSFORM]; // WorldToBone Matrix. == Inv(Bone의 WorldTransform)
    uint BoneIndex;
};

struct VertexInput
{
    float4 Position : Position;
    float2 Uv : Uv;
    float4 Color : Color;
    float3 Normal : Normal;
    float3 Tangent : Tangent;
    float4 Indices : BlendIndices;
    float4 Weight : BlendWeights;
};
 
struct VertexOutput
{
    float4 Position : SV_Position;
};

struct AnimationFrame
{
    uint Clip;          // 몇 번 째 clip인지
    uint CurrentFrame;  // 그 clip에서 몇 번째 Frame인지지

    float2 Padding;
};

cbuffer CB_AnimationFrame
{
    AnimationFrame KeyFrameData;
};

Texture2DArray ClipsTFMap;

void GetBoneWorldTransform(inout matrix BoneTransform)
{
    ClipsTFMap.Load
}

VertexOutput VS(VertexInput input)
{    
    VertexOutput output;
    matrix BoneWorldTF;
    GetBoneWorldTransform(BoneWorldTF);

    // float4 VertexInWorldOfBasicBoneSpace = input.Position;
    // float4 VertexInLocalOfBasicBoneSpace = mul(VertexInWorldOfBasicBoneSpace, BoneTransforms[BoneIndex]);
    // matrix AnimatedBoneSpaceToWorldTF;

    // SetAnimatedBoneToWorldTF(AnimatedBoneSpaceToWorldTF, input);
    
    // output.Position = mul(VertexInLocalOfBasicBoneSpace, AnimatedBoneSpaceToWorldTF);
    // output.Position = mul(output.Position, View);
    // output.Position = mul(output.Position, Projection);
    
    output.Normal = mul(input.Normal, (float3x3)AnimatedBoneSpaceToWorldTF);
    
    output.Uv = input.Uv;
    
    return output;
}

SamplerState Samp;
float4 PS(VertexOutput input) : SV_Target
{
    return float4(1,0,0,1);
}

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}

/*===========================================================================*/