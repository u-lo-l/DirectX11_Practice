#include "Framework.h"
#include "Math.h"

namespace Sdt
{
	const float Math::PI = 3.14159265f;

	float Math::ToRadian(float Degree)
	{
		return Degree * PI / 180.f;
	}

	float Math::ToDegree(float Radian)
	{
		return Radian * 180.f / PI;
	}
}
