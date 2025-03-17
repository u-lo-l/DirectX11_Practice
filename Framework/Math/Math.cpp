#include "Framework.h"
#include "Math.h"
#include <cstdlib>

const float Math::E = 2.71828175f;
const float Math::Log2E = 1.442695f;
const float Math::Log10E = 0.4342945f;
const float Math::Pi = 3.14159274f;
const float Math::TwoPi = 6.28318548f;
const float Math::PiOver2 = 1.57079637f;
const float Math::PiOver4 = 0.7853982f;
const float Math::DegToRadian = 0.0174533f;
const float Math::RadianToDeg = 57.2957764f;

#pragma warning( disable : 4146)
const int Math::IntMinValue = -2147483648;
const int Math::IntMaxValue = 2147483647;
const float Math::FloatMinValue = -3.402823E+38f;
const float Math::FloatMaxValue = 3.402823E+38f;
#pragma warning( default : 4146)

const float Math::Epsilon = 1e-6f;

float Math::AngleOfInclineY(Vector value)
{
	if (value == Vector::Up)
		return 90.0f;

	return ToDegrees(acosf(Vector::Dot(value, Vector(value.X, 0, value.Z))));
}

float Math::Barycentric(float value1, float value2, float value3, float amount1, float amount2)
{
	return value1 + amount1 * (value2 - value1) + amount2 * (value3 - value1);
}

float Math::CatmullRom(float value1, float value2, float value3, float value4, float amount)
{
	float single = amount * amount;
	float single1 = amount * single;

	return 0.5f * (2.0f * value2 + (-value1 + value3) * amount + (2.0f * value1 - 5.0f * value2 + 4.0f * value3 - value4) * single + (-value1 + 3.0f * value2 - 3.0f * value3 + value4) * single1);
}

float Math::Clamp(float value, float min, float max)
{
	value = (value > max ? max : value);
	value = (value < min ? min : value);
	return value;
}

float Math::ConstrainedPi(float x)
{
	x = fmodf(x + Pi, TwoPi);

	if (x < 0.0f)
		x += TwoPi;

	return x - Pi;
}

float Math::Constrained2Pi(float x)
{
	x = fmodf(x, TwoPi);

	if (x < 0.0f)
		x += TwoPi;

	return x;
}

float Math::Constrained180(float x)
{
	x = fmodf(x + 180.0f, 360.0f);

	if (x < 0.0f)
		x += 360.0f;

	return x - 180.0f;
}

float Math::Constrained360(float x)
{
	x = fmodf(x, 360.0f);

	if (x < 0.0f)
		x += 360.0;

	return x;
}

float Math::Distance(float value1, float value2)
{
	return fabsf(value1 - value2);
}

float Math::Hermite(float value1, float tangent1, float value2, float tangent2, float amount)
{
	float single = amount;

	float single1 = single * single;
	float single2 = single * single1;
	float single3 = 2.0f * single2 - 3.0f * single1 + 1.0f;
	float single4 = -2.0f * single2 + 3.0f * single1;
	float single5 = single2 - 2.0f * single1 + single;
	float single6 = single2 - single1;

	return value1 * single3 + value2 * single4 + tangent1 * single5 + tangent2 * single6;
}

float Math::Lerp(float value1, float value2, float amount)
{
	return value1 + (value2 - value1) * amount;
}

float Math::Max(float value1, float value2)
{
	return max(value1, value2);
}

float Math::Min(float value1, float value2)
{
	return min(value1, value2);
}

float Math::Round(float value)
{
	return Round(value, 0);
}

float Math::Round(float value, int position)
{
	float temp;
	temp = value * (float)pow(10.0f, position); //원하는 소수점 자리만큼의 10의 누승
	temp = floorf(temp + 0.5f); //0.5를 더해서 가장 가까운 낮은값을 찾음( ex=floor(0.2+0.5)->1.0, floor(0.7+0.5)->1.0f)
	temp *= (float)pow(10.0f, -position); //역승해 원값으로 돌림

	return temp;
}

float Math::SmoothStep(float amount)
{
	return (amount <= 0) ? 0
		: (amount >= 1) ? 1
		: amount * amount * (3 - (2 * amount));
}

float Math::SmoothStep(float value1, float value2, float amount)
{
	float single = Clamp(amount, 0.0f, 1.0f);

	return Lerp(value1, value2, single * single * (3.0f - 2.0f * single));
}

float Math::SmootherStep(float amount)
{
	return (amount <= 0) ? 0
		: (amount >= 1) ? 1
		: amount * amount * amount * (amount * ((amount * 6) - 15) + 10);
}

float Math::ToDegrees(float radians)
{
	return radians * 57.2957764f;
}

float Math::ToRadians(float degrees)
{
	return degrees * 0.0174532924f;
}

float Math::WrapAngle(float angle)
{
	angle = (float)remainder((double)angle, 6.28318548202515);

	if (angle <= -Pi)
	{
		angle += TwoPi;
	}
	else if (angle > Pi)
	{
		angle -= TwoPi;
	}

	return angle;
}

bool Math::IsZero(float a)
{
	return fabs(a) < Epsilon;
}

bool Math::IsOne(float a)
{
	return IsZero(a - 1.0f);
}

bool Math::NearEqual(float a, float b)
{
	if (IsZero(a - b))
		return true;

	int aInt = *reinterpret_cast<int *>(&a);
	int bInt = *reinterpret_cast<int *>(&b);

	if ((aInt < 0) != (bInt < 0))
		return false;

	int ulp = abs(aInt - bInt);
	const int maxUlp = 4;

	return (ulp <= maxUlp);
}

bool Math::WithinEpsilon(float a, float b)
{
	float num = a - b;

	return ((-Epsilon <= num) && (num <= Epsilon));
}

bool Math::WithinEpsilon(float a, float b, float epsilon)
{
	float num = a - b;

	return ((-epsilon <= num) && (num <= epsilon));
}

float Math::Gauss(float amplitude, float x, float y, float centerX, float centerY, float sigmaX, float sigmaY)
{
	float cx = x - centerX;
	float cy = y - centerY;

	float componentX = (cx * cx) / (2 * sigmaX * sigmaX);
	float componentY = (cy * cy) / (2 * sigmaY * sigmaY);

	return amplitude * expf(-(componentX + componentY));
}

int Math::Random( int min, int max )
{
	return min + rand() % (max - min);
}

float Math::Random( float min, float max )
{
	float random = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	float diff = max - min;
	float val = random * diff;

	return min + val;
}
