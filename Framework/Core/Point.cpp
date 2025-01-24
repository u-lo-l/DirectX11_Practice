#include "Framework.h"
#include "Point.h"

const Point Point::Zero = Point(0, 0);

Point::Point(void)
{
	X = 0;
	Y = 0;
}

Point::Point(int x, int y)
{
	X = x;
	Y = y;
}

Point::operator float* ()
{
	return reinterpret_cast<float *>(P);
}

Point::operator const float* () const
{
	return reinterpret_cast<const float *>(P);
}

bool Point::operator==(const Point& value2) const
{
	if (X == value2.X)
		return Y == value2.Y;
	return false;
}

bool Point::operator!=(const Point& value2) const
{
	if (X == value2.X)
		return Y != value2.Y;
	return true;
}

Vector2D Point::ToVector2() const
{
	return {static_cast<float>(X), static_cast<float>(Y)};
}

std::wstring Point::ToWString() const
{
	std::wstring Temp;

	Temp += L"X:" + std::to_wstring(X);
	Temp += L",Y:" + std::to_wstring(Y);

	return Temp;
}
