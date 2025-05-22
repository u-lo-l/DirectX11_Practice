#pragma once

class Vector2D;
class Point
{
public:
	Point();
	Point(int x, int y);

	operator float* ();
	operator const float* () const;

	bool operator ==(const Point& value2) const;
	bool operator !=(const Point& value2) const;

	Vector2D ToVector2() const;

	std::wstring ToWString() const;

public:
	const static Point Zero;///< 0, 0

	union
	{
		struct
		{
			int X;///< X
			int Y;///< Y
		};

		int P[2];
	};
};