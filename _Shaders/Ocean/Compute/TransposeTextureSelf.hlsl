#ifndef __TRANSEPOSE_TEXTURE_SELF_HLSL__
# define __TRANSEPOSE_TEXTURE_SELF_HLSL__
# include "../../ComputeShader/Complex.hlsl"

# ifndef THREAD_X
#  define THREAD_X 32
#  error "THREAD_X Not Defined"
# endif
# ifndef THREAD_Y
#  define THREAD_Y 32
#  error "THREAD_Y Not Defined"
# endif

// 스레드 그룹 크기 (예: THREAD_X=16, THREAD_Y=16)
// 각 스레드 그룹은 THREAD_X * THREAD_Y 크기의 블록을 처리합니다.

RWTexture2DArray<float4> InOutTexture : register(u0);

cbuffer CB_Const : register(b0)
{
    uint Width;
    uint Height;
    uint2 Padding; // 사용하지 않는다면 제거 가능
};

[numthreads(THREAD_X, THREAD_Y, 1)]
void CSMain(uint3 DTid  : SV_DispatchThreadID) // 전체 디스패치 내에서의 절대 스레드 ID
{
	if (DTid.x >= DTid.y)
		return ;
	float4 Temp = InOutTexture[DTid.xyz];
	InOutTexture[DTid.xyz] = InOutTexture[DTid.yxz];
	InOutTexture[DTid.yxz] = Temp;
}
#endif