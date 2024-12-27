#pragma once

class Vector2D;
class Point
{
public:
	Point(void);
	Point(int x, int y);

	bool operator ==(const Point& value2) const;
	bool operator !=(const Point& value2) const;

	D3DXVECTOR2 ToDX();
	Vector2D ToVector2();

	std::wstring ToString();

public:
	const static Point Zero;///< 0, 0

	int X;///< X
	int Y;///< Y
};