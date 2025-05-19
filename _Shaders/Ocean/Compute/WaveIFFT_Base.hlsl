#ifndef __WAVE_IFFT_BASE_HLSL__
#define __WAVE_IFFT_BASE_HLSL__
# include "../../ComputeShader/Math.hlsl"

# ifndef FFT_SIZE
#  define FFT_SIZE 512
# endif

# ifndef THREAD_GROUP_SIZE
#  define THREAD_GROUP_SIZE 256 // N / 2
# endif

# ifndef LOG_N
#  define LOG_N 9
# endif


// https://graphics.stanford.edu/~seander/bithacks.html#BitReverseObvious
// 01 swap odd and even bits
// 02 swap consecutixe pairs
// 03 swap nibbles ... 
// 04 swap bytes
// 05 swap 2-byte long pairs
uint BitReverse(uint x, uint LogN)
{
	x = ((x >> 1) & 0x55555555) | ((x & 0x55555555) << 1);
	x = ((x >> 2) & 0x33333333) | ((x & 0x33333333) << 2);
	x = ((x >> 4) & 0x0F0F0F0F) | ((x & 0x0F0F0F0F) << 4);
	x = ((x >> 8) & 0x00FF00FF) | ((x & 0x00FF00FF) << 8);
	x = ( x >> 16             ) | ( x               << 16);
	return x >> (32 - LogN);
}

float2 GetWaveVector(uint2 UV, float2 TextureSize)
{
	float2 Position = float2(UV.x - TextureSize.x * 0.5f, UV.y - TextureSize.y * 0.5f);
    return Position / TextureSize;
}

void GetFFTValues(
	in uint Stage,
	in uint X,
	in float kx,
	out uint u,
	out uint v,
	out Complex Twiddle
)
{
	uint FFTSize = 1 << Stage;
	uint FFTStride = FFTSize >> 1;
	uint GroupCount = 1 << (uint(LOG_N) - Stage);
	uint Base = (X / FFTStride) % GroupCount * FFTSize;
	uint k = X % FFTStride;

	float Angle = -2.0f * PI / FFTSize;
	Angle = (Angle * float(k)) % (2 * PI);
	u = Base + k;
	v = u + FFTStride;
	Twiddle = Complex(cos(Angle), sin(Angle));
	// 매번 cos, sin 계산하는 대신 CPU에서 미리 계산해서
	// Structured Buffer나 Constant Buffer로 넘겨줄 수 있음.
	// const uint TwiddleIndex = (1 << (s - 1)) - 1 + k;
	// Complex Twiddle = TwiddleFactor[TwiddleIndex];
}

/*============================================================================
	float2 WaveVector = GetWaveVector(uint2(GTid, Row), float2(Width, Height));

	for(uint s = 1 ; s <= LOG_N ; s++)
	{
		uint u;
		uint v; // pair of u
		Complex Twiddle;
		GetFFTValues(s, DTID_1.x, WaveVector.x, u, v, Twiddle);

		// Butterfly Op
		Complex EvenTerm = SharedData[u];
		Complex OddTermTwiddle = ComplexMul(Twiddle, SharedData[v]);
		SharedData[u] = EvenTerm + OddTermTwiddle ;
		SharedData[v] = EvenTerm - OddTermTwiddle ; // pair of u
		GroupMemoryBarrierWithGroupSync();
	}
=============================================================================*/


#endif