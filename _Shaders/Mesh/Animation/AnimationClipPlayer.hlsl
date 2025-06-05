#ifndef __ANIMATION_CLIP_PLAYER_HLSL__
#define __ANIMATION_CLIP_PLAYER_HLSL__

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
	float LerpRate;
	float Padding;
}

[numthreads(THREAD_X, 1, 1)]
void CSMain(uint3 DTid : SV_DISPATCHTHREADID)
{
	const uint BoneIndex = DTid.x;

	uint StructCount, StructStride;
	BoneMatrix.GetDimensions(StructCount, StructStride);
	[flatten]
	if (BoneIndex >= StructCount)
		return ;

	BoneMatrix[BoneIndex].M = ToMatrix(
		GetInterpolatedBoneTRS(
			KeyFrameAnimation, BoneIndex, CurrentFrame, NextFrame, LerpRate
		)
	);
}


#endif