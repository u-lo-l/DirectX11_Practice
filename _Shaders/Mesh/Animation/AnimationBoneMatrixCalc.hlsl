#ifndef __ANIMATION_BONE_MATRIX_CALC_HLSL__
#define __ANIMATION_BONE_MATRIX_CALC_HLSL__

# ifndef THREAD_X
# error "NumThread Size Not Defined"
# endif

#include "../../ComputeShader/Transform.hlsl"

Texture2D<float4> Anim[3] : register(t0);

RWStructuredBuffer<BoneMatrix_t> BoneMatrices : register(u0);

struct KeyFrameData
{
	int CurrentFrame;
	int NextFrame;
	float LerpRate;
	float Weight;
};
cbuffer CB_Info : register(b0)
{
	KeyFrameData AnimData[3];
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

	if (AnimData[0].Weight == 1.f)
	{
		BoneMatrices[BoneIndex].M = ToMatrix(GetInterpolatedBoneTRS(
			Anim[0], BoneIndex, AnimData[0].CurrentFrame, AnimData[0].NextFrame, AnimData[0].LerpRate
		));
		return;
	}

	if (AnimData[2].Weight == 0.f)
	{
		[unroll]
		for (int i = 0 ; i < 2 ; i++)
		{
			TRS[i] = GetInterpolatedBoneTRS(
				Anim[i], BoneIndex, AnimData[i].CurrentFrame, AnimData[i].NextFrame, AnimData[i].LerpRate
			);
		}
		BoneMatrices[BoneIndex].M = ToMatrix(lerp(TRS[0], TRS[1], AnimData[1].Weight));
		return ;
	}

	[unroll]
	for (int i = 0 ; i < 3 ; i++)
	{
		TRS[i] = GetInterpolatedBoneTRS(
			Anim[i], BoneIndex, AnimData[i].CurrentFrame, AnimData[i].NextFrame, AnimData[i].LerpRate
		);
	}
	BoneMatrices[BoneIndex].M = ToMatrix(
		barycentric(TRS[0], TRS[1], TRS[2], AnimData[0].Weight, AnimData[1].Weight, AnimData[2].Weight)
	);
}

#endif