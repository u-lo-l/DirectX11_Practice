#ifndef __OCEAN_NORMALMAP_GENERATOR_HLSL__
#define __OCEAN_NORMALMAP_GENERATOR_HLSL__


#define LEFT 0
#define RIGHT 1
#define TOP 2
#define BOTTOM 3

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
	float DisplacementMapTiling;
}

Texture2DArray<float4> InDisplacementMap : register(t0); // Z-outward Y-downward X-rightward LeftHanded
Texture2DArray<float>  FoamGrid : register(t1);
RWTexture2DArray<float4> OutNormalMap : register(u0);

uint2 GetWrappedTexCord(uint2 UV, int2 Offset);

/**
 * 모든 Texture는 Z-outward Y-downward X-rightward LeftHanded 로 구성된다.
 * 이후에 사용할 때는 TangentSpace에서 LocalSpace로 변화하여 사용해야함.
 * Z
 * ⬆
 * ┌⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯┐⮕U   Height(Displacement)Map에서 어떤 지점의 Normal을 구할 때 4방의 점의 Sampling하여 구한다.
 * │               │      LandScape에 HeightMap이 매칭될 때는 vec(R-L)이 +x(right)방향, vec(B-T)이 +z(down)방향이다.
 * │     T         │
 * │    L৹R        │      Normal은 (R-L) cross (T-B) 이다. (왼손)
 * │     B         │      이 결과를 [-1~1] -> [0~1] 로 범위를 줄여주면 NormalMap이 완성된다.
 * │               │
 * └⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯┘⮕X
 * ⬇
 * V
 *   < HeightMap >
 *
 * * Z
 * ⬆
 * ┌⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯┐⮕U
 * │               │      위에서 만든 NormalMap이라고 가정하자.
 * │               │      TangentSpace에서의 Normal이 <u, v, w> 일 때
 * │     ৹⮕       │      World에선 u->x v->z w->y로 매칭되지 때문에
 * │     ⬇         │      xyz = uwv 순서로 사용하면 된다.
 * │               │
 * └⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯┘⮕X
 * ⬇
 * V
 *   < NormalMap >
*/
[numthreads(THREAD_X, THREAD_Y, 1)] // Dispatch()
void CSMain(uint3 DTID : SV_DISPATCHTHREADID)
{
	const float DispScaler = 1 / DisplacementMapTiling;
	const float Scaler = HeightScaler * DispScaler;

	const uint2 UV = DTID.xy;

	int2 dUV[4];
	dUV[LEFT] = int2(-1, 0);
	dUV[RIGHT] = int2(+1, 0);
	dUV[TOP] = int2(0, -1);
	dUV[BOTTOM] = int2(0, +1);

	float3 SamplePositions[4];

	[unroll]
	for(int i = 0 ; i < 4 ; i++)
	{
		uint2 WrappedUV = GetWrappedTexCord(UV, dUV[i]);
		float3 DisplacementVector = InDisplacementMap.Load(uint4(WrappedUV, DTID.z, 0)).rgb * Scaler;
		SamplePositions[i] = float3(dUV[i], 0) * DispScaler + DisplacementVector;
	}

	float3 Tangent =   normalize(SamplePositions[RIGHT] - SamplePositions[LEFT]);
	float3 Bitangent = normalize(SamplePositions[BOTTOM] - SamplePositions[TOP]);
	float3 Normal = normalize(cross(Tangent, Bitangent));

	Normal = (Normal + 1.f) * 0.5f;
	OutNormalMap[DTID] = float4(Normal, 1);
}

uint2 GetWrappedTexCord(uint2 UV, int2 Offset)
{
	int2 wrapped = int2(UV) + Offset;

	wrapped.x = (wrapped.x + int(TextureWidth)) % ((uint)(TextureWidth));
	wrapped.y = (wrapped.y + int(TextureHeight)) % ((uint)(TextureHeight));

	return uint2(wrapped);
}
#endif