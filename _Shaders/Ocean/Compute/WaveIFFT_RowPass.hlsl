#ifndef __WAVE_ROW_PASS_HLSL__
#define __WAVE_ROW_PASS_HLSL__
# include "../../ComputeShader/Math.hlsl"
# include "WaveIFFT_Base.hlsl"

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
	uint3 DTID_1 = uint3(Input.GTid.x, Input.GroupId.x, Input.GroupId.z);
	uint3 DTID_2 = uint3(Input.GTid.x + THREAD_GROUP_COUNT, Input.GroupId.x, Input.GroupId.z);

    float2 WaveVector = GetWaveVector(DTID_1.xy, float2(Width, Height));

	uint ReversedIndex1 = BitReverse(DTID_1.x, LOG_N);
	uint2 UV_Input  = uint2(ReversedIndex1, DTID_1.y);
	SharedData[DTID_1.x] = InputSpectrum.Load(uint4(UV_Input, DTID_1.z, 0));
	GroupMemoryBarrierWithGroupSync();

	uint ReversedIndex2 = BitReverse(DTID_2.x, LOG_N);
	UV_Input = uint2(ReversedIndex2, DTID_2.y);
	SharedData[DTID_2.x] = InputSpectrum.Load(uint4(UV_Input, DTID_2.z, 0));
	GroupMemoryBarrierWithGroupSync();

	for(uint s = 1 ; s <= LOG_N ; s++)
	{
		uint u;
		uint v; // pair of u
		Complex Twiddle;
		GetFFTValues(s, DTID_1.x, WaveVector.x, u, v, Twiddle);

		// Butterfly Op
		Complex EvenTerm = SharedData[u];
		Complex OddTermTwiddle = ComplexMul(Twiddle, SharedData[v]);
		SharedData[u] = EvenTerm + OddTermTwiddle;
		SharedData[v] = EvenTerm - OddTermTwiddle;
		GroupMemoryBarrierWithGroupSync();
	}

	OutputSpectrum[uint3(DTID_1.x, DTID_1.y, DTID_1.z)] = SharedData[DTID_1.x] / (FFT_SIZE);
	OutputSpectrum[uint3(DTID_2.x, DTID_2.y, DTID_2.z)] = SharedData[DTID_2.x] / (FFT_SIZE);
}

#endif