#define MAX_MODEL_TRANSFORM 256
#define MAX_INSTANCE_COUNT 200
cbuffer CB_ModelBones
{
    matrix OffsetMatrix[MAX_MODEL_TRANSFORM]; // RootToBone Matrix. == Inv(Bone의 Root-Coordinate Transform)
    matrix BoneTransforms[MAX_MODEL_TRANSFORM];
};
cbuffer CB_BoneIndex
{
    uint BoneIndex;
    float3 Padding;
}
struct ModelInstanceVertexInput
{
    float4 Position : POSITION;
    float2 Uv : Uv;
    float4 Color : COLOR;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float4 Indices : BLENDINDICES;
    float4 Weight : BLENDWEIGHTS;

    matrix Transform : INSTANCE;
    // InstanceID : GPU가 자동으로 생성하며, 쉐이더로 전달됩니다. C++ 코드에서 직접 설정할 필요가 없음
    uint InstanceID : SV_InstanceID;
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

cbuffer CB_AnimationBlending
{
    BlendingFrame AnimationBlending[MAX_INSTANCE_COUNT];
};


void SetModelWorld(inout matrix world)
{
    world = mul(BoneTransforms[BoneIndex], world);
}

matrix SetModelInstanceWorld(ModelInstanceVertexInput input)
{
    return mul(BoneTransforms[BoneIndex], input.Transform);
}

/*======================================================================================================*/

Texture2DArray<float4> ClipsTFMap;

int g_BoneCountToFindWeight = 4;
int g_MipMapLevel = 0;

// Current = 0 , Next = 1
matrix InterpolateKeyFrameMatrix(in InterploateKeyframeParams Params, int InstanceID = 0);

float4 SetAnimatedBoneToWorldTF_Instancing(inout ModelInstanceVertexInput input)
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

/*===========================================================================*/

matrix ClipTransformMatrix(int BoneIndex, int TargetFrame, int ClipIndex);
matrix InterpolateKeyFrameMatrix(in InterploateKeyframeParams Params, int InstanceID)
{
    int     AnimationIndex[2]   = {AnimationBlending[InstanceID].Current.Clip,              AnimationBlending[InstanceID].Next.Clip};
    int     CurrentFrame[2]     = {AnimationBlending[InstanceID].Current.CurrentFrame,      AnimationBlending[InstanceID].Next.CurrentFrame};
    int     NextFrame[2]        = {AnimationBlending[InstanceID].Current.NextFrame,         AnimationBlending[InstanceID].Next.NextFrame};
    float   Time[2]             = {frac(AnimationBlending[InstanceID].Current.CurrentTime), frac(AnimationBlending[InstanceID].Next.CurrentTime)};

    const int TargetAnimationIndex = AnimationIndex[Params.CurrentOrNext];
    const int TargetCurrentFrame = CurrentFrame[Params.CurrentOrNext];
    const int TargetNextFrame = NextFrame[Params.CurrentOrNext];
    const int InterpRate = Time[Params.CurrentOrNext];

    matrix current = ClipTransformMatrix(Params.BoneIndex, TargetCurrentFrame, TargetAnimationIndex);
    current = mul(current, Params.Weight);

    matrix next = ClipTransformMatrix(Params.BoneIndex, TargetNextFrame, TargetAnimationIndex);
    next = mul(next, Params.Weight);

    return lerp(current, next, InterpRate);
}

matrix ClipTransformMatrix(int BoneIndex, int TargetFrame, int ClipIndex)
{
    float4 ClipTransform[4];

    ClipTransform[0] = ClipsTFMap.Load(int4(BoneIndex * g_BoneCountToFindWeight + 0, TargetFrame, ClipIndex, g_MipMapLevel));
    ClipTransform[1] = ClipsTFMap.Load(int4(BoneIndex * g_BoneCountToFindWeight + 1, TargetFrame, ClipIndex, g_MipMapLevel));
    ClipTransform[2] = ClipsTFMap.Load(int4(BoneIndex * g_BoneCountToFindWeight + 2, TargetFrame, ClipIndex, g_MipMapLevel));
    ClipTransform[3] = ClipsTFMap.Load(int4(BoneIndex * g_BoneCountToFindWeight + 3, TargetFrame, ClipIndex, g_MipMapLevel));
    
    matrix ret  = matrix(ClipTransform[0], ClipTransform[1], ClipTransform[2], ClipTransform[3]);
    return ret;
}