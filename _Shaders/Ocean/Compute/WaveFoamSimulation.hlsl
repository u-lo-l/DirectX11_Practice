#ifndef __WAVE_FOAM_SIMULATION_HLSL__
#define __WAVE_FOAM_SIMULATION_HLSL__
# include "../../ComputeShader/Complex.hlsl"
# include "../Ocean.Common.hlsli"

# ifndef THREAD_X
#  error "THREAD_X Not Defined"
# endif
# ifndef THREAD_Y
#  error "THREAD_Y Not Defined"
# endif

# define LEFT 0
# define RIGHT 1
# define TOP 2
# define BOTTOM 3
# define CENTER 4

// #define SHADER_DEBUG_FOAM
Texture2DArray<float4> DisplacementMap : register(t0); // Y-up
RWTexture2DArray<float> FoamTexture : register(u0);

cbuffer CB_TextureDim : register(b0)
{
	float Width;
	float Height;
	float DeltaSeconds;
	float DisplacementMapTiling;

	float FoamThreshold = 0.1f;
	float FoamMultiplier = 1.f;
	float HeightScaler;
	float FoamSharpness = 1.f;
};

const static int2 dUV[5] = {
	int2(-1,  0), // LEFT
	int2( 1,  0), // RIGHT
	int2( 0, -1), // TOP
	int2( 0,  1), // BOTTOM
	int2( 0,  0) // CENTER
};
uint2 GetWrappedTexCord(uint2 UV, int2 Offset);


/**
 * 수폄 방향의 위치는 P(x,z) + sD(x,z) 이다. [P : 위치], [D : 변위], [s : 변위 Scaler]
 * 이 때 미소 구간에 대한 자코비안 행렬은
 * J11 : 1 + s{partial(Dx) / partial(x)}
 * J12 :     s{partial(Dx) / partial(z)}
 * J21 :     s{partial(Dz) / partial(x)}
 * J22 : 1 + s{partial(Dz) / partial(z)}
*/
[numthreads(THREAD_X, THREAD_Y, 1)] // Dispatch(WIDTH / THREAD_X, HEIGHT / THREAD_Y, 3)
void CSMain(uint3 DTID : SV_DISPATCHTHREADID)
{
	int i = 0;
	int2 UV = DTID.xy;
	const float HorizontalScaler = GetHorizontalScaler(DisplacementMapTiling);
	const float VerticalScaler = GetVerticalScaler(HeightScaler, DisplacementMapTiling);
	if (HeightScaler == 0)
	{
		FoamTexture[DTID] = 0;
		return ;
	}
	// Get Current Foam Value By JacobianMat and EigenValue
	float2 HDisp[5];
	[unroll]
	for(i = 0 ; i < 5 ; i++) // Left->Right->Top->Bottom
	{
		uint2 WrappedUV = GetWrappedTexCord(UV, dUV[i]);
		HDisp[i] = DisplacementMap.Load(uint4(WrappedUV, DTID.z, 0)).xy * HorizontalScaler;
	}
	float2x2 J; // JacobianMat
	J._11 = 1.f + (HDisp[RIGHT].x - HDisp[LEFT].x) * 0.5f;
	J._21 =       (HDisp[RIGHT].y - HDisp[LEFT].y) * 0.5f;
	J._12 =       (HDisp[TOP].x - HDisp[RIGHT].x) * 0.5f;
	J._22 = 1.f + (HDisp[TOP].y - HDisp[RIGHT].y) * 0.5f;
	const float Det = (J._11 * J._22) - (J._12 * J._21);
	const float MinEigen = (J._11 + J._22) - sqrt((J._11 + J._22) * (J._11 + J._22) - 4 * Det);
	float CurrFoam = FoamMultiplier * saturate(1 - MinEigen + FoamThreshold);
	CurrFoam = pow(abs(CurrFoam) * 10, FoamSharpness);

	// CurrFoam = pow(abs(CurrFoam) , FoamSharpness) * FoamMultiplier;
	// FoamTexture[DTID] = CurrFoam;
	// return ;


	float PrevFoam = FoamTexture[DTID];
	float AccumulatedFoamValue = 0.f;
	[unroll]
	for(i = 0 ; i < 4 ; i++)
	{
		uint2 WrappedUV = GetWrappedTexCord(UV, dUV[i]);
		float SampleFoam = FoamTexture[uint3(WrappedUV, DTID.z)];
		AccumulatedFoamValue += SampleFoam * 0.25f;
	}
	DeviceMemoryBarrierWithGroupSync();

	const static float FoamBlur = 1.f;
	PrevFoam = lerp(PrevFoam, AccumulatedFoamValue, saturate(DeltaSeconds) * FoamBlur);
	const float FoamFade = 0.055f;
	PrevFoam = saturate(PrevFoam - FoamFade * DeltaSeconds / max(CurrFoam, 0.5f));
	CurrFoam = max(CurrFoam, PrevFoam);

	FoamTexture[DTID] = CurrFoam;
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
