#ifndef __BLENDSPACE_1D_HLSL__
#define __BLENDSPACE_1D_HLSL__

# ifndef THREAD_X
# error "NumThread Size Not Defined"
# endif

#include "../../ComputeShader/Transform.hlsl"


Texture2D<float4> Anim1 : register(t0);
Texture2D<float4> Anim2 : register(t1);
RWStructuredBuffer<BoneMatrix_t> BoneMatrices : register(u0);

cbuffer CB_Info : register(b0)
{
	int2   KeyFrameCurr;
	int2   KeyFrameNext;
	
	float2 LerpRate;

	float  alpha;
	float  Padding;
}

[numthreads(THREAD_X, 1, 1)]
void CSMain(uint3 DTid : SV_DISPATCHTHREADID)
{
	BoneTRS_t TRS[2];

	const uint BoneIndex = DTid.x;
	uint StructCount, StructStride;
	BoneMatrices.GetDimensions(StructCount, StructStride);
	[flatten]
	if (BoneIndex >= StructCount)
		return ;

	TRS[0] = GetInterpolatedBoneTRS(
		Anim1, BoneIndex, KeyFrameCurr.x, KeyFrameNext.x, LerpRate.x
	);
	TRS[1] = GetInterpolatedBoneTRS(
		Anim2, BoneIndex, KeyFrameCurr.y, KeyFrameNext.y, LerpRate.y
	);

	BoneMatrices[BoneIndex].M = ToMatrix(lerp(TRS[0], TRS[1], alpha));
}

#endif