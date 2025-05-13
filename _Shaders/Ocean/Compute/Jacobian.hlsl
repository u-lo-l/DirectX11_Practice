#ifndef __JACOBIAN_HLSL__
# define __JACOBIAN_HLSL__
# define TextureX 1024
# define TextureZ 1024

uint2 GetWrappedUV(uint2 InUV);

Texture2D<float> HeightMap : register(t0);
RWTexture2D<float4> JacobianMap : register(u0);

const static uint2 DeltaUV[4] = {
    uint2(-1, 0), uint2(1, 0), uint2(0, -1), uint2(0, 1)
};
[numthread(16, 16, 1)]
void CSMain(uint3 DTid : SV_DISPATCHTHREADID)
{
    int i = 0;
    float JacobianElement[4];
    uint2 Center = DTid.xy;
    uint2 TexCords[4];
    [unroll]
    for (i = 0 ; i < 4 ; i++)
        TexCords[i] = GetWrappedUV(Center + DeltaUV[i]);

    float dHdU = HeightMap[TexCords[1]] - HeightMap[TexCords[0]];
    float dHdV = HeightMap[TexCords[3]] - HeightMap[TexCords[2]];

}

uint2 GetWrappedUV(uint2 InUV)
{
    InUV.x = (InUV.x + 1024) % 1024;
    InUV.x = (InUV.x + 1024) % 1024;
    return InUV;
}

#endif