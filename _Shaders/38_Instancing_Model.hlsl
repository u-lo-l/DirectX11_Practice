#include "Slot.hlsl"
#include "00_Material.hlsl"
#include "00_WorldViewProjection.hlsl"

cbuffer CB_BoneMatrix : register(Const_VS_BoneMatrix)
{
    matrix OffsetMatrix[MAX_MODEL_TRANSFORM];      // RootToBone Matrix. == Inv(Bone의 Root-Coordinate Transform)
    matrix BoneTransforms[MAX_MODEL_TRANSFORM];    //
}

cbuffer CB_BoneIndex : register(Const_VS_BoneIndex)
{
    uint BoneIndex;
    float3 Padding;
}

struct VertexInput
{
    float4 Position              : POSITION;
    float2 Uv                    : UV;
    float4 Color                 : COLOR;
    float3 Normal                : NORMAL;
    float3 Tangent               : TANGENT;
    float4 Indices               : BLENDINDICES;
    float4 Weight                : BLENDWEIGHTS;

    row_major float4x4 Transform : INSTANCE;
    // InstanceID : GPU가 자동으로 생성하며, 쉐이더로 전달됩니다. C++ 코드에서 직접 설정할 필요가 없음
    uint InstanceID              : SV_InstanceID;
};

struct VertexOutput
{
    float4 Position              : SV_Position;
    float2 Uv                    : UV;
    float3 Normal                : NORMAL;
};

struct AnimationFrame
{
    uint    Clip;           // 몇 번 째 Clip인지
    float   CurrentTime;
    int     CurrentFrame;   // 그 clip에서 몇 번째 Frame인지
    int     NextFrame;
};

struct BlendingFrame
{
    float BlendingDuration;
    float ElapsedBlendTime;
    float2 Padding;

    AnimationFrame Current;
    AnimationFrame Next;
};

struct InterploateKeyframeParams
{
    int CurrentOrNext;
    int BoneIndex;
    float Weight;
};

cbuffer CB_AnimationBlending : register(Const_VS_AnimationBlending)
{
    BlendingFrame AnimationBlending[MAX_INSTANCE_COUNT];
};

void SetModelWorld(inout matrix world)
{
    world = mul(BoneTransforms[BoneIndex], world);
}

matrix SetModelInstanceWorld(VertexInput input)
{
    // matrix TF = input.Transform;
    matrix TF = WorldTF;
    return mul(BoneTransforms[BoneIndex], TF);
}

/*======================================================================================================*/

Texture2DArray<float4> ClipsTFMap : register(Texture_VS_KeyFrames);

int g_BoneCountToFindWeight = 4;
int g_MipMapLevel = 0;

// Current = 0 , Next = 1
matrix InterpolateKeyFrameMatrix(in InterploateKeyframeParams Params, int InstanceID = 0);

float4 SetAnimatedBoneToWorldTF_Instancing(inout VertexInput input)
{
    int   Indices[4] = { input.Indices.x, input.Indices.y, input.Indices.z, input.Indices.w };
    float Weights[4] = { input.Weight.x, input.Weight.y, input.Weight.z, input.Weight.w };

    float4 pos = 0;
    InterploateKeyframeParams ParamsCurrent;
    InterploateKeyframeParams ParamsNext;

    for(int i = 0 ; i < g_BoneCountToFindWeight ; i++)
    {
        int targetBoneIndex = Indices[i];
        matrix TargetRootBoneInvMat = OffsetMatrix[targetBoneIndex];
        float4 VertexPosInBoneSpace = mul(input.Position, TargetRootBoneInvMat);
        
        matrix currentAnim = 0;
        
        ParamsCurrent.CurrentOrNext = 0;
        ParamsCurrent.BoneIndex = targetBoneIndex;
        ParamsCurrent.Weight = Weights[i];
        currentAnim = InterpolateKeyFrameMatrix(ParamsCurrent, input.InstanceID);

        [flatten]
        if (AnimationBlending[input.InstanceID].Next.Clip >= 0)
        {
            ParamsNext.CurrentOrNext = 1;
            ParamsNext.BoneIndex = targetBoneIndex;
            ParamsNext.Weight = Weights[i];
            currentAnim = lerp(currentAnim, InterpolateKeyFrameMatrix(ParamsNext, input.InstanceID), AnimationBlending[input.InstanceID].ElapsedBlendTime);
        }

        pos += mul(VertexPosInBoneSpace, currentAnim);
    }
    return pos;
}

/*======================================================================================================*/

VertexOutput VSMain(VertexInput input)
{    
    VertexOutput output;
    // matrix TF = input.Transform;
    matrix TF = WorldTF;

    matrix ModelWorldTF = mul(BoneTransforms[BoneIndex], TF);

    output.Position = mul(input.Position, ModelWorldTF);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    
    output.Uv = input.Uv;

    output.Normal = mul(input.Normal, (float3x3) ModelWorldTF);
    
    // output.Position = mul(input.Position, WorldTF);
    // output.Position = mul(output.Position, View);
    // output.Position = mul(output.Position, Projection);
    // output.Uv = input.Uv;
    // output.Normal = mul(input.Normal, (float3x3) WorldTF);
    
    return output;
}

float4 PSMain(VertexOutput input) : SV_Target
{
    float3 normal = normalize(input.Normal);
	float Light = dot(LightDirection, normal);
    float3 color = float3(1,1,1);
    // float3 color = MaterialMaps[0].Sample(Samp, input.Uv).rgb;
    color *= Light;
    return float4(color, 1);
}