#pragma once

class Box
{
public:
	Box(const Vector & Center, const Vector & Extent);
	const array<Vector, 8> & GetPoints() const;
private:
	array<Vector, 8> Points;
};
