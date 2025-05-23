#include "framework.h"
#include "Box.h"

Box::Box(const Vector& Center, const Vector& Dimension)
	: Center(Center), Dimension(Dimension)
{
	CHECK(Dimension.X > 0 && Dimension.Y > 0 && Dimension.Z > 0);
	
	Points[0] = Center + Vector(-Dimension.X , -Dimension.Y , -Dimension.Z);
	Points[1] = Center + Vector(-Dimension.X , -Dimension.Y , +Dimension.Z);
	Points[2] = Center + Vector(+Dimension.X , -Dimension.Y , +Dimension.Z);
	Points[3] = Center + Vector(+Dimension.X , -Dimension.Y , -Dimension.Z);
	Points[4] = Center + Vector(-Dimension.X , +Dimension.Y , -Dimension.Z);
	Points[5] = Center + Vector(-Dimension.X , +Dimension.Y , +Dimension.Z);
	Points[6] = Center + Vector(+Dimension.X , +Dimension.Y , +Dimension.Z);
	Points[7] = Center + Vector(+Dimension.X , +Dimension.Y , -Dimension.Z);

	Diagonal = sqrt((Dimension.X * Dimension.X) + (Dimension.Y * Dimension.Y) + (Dimension.Z * Dimension.Z));
}

const array<Vector, 8>& Box::GetPoints() const
{
	return Points;
}

const Vector& Box::GetCenter() const
{
	return Center;
}

const Vector& Box::GetDimension() const
{
	return Dimension;
}

void Box::SetCenter(const Vector& Center)
{
	this->Center = Center;
}

void Box::SetDimension(const Vector& Dimension)
{
	this->Dimension = Dimension;
	Diagonal = sqrt((Dimension.X * Dimension.X) + (Dimension.Y * Dimension.Y) + (Dimension.Z * Dimension.Z));
}

float Box::GetDiagonal() const
{
	return Diagonal;
}
