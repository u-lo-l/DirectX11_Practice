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
RWTexture2D<Complex> InitialSpectrum : register(u0);

// Dispatch(Size / THREAD_X, Size / THREAD_Y, 1)
[numthreads(THREAD_X, THREAD_Y, 1)]
void CSMain(uint3 DTID : SV_DISPATCHTHREADID)
{
    float2 Position = float2(DTID.x - Width * 0.5f, DTID.y - Height * 0.5f);
    float2 k = 2 * PI * float2(Position.x / Width, Position.y / Height);

    float2 w = normalize(Wind);
    float WindSpeed = length(Wind);
    float L = (WindSpeed * WindSpeed) / Gravity;

    float  kMag = length(k);
    float2 kDir = normalize(k);
    float  kDotw = dot(kDir, w);

    if (kMag < EPSILON) // 아주 작은 값으로 비교하여 0 나누기 방지
    {
        InitialSpectrum[DTID.xy] = Complex(0, 0);
        return;
    }

    float PhilipsSpectrum = exp(-1 / pow(kMag * L , 2)) / pow(kMag, 4) * (pow(kDotw, 2));
    Complex xi = GaussianNoise[DTID.xy];
    InitialSpectrum[DTID.xy] = xi * sqrt(PhilipsSpectrum * 0.5f);

    // H_Init이 1일 때 H_t가 DeltaFunction 형태여야 함.
    // InitialSpectrum[DTID.xy] = float2(1, 0);
}

#endif