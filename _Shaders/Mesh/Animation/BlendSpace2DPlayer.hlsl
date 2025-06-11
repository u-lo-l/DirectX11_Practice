#ifndef __BLENDSPACE_2D_HLSL__
#define __BLENDSPACE_2D_HLSL__

# ifndef THREAD_X
# error "NumThread Size Not Defined"
# endif

#include "../../ComputeShader/Transform.hlsl"

Texture2D<float4> Anim1 : register(t0);
Texture2D<float4> Anim2 : register(t1);
Texture2D<float4> Anim3 : register(t2);

RWStructuredBuffer<BoneMatrix_t> BoneMatrices : register(u0);

cbuffer CB_Info : register(b0)
{
	int   KeyFrameCurr[3];
	int   KeyFrameNext[3];
	float LerpRate[3];
	float Weight[3];

	float4 Padding;
}

[numthreads(THREAD_X, 1, 1)]
void CSMain(uint3 DTid : SV_DISPATCHTHREADID)
{
	BoneTRS_t TRS[3];

	const uint BoneIndex = DTid.x;
	uint StructCount, StructStride;
	BoneMatrices.GetDimensions(StructCount, StructStride);
	[flatten]
	if (BoneIndex >= StructCount)
		return ;

	TRS[0] = GetInterpolatedBoneTRS(
		Anim1, BoneIndex, KeyFrameCurr[0], KeyFrameNext[0], LerpRate[0]
	);
	TRS[1] = GetInterpolatedBoneTRS(
		Anim2, BoneIndex, KeyFrameCurr[1], KeyFrameNext[1], LerpRate[1]
	);
	TRS[2] = GetInterpolatedBoneTRS(
		Anim1, BoneIndex, KeyFrameCurr[2], KeyFrameNext[2], LerpRate[2]
	);

	BoneMatrices[BoneIndex].M = ToMatrix(barycentric(TRS[0], TRS[1], TRS[2], Weight[0], Weight[1], Weight[2]));
}

#endif