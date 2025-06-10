#ifndef __BLENDSPACE_2D_HLSL__
#define __BLENDSPACE_2D_HLSL__

# ifndef THREAD_X
# error "NumThread Size Not Defined"
# endif

#include "../../ComputeShader/Transform.hlsl"

Texture2D<float4> Anim1 : register(t0);
Texture2D<float4> Anim2 : register(t1);
Texture2D<float4> Anim3 : register(t2);
Texture2D<float4> Anim4 : register(t3);

RWStructuredBuffer<BoneMatrix_t> BoneMatrices : register(u0);


cbuffer CB_Info : register(b0)
{
	int   KeyFrameCurr[4];
	int   KeyFrameNext[4];
	
	float LerpRate[4];

	float alpha[2];

	float2 Padding;
}

[numthreads(THREAD_X, 1, 1)]
void CSMain(uint3 DTid : SV_DISPATCHTHREADID)
{
	const Texture2D<float4> Textures[4] = {
		Anim1, Anim2, Anim3, Anim4
	};
	BoneTRS_t TRS[4];

	const uint BoneIndex = DTid.x;
	uint StructCount, StructStride;
	BoneMatrices.GetDimensions(StructCount, StructStride);
	[flatten]
	if (BoneIndex >= StructCount)
		return ;

	[unroll]
	for(int i = 0 ; i < 4 ; i++)
	{
		TRS[i] = GetInterpolatedBoneTRS(
			Anim1, BoneIndex, KeyFrameCurr[i], KeyFrameNext[i], LerpRate[i]
		);
	}
	
	// BoneMatrices[BoneIndex].M = ToMatrix(lerp(TRS[0], TRS[1], alpha));
}

#endif