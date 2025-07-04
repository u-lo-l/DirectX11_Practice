#ifndef __WAVE_IFFT_COLPASS_HLSL__
#define __WAVE_IFFT_COLPASS_HLSL__
# include "../../ComputeShader/Complex.hlsl"
# include "WaveIFFT_Base.hlsl"

# define CASCADE_SIZE 3
/**
 * DISP : 수평성분 (AXIS_X, AXIS_Z) -> 각각 -ik_xHt, -ik_zHt
 * HEIGHT : 수직성분 (AXIS_Y) -> Ht
*/
#define X 0
#define Y 1
#define Z 2
cbuffer CB_IFFTSize : register(b0)
{
	float  Width;
	float  Height;
	float2 Padding;
}

// {-ik_xH_t. -ik_yH_t, H_t}_CASCASE_1,
// {-ik_xH_t. -ik_yH_t, H_t}_CASCASE_2,
// {-ik_xH_t. -ik_yH_t, H_t}_CASCASE_3,
Texture2DArray<Complex> 	InputSpectrum : register(t0); // ArraySize : 9
// StructuredBuffer<Complex> TwiddleFactor : register(t1);
RWTexture2DArray<float4>	Displacement : register(u0);  // ArraySize : 3

groupshared Complex SharedData[3][FFT_SIZE]; // Shared Data Per ThreadGroup

struct CSInput
{
	uint3 GTid : SV_GroupThreadID;
	uint3 GroupId : SV_GROUPID;
};

uint BitReverse(uint x, uint LogN);
float3 GetDisplacement(uint2 UV);

// THREAD_GROUP_SIZE == FFT_SIZE / 2
[numthreads(THREAD_GROUP_SIZE, 1, 1)] // Dispatch(FFT_SIZE, 1, CASCADE_SIZE)
void CSMain(CSInput Input)
{
	uint CascadeChannel = Input.GroupId.z;

	uint  GTid = Input.GTid.x;
	uint2 FFTCoord = uint2(GTid, Input.GroupId.x);
	uint2 FFTCoordPair	= uint2(GTid + THREAD_GROUP_SIZE, Input.GroupId.x);

	uint ReversedIndex = BitReverse(FFTCoord.x, LOG_N);
	uint ReversedIndexPair = BitReverse(FFTCoordPair.x, LOG_N);

	uint2 SpectrumInputUV  = uint2(ReversedIndex, FFTCoord.y);
	uint2 SpectrumInputUVPair = uint2(ReversedIndexPair, FFTCoordPair.y);

	[unroll]
	for (uint AxisChannel = 0 ; AxisChannel < 3 ; AxisChannel++)
	{
		SharedData[AxisChannel][FFTCoord.x]     = InputSpectrum.Load(uint4(SpectrumInputUV,     AxisChannel + CascadeChannel * 3, 0));
		SharedData[AxisChannel][FFTCoordPair.x] = InputSpectrum.Load(uint4(SpectrumInputUVPair, AxisChannel + CascadeChannel * 3, 0));
		AllMemoryBarrierWithGroupSync();
	}

    float2 WaveVector = GetWaveVector(FFTCoord, float2(Width, Height));
	for(uint s = 1 ; s <= LOG_N ; s++)
	{
		uint u;
		uint v; // pair of u
		Complex Twiddle;
		GetFFTValues(s, FFTCoord.x, WaveVector.x, u, v, Twiddle);

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

	// Flip the sign of odd indices
	float3 Disp = GetDisplacement(FFTCoord) / FFT_SIZE * float(LOG_N);
	Displacement[uint3(FFTCoord, CascadeChannel)] = float4(Disp, 1.f);
	Disp = GetDisplacement(FFTCoordPair) / FFT_SIZE * float(LOG_N);
	Displacement[uint3(FFTCoordPair, CascadeChannel)] = float4(Disp, 1.f);
	return ;
}

// Z-outward Y-downward X-rightward LeftHanded
float3 GetDisplacement(uint2 UV)
{
	int   index = (UV.x + UV.y) % 2;
	float TShiftingCorrection = ((index == 0) ? 1.f : -1.f);

	float dX = SharedData[X][UV.x].x;
	float dY = SharedData[Y][UV.x].x;
	float dZ = SharedData[Z][UV.x].x;
	return float3(dX, dY, dZ) * TShiftingCorrection;
}
#endif