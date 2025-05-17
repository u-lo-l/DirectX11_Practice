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
SamplerState LinearSampler_Wrap : register(s0);
cbuffer CB_TextureDim : register(b0)
{
	float Width;
	float Height;
	float DeltaSeconds;
	float Padding;
	
	float FoamMultiplier = 1.f;
	float FoamThreshold = 1.f;
	float FoamBlur = 1.f;
	float FoamFade = 0.1f;
};

const static int2 FoamSampleOffsets[5] = {
	int2( 0,  0),	// CENTER
	int2(-1,  0),	// LEFT
	int2( 1,  0),	// RIGHT
	int2( 0, -1),	// TOP
	int2( 0,  1)	// BOTTOM
};

uint2 GetWrappedTexCord(uint2 UV, int2 Offset);

[numthreads(THREAD_X, THREAD_Y, 1)] // Dispatch(WIDTH / THREAD_X, HEIGHT / THREAD_Y, 3)
void CSMain(uint3 Input : SV_DISPATCHTHREADID)
{
	int i = 0;
	int2 UV = Input.xy;

	// Get Current Foam Value By JacobianMat and EigenValue
	float2 Horizontal_Disps[5];
	[unroll] for (i = 0 ; i < 5 ; i++)
	{
		// float2 TexCord = float2(UV.x / Width, UV.y / Height) + TexelSize * FoamSampleOffsets[i];
		// Horizontal_Disps[i] = DisplacementMap.SampleLevel(LinearSampler_Wrap, TexCord, 0).xz;

		uint2 TexCord = GetWrappedTexCord(UV, FoamSampleOffsets[i]);
		Horizontal_Disps[i] = DisplacementMap[TexCord].xz;
	}

	float2x2 J; // JacobianMat
	J._11 = 1.f + (Horizontal_Disps[RIGHT].x  - Horizontal_Disps[LEFT].x) / 2;
	J._12 =       (Horizontal_Disps[BOTTOM].x - Horizontal_Disps[TOP].x) / 2;
	J._21 =       (Horizontal_Disps[RIGHT].y  - Horizontal_Disps[LEFT].y) / 2;
	J._22 = 1.f + (Horizontal_Disps[BOTTOM].y - Horizontal_Disps[TOP].y) / 2;
	const float Det = (J._11 * J._22) - (J._12 * J._21);
	const float MinEigen = (J._11 + J._22) - sqrt((J._11 + J._22) * (J._11 + J._22) - 4 * Det);
	float FoamValue = FoamMultiplier * saturate(1 - MinEigen + FoamThreshold) * 10;

	float PrevFoam = FoamTexture[UV];
	float AccumulatedFoamValue = 0.f;
	[unroll] for(i = 1 ; i < 5 ; i++)
	{
		uint2 TexCord = GetWrappedTexCord(UV, FoamSampleOffsets[i]);
		float SampleFoam = FoamTexture[TexCord];
		AccumulatedFoamValue += SampleFoam * 0.25f;
	}
	PrevFoam = lerp(PrevFoam, AccumulatedFoamValue, saturate(DeltaSeconds) * FoamBlur);
	PrevFoam = saturate(PrevFoam - FoamFade * DeltaSeconds);
	FoamValue = max(FoamValue, PrevFoam);

	FoamTexture[UV] = FoamValue;
}

uint2 GetWrappedTexCord(uint2 UV, int2 Offset)
{
	int2 wrapped = int2(UV) + Offset;

	// 수동으로 wrap
	wrapped.x = (wrapped.x + int(Width)) % (Width);
	wrapped.y = (wrapped.y + int(Height)) % (Height);

	return uint2(wrapped);
}
#endif
