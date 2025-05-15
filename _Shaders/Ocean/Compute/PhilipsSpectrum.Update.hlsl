#ifndef __PHILIPS_SPECTRUM_UPDATE_HLSL__
# define __PHILIPS_SPECTRUM_UPDATE_HLSL__
# include "../../ComputeShader/Math.hlsl"

# ifndef THREAD_X
#  error "THREAD_X Not Defined"
# endif
# ifndef THREAD_Y
#  error "THREAD_Y Not Defined"
# endif

const static float Gravity = 9.81f;

// ========== INPUT ==========
Texture2D<float4>           H_Init : register(t0);
cbuffer CB_Const : register(b0)
{
    float Width;
    float Height;
    float Time;
    float Padding;
};

// ========== OUTPUT =========

const static uint T_HT = 0;
const static uint T_DISP_X = 1;
const static uint T_DISP_Z = 2;

RWTexture2DArray<float2> H_t : register(u0); // Size : 9

// Dispatch(Size / THREAD_X, Size / THREAD_Y, 1)
[numthreads(THREAD_X, THREAD_Y, 1)]
void CSMain(uint3 DTID : SV_DISPATCHTHREADID)
{
    if (Width == 0 || Height == 0)
        return ;

    const Complex H0              = H_Init[DTID.xy].xy;
    const Complex H0_minus_k_conj = H_Init[DTID.xy].zw;

    const float2 Position = float2(DTID.x - Width * 0.5f, DTID.y - Height * 0.5f);
    const float2 k = float2(Position.x / Width, Position.y / Height) * 2 * PI;
    const float  OneOverLenghtK = 1 / (length(k) + 0.001);
    const float omega = sqrt(Gravity * length(k));
    const float angle = omega * Time;
    const Complex Exponent = Complex(cos(angle), sin(angle));

    Complex Ht = ComplexMul(H0, Exponent) + ComplexMul(H0_minus_k_conj, ComplexConj(Exponent));
    Complex iHt  = Complex(-Ht.y, Ht.x);


    const uint2 UV = DTID.xy;
    H_t[uint3(UV, T_HT)] = Ht;
    H_t[uint3(UV, T_DISP_X)] = iHt * k.x * (-OneOverLenghtK);
    H_t[uint3(UV, T_DISP_Z)] = iHt * k.y * (-OneOverLenghtK);
}

#endif
