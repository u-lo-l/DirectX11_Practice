#include "framework.h"
#include "Circle2D.h"

Circle2D::Circle2D(const Vector2D& InCenter, float InRadius)
	: Center(InCenter), Radius(InRadius)
{
}

const Vector2D& Circle2D::GetCenter() const
{
	return Center;
}

float Circle2D::GetRadius() const
{
	return Radius;
}

Circle2D::~Circle2D()
{
}
