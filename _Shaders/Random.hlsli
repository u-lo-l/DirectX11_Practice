#ifndef __RANDOM_HLSLI__
#define __RANDOM_HLSLI__

float Rand(float2 p) // between 0, 1
{
	// 임의의 큰 상수와 sin/frac을 조합
	float h = dot(p, float2(127.1, 311.7));
	return frac((sin(h) + 1.f) * 0.5f);
}


#endif