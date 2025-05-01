#pragma once

class Frustum
{
public:
	Frustum(float Aspect, float VFOV, float Near, float Far);
	void Transform(const Matrix & Mat);
	bool Contains(const Vector & InPoint) const;
	bool Intersects(const Vector & Center, float Radius) const;
	bool Intersects(const Box & InBox) const;
	const array<Plane, 6> & GetPlanes() const;

private:
	enum FrustumPlaneType : uint8_t
	{
		Near = 0, Left, Right, Top, Bottom, Far
	};
	array<Plane, 6> Planes;
};
