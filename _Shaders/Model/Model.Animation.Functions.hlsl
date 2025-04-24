#ifndef __MODEL_ANIMATION_FUNCTIONS_HLSL__
# define __MODEL_ANIMATION_FUNCTIONS_HLSL__

#include "Model.Slot.hlsl"
#include "Model.Struct.hlsl"
#include "Model.Resources.hlsl"
#include "Model.Animation.Structure.hlsl"

matrix GetClipTransformMatrix(int BoneIndex, int TargetFrame, int ClipIndex);
matrix InterpolateKeyFrameMatrix(in InterploateKeyframeParams Params, int InstanceID = 0);

// Input으로 들어오는 Vertex의 Position은 Model의 Root-Coordinate를 기준으로하는 정보.
// 이를 Mesh의 기준 Bone의 Coordinate으로 변환한 후 처리해야 한다.
// BindPoseLocalPosition이란, BindPose(T-Pose or A-Pose)상태에서의 Vertex의 위치(Root 기준)를 의미한다.
// 
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
        float4 VertexPosInBoneSpace = mul(input.Position, OffsetMatrix[targetBoneIndex]);
        
        matrix currentAnim = 0;
        
        ParamsCurrent.TargetFrame = CURR_FRAME;
        ParamsCurrent.BoneIndex = targetBoneIndex;
        ParamsCurrent.Weight = Weights[i];

        currentAnim = InterpolateKeyFrameMatrix(ParamsCurrent, input.InstanceID);

        [flatten]
        if (AnimationBlending[input.InstanceID].Next.Clip >= 0)
        {
            ParamsNext.TargetFrame = NEXT_FRAME;
            ParamsNext.BoneIndex = targetBoneIndex;
            ParamsNext.Weight = Weights[i];
            currentAnim = lerp(currentAnim, InterpolateKeyFrameMatrix(ParamsNext, input.InstanceID), AnimationBlending[input.InstanceID].ElapsedBlendTime);
        }

        pos += mul(VertexPosInBoneSpace, currentAnim);
    }
    return pos;
}

/*======================================================================================================*/

matrix InterpolateKeyFrameMatrix(in InterploateKeyframeParams Params, int InstanceID)
{
    int     AnimationIndex[2]   = {AnimationBlending[InstanceID].Current.Clip,              AnimationBlending[InstanceID].Next.Clip};
    int     CurrentFrame[2]     = {AnimationBlending[InstanceID].Current.CurrentFrame,      AnimationBlending[InstanceID].Next.CurrentFrame};
    int     NextFrame[2]        = {AnimationBlending[InstanceID].Current.NextFrame,         AnimationBlending[InstanceID].Next.NextFrame};
    float   Time[2]             = {frac(AnimationBlending[InstanceID].Current.CurrentTime), frac(AnimationBlending[InstanceID].Next.CurrentTime)};

    const int TargetAnimationIndex = AnimationIndex[Params.TargetFrame];
    const int TargetCurrentFrame = CurrentFrame[Params.TargetFrame];
    const int TargetNextFrame = NextFrame[Params.TargetFrame];
    const int InterpRate = Time[Params.TargetFrame];

    matrix current = GetClipTransformMatrix(Params.BoneIndex, TargetCurrentFrame, TargetAnimationIndex);
    current = mul(current, Params.Weight);

    matrix next = GetClipTransformMatrix(Params.BoneIndex, TargetNextFrame, TargetAnimationIndex);
    next = mul(next, Params.Weight);

    return lerp(current, next, InterpRate);
}

// Load : Texel Data를 Filtering이나 Sampling 없이 읽는다.
// https://learn.microsoft.com/ko-kr/windows/win32/direct3dhlsl/dx-graphics-hlsl-to-load
matrix GetClipTransformMatrix(int BoneIndex, int TargetFrame, int ClipIndex)
{
    float4 ClipTransform[4];

    ClipTransform[0] = ClipsTFMap.Load(int4(BoneIndex * g_BoneCountToFindWeight + 0, TargetFrame, ClipIndex, g_MipMapLevel));
    ClipTransform[1] = ClipsTFMap.Load(int4(BoneIndex * g_BoneCountToFindWeight + 1, TargetFrame, ClipIndex, g_MipMapLevel));
    ClipTransform[2] = ClipsTFMap.Load(int4(BoneIndex * g_BoneCountToFindWeight + 2, TargetFrame, ClipIndex, g_MipMapLevel));
    ClipTransform[3] = ClipsTFMap.Load(int4(BoneIndex * g_BoneCountToFindWeight + 3, TargetFrame, ClipIndex, g_MipMapLevel));
    
    return matrix(ClipTransform[0], ClipTransform[1], ClipTransform[2], ClipTransform[3]);
}

#endif