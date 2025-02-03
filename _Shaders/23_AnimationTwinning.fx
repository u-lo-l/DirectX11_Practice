#include "00_Context.fx"
#include "00_Material.fx"

#define MAX_MODEL_TRANSFORM 256
cbuffer CB_ModelBones
{
    matrix RootToBoneTF[MAX_MODEL_TRANSFORM]; // RootToBone Matrix. == Inv(Bone의 Root-Coordinate Transform)
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
    float2 Uv : Uv;
    float3 Normal : Normal;
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
    float TakeTime;
    float ChangingTime;
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
    BlendingFrame AnimationBlending;
};

Texture2DArray<float4> ClipsTFMap;

int BoneCountToFindWeight = 4;
int MipMapLevel = 0;

// Current = 0 , Next = 1
matrix InterpolateKeyFrameMatrix(in InterploateKeyframeParams Params);

float4 SetAnimatedBoneToWorldTF(VertexInput input)
{
    int   Indices[4] = { input.Indices.x, input.Indices.y, input.Indices.z, input.Indices.w };
    float Weights[4] = { input.Weight.x, input.Weight.y, input.Weight.z, input.Weight.w };

    float4 pos = 0;
    InterploateKeyframeParams Params;

    // 4개의 Bone에 대해서 Weight를 탐색.
    for(int i = 0 ; i < BoneCountToFindWeight ; i++)
    {
        int targetBoneIndex = Indices[i];
        matrix TargetRootBoneInvMat = RootToBoneTF[targetBoneIndex];
        float4 VertexPosInBoneSpace = mul(input.Position, TargetRootBoneInvMat);
        
        matrix currentAnim = 0;
        
        Params.CurrentOrNext = 0;
        Params.BoneIndex = targetBoneIndex;
        Params.Weight = Weights[i];
        currentAnim = InterpolateKeyFrameMatrix(Params);

        [flatten] // https://learn.microsoft.com/ko-kr/windows/win32/direct3dhlsl/dx-graphics-hlsl-if
        if (AnimationBlending.Next.Clip > -1)
        {
            Params.CurrentOrNext = 1;
            currentAnim = lerp(currentAnim, InterpolateKeyFrameMatrix(Params), AnimationBlending.ChangingTime);
        }

        pos += mul(VertexPosInBoneSpace, currentAnim);
    }
    return pos;
}

VertexOutput VS(VertexInput input)
{    
    VertexOutput output;
    output.Position = SetAnimatedBoneToWorldTF(input); // Local_Space(Bone Root Space)
    output.Position = mul(output.Position, ModelWorldTF);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    
    output.Normal = input.Normal;
    output.Uv = input.Uv;
    
    return output;
}

SamplerState Samp;
float4 PS(VertexOutput input) : SV_Target
{
    float3 normal = normalize(input.Normal);
	float Light = dot(-LightDirection, normal);
    float3 color = MaterialMaps[0].Sample(Samp, input.Uv).rgb;
    color *= Light;
    return float4(color, 1);
}

RasterizerState FillMode_Wireframe
{
    FillMode = Wireframe;
};

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }

    pass P1
    {
        SetRasterizerState(FillMode_Wireframe);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }

}

/*===========================================================================*/

matrix InterpolateKeyFrameMatrix(in InterploateKeyframeParams Params)
{
    int AnimationIndex[2] = {AnimationBlending.Current.Clip, AnimationBlending.Next.Clip};
    float CurrentFrame[2] = {AnimationBlending.Current.CurrentFrame, AnimationBlending.Next.CurrentFrame};
    int NextFrame[2] = {AnimationBlending.Current.NextFrame, AnimationBlending.Next.NextFrame};
    float Time[2] = {frac(AnimationBlending.Current.CurrentTime), frac(AnimationBlending.Next.CurrentTime)};

    const int TargetCurrentFrame = CurrentFrame[Params.CurrentOrNext];
    const int TargetAnimationIndex = AnimationIndex[Params.CurrentOrNext];
    const int TargetNextFrame = NextFrame[Params.CurrentOrNext];
    const int InterpRate = Time[Params.CurrentOrNext];

    float4 ClipTransform[4];
    matrix current = 0;
    matrix next = 0;

    ClipTransform[0] = ClipsTFMap.Load(int4(Params.BoneIndex * BoneCountToFindWeight + 0, TargetCurrentFrame, TargetAnimationIndex, MipMapLevel));
    ClipTransform[1] = ClipsTFMap.Load(int4(Params.BoneIndex * BoneCountToFindWeight + 1, TargetCurrentFrame, TargetAnimationIndex, MipMapLevel));
    ClipTransform[2] = ClipsTFMap.Load(int4(Params.BoneIndex * BoneCountToFindWeight + 2, TargetCurrentFrame, TargetAnimationIndex, MipMapLevel));
    ClipTransform[3] = ClipsTFMap.Load(int4(Params.BoneIndex * BoneCountToFindWeight + 3, TargetCurrentFrame, TargetAnimationIndex, MipMapLevel));
    //이렇게 접근하면 안 된다. 2차원 배열은 일반적으로 Arr[row][col]이지만 Texture는 Tex[col][row]
    // ClipTransform[0] = ClipsTFMap.Load(int4(CurrentFrame, Indices[i] * 4 + 0, AnimationIndex, 0));
    current = matrix(ClipTransform[0], ClipTransform[1], ClipTransform[2], ClipTransform[3]);
    current = mul(current, Params.Weight);

    ClipTransform[0] = ClipsTFMap.Load(int4(Params.BoneIndex * BoneCountToFindWeight + 0, TargetNextFrame, TargetAnimationIndex, MipMapLevel));
    ClipTransform[1] = ClipsTFMap.Load(int4(Params.BoneIndex * BoneCountToFindWeight + 1, TargetNextFrame, TargetAnimationIndex, MipMapLevel));
    ClipTransform[2] = ClipsTFMap.Load(int4(Params.BoneIndex * BoneCountToFindWeight + 2, TargetNextFrame, TargetAnimationIndex, MipMapLevel));
    ClipTransform[3] = ClipsTFMap.Load(int4(Params.BoneIndex * BoneCountToFindWeight + 3, TargetNextFrame, TargetAnimationIndex, MipMapLevel));
    next = matrix(ClipTransform[0], ClipTransform[1], ClipTransform[2], ClipTransform[3]);
    next = mul(next, Params.Weight);

    return lerp(current, next, InterpRate);
}