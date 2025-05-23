#pragma once

class Frustum
{
public:
	Frustum(float Aspect, float VFOV, float Near, float Far);
	Frustum(float Aspect, float VFOV, float Near, float Far,
		const Vector & Origin, const Vector & Forward, const Vector & Up, const Vector & Right);
	void UpdateShape(float Aspect, float VFOV, float Near, float Far, const Vector& Origin, const Vector& Forward, const Vector& Up, const
	                 Vector& Right);
	void UpdateTransform(const Vector& Origin, const Vector& Forward, const Vector& Up, const Vector& Right);
	void UpdateTransform(const Matrix & TransformMat);
	bool Contains(const Vector & InPoint) const;
	bool Intersects(const Vector & Center, float Radius) const;
	bool Intersects(const Box & InBox) const;
	bool Intersects(const Box* InBox) const;
	bool Intersects(const Vector& BoxCenter, const Vector& BoxDimension) const;
	const array<Plane, 6> & GetPlanes() const;

private:
	enum FrustumPlaneType : uint8_t
	{
		Near = 0, Far, Left, Right, Top, Bottom
	};
	array<Plane, 6> Planes;
	float Aspect;
	float VFOV;
	float NearDistance;
	float FarDistance;
};
