#pragma once

class Box
{
public:
	Box(const Vector & Center, const Vector & Dimension);
	const array<Vector, 8> & GetPoints() const;
	const Vector & GetCenter() const;
	const Vector & GetDimension() const;
	void SetCenter(const Vector & Center);
	void SetDimension(const Vector & Dimension);
	float GetDiagonal() const;
private:
	array<Vector, 8> Points;
	Vector Center;
	Vector Dimension;
	float Diagonal;
};
