#ifndef __PHILIPS_SPECTRUM_UPDATE_HLSL__
# define __PHILIPS_SPECTRUM_UPDATE_HLSL__
# include "../../ComputeShader/Complex.hlsl"

# ifndef THREAD_X
#  error "THREAD_X Not Defined"
# endif
# ifndef THREAD_Y
#  error "THREAD_Y Not Defined"
# endif

# define CASCADE_SIZE 3

const static float Gravity = 9.81f;

cbuffer CB_Const : register(b0)
{
    float Width;
    float Height;
    float Time;
    float Padding;
};

// ========== INPUT ==========
Texture2DArray<float4>    H_Init : register(t0); // size : 3

// ========== OUTPUT =========
// ArraySize = 9 : 3 * CASCADE_SIZE -> {H_t, -ik_xH_t. -ik_yH_t} * CASCADE_SIZE(3)
RWTexture2DArray<Complex> Disp_t : register(u0);

// Dispatch(Size / THREAD_X, Size / THREAD_Y, CASCADE_SIZE)
[numthreads(THREAD_X, THREAD_Y, 1)]
void CSMain(uint3 DTID : SV_DISPATCHTHREADID)
{
	if (Width == 0 || Height == 0)
		return ;

	const uint CascadeChannel = DTID.z;
	const uint2		UV = DTID.xy;
	const uint3		TexCoord = DTID;
	const float4    InitSpectrumValue = H_Init.Load(uint4(TexCoord, 0));
	const Complex	H0                = InitSpectrumValue.xy;
	const Complex	H0_minus_k_conj   = InitSpectrumValue.zw;

	const float2 Position = float2(DTID.x - Width * 0.5f, DTID.y - Height * 0.5f);
	const float2 k = float2(Position.x / Width, Position.y / Height) * 2 * PI;
	const float  OneOverLenghtK = 1 / (length(k) + 0.001);
	const float omega = sqrt(Gravity * length(k));
	const float angle = omega * Time;
	const Complex Exponent = Complex(cos(angle), sin(angle));

	Complex Ht   = ComplexMul(H0, Exponent) + ComplexMul(H0_minus_k_conj, ComplexConj(Exponent));
	Complex iHt  = Complex(-Ht.y, Ht.x);

	// Height에 대한 스펙트럼은 Ht이다.
	// 수평 성분에 대한 스펙트럼은 i(hat_k)Ht이다.
	iHt *= OneOverLenghtK;
	uint UVOffset = CASCADE_SIZE * CascadeChannel;
	Disp_t[uint3(UV, 0 + UVOffset)] = -1.f * iHt * k.x;
	Disp_t[uint3(UV, 1 + UVOffset)] = -1.f * iHt * k.y;
	Disp_t[uint3(UV, 2 + UVOffset)] = Ht;
}

#endif
