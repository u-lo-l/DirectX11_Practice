#define MAX_MODEL_TRANSFORM 256
cbuffer CB_ModelBones
{
    matrix RootToBoneTF[MAX_MODEL_TRANSFORM]; // RootToBone Matrix. == Inv(Bone의 Root-Coordinate Transform)
    uint BoneIndex;
};

struct AnimationFrame
{
    uint    Clip;           // 몇 번 째 Clip인지
    float   CurrentTime; // 그 clip에서 몇 번째 Frame인지
    int     CurrentFrame;
    int     NextFrame;
};

struct BlendingFrame
{
    float TakeTime;
    float ChangingTime;
    float2 Padding;

    AnimationFrame Current;
    AnimationFrame Next;
};

cbuffer CB_AnimationBlending
{
    BlendingFrame AnimationBlending;
};

struct ModelVertexInput
{
    float4 Position : Position;
    float2 Uv : Uv;
    float4 Color : Color;
    float3 Normal : Normal;
    float3 Tangent : Tangent;
    float4 Indices : BlendIndices;
    float4 Weight : BlendWeights;
};

void SetModelWorld(inout matrix world, ModelVertexInput input)
{
    world = mul(RootToBoneTF[BoneIndex], world);
}

/*======================================================================================================*/

Texture2DArray<float4> ClipsTFMap;

int BoneCountToFindWeight = 4;
int MipMapLevel = 0;

float4 SetAnimatedBoneToWorldTF(ModelVertexInput input)
{
    int   Indices[4] = { input.Indices.x, input.Indices.y, input.Indices.z, input.Indices.w };
    float Weights[4] = { input.Weight.x, input.Weight.y, input.Weight.z, input.Weight.w };

    int AnimationIndex;
    float CurrentFrame;
    int NextFrame;
    float Time;


    AnimationIndex = AnimationBlending.Current.Clip;
    CurrentFrame = AnimationBlending.Current.CurrentFrame;
    NextFrame = AnimationBlending.Current.NextFrame;
    Time = frac(AnimationBlending.Current.CurrentTime);

    float4 ClipTransform[4];

    matrix current = 0;
    matrix next = 0;
    float4 pos = 0;

    // 4개의 Bone에 대해서 Weight를 탐색.
    for(int i = 0 ; i < BoneCountToFindWeight ; i++)
    {
        int targetBoneIndex = Indices[i];
        matrix TargetRootBoneInvMat = RootToBoneTF[targetBoneIndex];
        float4 VertexPosInBoneSpace = mul(input.Position, TargetRootBoneInvMat);

        // Indices[i] : 이 Vertex가 참조 할 Bone. -> Indices[i] * 4 + j : 몇 째 열의 데이터인지
        // CurrentFrame : 몇 째 행의 데이터인지지
        // Clip : 몇 번 째 Animation인지. 지금은 하나만 있다.
        // 0 : MipMap Level : 사용하지 않는다. 0최상위 해상도를 사용한다는 의미.
        ClipTransform[0] = ClipsTFMap.Load(int4(targetBoneIndex * BoneCountToFindWeight + 0, CurrentFrame, AnimationIndex, MipMapLevel));
        ClipTransform[1] = ClipsTFMap.Load(int4(targetBoneIndex * BoneCountToFindWeight + 1, CurrentFrame, AnimationIndex, MipMapLevel));
        ClipTransform[2] = ClipsTFMap.Load(int4(targetBoneIndex * BoneCountToFindWeight + 2, CurrentFrame, AnimationIndex, MipMapLevel));
        ClipTransform[3] = ClipsTFMap.Load(int4(targetBoneIndex * BoneCountToFindWeight + 3, CurrentFrame, AnimationIndex, MipMapLevel));
        //이렇게 접근하면 안 된다. 2차원 배열은 일반적으로 Arr[row][col]이지만 Texture는 Tex[col][row]
        // ClipTransform[0] = ClipsTFMap.Load(int4(CurrentFrame, Indices[i] * 4 + 0, AnimationIndex, 0));
        current = matrix(ClipTransform[0], ClipTransform[1], ClipTransform[2], ClipTransform[3]);
        current = mul(current, Weights[i]);

        ClipTransform[0] = ClipsTFMap.Load(int4(targetBoneIndex * BoneCountToFindWeight + 0, NextFrame, AnimationIndex, MipMapLevel));
        ClipTransform[1] = ClipsTFMap.Load(int4(targetBoneIndex * BoneCountToFindWeight + 1, NextFrame, AnimationIndex, MipMapLevel));
        ClipTransform[2] = ClipsTFMap.Load(int4(targetBoneIndex * BoneCountToFindWeight + 2, NextFrame, AnimationIndex, MipMapLevel));
        ClipTransform[3] = ClipsTFMap.Load(int4(targetBoneIndex * BoneCountToFindWeight + 3, NextFrame, AnimationIndex, MipMapLevel));
        next = matrix(ClipTransform[0], ClipTransform[1], ClipTransform[2], ClipTransform[3]);
        next = mul(next, Weights[i]);

        current = lerp(current, next, Time);

        pos += mul(VertexPosInBoneSpace, current);
    }
    return pos;
}