#ifndef __MODEL_ANIMATION_STRUCTURE_HLSL__
# define __MODEL_ANIMATION_STRUCTURE_HLSL__

#include "Model.Slot.hlsl"
#include "Model.Struct.hlsl"

const static int g_BoneCountToFindWeight = 4;
const static int g_MipMapLevel = 0;

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

static const int CURR_FRAME = 0;
static const int NEXT_FRAME = 1;
struct InterploateKeyframeParams
{
    int TargetFrame;
    int BoneIndex;
    float Weight;
};

cbuffer CB_AnimationBlending : register(Const_VS_AnimationBlending)
{
    BlendingFrame AnimationBlending[MAX_INSTANCE_COUNT];
};

Texture2DArray<float4> ClipsTFMap : register(Texture_VS_KeyFrames);

#endif