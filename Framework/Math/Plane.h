#pragma once

class Vector4;
class Matrix;
class Quaternion;

enum class PlaneIntersectionType;

//////////////////////////////////////////////////////////////////////////
///@brief 3차원 평면은 평면위의 한 점P(l, m, n)과 법선벡터N(a, b, c)로 구성되며,<\br>
///평면의 방정식은 ax + by + cz + d = 0이다.<\br>
///이 때 d = -(al + bm + cn)이다.<\br>
/// D는 원점으로부터 평면까지의 거리로, 평면의 Origin은 Normal * D가 된다.
//////////////////////////////////////////////////////////////////////////
class Plane
{
public:
	Plane(void);
	Plane(float a, float b, float c, float d);
	Plane( const Vector& normal, float d);
	explicit Plane( const Vector4& value);
	Plane( const Vector& point1, const Vector& point2, const Vector& point3);

	bool operator ==(const Plane& value) const;
	bool operator !=(const Plane& value) const;
	
	std::wstring ToWString();

	float Dot( const Vector4& value) const;
	float DotCoordinate( const Vector& value) const;
	float DotNormal( const Vector& value) const;

	void Normalize();
	void Transform(const Matrix & InMatrix);
	Vector GetNormal() const;
	Vector GetOrigin() const;
	float GetDistance( const Vector & Point) const;
	static Plane Normalize( const Plane& value);
	static Plane Transform( const Plane& plane, const Matrix& matrix);
	static Plane Transform( const Plane& plane, const Quaternion& rotation);

public:
	Vector Normal;///< 노멀 벡터
	float D; // 
};