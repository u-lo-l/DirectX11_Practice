#pragma once

class Circle2D;

class Triangle2D
{
public:
	explicit Triangle2D(const array<Vector2D, 3> & Vertices);
	Triangle2D(const Vector2D & A, const Vector2D & B, const Vector2D & C);
	static Circle2D GetCircumcircle(const Triangle2D & Triangle);
	const array<Vector2D, 3> & GetVertices() const;
	const Vector2D & GetVertex(int Index) const;
	void GetBarycentric(const Vector2D & Point, array<float, 3> & OutBarycentric) const;
	bool Contains(const Vector2D& Point, bool IncludeEdge = true) const;
	static bool IsValid(const array<Vector2D, 3> & Vertices);
	static bool IsValid(const Vector2D & A, const Vector2D & B, const Vector2D & C);
private:
	array<Vector2D, 3> Vertices;
};
