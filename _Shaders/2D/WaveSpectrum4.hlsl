#ifndef __WAVE_SPECTRUM_HLSL__
#define __WAVE_SPECTRUM_HLSL__
#include "../ComputeShader/Math.hlsl"

cbuffer CB_World : register(b0)
{
    matrix World;
};

cbuffer CB_Render2D : register(b1)
{
    matrix View2D;
    matrix Projection2D;
};

Texture2D<float4> Texture : register (t0);
SamplerState LinearSampler : register(s0);

struct VertexInput
{
    float4 Position : Position;
    float2 Uv : Uv;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float2 Uv : Uv;
};

VertexOutput VSMain(VertexInput input)
{
    VertexOutput output;
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View2D);
    output.Position = mul(output.Position, Projection2D);
    output.Uv = input.Uv;
    
    return output;
}

/* 
 * Initial Spectrum = GausianNoise * sqrt(PhilipsSpectrum / 2)
 * - 0 <= PhilipsSpectrum <= 0.000938 ( A : 1 일 때 )
 * - 0 <= Mag <= 0.02165
 * - -PI <= Ang <= -PI
*/
float4 PSMain(VertexOutput input) : SV_Target
{
    float3 color = Texture.Sample(LinearSampler, input.Uv).rgb;
    return float4(color, 1);
#if defined (USE_HSV)
    float Mag = length(color);
    float Ang = atan2(color.y, color.x);
    
    float H = (Ang / (2.0f * PI)) + 0.5f;
    float S = Mag / MagMax;
    float V = 1.0f;

    if (S < EPSILON)
    {
        H = S = V = 0;
    }

    float i = floor(H * 6);
    float f = V * (1 - S);
    float p = V * (1 - S);
    float q = V * (1 - f * S);
    float t = V * (1 - (1 - f) * S);

    if (i == 0)
        return float4(V, t, p, 1);
    else if (i == 1)
        return float4(q, V, p, 1);
    else if (i == 2)
        return float4(p, V, t, 1);
    else if (i == 3)
        return float4(p, q, V, 1);
    else if (i == 4)
        return float4(t, p, V, 1);
    else            
        return float4(V, p, q, 1);
#else
#endif
}

#endif