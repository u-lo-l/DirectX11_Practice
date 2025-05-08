#ifndef __GAUSSIAN_TEXTURE_GENERATOR_HLSL__
#define __GAUSSIAN_TEXTURE_GENERATOR_HLSL__
# include "../../ComputeShader/Math.hlsl"

RWTexture2D<Complex> GaussianNoise : register(u0);

[numthreads(TEXTURE_SIZE,1,1)]
void CSMain(uint3 DTid : SV_DISPATCHTHREADID)
{

}

#endif