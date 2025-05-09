#ifndef __COMPUTE_FOLIAGE_POSITION_HLSL__
# define __COMPUTE_FOLIAGE_POSITION_HLSL__

# ifndef THREAD_X
#  error "THREAD_X Not Defined"
# endif
# ifndef THREAD_Y
#  error "THREAD_Y Not Defined"
# endif
# ifndef THREAD_Z
#  error "THREAD_Z Not Defined"
# endif

Texture2D TerrainHeightMap : register(t0);
RWByteAddressBuffer Output : register(u0);

SamplerState LinearSampler_Border : register(s0);

cbuffer CB_HeightScaler : register(b0)
{
    float Width;
    float Height;
    float HeightMin;
    float HeightMax;

    float HeightScaler;
    float3 Padding;
}

//Dispatch (WIDTH / THREAD_X, HEIGHT / THREAD_Y, 1)
[numthreads(THREAD_X, THREAD_Y, 1)]
void CSMain(uint3 DTID : SV_DISPATCHTHREADID)
{


}
#endif