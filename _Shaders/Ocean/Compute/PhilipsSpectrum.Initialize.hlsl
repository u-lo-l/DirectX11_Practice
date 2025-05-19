#ifndef __PHILIPS_SPECTRUM_INITIALIZE_HLSL__
#define __PHILIPS_SPECTRUM_INITIALIZE_HLSL__
# include "../../ComputeShader/Math.hlsl"

# ifndef THREAD_X
#  error "THREAD_X Not Defined"
# endif
# ifndef THREAD_Y
#  error "THREAD_Y Not Defined"
# endif

const static float Gravity = 9.81f;

cbuffer CB_Philips : register(b0)
{
    float Width;
    float Height;
    float2 Wind;
}

Texture2D<Complex> GaussianNoise : register(t0);
RWTexture2D<float4> InitialSpectrum : register(u0);

float GetPhillipSpectrum(uint2 UV);

// Dispatch(Size / THREAD_X, Size / THREAD_Y, 1)
[numthreads(THREAD_X, THREAD_Y, 1)]
void CSMain(uint3 DTID : SV_DISPATCHTHREADID)
{
    uint2 UV1 = DTID.xy;
    uint2 UV2 = uint2((Width - DTID.x) % Width, (Height - DTID.y) % Height);
    Complex H1 = GaussianNoise[UV1] * sqrt(GetPhillipSpectrum(UV1) * 0.5f);
    Complex H2 = GaussianNoise[UV2] * sqrt(GetPhillipSpectrum(UV2) * 0.5f);
    InitialSpectrum[DTID.xy] = float4(H1, ComplexConj(H2));
}

float GetPhillipSpectrum(uint2 UV)
{
    float2 Position = float2(UV.x - Width * 0.5f, UV.y - Height * 0.5f);
    float2 k = 2 * PI * float2(Position.x / Width, Position.y / Height);

    float2 w = normalize(Wind);
    float WindSpeed = length(Wind);
    float L = (WindSpeed * WindSpeed) / Gravity;

    float  kMag = length(k);
    float2 kDir = normalize(k);
    float  kDotw = dot(kDir, w);

    if (kMag < EPSILON) // 아주 작은 값으로 비교하여 0 나누기 방지
    {
        return 0;
    }

    float PhilipsSpectrum = 2 * exp(-1 / pow(kMag * L , 2)) / pow(kMag, 4) * pow(kDotw, 2);
    return PhilipsSpectrum;
}

#endif