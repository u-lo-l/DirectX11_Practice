#ifndef __KEYFRAME_CALCULATOR_HLSL__
#define __KEYFRAME_CALCULATOR_HLSL__

# ifndef THREAD_X
# error "NumThread Size Not Defined"
# endif

#include "../../ComputeShader/Transform.hlsl"

Texture2D<float4> KeyFrameAnimation : register(t0);
RWStructuredBuffer<BoneMatrix_t> BoneMatrix : register(u0);

cbuffer CB_Info : register(b0)
{
	int CurrentFrame;
	int NextFrame;
	float CurrentTime;
	float LerpRate;
}

[numthreads(THREAD_X, 1, 1)]
void CSMain(uint3 DTid : SV_DISPATCHTHREADID)
{
	BoneTRS_t Curr, Next;
	const uint BoneIndex = DTid.x;

	uint StructCount, StructStride;
	BoneMatrix.GetDimensions(StructCount, StructStride);
	[flatten]
	if (BoneIndex >= StructCount)
		return ;

	Curr = GetBoneTRS(KeyFrameAnimation, BoneIndex, CurrentFrame);
	Next = GetBoneTRS(KeyFrameAnimation, BoneIndex, NextFrame);

	BoneMatrix[BoneIndex].M = ToMatrix(lerp(Curr, Next, LerpRate));
}


#endif