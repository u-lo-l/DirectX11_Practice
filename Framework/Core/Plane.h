#pragma once

class Vector4;
class Matrix;
class Quaternion;

class BBox;
class BFrustum;
class BSphere;

enum class PlaneIntersectionType;

//////////////////////////////////////////////////////////////////////////
///@brief 평면
//////////////////////////////////////////////////////////////////////////
class Plane
{
public:
	Plane(void);
	Plane(float a, float b, float c, float d);
	Plane(Vector& normal, float d);
	Plane(Vector4& value);
	Plane(Vector& point1, Vector& point2, Vector& point3);

	bool operator ==(const Plane& value) const;
	bool operator !=(const Plane& value) const;

	std::wstring ToString();
	D3DXPLANE ToD3DXPLANE();

	float Dot(Vector4& value);
	float DotCoordinate(Vector& value);
	float DotNormal(Vector& value);

	void Normalize();
	static Plane Normalize(Plane& value);
	static Plane Transform(Plane& plane, Matrix& matrix);
	static Plane Transform(Plane& plane, Quaternion& rotation);

public:
	Vector Normal;///< 노멀 벡터
	float D;///< 원점으로부터의 거리
};