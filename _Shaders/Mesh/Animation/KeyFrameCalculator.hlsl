#ifndef __KEYFRAME_CALCULATOR_HLSL__
#define __KEYFRAME_CALCULATOR_HLSL__

Texture2D KeyFrameTexture : register(t0);
RWByteAddressBuffer BoneMatrix : register(u0);

cbuffer CB_Info : register(b0)
{
    float CurrentTime;
    int   CurrentFrame;
    int   NextFrame;
    int   Duration;
    float TicksPerSeconds = 30.f;
    float3 Padding;
}

[numthreads(THREAD_X, THREAD_Y, 1)]
void CSMain(uint3 DTid : SV_DISPATCHTHREADID)
{
    const int BoneIndex = 0;


    float LerpRate;
    float CurrentFrameTime = CurrentFrame * TicksPerSeconds;
    float NextFrameTime;
    [flatten]
    if (NextFrame < CurrentFrame)
        NextFrameTime = (Duration + NextFrame) * TicksPerSeconds;
    else
        NextFrameTime = NextFrame * TicksPerSeconds;
    [flatten]
    if (NextFrame - CurrentFrame < 0.0001)
        LerpRate = 0;
    else
        LerpRate = (CurrentTime - CurrentFrameTime) / (NextFrameTime - CurrentFrameTime);

    float4 CurrentMatrix[4];
    float4 NextMatrix[4];

    CurrentMatrix[0] = KeyFrameTexture.Load(int3(BoneIndex * 4 + 0, CurrentFrame,0));
    CurrentMatrix[1] = KeyFrameTexture.Load(int3(BoneIndex * 4 + 1, CurrentFrame,0));
    CurrentMatrix[2] = KeyFrameTexture.Load(int3(BoneIndex * 4 + 2, CurrentFrame,0));
    CurrentMatrix[3] = KeyFrameTexture.Load(int3(BoneIndex * 4 + 3, CurrentFrame,0));

    NextMatrix[0] = KeyFrameTexture.Load(int3(BoneIndex * 4 + 0, NextFrame,0));
    NextMatrix[1] = KeyFrameTexture.Load(int3(BoneIndex * 4 + 1, NextFrame,0));
    NextMatrix[2] = KeyFrameTexture.Load(int3(BoneIndex * 4 + 2, NextFrame,0));
    NextMatrix[3] = KeyFrameTexture.Load(int3(BoneIndex * 4 + 3, NextFrame,0));
    
    matrix Curr, Next;
    Curr = matrix(CurrentMatrix[0],CurrentMatrix[1],CurrentMatrix[2],CurrentMatrix[3]);
    Next = matrix(NextMatrix[0],NextMatrix[1],NextMatrix[2],NextMatrix[3]);

    matrix Result = lerp(Curr, Next, LerpRate);
    BoneMatrix.Store4(BoneIndex, asint(Result[0]));
}

#endif