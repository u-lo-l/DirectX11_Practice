#include "framework.h"
#include "Frustum.h"

Frustum::Frustum(float Aspect, float VFOV, float Near, float Far)
	: Aspect(Aspect), VFOV(VFOV), NearDistance(Near), FarDistance(Far)
{
	Vector Normal;

	Planes[FrustumPlaneType::Near] = Plane({0, 0, 1}, this->NearDistance);
	Normal = Vector(-1, 0, tan(VFOV / 2) * Aspect);
	Normal.Normalize();
	Planes[FrustumPlaneType::Left] = Plane(-Normal, 0);
	Normal.X = - Normal.X;
	Planes[FrustumPlaneType::Right] = Plane(-Normal, 0);
	Normal = Vector(0, -1, tan(VFOV * 0.5f));
	Planes[FrustumPlaneType::Top] = Plane(-Normal, 0);
	Normal.Y = - Normal.Y;
	Planes[FrustumPlaneType::Bottom] = Plane(-Normal, 0);
	Planes[FrustumPlaneType::Far] = Plane({0, 0, -1}, -this->FarDistance);
}

Frustum::Frustum(float Aspect, float VFOV, float Near, float Far,
	const Vector& Origin, const Vector & Forward, const Vector& Up,	const Vector& Right)
: Aspect(Aspect), VFOV(VFOV), NearDistance(Near), FarDistance(Far)
	{
		const Vector NearCenter = Origin + Forward * this->NearDistance;
		const Vector FarCenter = Origin + Forward * this->FarDistance;
		const float HalfVSide = Far * tanf(VFOV / 2.0f);
		const float HalfHSide = HalfVSide * Aspect;
		
		Planes[FrustumPlaneType::Near] = Plane(Forward, NearCenter);
		Planes[FrustumPlaneType::Far] = Plane(-Forward, FarCenter);

		Vector SideVector = Vector::Normalize(Forward - Right * HalfHSide);
		Vector Normal = Vector::Cross(Up, SideVector);
		Planes[FrustumPlaneType::Left] = Plane(Normal, Origin);

		SideVector = Vector::Normalize(Forward + Right * HalfHSide);
		Normal = Vector::Cross(SideVector, Up);
		Planes[FrustumPlaneType::Right] = Plane(Normal, Origin);
		
		SideVector = Vector::Normalize(Forward + Up * HalfVSide);
		Normal = Vector::Cross(Right, SideVector);
		Planes[FrustumPlaneType::Top] = Plane(Normal, Origin);
		
		SideVector = Vector::Normalize(Forward - Up * HalfVSide);
		Normal = Vector::Cross(SideVector, Right);
		Planes[FrustumPlaneType::Bottom] = Plane(Normal, Origin);
	}

void Frustum::UpdateTransform
(
	const Vector& Origin,
	const Vector& Forward,
	const Vector& Up,
	const Vector& Right)
{
	const Vector NearCenter = Origin + Forward * this->NearDistance;
	const Vector FarCenter = Origin + Forward * this->FarDistance;
	const float HalfVSide = Far * tanf(this->VFOV / 2.0f);
	const float HalfHSide = HalfVSide * Aspect;
		
	Planes[FrustumPlaneType::Near] = Plane(Forward, NearCenter);
	Planes[FrustumPlaneType::Far] = Plane(-Forward, FarCenter);

	Vector SideVector = Vector::Normalize(Forward - Right * HalfHSide);
	Vector Normal = Vector::Cross(Up, SideVector);
	Planes[FrustumPlaneType::Left] = Plane(Normal, Origin);

	SideVector = Vector::Normalize(Forward + Right * HalfHSide);
	Normal = Vector::Cross(SideVector, Up);
	Planes[FrustumPlaneType::Right] = Plane(Normal, Origin);
		
	SideVector = Vector::Normalize(Forward + Up * HalfVSide);
	Normal = Vector::Cross(Right, SideVector);
	Planes[FrustumPlaneType::Top] = Plane(Normal, Origin);
		
	SideVector = Vector::Normalize(Forward - Up * HalfVSide);
	Normal = Vector::Cross(SideVector, Right);
	Planes[FrustumPlaneType::Bottom] = Plane(Normal, Origin);
}

bool Frustum::Contains(const Vector& InPoint) const 
	{
		for (const Plane & plane : Planes)
		{
			const float Distance = plane.GetDistance(InPoint);
			if (Distance < 0)
				return false;
		}
		return true;
	}

	bool Frustum::Intersects(const Vector& Center, float Radius) const 
	{
		for (const Plane & plane : Planes)
		{
			if (plane.GetDistance(Center) < -Radius)
				return false;
		}
		return true;
	}

	bool Frustum::Intersects(const Box& InBox) const
	{
		for (const Plane & plane : Planes)
		{
			Vector AbsNormal = Vector::Abs(plane.GetNormal());
			float MaxRadius = Vector::Dot(InBox.GetExtent(), AbsNormal);
			float DistanceToCenter = plane.GetDistance(InBox.GetCenter());

			if (MaxRadius + DistanceToCenter < 0)
				return false;
		}
		return true;
	}

	bool Frustum::Intersects(const Box * InBox) const
	{
		const array<Vector, 8> & BoxPoints = InBox->GetPoints();

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
