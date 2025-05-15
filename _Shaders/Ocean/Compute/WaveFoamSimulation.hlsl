#ifndef __WAVE_FOAM_SIMULATION_HLSL__
#define __WAVE_FOAM_SIMULATION_HLSL__
# include "../../ComputeShader/Math.hlsl"

# ifndef THREAD_X
#  error "THREAD_X Not Defined"
# endif
# ifndef THREAD_Y
#  error "THREAD_Y Not Defined"
# endif

# define CENTER 0
# define LEFT 1
# define RIGHT 2
# define TOP 3
# define BOTTOM 4

Texture2D<float4> DisplacementMap : register(t0);
RWTexture2D<float> FoamTexture : register(u0);

cbuffer CB_TextureDim : register(b0)
{
	uint Width;
	uint Height;
	float DeltaTime;
	float Padding;
	
	float FoamMultiplier = 1.f;
	float FoamThreshold = 2.f;
	float FoamBlur = 1.f;
	float FoamFade = 0.1f;
};

const static int SamplingSize = 2;
const static int2 FoamSampleOffsets[5] = {
	int2(0, 0),	// CENTER
	int2(-SamplingSize / 2, 0),	// LEFT
	int2( SamplingSize / 2, 0),	// RIGHT
	int2(0, -SamplingSize / 2),	// TOP
	int2(0,  SamplingSize / 2)	// BOTTOM
};

uint2 GetWrappedTexCord(uint2 UV, int2 Offset);

[numthreads(THREAD_X, THREAD_Y, 1)] // Dispatch(WIDTH / THREAD_X, HEIGHT / THREAD_Y, 3)
void CSMain(uint3 Input : SV_DISPATCHTHREADID)
{
	int i = 0;
	uint2 UV = Input.xy;

	// Get Current Foam Value By JacobianMat and EigenValue
	uint2 TexCords[5];
	float2 Horizontal_Disps[5];
	[unroll] for (i = 0 ; i < 5 ; i++)
	{
		TexCords[i] = GetWrappedTexCord(UV, FoamSampleOffsets[i]);
		Horizontal_Disps[i] = DisplacementMap[TexCords[i]].xz;
	}

	float2x2 J; // JacobianMat
	J._11 = 1.f + (float)(Horizontal_Disps[RIGHT].x  - Horizontal_Disps[LEFT].x) / SamplingSize;
	J._12 =       (float)(Horizontal_Disps[BOTTOM].x - Horizontal_Disps[TOP].x)  / SamplingSize;
	J._21 =       (float)(Horizontal_Disps[RIGHT].y  - Horizontal_Disps[LEFT].y) / SamplingSize;
	J._22 = 1.f + (float)(Horizontal_Disps[BOTTOM].y - Horizontal_Disps[TOP].y)  / SamplingSize;
	const float Det = (J._11 * J._22) - (J._12 * J._21);
	const float MinEigen = (J._11 + J._22) - sqrt((J._11 + J._22) * (J._11 + J._22) - 4 * Det);
	float FoamValue = FoamMultiplier * (1 - MinEigen + FoamThreshold);

	FoamTexture[UV] = FoamValue;
	return ;

	// Smooth Foam Value
	float PrevFoam = FoamTexture[UV];
	float AccumulatedFoamValue = 0.f;
	[unroll] for(i = 1 ; i < 5 ; i++)
	{
		uint2 SamplePosition = GetWrappedTexCord(UV, FoamSampleOffsets[i]);
		float SampleFoam = FoamTexture[SamplePosition];
		AccumulatedFoamValue += SampleFoam * 0.25f;
	}
	PrevFoam = lerp(PrevFoam, AccumulatedFoamValue, saturate(DeltaTime) * FoamBlur);
	PrevFoam = saturate(PrevFoam - FoamFade * DeltaTime);
	FoamValue = max(FoamValue, PrevFoam);

	FoamTexture[UV] = FoamValue;
}

uint2 GetWrappedTexCord(uint2 UV, int2 Offset)
{
	int2 TexCord = (int2)(UV) + Offset;
	TexCord.x = (TexCord.x + Width) % Width;
	TexCord.y = (TexCord.y + Height) % Height;
	return uint2(TexCord);
}
#endif
