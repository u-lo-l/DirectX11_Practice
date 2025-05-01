#include "framework.h"
#include "Frustum.h"

Frustum::Frustum(float Aspect, float VFOV, float Near, float Far)
{
	Vector Normal;
	
	Planes[Near] = Plane({0, 0, -1}, Near);
	Normal = Vector(-1, 0, tan(VFOV / 2) * Aspect);
	Normal.Normalize();
	Planes[Left] = Plane(Normal, 0);
	Normal.X = - Normal.X;
	Planes[Right] = Plane(Normal, 0);
	Normal = Vector(0, -1, tan(VFOV * 0.5f));
	Planes[Top] = Plane(Normal, 0);
	Normal.Y = - Normal.Y;
	Planes[Bottom] = Plane(Normal, 0);
	Planes[Far] = Plane({0, 0, 1}, -Far);
}

void Frustum::Transform(const Matrix& Mat)
{
	for (Plane & plane : Planes)
	{
		plane = Plane::Transform(plane, Mat);
	}
}

bool Frustum::Contains(const Vector& InPoint) const 
{
	for (const Plane & plane : Planes)
	{
		if (plane.GetDistance(InPoint) > 0)
			return false;
	}
	return true;
}

bool Frustum::Intersects(const Vector& Center, float Radius) const 
{
	for (const Plane & plane : Planes)
	{
		if (plane.GetDistance(Center) > Radius)
			return false;
	}
	return true;
}

bool Frustum::Intersects(const Box& InBox) const
{
	const array<Vector, 8> & BoxPoints = InBox.GetPoints();

	for (const Vector & Point : BoxPoints)
	{
		if (Contains(Point) == true)
			return true;
	}
	return false;
}


const array<Plane, 6>& Frustum::GetPlanes() const
{
	return Planes;
}
