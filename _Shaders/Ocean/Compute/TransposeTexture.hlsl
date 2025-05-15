#ifndef __TRANSEPOSE_TEXTURE_HLSL__
# define __TRANSEPOSE_TEXTURE_HLSL__
# include "../../ComputeShader/Math.hlsl"

# ifndef THREAD_X
#  error "THREAD_X Not Defined"
# endif
# ifndef THREAD_Y
#  error "THREAD_Y Not Defined"
# endif


Texture2D InputTexture : register(t0);
RWTexture2D Transposed : register(u0);

cbuffer CB_Const : register(b0)
{
    uint Width;
    uint Height;
    uint2 Padding;
};

// In-Place방식은 Out-of-Place방식보다 성능이 낮을 수 있다.
// GPU의 메모리 접근 방식에 의해 그렇다.
//Dispatch(ConstData.Width / ThreadX, ConstData.Height / ThreadY,1);
[numthreads(THREAD_X, THREAD_Y, 1)]
void CSMain(uint3 DTid  : SV_DispatchThreadID)
{
	if (DTid.x >= Width || DTid.y >= Height)
        return;
    Transposed[DTid.yx] = InputTexture[DTid.xy];
}

#endif