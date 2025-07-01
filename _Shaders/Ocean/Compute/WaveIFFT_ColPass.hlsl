#ifndef __WAVE_IFFT_COLPASS_HLSL__
#define __WAVE_IFFT_COLPASS_HLSL__
# include "../../ComputeShader/Complex.hlsl"
# include "WaveIFFT_Base.hlsl"

#define X 0
#define Y 1
#define Z 2
#define HEIGHT 0
#define DISP_X 1
#define DISP_Z 2
cbuffer CB_IFFTSize : register(b0)
{
    float  Width;
    float  Height;
    float2 Padding;
}

Texture2DArray<Complex> InputSpectrum : register(t0);	// H_t, iH_t. ikH_t
// StructuredBuffer<Complex> TwiddleFactor : register(t1);
RWTexture2D<float4>	Displacement : register(u0);

groupshared Complex SharedData[3][FFT_SIZE]; // Shared Data Per ThreadGroup

struct CSInput
{
	uint3 GTid : SV_GroupThreadID;
	uint3 GroupId : SV_GROUPID;
};

uint BitReverse(uint x, uint LogN);

/*
* One ThreadGroup Per Row
*/
[numthreads(THREAD_GROUP_SIZE, 1, 1)] // Dispatch(FFT_SIZE, 1, 3)
void CSMain(CSInput Input)
{
	uint2 DTID_1 = uint2(Input.GTid.x, Input.GroupId.x);
	uint2 DTID_2 = uint2(Input.GTid.x + THREAD_GROUP_SIZE, Input.GroupId.x);

    float2 WaveVector = GetWaveVector(DTID_1.xy, float2(Width, Height));

	uint ReversedIndex1 = BitReverse(DTID_1.x, LOG_N);
	uint2 UV_Input  = uint2(ReversedIndex1, DTID_1.y);
	SharedData[X][DTID_1.x] = InputSpectrum.Load(uint4(UV_Input, DISP_X, 0));
	SharedData[Y][DTID_1.x] = InputSpectrum.Load(uint4(UV_Input, HEIGHT, 0));
	SharedData[Z][DTID_1.x] = InputSpectrum.Load(uint4(UV_Input, DISP_Z, 0));

	GroupMemoryBarrierWithGroupSync();

	uint ReversedIndex2 = BitReverse(DTID_2.x, LOG_N);
	UV_Input = uint2(ReversedIndex2, DTID_2.y);
	SharedData[X][DTID_2.x] = InputSpectrum.Load(uint4(UV_Input, DISP_X, 0));
	SharedData[Y][DTID_2.x] = InputSpectrum.Load(uint4(UV_Input, HEIGHT, 0));
	SharedData[Z][DTID_2.x] = InputSpectrum.Load(uint4(UV_Input, DISP_Z, 0));
	GroupMemoryBarrierWithGroupSync();

	for(uint s = 1 ; s <= LOG_N ; s++)
	{
		uint u;
		uint v; // pair of u
		Complex Twiddle;
		GetFFTValues(s, DTID_1.x, WaveVector.x, u, v, Twiddle);

		// Butterfly Op : Dx -> Height -> Dz
		[unroll] for (uint i = 0 ; i < 3 ; i++)
		{
			Complex EvenTerm = SharedData[i][u];
			Complex OddTermTwiddle = ComplexMul(Twiddle, SharedData[i][v]);
			SharedData[i][u] = EvenTerm + OddTermTwiddle;
			SharedData[i][v] = EvenTerm - OddTermTwiddle;
			GroupMemoryBarrierWithGroupSync();
		}
	}

	// Transpose
	float3 Disp1 = float3(SharedData[Z][DTID_1.x].x, SharedData[Y][DTID_1.x].x, SharedData[X][DTID_1.x].x) / (FFT_SIZE);
	float3 Disp2 = float3(SharedData[Z][DTID_2.x].x, SharedData[Y][DTID_2.x].x, SharedData[X][DTID_2.x].x) / (FFT_SIZE);

	// Flip the sign of odd indices ( : Permutation )
	float perms[] = {1.0,-1.0};
	int   index = (DTID_1.x + DTID_1.y) % 2;
	float perm = perms[index];
	Disp1 *= perm * float(LOG_N);
	Disp2 *= perm * float(LOG_N);

	Displacement[DTID_1] = float4((Disp1 + 1.f * 0.5f), 1.f) ;
	Displacement[DTID_2] = float4((Disp2 + 1.f * 0.5f), 1.f);
}
#endif