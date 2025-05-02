#ifndef __TEXTURE_CREATION_HLSL__
# define __TEXTURE_CREATION_HLSL__
# ifndef THREAD_X
#  error "THREAD_X Not Defined"
# endif
# ifndef THREAD_Y
#  error "THREAD_Y Not Defined"
# endif
# ifndef THREAD_Z
#  error "THREAD_Z Not Defined"
# endif

cbuffer CB_Const : register(b0)
{
    uint Width;
    uint Height;
    uint2 Padding;
};

Texture2D<float4> InputTexture : register(t0);
RWTexture2D<float4> Texture : register(u0);

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void CSMain(uint3 DTID : SV_DISPATCHTHREADID)
{
    if (DTID.x >= Width || DTID.y >= Height)
        return;
    Texture[DTID.yx] = InputTexture[DTID.xy];
}

#endif