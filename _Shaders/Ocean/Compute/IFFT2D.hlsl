#ifndef __IFFT2D_HLSL__
#define __IFFT2D_HLSL__
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

Texture2D<Complex>        InputSpectrum : register(t0);
// StructuredBuffer<Complex> TwiddleFactor : register(t1);

groupshared Complex SharedData[FFT_SIZE]; // Shared Data Per ThreadGroup

#if defined (ROWPASS)
RWTexture2D<Complex> OutputSpectrum : register(u0);
#elif defined (COLPASS)
# if defined (DISPLACEMENT_GRID)
RWTexture2D<float2> DisplacementGrid : register(u0);
# elif defined (BOTH)
RWTexture2D<float3> DisplacementMap : register(u0);
# else
RWTexture2D<float> HeightMap : register(u0);
# endif
#else
#error "ROW or COL Not Defined"
#endif

struct CSInput
{
	uint3 GTid : SV_GroupThreadID;
	uint GroupId : SV_GROUPID;
};

uint BitReverse(uint x, uint LogN);

/*
* One ThreadGroup Per Row
*/
[numthreads(THREAD_GROUP_COUNT, 1, 1)] // Dispatch(FFT_SIZE, 1, 1)
void CSMain(CSInput Input)
{
	uint Row = Input.GroupId;
	uint GTid = Input.GTid.x;

	uint ReversedIndex1 = BitReverse(GTid, LOG_N);
	uint ReversedIndex2 = BitReverse(GTid + THREAD_GROUP_COUNT, LOG_N);

	uint2 UV_Input  = uint2(Row, ReversedIndex1);
	uint2 UV_Output = uint2(Row, GTid);
#  ifdef HEIGHT_MAP
	SharedData[GTid] = InputSpectrum[UV_Input];
#  else
	float 
	SharedData[GTid] = InputSpectrum[UV_Input];
#  endif
	UV_Input  = uint2(Row, ReversedIndex2);
	UV_Output = uint2(Row, GTid + THREAD_GROUP_COUNT);

#  ifdef HEIGHT_MAP
	SharedData[GTid + THREAD_GROUP_COUNT] = InputSpectrum[UV_Input];
#  else
	SharedData[GTid + THREAD_GROUP_COUNT] = InputSpectrum[UV_Input];
#  endif
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

	#if defined (ROWPASS)
	OutputSpectrum[uint2(Row, GTid)] = SharedData[GTid];
	OutputSpectrum[uint2(Row, GTid + THREAD_GROUP_COUNT)] = SharedData[GTid + THREAD_GROUP_COUNT];
	#elif defined (COLPASS)
	
	// Flip the sign of odd indices
	float perms[] = {1.0,-1.0};
	int   index = (GTid + Row) % 2;
	float perm = perms[index];

	float Height = (SharedData[GTid].x) / (FFT_SIZE * FFT_SIZE);
	HeightMap[uint2(Row, GTid)] = Height * perm;

	Height = (SharedData[GTid + THREAD_GROUP_COUNT].x) / (FFT_SIZE * FFT_SIZE);
	HeightMap[uint2(Row, GTid + THREAD_GROUP_COUNT)] = Height * perm;
	
	#else
	#error "ROW or COL Not Defined"
	#endif
}

// https://graphics.stanford.edu/~seander/bithacks.html#BitReverseObvious
uint BitReverse(uint x, uint LogN)
{
	// swap odd and even bits
	x = ((x >> 1) & 0x55555555) | ((x & 0x55555555) << 1);
	// swap consecutixe pairs
	x = ((x >> 2) & 0x33333333) | ((x & 0x33333333) << 2);
	// swap nibbles ... 
	x = ((x >> 4) & 0x0F0F0F0F) | ((x & 0x0F0F0F0F) << 4);
	// swap bytes
	x = ((x >> 8) & 0x00FF00FF) | ((x & 0x00FF00FF) << 8);
	// swap 2-byte long pairs
	x = ( x >> 16             ) | ( x               << 16);
	return x >> (32 - LogN);
}
#endif