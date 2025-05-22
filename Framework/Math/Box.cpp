#include "framework.h"
#include "Box.h"

Box::Box(const Vector& Center, const Vector& Extent)
	: Center(Center), Extent(Extent)
{
	CHECK(Extent.X > 0 && Extent.Y > 0 && Extent.Z > 0);
	
	Points[0] = Center + Vector(-Extent.X , -Extent.Y , -Extent.Z);
	Points[1] = Center + Vector(-Extent.X , -Extent.Y , +Extent.Z);
	Points[2] = Center + Vector(+Extent.X , -Extent.Y , +Extent.Z);
	Points[3] = Center + Vector(+Extent.X , -Extent.Y , -Extent.Z);
	Points[4] = Center + Vector(-Extent.X , +Extent.Y , -Extent.Z);
	Points[5] = Center + Vector(-Extent.X , +Extent.Y , +Extent.Z);
	Points[6] = Center + Vector(+Extent.X , +Extent.Y , +Extent.Z);
	Points[7] = Center + Vector(+Extent.X , +Extent.Y , -Extent.Z);
}

const array<Vector, 8>& Box::GetPoints() const
{
	return Points;
}

const Vector& Box::GetCenter() const
{
	return Center;
}

const Vector& Box::GetExtent() const
{
	return Extent;
}
