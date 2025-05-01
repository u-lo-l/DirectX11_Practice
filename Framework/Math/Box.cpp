#include "framework.h"
#include "Box.h"

Box::Box(const Vector& Center, const Vector& Extent)
{
	Vector Size = Extent * 0.5f;;
	Points[0] = Center + Vector(-Size.X , -Size.Y , -Size.Z);
	Points[1] = Center + Vector(-Size.X , -Size.Y , +Size.Z);
	Points[2] = Center + Vector(+Size.X , -Size.Y , +Size.Z);
	Points[3] = Center + Vector(+Size.X , -Size.Y , -Size.Z);
	Points[4] = Center + Vector(-Size.X , +Size.Y , -Size.Z);
	Points[5] = Center + Vector(-Size.X , +Size.Y , +Size.Z);
	Points[6] = Center + Vector(+Size.X , +Size.Y , +Size.Z);
	Points[7] = Center + Vector(+Size.X , +Size.Y , -Size.Z);
}

const array<Vector, 8>& Box::GetPoints() const
{
	return Points;
}
