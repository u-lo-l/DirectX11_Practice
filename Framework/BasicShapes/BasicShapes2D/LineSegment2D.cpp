#include "framework.h"
#include "LineSegment2D.h"

LineSegment2D::LineSegment2D(const Vector2D& EndPoint1, const Vector2D& EndPoint2)
	: EndPoint1(EndPoint1), EndPoint2(EndPoint2)
{
}

bool LineSegment2D::operator==(const LineSegment2D& Other) const
{
	bool bPoint1Same = Vector2D::NearEqual(EndPoint1, Other.EndPoint1);
	bool bPoint2Same = Vector2D::NearEqual(EndPoint2, Other.EndPoint2);
	bool SameLine = bPoint1Same && bPoint2Same;

	bPoint1Same = Vector2D::NearEqual(EndPoint1, Other.EndPoint2);
	bPoint2Same = Vector2D::NearEqual(EndPoint2, Other.EndPoint1);
	bool OppositeDirection = bPoint1Same && bPoint2Same;
	
	return SameLine || OppositeDirection;
}

array<Vector2D, 2> LineSegment2D::GetEndPoints() const
{
	return {EndPoint1, EndPoint2};
}

const Vector2D & LineSegment2D::GetEndPoint(int Index) const
{
	CHECK(Index >= 0 && Index < 2);
	return Index == 0 ? EndPoint1 : EndPoint2;
}

float LineSegment2D::GetLength() const
{
	return Vector2D::Distance(EndPoint1, EndPoint2);
}

float LineSegment2D::GetLengthSquared() const
{
	return Vector2D::DistanceSquared(EndPoint1, EndPoint2);
}

float LineSegment2D::GetXDisplacement() const
{
	return EndPoint2.X - EndPoint1.X;
}

float LineSegment2D::GetYDisplacement() const
{
	return EndPoint2.Y - EndPoint1.Y;
}



