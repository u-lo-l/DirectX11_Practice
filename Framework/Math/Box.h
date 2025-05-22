#pragma once

class Box
{
public:
	Box(const Vector & Center, const Vector & Extent);
	const array<Vector, 8> & GetPoints() const;
	const Vector & GetCenter() const;
	const Vector & GetExtent() const;
private:
	array<Vector, 8> Points;
	Vector Center;
	Vector Extent;
};
