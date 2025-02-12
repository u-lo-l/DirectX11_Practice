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
	Plane( const Vector& normal, float d);
	Plane( const Vector4& value);
	Plane( const Vector& point1, const Vector& point2, const Vector& point3);

	bool operator ==(const Plane& value) const;
	bool operator !=(const Plane& value) const;

	std::wstring ToWString();
	// D3DXPLANE ToD3DXPLANE();

	float Dot( const Vector4& value) const;
	float DotCoordinate( const Vector& value) const;
	float DotNormal( const Vector& value) const;

	void Normalize();
	static Plane Normalize( const Plane& value);
	static Plane Transform( const Plane& plane, const Matrix& matrix);
	static Plane Transform( const Plane& plane, const Quaternion& rotation);

public:
	Vector Normal;///< 노멀 벡터
	float D;///< 원점으로부터의 거리
};