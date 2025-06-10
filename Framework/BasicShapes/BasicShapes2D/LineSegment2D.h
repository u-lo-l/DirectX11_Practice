#pragma once

class LineSegment2D
{
public:
	LineSegment2D(const Vector2D & EndPoint1, const Vector2D & EndPoint2);
	bool operator==(const LineSegment2D & Other) const;
	array<Vector2D, 2> GetEndPoints() const;
	const Vector2D& GetEndPoint(int Index) const;
	float GetLength() const;
	float GetLengthSquared() const;
	float GetXDisplacement() const;
	float GetYDisplacement() const;
private:
	Vector2D EndPoint1;
	Vector2D EndPoint2;
};

