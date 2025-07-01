#ifndef __COMPUTE_NORMALMAP_BY_DISPLACEMENT_MAP__
#define __COMPUTE_NORMALMAP_BY_DISPLACEMENT_MAP__

# ifndef THREAD_X
#  error "THREAD_X Not Defined"
# endif
# ifndef THREAD_Y
#  error "THREAD_Y Not Defined"
# endif

cbuffer CB_Const : register(b0)
{
	float TextureWidth;
	float TextureHeight;
	float HeightScaler;
	float Padding;
}

Texture2D<float4> InDisplacementMap : register(t0); // Y-up
RWTexture2D<float4> OutNormalMap : register(u0);

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
	float3 DisplacementVector[4] = {0, 0, 0, 0};

	[unroll]
	for(int i = 0 ; i < 4 ; i++)
	{
		DisplacementVector[i] = (InDisplacementMap[UV + dUV[i]].rgb * 2.f - 1.f);
	}


	float StrideX = ((UV.x == 0) || (UV.x == TextureWidth - 1)) ? 1.f : 2.f;
	float StrideY = ((UV.y == 0) || (UV.y == TextureHeight - 1)) ? 1.f : 2.f;

	float HeightDiffX = (DisplacementVector[1].y - DisplacementVector[0].y);
	float HeightDiffY = (DisplacementVector[3].y - DisplacementVector[2].y);

	float3 Tangent = normalize(float3(StrideX, 0, HeightDiffX));
	float3 Bitangent = normalize(float3(0, StrideY, HeightDiffY));  // y
	float3 Normal = normalize(cross(Tangent, Bitangent));           // z

	Normal = (Normal + 1.f) * 0.5f ;

	OutNormalMap[UV] = float4(Normal, 1);
}

#endif