#ifndef __ANIMATION_BONE_MATRIX_CALC_HLSL__
#define __ANIMATION_BONE_MATRIX_CALC_HLSL__

# ifndef THREAD_X
# error "NumThread Dimension Not Defined"
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

// Dispatch( BoneCount / THREAD_X + 1, 1, 1 )
[numthreads(THREAD_X, 1, 1)]
void CSMain(uint3 DTid : SV_DISPATCHTHREADID)
{
	const uint BoneIndex = DTid.x;
	BoneTRS_t TRS[3];

	uint StructCount, StructStride;
	BoneMatrices.GetDimensions(StructCount, StructStride);
	[flatten]
	if (BoneIndex >= StructCount)
		return ;

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