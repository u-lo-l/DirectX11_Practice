#pragma once

class Vector;
class Math
{
public:
	static float AngleOfInclineY(Vector value);
	static float Barycentric(float value1, float value2, float value3, float amount1, float amount2);
	static float CatmullRom(float value1, float value2, float value3, float value4, float amount);
	static float Clamp(float value, float min, float max);
	static float ConstrainedPi(float x);
	static float Constrained2Pi(float x);
	static float Constrained180(float x);
	static float Constrained360(float x);
	static float Distance(float value1, float value2);
	static float Hermite(float value1, float tangent1, float value2, float tangent2, float amount);
	static float Lerp(float value1, float value2, float amount);
	static float Max(float value1, float value2);
	static float Min(float value1, float value2);
	static float Round(float value);
	static float Round(float value, int position);
	static float SmoothStep(float amount);
	static float SmoothStep(float value1, float value2, float amount);
	static float SmootherStep(float amount);
	static float ToDegrees(float radians);
	static float ToRadians(float degrees);
	static float WrapAngle(float angle);

	static bool IsZero(float a);
	static bool IsOne(float a);

	static bool NearEqual(float a, float b);
	static bool WithinEpsilon(float a, float b);
	static bool WithinEpsilon(float a, float b, float epsilon);

	static float Gauss(float amplitude, float x, float y, float centerX, float centerY, float sigmaX, float sigmaY);

public:
	const static float E;///< 자연로그의 밑수
	const static float Log2E;///< 자연로그의 밑수 2
	const static float Log10E;///< 자연로그의 밑수 10
	const static float Pi;///< PI
	const static float TwoPi;///< PI * 2
	const static float PiOver2;///< PI / 2
	const static float PiOver4;///< PI / 4
	const static float Epsilon;///< 0.000001f

	const static int IntMinValue;///< Int형의 최소값
	const static int IntMaxValue;///< Int형의 최대값
	const static float FloatMinValue;///< Flaot형의 최소값
	const static float FloatMaxValue;///< Float형의 최대값
};