#ifndef __PHILIPS_SPECTRUM_INITIALIZE_HLSL__
#define __PHILIPS_SPECTRUM_INITIALIZE_HLSL__
# include "../../ComputeShader/Complex.hlsl"

# ifndef THREAD_X
#  error "THREAD_X Not Defined"
# endif
# ifndef THREAD_Y
#  error "THREAD_Y Not Defined"
# endif

const static float Gravity = 9.81f;

cbuffer CB_Philips : register(b0)
{
	float Width;
	float Height;
	float2 Wind;

	float LowCutoff0;
	float LowCutoff1;
	float LowCutoff2;
	float P1;
	float HighCutoff0;
	float HighCutoff1;
	float HighCutoff2;
	float P2;
}

Texture2D<Complex> GaussianNoise : register(t0);
RWTexture2DArray<float4> InitialSpectrum : register(u0);

float GetPhillipSpectrum(uint3 UV);

// Dispatch(Size / THREAD_X, Size / THREAD_Y, CASCADE_SIZE)
[numthreads(THREAD_X, THREAD_Y, 1)]
void CSMain(uint3 DTID : SV_DISPATCHTHREADID)
{
	uint3 DTID1 = DTID.xyz;
	uint3 DTID2 = uint3((Width - DTID.x) % Width, (Height - DTID.y) % Height, DTID.z);

	float PhilipFactor = 0;

	PhilipFactor = GetPhillipSpectrum(DTID1);
	Complex H1 = PhilipFactor == 0 ? Complex(0, 0) : GaussianNoise[DTID1.xy] * sqrt(PhilipFactor * 0.5f);
	PhilipFactor = GetPhillipSpectrum(DTID2);
	Complex H2 = PhilipFactor == 0 ? Complex(0, 0) : GaussianNoise[DTID2.xy] * sqrt(PhilipFactor * 0.5f);

	InitialSpectrum[DTID] = float4(H1, ComplexConj(H2));
}

float GetPhillipSpectrum(uint3 DTID)
{
	float LowCutoff[3] = { LowCutoff0, LowCutoff1, LowCutoff2 };
	float HighCutoff[3] = { HighCutoff0, HighCutoff1, HighCutoff2 };
	float2 Position = float2(DTID.x - Width * 0.5f, DTID.y - Height * 0.5f);
	float2 k = 2 * PI * float2(Position.x / Width, Position.y / Height);

	float2 w = normalize(Wind);
	float WindSpeed = length(Wind);
	float L = (WindSpeed * WindSpeed) / Gravity;

	float kLength = length(k); // Pervent ZeroDividing
	if(kLength > LowCutoff[DTID.z] && kLength < HighCutoff[DTID.z])
	{
		float2 kDir = normalize(k);
		float  kDotw = dot(kDir, w);
		return (kLength < EPSILON) ? 0 : 2 * exp(-1 / pow(kLength * L , 2)) / pow(kLength, 4) * pow(kDotw, 2);
	}
	else
		return 0;
}

#endif