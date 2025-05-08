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

cbuffer CB_Const : register(b0)
{
    uint  Width;
    uint  Height;
    float Time;
    float Padding;
};

Texture2D<float2> H_Init : register(t0);
RWTexture2D<float2> H_t  : register(u0);

// Dispatch(Size / THREAD_X, Size / THREAD_Y, 1)
[numthreads(THREAD_X, THREAD_Y, 1)]
void CSMain(uint3 DTID : SV_DISPATCHTHREADID)
{
    if (Width == 0 || Height == 0)
        return ;

    float2 H_Init_pos = H_Init[DTID.xy];
    
    float2 Position = float2(DTID.x - Width * 0.5f, DTID.y - Height * 0.5f);
    float2 k = 2 * PI * float2(Position.x / Width, Position.y / Height);
    float omega = sqrt(Gravity * length(k));
    float angle = omega * Time;

    // Pos Frequency
    float2 H_t_Pos = float2(cos(angle), sin(angle));
    H_t_Pos = ComplexMul(H_Init_pos, H_t_Pos);
    
    // Neg Frequency
    uint2 DTID_Neg;
    DTID_Neg.x = Width - DTID.x - 1;
    DTID_Neg.y = Height - DTID.y - 1;
    float2 H_Init_neg = H_Init[DTID_Neg.xy];
    H_Init_neg.y = -H_Init_neg.y; // conj
    Position = float2(DTID_Neg.x - Width * 0.5f, DTID_Neg.y - Height * 0.5f);
    k = 2 * PI * float2(Position.x / Width, Position.y / Height);
    omega = sqrt(Gravity * length(k));
    angle = -1 * omega * Time;

    float2 H_t_Neg = float2(cos(-angle), sin(-angle));
    H_t_Neg = ComplexMul(H_Init_neg, H_t_Neg);

    H_t[DTID.xy] = H_t_Pos + H_t_Neg;


}

#endif
