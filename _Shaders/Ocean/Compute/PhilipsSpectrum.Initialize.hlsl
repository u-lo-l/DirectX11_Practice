#ifndef __PHILIPS_SPECTRUM_INITIALIZE_HLSL__
#define __PHILIPS_SPECTRUM_INITIALIZE_HLSL__
# include "../../ComputeShader/Complex.hlsl"

# ifndef THREAD_X
#  error "THREAD_X Not Defined"
# endif
# ifndef THREAD_Y
#  error "THREAD_Y Not Defined"
# endif

# define CASCADE_SIZE 3

const static float Gravity = 9.81f;

struct CascadeDesc
{
	float LengthScale;
	float LowCutoff;
	float HighCutoff;
	float Padding;
};
cbuffer CB_Philips : register(b0)
{
    float Width;
    float Height;
    float2 Wind;
	CascadeDesc CascadeData[CASCADE_SIZE];
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
	float2 Position = float2(DTID.x - Width * 0.5f, DTID.y - Height * 0.5f);
	float2 k = 2 * PI * float2(Position.x / Width, Position.y / Height);

	float2 w = normalize(Wind);
	float WindSpeed = length(Wind);
	float L = (WindSpeed * WindSpeed) / Gravity;

	float kLength = length(k); // Pervent ZeroDividing
	if(kLength > CascadeData[DTID.z].LowCutoff && kLength < CascadeData[DTID.z].HighCutoff)
	{
		float2 kDir = normalize(k);
		float  kDotw = dot(kDir, w);
		return (kLength < EPSILON) ? 0 : 2 * exp(-1 / pow(kLength * L , 2)) / pow(kLength, 4) * pow(kDotw, 2);
	}
	return 0;
}

#endif