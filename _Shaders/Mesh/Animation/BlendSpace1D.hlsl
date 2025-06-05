#ifndef __BLENDSPACE_1D_HLSL__
#define __BLENDSPACE_1D_HLSL__

# ifndef THREAD_X
# error "NumThread Size Not Defined"
# endif

struct BoneMatrix_s
{
	matrix Transform;
};

Texture2D<float4> Anim1 : register(t0);
Texture2D<float4> Anim2 : register(t1);
RWStructuredBuffer<BoneMatrix_s> BoneMatrices : register(u0);

cbuffer CB_Info : register(b0)
{
	int2   CurrentFrame;
	int2   NextFrame;
	float2 CurrentTime;
	float2 LerpRate;
	float  alpha;
	float3 Padding;
}

matrix GetBoneMatrix(in Texture2D<float4> Animation, uint BoneIndex, uint Frame);

[numthreads(THREAD_X, 1, 1)]
void CSMain(uint3 DTid : SV_DISPATCHTHREADID)
{
	matrix BoneMatrix[2];

	const uint BoneIndex = DTid.x;
	uint StructCount, StructStride;
	BoneMatrices.GetDimensions(StructCount, StructStride);
	[flatten]
	if (BoneIndex >= StructCount)
		return ;

	matrix Curr = GetBoneMatrix(Anim1, BoneIndex, (int)CurrentFrame.x);
	matrix Next = GetBoneMatrix(Anim1, BoneIndex, (int)NextFrame.x);
	BoneMatrix[0] = lerp(Curr, Next, (float)LerpRate.x);

	Curr = GetBoneMatrix(Anim2, BoneIndex, (int)CurrentFrame.y);
	Next = GetBoneMatrix(Anim2, BoneIndex, (int)NextFrame.y);
	BoneMatrix[1] = lerp(Curr, Next, (float)LerpRate.y);

	BoneMatrix_s Result;
	Result.Transform = lerp(BoneMatrix[0], BoneMatrix[1], alpha);
	BoneMatrix[BoneIndex] = Result;
}

matrix GetBoneMatrix(in Texture2D<float4> Animation, uint BoneIndex, uint Frame)
{
	matrix Result;
	Result[0] = Animation[uint2(BoneIndex * 4 + 0, Frame)];
	Result[1] = Animation[uint2(BoneIndex * 4 + 1, Frame)];
	Result[2] = Animation[uint2(BoneIndex * 4 + 2, Frame)];
	Result[3] = Animation[uint2(BoneIndex * 4 + 3, Frame)];
	return Result;
}
#endif