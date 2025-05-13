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

#if defined (ROWPASS)
Texture2D<Complex>        InputSpectrum : register(t0);
#elif defined (COLPASS)
Texture2DArray<float2>    InputSpectrum : register(t0); // Size : 3
#else
#error "ROW or COL Not Defined"
#endif
// StructuredBuffer<Complex> TwiddleFactor : register(t1);

groupshared Complex SharedData_X[FFT_SIZE]; // Shared Data Per ThreadGroup
groupshared Complex SharedData_Y[FFT_SIZE]; // Shared Data Per ThreadGroup
groupshared Complex SharedData_Z[FFT_SIZE]; // Shared Data Per ThreadGroup

#if defined (ROWPASS)
RWTexture2DArray<float2> OutputSpectrum : register(u0); // Size : 3
#elif defined (COLPASS)
RWTexture2D<float4> DisplacementGrid : register(u0);
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

    float2 Position = float2((float)GTid - (float)Width * 0.5f, (float)Row - (float)Height * 0.5f);
    float2 WaveVector = float2(Position.x / Width, Position.y / Height);
    WaveVector = length(WaveVector) < EPSILON ? float2(0, 0) : normalize(WaveVector);

    Complex DisplacementFactor_X = Complex(0, -WaveVector.x);
	Complex DisplacementFactor_Y = Complex(1, 0);
    Complex DisplacementFactor_Z = Complex(0, -WaveVector.y);


	uint ReversedIndex1 = BitReverse(GTid, LOG_N);
	uint2 UV_Input  = uint2(Row, ReversedIndex1);
	uint2 UV_Output = uint2(Row, GTid);
#if defined(ROWPASS)
	SharedData_X[GTid] = ComplexMul(InputSpectrum[UV_Input], DisplacementFactor_X);
	SharedData_Y[GTid] = ComplexMul(InputSpectrum[UV_Input], DisplacementFactor_Y);
	SharedData_Z[GTid] = ComplexMul(InputSpectrum[UV_Input], DisplacementFactor_Z);
#elif defined (COLPASS)
	SharedData_X[GTid] = ComplexMul(InputSpectrum.Load(uint4(UV_Input, 0, 0)), DisplacementFactor_X);
	SharedData_Y[GTid] = ComplexMul(InputSpectrum.Load(uint4(UV_Input, 1, 0)), DisplacementFactor_Y);
	SharedData_Z[GTid] = ComplexMul(InputSpectrum.Load(uint4(UV_Input, 2, 0)), DisplacementFactor_Z);
#else
#error "ROW or COL Not Defined"
#endif

	uint ReversedIndex2 = BitReverse(GTid + THREAD_GROUP_COUNT, LOG_N);
	UV_Input  = uint2(Row, ReversedIndex2);
	UV_Output = uint2(Row, GTid + THREAD_GROUP_COUNT);
#if defined(ROWPASS)
	SharedData_X[GTid + THREAD_GROUP_COUNT] = ComplexMul(InputSpectrum[UV_Input], DisplacementFactor_X);
	SharedData_Y[GTid + THREAD_GROUP_COUNT] = ComplexMul(InputSpectrum[UV_Input], DisplacementFactor_Y);
	SharedData_Z[GTid + THREAD_GROUP_COUNT] = ComplexMul(InputSpectrum[UV_Input], DisplacementFactor_Z);
#elif defined (COLPASS)
	SharedData_X[GTid + THREAD_GROUP_COUNT] = ComplexMul(InputSpectrum.Load(uint4(UV_Input, 0, 0)), DisplacementFactor_X);
	SharedData_Y[GTid + THREAD_GROUP_COUNT] = ComplexMul(InputSpectrum.Load(uint4(UV_Input, 1, 0)), DisplacementFactor_Y);
	SharedData_Z[GTid + THREAD_GROUP_COUNT] = ComplexMul(InputSpectrum.Load(uint4(UV_Input, 2, 0)), DisplacementFactor_Z);
#else
#error "ROW or COL Not Defined"
#endif

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

		// Butterfly Op _X
		Complex EvenTerm = SharedData_X[u];
		Complex OddTermTwiddle = ComplexMul(Twiddle, SharedData_X[v]);
		SharedData_X[u] = EvenTerm + OddTermTwiddle ;
		SharedData_X[v] = EvenTerm - OddTermTwiddle ; // pair of u

		// Butterfly Op _Y
		EvenTerm = SharedData_Y[u];
		OddTermTwiddle = ComplexMul(Twiddle, SharedData_Y[v]);
		SharedData_Y[u] = EvenTerm + OddTermTwiddle ;
		SharedData_Y[v] = EvenTerm - OddTermTwiddle ;

        // Butterfly Op _Z
        EvenTerm = SharedData_Z[u];
		OddTermTwiddle = ComplexMul(Twiddle, SharedData_Z[v]);
		SharedData_Z[u] = EvenTerm + OddTermTwiddle ;
		SharedData_Z[v] = EvenTerm - OddTermTwiddle ;
		GroupMemoryBarrierWithGroupSync();
	}

#if defined (ROWPASS)
	OutputSpectrum[uint3(Row, GTid, 0)] = SharedData_X[GTid];
	OutputSpectrum[uint3(Row, GTid, 1)] = SharedData_Y[GTid];
    OutputSpectrum[uint3(Row, GTid, 2)] = SharedData_Z[GTid];
	OutputSpectrum[uint3(Row, GTid + THREAD_GROUP_COUNT, 0)] = SharedData_X[GTid + THREAD_GROUP_COUNT];
	OutputSpectrum[uint3(Row, GTid + THREAD_GROUP_COUNT, 1)] = SharedData_Y[GTid + THREAD_GROUP_COUNT];
	OutputSpectrum[uint3(Row, GTid + THREAD_GROUP_COUNT, 2)] = SharedData_Z[GTid + THREAD_GROUP_COUNT];
#elif defined (COLPASS)
	// Flip the sign of odd indices
	float perms[] = {1.0,-1.0};
	int   index = (GTid + Row) % 2;
	float perm = perms[index];

	float3 Displacement = float3(
		SharedData_X[GTid].x,
		SharedData_Y[GTid].x,
		SharedData_Z[GTid].x
	) / (FFT_SIZE * FFT_SIZE);
	DisplacementGrid[uint2(Row, GTid)] = float4(Displacement * perm, 1);

	Displacement = float3(
		SharedData_X[GTid + THREAD_GROUP_COUNT].x,
		SharedData_Y[GTid + THREAD_GROUP_COUNT].x,
		SharedData_Z[GTid + THREAD_GROUP_COUNT].x
	) / (FFT_SIZE * FFT_SIZE);
	DisplacementGrid[uint2(Row, GTid + THREAD_GROUP_COUNT)] = float4(Displacement * perm, 1);
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