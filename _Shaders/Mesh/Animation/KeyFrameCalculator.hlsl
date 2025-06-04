#ifndef __KEYFRAME_CALCULATOR_HLSL__
#define __KEYFRAME_CALCULATOR_HLSL__

# ifndef THREAD_X
# error "NumThread Size Not Defined"
# endif

struct BoneMatrix_s
{
	matrix Transform;
};

Texture2D<float4> KeyFrameTexture : register(t0);
RWStructuredBuffer<BoneMatrix_s> BoneMatrix : register(u0);

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
	matrix Curr, Next;
	const uint BoneIndex = DTid.x;
	uint StructCount, StructStride;
	BoneMatrix.GetDimensions(StructCount, StructStride);
	[flatten]
	if (BoneIndex >= StructCount)
		return ;

	Curr[0] = KeyFrameTexture[uint2(BoneIndex * 4 + 0, CurrentFrame)];
	Curr[1] = KeyFrameTexture[uint2(BoneIndex * 4 + 1, CurrentFrame)];
	Curr[2] = KeyFrameTexture[uint2(BoneIndex * 4 + 2, CurrentFrame)];
	Curr[3] = KeyFrameTexture[uint2(BoneIndex * 4 + 3, CurrentFrame)];

	Next[0] = KeyFrameTexture[uint2(BoneIndex * 4 + 0, NextFrame)];
	Next[1] = KeyFrameTexture[uint2(BoneIndex * 4 + 1, NextFrame)];
	Next[2] = KeyFrameTexture[uint2(BoneIndex * 4 + 2, NextFrame)];
	Next[3] = KeyFrameTexture[uint2(BoneIndex * 4 + 3, NextFrame)];

	BoneMatrix_s Result;
	Result.Transform = lerp(Curr, Next, LerpRate);
	BoneMatrix[BoneIndex] = Result;
}

#endif