#ifndef __GET_TEXTURE_DATA_HLSL__
#define __GET_TEXTURE_DATA_HLSL__

const static uint SizeOfFloat4 = 16;

#ifndef FORMAT
# define FORMAT float4
#endif
Texture2D<FORMAT> InputTexture : register(t0);
RWByteAddressBuffer Output : register (u0);
SamplerState LinearSampler_Clamp : register(s0);

cbuffer CB_Resolution : register(b0)
{
    float2 Resolution;
    float2 Padding;
}

[numthreads(THREAD_X, THREAD_Y, 1)]
void CSMain(uint3 DTID : SV_DispatchThreadID)
{
    float2 UV = float2(
        (float)DTID.x / max((float)(Resolution.x - 1), 1),
        (float)DTID.y / max((float)(Resolution.y - 1), 1)
    );

    float4 ColorData = InputTexture.SampleLevel(LinearSampler_Clamp, UV, 0);
    uint OutIndex = (DTID.y * Resolution.x + DTID.x) * SizeOfFloat4;
    Output.Store4(OutIndex, asint(float4(ColorData)));
}

#endif