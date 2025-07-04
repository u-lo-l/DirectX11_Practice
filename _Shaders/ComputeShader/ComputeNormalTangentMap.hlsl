#ifndef __COMPUTE_NORMAL_MAP_HLSL__
# define __COMPUTE_NORMAL_MAP_HLSL__

#define LEFT 0
#define RIGHT 1
#define BOTTOM 2
#define TOP 3

# ifndef THREAD_X
#  error "THREAD_X Not Defined"
# endif
# ifndef THREAD_Y
#  error "THREAD_Y Not Defined"
# endif

Texture2D<float>     TerrainHeightMap	: register(t0);
RWTexture2D<float4>  OutNormal			: register(u0);
RWTexture2D<float4>  OutTangent			: register(u1);

cbuffer CB_WeightMapCreate : register(b0)
{
    float HeightScaler;
	uint  TextureWidth;
	uint  TextureHeight;
    float Tiling;
}

[numthreads(THREAD_X, THREAD_Y, 1)]
void CSMain(uint3 DTID : SV_DISPATCHTHREADID)
{
    const uint2 UV = DTID.xy;

	int2 dUV[4];
	dUV[LEFT] = int2(-1, 0);
	dUV[RIGHT] = int2(+1, 0);
	dUV[BOTTOM] = int2(0, +1);
	dUV[TOP] = int2(0, -1);
    float height[4] = {0, 0, 0, 0};

    [unroll]
    for(int i = 0 ; i < 4 ; i++)
    {
        height[i] = TerrainHeightMap[UV + dUV[i]].r * HeightScaler;
    }

    float StrideX = ((UV.x == 0) || (UV.x == TextureWidth - 1)) ? 1.f : 2.f;
    float StrideY = ((UV.y == 0) || (UV.y == TextureHeight - 1)) ? 1.f : 2.f;
    float HeightDiffX = (height[RIGHT] - height[LEFT]);
    float HeightDiffY = (height[TOP] - height[BOTTOM]);

	float3 Tangent = normalize(float3(StrideX, 0, HeightDiffX));
    float3 Bitangent = normalize(float3(0, StrideY, HeightDiffY));  // y
    float3 Normal = normalize(cross(Tangent, Bitangent));           // z

	Normal = (Normal + 1.f) * 0.5f ;
	Tangent = (Tangent + 1.f) * 0.5f ;
	OutNormal[UV] = float4(Normal, 1);
	OutTangent[UV] = float4(Tangent, 1);
}
#endif