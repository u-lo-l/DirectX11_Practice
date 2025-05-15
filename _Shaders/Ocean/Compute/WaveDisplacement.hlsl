#ifndef __WAVE_DISPLACEMENT_HLSL__
#define __WAVE_DISPLACEMENT_HLSL__
# include "../../ComputeShader/Math.hlsl"

# ifndef FFT_SIZE
#  define FFT_SIZE 512
# endif

# ifndef THREAD_GROUP_COUNT
#  define THREAD_GROUP_COUNT 256 // N / 2
# endif

# ifndef LOG_N
#  define LOG_N 9
# endif

# ifndef DISPLACEMENT_GRID
# define HEIGHT_MAP 1
# endif

cbuffer CB_IFFTSize : register(b0)
{
    float  Width;
    float  Height;
    float2 Padding;
}

Texture2DArray<Complex>		InputSpectrum : register(t0);	// H_t, iH_t. ikH_t
// StructuredBuffer<Complex> TwiddleFactor : register(t1);
RWTexture2DArray<Complex>	OutputSpectrum : register(u0);	// HEIGHT, NORMALX, NORMALZ

groupshared Complex SharedData[FFT_SIZE]; // Shared Data Per ThreadGroup
struct CSInput
{
	uint3 DTID : SV_DispatchThreadID;
	uint3 GTid : SV_GroupThreadID;
	uint3 GroupId : SV_GROUPID;
};

uint BitReverse(uint x, uint LogN);

/*
* One ThreadGroup Per Row
*/
[numthreads(THREAD_GROUP_COUNT, 1, 1)] // Dispatch(FFT_SIZE, 1, ArraySize)
void CSMain(CSInput Input)
{
	uint Row = Input.GroupId.x;
	uint GTid = Input.GTid.x;
	uint TextureType = Input.GroupId.z;

    float2 Position = float2(GTid - Width * 0.5f, Row - Height * 0.5f);
    float2 WaveVector = float2(Position.x / Width, Position.y / Height);
    WaveVector = normalize(WaveVector);

	uint ReversedIndex1 = BitReverse(GTid, LOG_N);
	uint2 UV_Input  = uint2(ReversedIndex1, Row);
	SharedData[GTid] = InputSpectrum.Load(uint4(UV_Input, TextureType, 0));

	GroupMemoryBarrierWithGroupSync();

	uint ReversedIndex2 = BitReverse(GTid + THREAD_GROUP_COUNT, LOG_N);
	UV_Input  = uint2(ReversedIndex2, Row);
	SharedData[GTid + THREAD_GROUP_COUNT] = InputSpectrum.Load(uint4(UV_Input, TextureType, 0));
	GroupMemoryBarrierWithGroupSync();


	for(uint s = 1 ; s <= LOG_N ; s++)
	{
		uint FFTSize = 1 << s;
		uint FFTStride = FFTSize >> 1;
		uint GroupCount = 1 << (LOG_N - s);
		float Angle = -2.0f * PI / FFTSize;
			
		uint Base = (GTid / FFTStride) % GroupCount * FFTSize;
		uint k = GTid % FFTStride;

		uint u = Base + k;
		uint v = u + FFTStride;

		// 매번 cos, sin 계산하는 대신 CPU에서 미리 계산해서
		// Structured Buffer나 Constant Buffer로 넘겨줄 수 있음.
		Complex Twiddle;
		Angle = (Angle * float(k)) % (2 * PI);
		Twiddle.x = cos(Angle);
		Twiddle.y = sin(Angle);
		// const uint TwiddleIndex = (1 << (s - 1)) - 1 + k;
		// Complex Twiddle = TwiddleFactor[TwiddleIndex];

		// Butterfly Op
		Complex EvenTerm = SharedData[u];
		Complex OddTermTwiddle = ComplexMul(Twiddle, SharedData[v]);
		SharedData[u] = EvenTerm + OddTermTwiddle ;
		SharedData[v] = EvenTerm - OddTermTwiddle ; // pair of u
		GroupMemoryBarrierWithGroupSync();
	}

	OutputSpectrum[uint3(GTid, Row, TextureType)] = SharedData[GTid] / (FFT_SIZE);
	OutputSpectrum[uint3(GTid + THREAD_GROUP_COUNT, Row, TextureType)] = SharedData[GTid + THREAD_GROUP_COUNT] / (FFT_SIZE);
#if defined (COLPASS)
	// Flip the sign of odd indices
	float perms[] = {1.0,-1.0};
	int   index = (GTid + Row) % 2;
	float perm = perms[index];
	OutputSpectrum[uint3(GTid, Row, TextureType)] *= perm;
	OutputSpectrum[uint3(GTid + THREAD_GROUP_COUNT, Row, TextureType)] *= perm;
#endif
}

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
#endif