#ifndef __COMPUTE_Normal_MAP_HLSL__
# define __COMPUTE_Normal_MAP_HLSL__

# ifndef THREAD_X
#  error "THREAD_X Not Defined"
# endif
# ifndef THREAD_Y
#  error "THREAD_Y Not Defined"
# endif

Texture2D<float>     TerrainHeightMap : register(t0);
Texture2D<float>     TerrainBumpMap : register(t1);
RWTexture2D<float4>  Output : register(u0);

cbuffer CB_WeightMapCreate : register(b0)
{
    uint DimX;
    uint DimZ;
    float HeightScaler;
    float BumpScaler;
}

[numthreads(THREAD_X, THREAD_Y, 1)]
void CSMain(uint3 DTID : SV_DISPATCHTHREADID)
{
    const uint2 UV = DTID.xy;

    float2 dUV[4] = {
        float2(-1, 0), 
        float2(+1, 0),
        float2(0, -1),
        float2(0, +1)
    };
    float height[4] = {0, 0, 0, 0};

    int i;
    [unroll]
    for(i = 0 ; i < 4 ; i++)
    {
        height[i] = TerrainHeightMap[UV + dUV[i]].r * HeightScaler;
    }

    float StrideX = ((UV.x == 0) || (UV.x == DimX - 1)) ? 1.f : 2.f;
    float StrideZ = ((UV.y == 0) || (UV.y == DimZ - 1)) ? 1.f : 2.f;
    float HeightDiffX = (height[1] - height[0]);
    float HeightDiffZ = (height[3] - height[2]);

    float3 Tangent = normalize(float3(StrideX, 0, HeightDiffX));    // x
    float3 Bitangent = normalize(float3(0, StrideZ, HeightDiffZ));  // y
    float3 Normal = normalize(cross(Tangent, Bitangent));           // z
    float3x3 TBN = float3x3(Tangent, Bitangent, Normal);

    for(i = 0 ; i < 4 ; i++)
    {
        height[i] = TerrainBumpMap[UV + dUV[i]].r * BumpScaler;
    }
    StrideX = ((UV.x == 0) || (UV.x == DimX - 1)) ? 1.f : 2.f;
    StrideZ = ((UV.y == 0) || (UV.y == DimZ - 1)) ? 1.f : 2.f;
    HeightDiffX = (height[1] - height[0]);
    HeightDiffZ = (height[3] - height[2]);

    Tangent = normalize(float3(StrideX, 0, HeightDiffX));    // x
    Bitangent = normalize(float3(0, StrideZ, HeightDiffZ));  // y
    Normal = normalize(cross(Tangent, Bitangent)); 

    Normal = mul(Normal, TBN);
    Normal = (Normal + float3(1,1,1)) * 0.5f;
    Output[UV] = float4(Normal, 1);
}
#endif