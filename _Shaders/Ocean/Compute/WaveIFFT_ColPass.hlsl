#ifndef __WAVE_IFFT_COLPASS_HLSL__
#define __WAVE_IFFT_COLPASS_HLSL__
# include "../../ComputeShader/Math.hlsl"
# include "WaveIFFT_Base.hlsl"
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
	SharedData[0][DTID_1.x] = InputSpectrum.Load(uint4(UV_Input, 1, 0));
	SharedData[1][DTID_1.x] = InputSpectrum.Load(uint4(UV_Input, 0, 0));
	SharedData[2][DTID_1.x] = InputSpectrum.Load(uint4(UV_Input, 2, 0));

	GroupMemoryBarrierWithGroupSync();

	uint ReversedIndex2 = BitReverse(DTID_2.x, LOG_N);
	UV_Input = uint2(ReversedIndex2, DTID_2.y);
	SharedData[0][DTID_2.x] = InputSpectrum.Load(uint4(UV_Input, 1, 0));
	SharedData[1][DTID_2.x] = InputSpectrum.Load(uint4(UV_Input, 0, 0));
	SharedData[2][DTID_2.x] = InputSpectrum.Load(uint4(UV_Input, 2, 0));
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
	Displacement[DTID_1] = float4(SharedData[0][DTID_1.x].x, SharedData[1][DTID_1.x].x, SharedData[2][DTID_1.x].x, 1) / (FFT_SIZE);
	Displacement[DTID_2] = float4(SharedData[0][DTID_2.x].x, SharedData[1][DTID_2.x].x, SharedData[2][DTID_2.x].x, 1) / (FFT_SIZE);
	// Flip the sign of odd indices ( : Permutation )

	float perms[] = {1.0,-1.0};
	int   index = (DTID_1.x + DTID_1.y) % 2;
	float perm = perms[index];
	Displacement[DTID_1] *= perm * float(LOG_N);
	Displacement[DTID_2] *= perm * float(LOG_N);
}
#endif